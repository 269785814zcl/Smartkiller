#pragma once
#include "hardwareserial.h"
HardwareSerial Serial(SERIAL1);
#define SERIAL_CLASS  HardwareSerial

class LS1CHardware {
  public:
  	LS1CHardware(SERIAL_CLASS* io , long baud= 115200){
      iostream = io;
      baud_ = baud;
    }
    LS1CHardware()
    {
      iostream = &Serial;
      baud_ = 115200;
    }
    LS1CHardware(LS1CHardware& h){
	  this->iostream = iostream;
      this->baud_ = h.baud_;
    }

    void setBaud(long baud){
      this->baud_= baud;
    }

    int getBaud(){return baud_;}

    void init(){
      iostream->begin(baud_);
    }

    int read(){
      if(iostream->available()){
	  	return iostream->read();
      }else{
	    return -1;
      }
    };

    void write(uint8_t* data, int length){
      for(int i=0; i<length; i++){
		  iostream->write(data[i]);
      }
    }

    unsigned long time(){return millis();}

  protected:
    SERIAL_CLASS* iostream;
    long baud_;
};
