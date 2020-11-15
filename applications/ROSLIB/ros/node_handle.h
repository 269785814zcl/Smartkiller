/*
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2011, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of Willow Garage, Inc. nor the names of its
 *    contributors may be used to endorse or promote prducts derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ROS_NODE_HANDLE_H_
#define ROS_NODE_HANDLE_H_
#include <rtthread.h>

#include "../std_msgs/Time.h"
#include "../rosserial_msgs/TopicInfo.h"
#include "../rosserial_msgs/Log.h"
#include "../rosserial_msgs/RequestParam.h"

#define SYNC_SECONDS        5

#define MODE_FIRST_FF       0
/*
 * The second sync byte is a protocol version. It's value is 0xff for the first
 * version of the rosserial protocol (used up to hydro), 0xfe for the second version
 * (introduced in hydro), 0xfd for the next, and so on. Its purpose is to enable
 * detection of mismatched protocol versions (e.g. hydro rosserial_python with groovy
 * rosserial_arduino. It must be changed in both this file and in
 * rosserial_python/src/rosserial_python/SerialClient.py
 */
#define MODE_PROTOCOL_VER   1
#define PROTOCOL_VER1		0xff // through groovy
#define PROTOCOL_VER2		0xfe // in hydro
#define PROTOCOL_VER 		PROTOCOL_VER2
#define MODE_SIZE_L         2   
#define MODE_SIZE_H         3
#define MODE_SIZE_CHECKSUM  4   // checksum for msg size received from size L and H
#define MODE_TOPIC_L        5   // waiting for topic id
#define MODE_TOPIC_H        6
#define MODE_MESSAGE        7
#define MODE_MSG_CHECKSUM   8   // checksum for msg and topic id 


#define MSG_TIMEOUT 20  //20 milliseconds to recieve all of message data

#include "msg.h"
#include "time.h"
namespace ros {

  class NodeHandleBase_{
    public:
      virtual int publish(int id, const Msg* msg)=0;
      virtual int spinOnce()=0;
      virtual bool connected()=0;
    };
}

#include "publisher.h"
#include "subscriber.h"
#include "service_server.h"
#include "service_client.h"

namespace ros {

  using rosserial_msgs::TopicInfo;

  /* Node Handle */
  template<class Hardware,
           int MAX_SUBSCRIBERS=25,
           int MAX_PUBLISHERS=25,
           int INPUT_SIZE=512,
           int OUTPUT_SIZE=512>
  class NodeHandle_ : public NodeHandleBase_
  {
    protected:
      Hardware hardware_;

      /* time used for syncing */
      unsigned long rt_time;

      /* used for computing current time */
      unsigned long sec_offset, nsec_offset;

      unsigned char message_in[INPUT_SIZE];
      unsigned char message_out[OUTPUT_SIZE];

      Publisher * publishers[MAX_PUBLISHERS];
      Subscriber_ * subscribers[MAX_SUBSCRIBERS];

      /*
       * Setup Functions
       */
    public:
      NodeHandle_() : configured_(false) {

        for(unsigned int i=0; i< MAX_PUBLISHERS; i++) 
	   publishers[i] = 0;

        for(unsigned int i=0; i< MAX_SUBSCRIBERS; i++) 
	   subscribers[i] = 0;

        for(unsigned int i=0; i< INPUT_SIZE; i++) 
	   message_in[i] = 0;

        for(unsigned int i=0; i< OUTPUT_SIZE; i++) 
	   message_out[i] = 0;

        req_param_resp.ints_length = 0;
        req_param_resp.ints = NULL;
        req_param_resp.floats_length = 0;
        req_param_resp.floats = NULL;
        req_param_resp.ints_length = 0;
        req_param_resp.ints = NULL;
      }
      
      Hardware* getHardware(){
        return &hardware_;
      }

      /* Start serial, initialize buffers */
      void initNode(){
        hardware_.init();
        mode_ = 0;
        bytes_ = 0;
        index_ = 0;
        topic_ = 0;
      };

