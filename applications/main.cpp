/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-10     zhuangwei    first version
 
mtd_erase /dev/mtd0
devcp tftp://193.169.2.215/rtthread.elf /dev/mtd0
set al /dev/mtd0
reboot

set al tftp://193.169.2.215/rtthread.elf
reboot

ifaddr syn0 193.169.2.230
set ifconfig syn0:193.169.2.230
 */
extern "C"{ void * __dso_handle = 0 ;}

#include <rtthread.h>
extern "C"{
#include "ls1c_uart.h"
#include "../libraries/ls1c_gpio.h"
#include "../libraries/ls1c_delay.h"
}

#include "pwm.h"
#include "gpio.h"
#include "RS485.h"
#include "topics.h"
#include "bluetooth.h"
#include "interrupt.h"
#include "Kinematics.h"
#include "lsscheduler.h"
#include "hardwareserial.h"
#include "millisecondtimer.h"

#include "lsconfig.h"

#include <stdio.h>
#include <ROSLIB/ros.h>
#include <ROSLIB/ros/time.h>
#include <ROSLIB/std_msgs/UInt8.h>
#include <ROSLIB/riki_msgs/PID.h>
#include <ROSLIB/riki_msgs/Imu.h>
#include <ROSLIB/riki_msgs/Battery.h>
#include <ROSLIB/riki_msgs/Velocities.h>
#include <ROSLIB/geometry_msgs/Twist.h>
#include <ROSLIB/geometry_msgs/Vector3.h>


float required_angular_vel = 0;
float required_linear_vel = 0;
uint32_t previous_command_time = 0;

Kinematics kinematics(MAX_RPM, WHEEL_DIAMETER, FR_WHEELS_DISTANCE, LR_WHEELS_DISTANCE);


// void pid_callback( const riki_msgs::PID& pid);
void command_callback( const geometry_msgs::Twist& cmd_msg);
void disinfect_callback( const std_msgs::UInt8& disinfect_msg);
void lamp_callback(const std_msgs::UInt8& lamp_msg);

ros::NodeHandle  nh;

riki_msgs::Imu raw_imu_msg;
riki_msgs::Velocities raw_vel_msg;
uint8_t disinfect = 0;	//消毒标志，1开始0停止
uint8_t lastlamp = 2;
uint8_t lamp = 2;	//绿灯2红灯0黄灯1
ros::Subscriber<geometry_msgs::Twist> cmd_sub("cmd_vel", command_callback);
// ros::Subscriber<riki_msgs::PID> pid_sub("pid", pid_callback);
ros::Publisher raw_vel_pub("raw_vel", &raw_vel_msg);
ros::Publisher raw_imu_pub("raw_imu", &raw_imu_msg);
ros::Subscriber<std_msgs::UInt8> disinfect_sub("/disinfect", disinfect_callback);
ros::Subscriber<std_msgs::UInt8> lamp_sub("/lamp", lamp_callback);

/* void pid_callback( const riki_msgs::PID& pid)
{
    motor1_pid.updateConstants(pid.p, pid.i, pid.d);
    motor2_pid.updateConstants(pid.p, pid.i, pid.d);
} */

void command_callback( const geometry_msgs::Twist& cmd_msg)
{
    required_linear_vel = (float)cmd_msg.linear.x;
    required_angular_vel = (float)cmd_msg.angular.z;

	// rt_kprintf("liner:%d %d %d \n",(int)(cmd_msg.linear.x*1000),(int)(cmd_msg.linear.y*1000),(int)(cmd_msg.linear.z*1000));
	// rt_kprintf("angular:%d %d %d \n",(int)(cmd_msg.angular.x*1000),(int)(cmd_msg.angular.y*1000),(int)(cmd_msg.angular.z*1000));
    previous_command_time = millis();
}

void move_base()
{
	 //fill in the object
    raw_vel_msg.linear_x = 0.0;
    raw_vel_msg.linear_y = 0.0;
    raw_vel_msg.angular_z = 0.0;

    //publish raw_vel_msg object to ROS
    raw_vel_pub.publish(&raw_vel_msg);

}

void disinfect_callback( const std_msgs::UInt8& disinfect_msg)
{
		disinfect = disinfect_msg.data;
}

void lamp_callback( const std_msgs::UInt8& lamp_msg)
{
		lamp = lamp_msg.data;
}