      /* Start a named port, which may be network server IP, initialize buffers */
      void initNode(char *portName){
        hardware_.init(portName);
        mode_ = 0;
        bytes_ = 0;
        index_ = 0;
        topic_ = 0;
      };

    protected:
      //State machine variables for spinOnce
      int mode_;
      int bytes_;
      int topic_;
      int index_;
      int checksum_;

      bool configured_;

      /* used for syncing the time */
      unsigned long last_sync_time;
      unsigned long last_sync_receive_time;
      unsigned long last_msg_timeout_time;

    public:
      /* This function goes in your loop() function, it handles
       *  serial input and callbacks for subscribers.
       */
		

      virtual int spinOnce(){

        /* restart if timed out */
        unsigned long c_time = hardware_.time();
        if( (c_time - last_sync_receive_time) > (SYNC_SECONDS*2200) ){
            configured_ = false;
         }
         
        /* reset if message has timed out */
        if ( mode_ != MODE_FIRST_FF){ 
          if (c_time > last_msg_timeout_time){
            mode_ = MODE_FIRST_FF;
          }
        }

        /* while available buffer, read data */
        while( true )
        {
          int data = (int)hardware_.read();
		  // rt_kprintf("ringbuffer:%d\n",data);
          if( data < 0 )
          {
			  break;
		  }
          checksum_ += data;
          if( mode_ == MODE_MESSAGE ){        /*7 message data being recieved */
            message_in[index_++] = data;
            bytes_--;
            if(bytes_ == 0)                  /* is message complete? if so, checksum */
              mode_ = MODE_MSG_CHECKSUM;
          }else if( mode_ == MODE_FIRST_FF ){  //0
            if(data == 0xff){   //255
              mode_++;
              last_msg_timeout_time = c_time + MSG_TIMEOUT;
            }
            else if( hardware_.time() - c_time > (SYNC_SECONDS)){
              /* We have been stuck in spinOnce too long, return error */
              configured_=false;
              return -2;
            }
          }else if( mode_ == MODE_PROTOCOL_VER ){ //1
            if(data == PROTOCOL_VER){   		//0xfe 254
              mode_++;
            }else{
              mode_ = MODE_FIRST_FF; 			//0
              if (configured_ == false)
                  requestSyncTime(); 	/* send a msg back showing our protocol version */
            }
		  }else if( mode_ == MODE_SIZE_L ){   /*2 bottom half of message size */
            bytes_ = data;
            index_ = 0;
            mode_++;
            checksum_ = data;               /* first byte for calculating size checksum */
          }else if( mode_ == MODE_SIZE_H ){   /*3 top half of message size */
            bytes_ += data<<8;
			mode_++;
          }else if( mode_ == MODE_SIZE_CHECKSUM ){  //4
            if( (checksum_%256) == 255)
				mode_++;
			else 
				mode_ = MODE_FIRST_FF;          /* Abandon the frame if the msg len is wrong */
		  }else if( mode_ == MODE_TOPIC_L ){  /*5 bottom half of topic id */
            topic_ = data;
            mode_++;
            checksum_ = data;               /* first byte included in checksum */
          }else if( mode_ == MODE_TOPIC_H ){  /*6 top half of topic id */
            topic_ += data<<8;
            mode_ = MODE_MESSAGE;
            if(bytes_ == 0)
              mode_ = MODE_MSG_CHECKSUM;  
          }else if( mode_ == MODE_MSG_CHECKSUM ){ /*8 do checksum */
            mode_ = MODE_FIRST_FF;
            if( (checksum_%256) == 255){
              if(topic_ == TopicInfo::ID_PUBLISHER){
                requestSyncTime();
                negotiateTopics();
                last_sync_time = c_time;
                last_sync_receive_time = c_time;
                return -1;
              }else if(topic_ == TopicInfo::ID_TIME){
                syncTime(message_in);
              }else if (topic_ == TopicInfo::ID_PARAMETER_REQUEST){
                  req_param_resp.deserialize(message_in);
                  param_recieved= true;
              }else if(topic_ == TopicInfo::ID_TX_STOP){
                  configured_ = false;
              }else{
                if(subscribers[topic_-100])
				{
                  subscribers[topic_-100]->callback( message_in );
				}
              }
            }
          }
        }

        /* occasionally sync time */
        if( configured_ && ((c_time-last_sync_time) > (SYNC_SECONDS*500) )){
          requestSyncTime();
          last_sync_time = c_time;
        }

        return 0;
      }


      /* Are we connected to the PC? */
      virtual bool connected() {
        return configured_;
      };

      /********************************************************************
       * Time functions
       */

      void requestSyncTime()
      {
        std_msgs::Time t;
        publish(TopicInfo::ID_TIME, &t);
        rt_time = hardware_.time();
      }

      void syncTime( unsigned char * data )
      {
        std_msgs::Time t;
        unsigned long offset = hardware_.time() - rt_time;

        t.deserialize(data);
        t.data.sec += offset/1000;
        t.data.nsec += (offset%1000)*1000000UL;

        this->setNow(t.data);
        last_sync_receive_time = hardware_.time();
      }

      Time now(){
        unsigned long ms = hardware_.time();
        Time current_time;
        current_time.sec = ms/1000 + sec_offset;
        current_time.nsec = (ms%1000)*1000000UL + nsec_offset;
        normalizeSecNSec(current_time.sec, current_time.nsec);
        return current_time;
      }

      void setNow( Time & new_now )
      {
        unsigned long ms = hardware_.time();
        sec_offset = new_now.sec - ms/1000 - 1;
        nsec_offset = new_now.nsec - (ms%1000)*1000000UL + 1000000000UL;
        normalizeSecNSec(sec_offset, nsec_offset);
      }

      /********************************************************************
       * Topic Management 
       */

      /* Register a new publisher */    
      bool advertise(Publisher & p)
      {
        for(int i = 0; i < MAX_PUBLISHERS; i++){
          if(publishers[i] == 0){ // empty slot
            publishers[i] = &p;
            p.id_ = i+100+MAX_SUBSCRIBERS;
            p.nh_ = this;
            return true;
          }
        }
        return false;
      }

      /* Register a new subscriber */
      template<typename MsgT>
      bool subscribe(Subscriber< MsgT> & s){
        for(int i = 0; i < MAX_SUBSCRIBERS; i++){
          if(subscribers[i] == 0){ // empty slot
            subscribers[i] = (Subscriber_*) &s;
            s.id_ = i+100;
            return true;
          }
        }
        return false;
      }

      /* Register a new Service Server */
      template<typename MReq, typename MRes>
      bool advertiseService(ServiceServer<MReq,MRes>& srv){
        bool v = advertise(srv.pub);
        for(int i = 0; i < MAX_SUBSCRIBERS; i++){
          if(subscribers[i] == 0){ // empty slot
            subscribers[i] = (Subscriber_*) &srv;
            srv.id_ = i+100;
            return v;
          }
        }
        return false;
      }

      /* Register a new Service Client */
      template<typename MReq, typename MRes>
      bool serviceClient(ServiceClient<MReq, MRes>& srv){
        bool v = advertise(srv.pub);
        for(int i = 0; i < MAX_SUBSCRIBERS; i++){
          if(subscribers[i] == 0){ // empty slot
            subscribers[i] = (Subscriber_*) &srv;
            srv.id_ = i+100;
            return v;
          }
        }
        return false;
      }