void stop_base()
{
    required_linear_vel = 0;
    required_angular_vel = 0;
}
void print_debug()
{
    char buffer[50]; 
    sprintf (buffer, "get line speed : %f, pwm: %d", required_linear_vel, 1);
    nh.loginfo(buffer);
}

void ros_init(void)
{
	nh.initNode();
	nh.advertise(raw_vel_pub);
	// nh.subscribe(pid_sub);
	nh.subscribe(cmd_sub);
	nh.subscribe(disinfect_sub);
	nh.subscribe(lamp_sub);

	while (!nh.connected()){
		nh.spinOnce();
	}
	rt_kprintf("Rikibase Connected!\n");
	nh.loginfo("Rikibase Connected!");
}

/* 用于接收消息的信号量 */
static struct rt_semaphore rx7_sem;
static struct rt_semaphore tx7_sem;

static rt_timer_t scheduler;
static rt_device_t serial4;
static rt_device_t serial7;

#if USING_BLUET
static struct rt_semaphore rx8_sem;
static rt_device_t serial8;
#endif

uint32_t s;
uint32_t s_last;
int flag=1;

uint8_t HardwareSerial_rx()
{
	uint8_t data;
	
	while (rt_device_read(serial7, 0, &data, 1) != 1)
    {
        /* 阻塞等待接收信号量，等到信号量后再次读取数据 */
        rt_sem_take(&rx7_sem, RT_WAITING_FOREVER);
    }
	//rt_kprintf("%d\n",data);
	
	// rt_device_write(serial7,0,&data,1);
	
	return data;
}

void HardwareSerial_tx(uint8_t ch)
{
	// rt_kprintf("%c",ch);
	while( rt_device_write(serial7, 0, &ch, 1) != 1)
	{
		rt_sem_take(&tx7_sem, RT_WAITING_FOREVER);
	}
}

uint8_t bluetooth_rx(void)
{
	uint8_t data;
	
	while (rt_device_read(serial8, 0, &data, 1) != 1)
	{
		/* 阻塞等待接收信号量，等到信号量后再次读取数据 */
		rt_sem_take(&rx8_sem, RT_WAITING_FOREVER);
	}
	
	rt_kprintf("   %c\n",data);

	return data;
	
}
void RS485_Send_Data(uint8_t *buf,uint8_t len)
{
	uint8_t t=0;
	//uart7_output(LS1C_UART8,buf);
	 rt_device_write(serial4,0,buf,len);
	
	// {
		// rt_sem_take(&rx8_sem, RT_WAITING_FOREVER);
	// }
	// while(  != len)
	// {
		// rt_sem_take(&rx8_sem, RT_WAITING_FOREVER);
	// }

  	// for(t=0;t<len;t++)
	// {
	    // while(UART_GetFlagStatus(UART2,UART_IT_TXIEN)==RESET){};	
		// UART_SendData(UART2,buf[t]); //发送数据
	// }
	// while(UART_GetFlagStatus(UART2,UART_IT_TXIEN)==RESET); //等待发送结束		
	// RS485_RX_CNT=0;
}

static void scheduler_timer(void *parameter)
{
	
	nh.spinOnce();
		
	  //_schedule();

	s=millis();
	if(_schedule_new(flag,s,s_last)!=flag)
	{
		if(flag==1){	
			flag=2;
		}else{
			flag=1;
		}
		s_last=s;
	}
	
	// rt_kprintf("%d %d\n",(int)(_actuator.linear_auto_x*1000),(int)(_actuator.angular_auto_z *1000));
}

rt_err_t uart7_input(rt_device_t dev, rt_size_t size)
{
    /* 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(&rx7_sem);
    return RT_EOK;
}

rt_err_t uart7_output(rt_device_t dev, void *buffer)
{
	rt_sem_release(&tx7_sem);
	return RT_EOK;
}

rt_err_t uart8_input(rt_device_t dev, rt_size_t size)
{
	rt_sem_release(&rx8_sem);
	return RT_EOK;
}

 void uart_set(ls1c_uart_t uart_num, unsigned int baudrate)
 {
	ls1c_uart_info_t uart_info;	
	switch(uart_num)
    {
	case LS1C_UART4:
        uart_info.UARTx = uart_num;
        uart_info.baudrate = baudrate;
        uart_info.rx_enable = TRUE;  
        uart_init(&uart_info);
		break;
    case LS1C_UART7:
        uart_info.UARTx = uart_num;
        uart_info.baudrate = baudrate;
        uart_info.rx_enable = TRUE;  
        uart_init(&uart_info);
		break;
    case LS1C_UART8:
		uart_info.UARTx = uart_num;
        uart_info.baudrate = baudrate;
        uart_info.rx_enable = TRUE;  
        uart_init(&uart_info);
		break;
	default:
		break;
	}
 }
 
 void serial7_thread_entry(void *parameter)
{
	while(1)
	{
		serial1_irq();
	}
}

 void serial8_thread_entry(void *parameter)
{
	while(1)
	{
		bluetooth_handle();
	}
}
 
int main(void)
{   
	uint32_t previous_debug_time = 0;

	initialise();
#if USING_GPIO
	gpio1_init();
	gpio2_init();
	gpio3_init();
	gpio_set_level(gpio1,gpio_level_low);
	gpio_set_level(gpio2,gpio_level_low);
	gpio_set_level(gpio3,gpio_level_high);
	delay_us(100);
#endif
	uart_set((ls1c_uart_t)5,9600);//uart4	RS485
	uart_set((ls1c_uart_t)8,115200);//uart7	PC
	uart_set((ls1c_uart_t)9,9600);//uart8	Bluetooth
#if USING_PWM
	pwm2_init(0,1000*1000*1000/87);//80% 87Hz
	pwm2_enable();
//	pwm2_disable();
#endif
	s=millis();
	s_last=s;

	rt_kprintf("XMU Haiyun Lab Loongson Start!\n");
	
	serial4 = rt_device_find("uart4");
    if (!serial4)
    {
        rt_kprintf("find %s failed!\n", "uart4");
        return RT_ERROR;
    }
	else
	{
		rt_kprintf("find %s succeed!\n", "uart4");
	}
	 
	serial7 = rt_device_find("uart7");
    if (!serial7)
    {
        rt_kprintf("find %s failed!\n", "uart7");
        return RT_ERROR;
    }
	else
	{
		rt_kprintf("find %s succeed!\n", "uart7");
	}
	
#if USING_BLUET
	serial8 = rt_device_find("uart8");
    if (!serial8)
    {
        rt_kprintf("find %s failed!\n", "uart8");
        return RT_ERROR;
    }
	else
	{
		rt_kprintf("find %s succeed!\n", "uart8");
	}
#endif

	/* 初始化信号量 */
    rt_sem_init(&rx7_sem, "rx7_sem", 0, RT_IPC_FLAG_FIFO);
	rt_sem_init(&tx7_sem, "tx7_sem", 0, RT_IPC_FLAG_FIFO);
#if USING_BLUET
	rt_sem_init(&rx8_sem, "rx8_sem", 0, RT_IPC_FLAG_FIFO);
#endif
	/* 以读写及中断接收方式打开串口设备 */
	rt_device_open(serial4, RT_DEVICE_OFLAG_RDWR );
    rt_device_open(serial7, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
#if USING_BLUET
	rt_device_open(serial8, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
#endif
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(serial7, uart7_input);
	rt_device_set_tx_complete(serial7, uart7_output);
#if USING_BLUET
	rt_device_set_rx_indicate(serial8, uart8_input);
#endif
    /* 创建 serial7 线程 */
    rt_thread_t thread7 = rt_thread_create("serial7", serial7_thread_entry, RT_NULL, 1024, 5, 20);
#if USING_BLUET
	rt_thread_t thread8 = rt_thread_create("serial8", serial8_thread_entry, RT_NULL, 1024, 5, 20);
#endif
    /* 创建成功则启动线程 */
    if (thread7 != RT_NULL)
    {
        rt_thread_startup(thread7);
		rt_kprintf("serial7 thread succeed!\n");
    }
	else
	{
		rt_kprintf("serial7 thread failed!\n");
	}
#if USING_BLUET
	if (thread8 != RT_NULL)
    {
        rt_thread_startup(thread8);
		rt_kprintf("serial8 thread succeed!\n");
    }
	else
	{
		rt_kprintf("serial8 thread failed!\n");
	}
#endif
	
#if USING_ROS
	ros_init();
#endif
	rt_kprintf("ros init secceed!\n");

    scheduler = rt_timer_create("scheduler", scheduler_timer,
                             RT_NULL, 20,
                             RT_TIMER_FLAG_PERIODIC);
	if (scheduler != RT_NULL)
	{
        rt_timer_start(scheduler);
		rt_kprintf("scheduler timer start!\n");
	}

	
    rt_thread_delay(15000);

    return 0;
}