      void negotiateTopics()
      {
        rosserial_msgs::TopicInfo ti;
        int i;
        for(i = 0; i < MAX_PUBLISHERS; i++)
        {
          if(publishers[i] != 0) // non-empty slot
          {
            ti.topic_id = publishers[i]->id_;
            ti.topic_name = (char *) publishers[i]->topic_;
            ti.message_type = (char *) publishers[i]->msg_->getType();
            ti.md5sum = (char *) publishers[i]->msg_->getMD5();
            ti.buffer_size = OUTPUT_SIZE;
            publish( publishers[i]->getEndpointType(), &ti );
          }
        }
        for(i = 0; i < MAX_SUBSCRIBERS; i++)
        {
          if(subscribers[i] != 0) // non-empty slot
          {
            ti.topic_id = subscribers[i]->id_;
            ti.topic_name = (char *) subscribers[i]->topic_;
            ti.message_type = (char *) subscribers[i]->getMsgType();
            ti.md5sum = (char *) subscribers[i]->getMsgMD5();
            ti.buffer_size = INPUT_SIZE;
            publish( subscribers[i]->getEndpointType(), &ti );
          }
        }
        configured_ = true;
      }

      virtual int publish(int id, const Msg * msg)
      {
        if(id >= 100 && !configured_) 
	  return 0;

        /* serialize message */
        unsigned int l = msg->serialize(message_out+7);

        /* setup the header */
        message_out[0] = 0xff;
        message_out[1] = PROTOCOL_VER;
        message_out[2] = (unsigned char) ((unsigned int)l&255);
        message_out[3] = (unsigned char) ((unsigned int)l>>8);
		message_out[4] = 255 - ((message_out[2] + message_out[3])%256);
        message_out[5] = (unsigned char) ((int)id&255);
        message_out[6] = (unsigned char) ((int)id>>8);

        /* calculate checksum */
        int chk = 0;
        for(int i =5; i<l+7; i++)
          chk += message_out[i];
        l += 7;
        message_out[l++] = 255 - (chk%256);

        if( l <= OUTPUT_SIZE ){
          hardware_.write(message_out, l);
          return l;
        }else{
          logerror("Message from device dropped: message larger than buffer.");
          return -1;
        }
      }

      /********************************************************************
       * Logging
       */

    private:
      void log(char byte, const char * msg){
        rosserial_msgs::Log l;
        l.level= byte;
        l.msg = (char*)msg;
        publish(rosserial_msgs::TopicInfo::ID_LOG, &l);
      }

    public:
      void logdebug(const char* msg){
        log(rosserial_msgs::Log::ROSDEBUG, msg);
      }
      void loginfo(const char * msg){
        log(rosserial_msgs::Log::INFO, msg);
      }
      void logwarn(const char *msg){
        log(rosserial_msgs::Log::WARN, msg);
      }
      void logerror(const char*msg){
        log(rosserial_msgs::Log::ERROR, msg);
      }
      void logfatal(const char*msg){
        log(rosserial_msgs::Log::FATAL, msg);
      }

      /********************************************************************
       * Parameters
       */

    private:
      bool param_recieved;
      rosserial_msgs::RequestParamResponse req_param_resp;

      bool requestParam(const char * name, int time_out =  1000){
        param_recieved = false;
        rosserial_msgs::RequestParamRequest req;
        req.name  = (char*)name;
        publish(TopicInfo::ID_PARAMETER_REQUEST, &req);
        unsigned int end_time = hardware_.time() + time_out;
        while(!param_recieved ){
          spinOnce();
          if (hardware_.time() > end_time) return false;
        }
        return true;
      }

    public:
      bool getParam(const char* name, int* param, int length =1){
        if (requestParam(name) ){
          if (length == req_param_resp.ints_length){
            //copy it over
            for(int i=0; i<length; i++)
              param[i] = req_param_resp.ints[i];
            return true;
          }
        }
        return false;
      }
      bool getParam(const char* name, float* param, int length=1){
        if (requestParam(name) ){
          if (length == req_param_resp.floats_length){
            //copy it over
            for(int i=0; i<length; i++) 
              param[i] = req_param_resp.floats[i];
            return true;
          }
        }
        return false;
      }
      bool getParam(const char* name, char** param, int length=1){
        if (requestParam(name) ){
          if (length == req_param_resp.strings_length){
            //copy it over
            for(int i=0; i<length; i++)
              strcpy(param[i],req_param_resp.strings[i]);
            return true;
          }
        }
        return false;
      }  
  };

}

#endif
