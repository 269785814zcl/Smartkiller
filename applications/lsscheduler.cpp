#ifdef __cplusplus
extern "C" {
#endif

#include <rtthread.h>
#include "lsscheduler.h"
#include "control.h"
#include "topics.h"
// #include "icm20600.h"
// #include "estimator.h"
#include "Android.h"
// #include "commander.h"
#include "modbus.h"
#include "RS485.h"
#include "millisecondtimer.h"
#include "lsconfig.h"

//extern float pause_flag;
//extern void print_debug();
// int gear;
float joyslope_up = 1;
float joyslope_down = 0;
float pre_linear_joystick , ppre_linear_joystick = 0;
float pre_angular_joystick , ppre_angular_joystick = 0;
uint32_t uart_tick = 0;
float linear_vel_x = 0 , angular_vel_z = 0;
//void _schedule(void)
//{
//	schedule_tick = millis();
//	

//	//imu任务
////	icm20600_task();

//////	//姿态解算
////	estimator_task();

//	//校准命令
////	commander_task();

//	//速度测试
////	speed_test(); 

//	//安卓任务
//	AndroidTask();

//	//控制任务
//	control_task();
//    
//	//急停任务
//	Emergency_stop();



///****************************************单串口驱动输出***************************************/
//		if(schedule_tick %60 == 30 )
//    {        //每20ms写一次
//        joyslope_up = _actuator.linear_joystick == 0 ? 0.5 * (1 - gear/50) : ( joyslope_up >= 1 ? 1 : joyslope_up + 0.05f);
//			
//		rs485_write_single(1,
//                        (
//                          _actuator.linear_test
//                        - _actuator.linear_auto_x
//                        + _actuator.linear_joystick * joyslope_up   +   ppre_linear_joystick * joyslope_down
//                        + _actuator.linear_gravity
//                        + _actuator.linear_voice
//                        ) 
//                        , 
//                        (
//                        - _actuator.angular_auto_z 
//                        + _actuator.angular_euler
//                        + _actuator.angular_joystick * joyslope_up
//                        + _actuator.angular_gravity
//                        + _actuator.angular_rotate
//                        ) 
//                    );
//			uart_tick++;
//    }else if(schedule_tick %60 == 0 )
//    {        //每20ms写一次
//			rs485_write_single(2,
//                        (        
//                          _actuator.linear_test
//                        + _actuator.linear_auto_x
//                        + _actuator.linear_joystick * joyslope_up   +   ppre_linear_joystick * joyslope_down
//                        + _actuator.linear_gravity
//                        + _actuator.linear_voice
//                        ) 
//                        , 
//                        (
//                          _actuator.angular_auto_z 
//                        + _actuator.angular_euler
//                        + _actuator.angular_joystick * joyslope_up
//                        + _actuator.angular_gravity
//                        + _actuator.angular_rotate
//                        ) 
//                    );
//        if(pre_linear_joystick != _actuator.linear_joystick)
//        {
//            ppre_linear_joystick = pre_linear_joystick;
//        }
//        if(pre_angular_joystick != _actuator.angular_joystick)
//        {
//            ppre_angular_joystick = pre_angular_joystick;
//        }
//        pre_linear_joystick = _actuator.linear_joystick;
//        pre_angular_joystick = _actuator.angular_joystick;
//    }
//}


int _schedule_new(int flag,uint32_t s,uint32_t s_last)
{
	// rt_kprintf("XMU Haiyun Lab Loongson Start!\n");
    //rt_kprintf("%d\n",lamp);
	//安卓任务
	AndroidTask();
	//控制任务
	control_task();
	//急停任务
	Emergency_stop();
#if PRINT_SPEED
	rt_kprintf("%d %d\n",(int)(linear_vel_x*1000),(int)(angular_vel_z *1000));
#endif

#if USING_PWM
    //消毒器任务
    disinfector_control();
#endif
    //灯带任务
    lamp_control();

/****************************************单串口驱动输出***************************************/
	if( flag==1 && s-s_last>=30)
    {        //每20ms写一次
        //joyslope_up = _actuator.linear_joystick == 0 ? 0.5 * (1 - gear/50) : ( joyslope_up >= 1 ? 1 : joyslope_up + 0.05f);
			
		linear_vel_x =  _actuator.linear_test
                        + _actuator.linear_auto_x
                        + _actuator.linear_joystick * joyslope_up   +   ppre_linear_joystick * joyslope_down
                        + _actuator.linear_gravity
                        + _actuator.linear_voice;
		angular_vel_z =	_actuator.angular_auto_z 
                        + _actuator.angular_euler
                        + _actuator.angular_joystick * joyslope_up
                        + _actuator.angular_gravity
                        + _actuator.angular_rotate;
		rs485_write_single(1,linear_vel_x,angular_vel_z);

		uart_tick++;
		return 2;
    }
	else if(flag==2 && s-s_last>=30)
    {        //每20ms写一次
		// joyslope_up = _actuator.linear_joystick == 0 ? 0.5 * (1 - gear/50) : ( joyslope_up >= 1 ? 1 : joyslope_up + 0.05f);
		
		linear_vel_x =  _actuator.linear_test
                        + _actuator.linear_auto_x
                        + _actuator.linear_joystick * joyslope_up   +   ppre_linear_joystick * joyslope_down
                        + _actuator.linear_gravity
                        + _actuator.linear_voice;
		angular_vel_z =	_actuator.angular_auto_z 
                        + _actuator.angular_euler
                        + _actuator.angular_joystick * joyslope_up
                        + _actuator.angular_gravity
                        + _actuator.angular_rotate;
		rs485_write_single(2,linear_vel_x,angular_vel_z);

/*         if(pre_linear_joystick != _actuator.linear_joystick)
        {
            ppre_linear_joystick = pre_linear_joystick;
        }
        if(pre_angular_joystick != _actuator.angular_joystick)
        {
            ppre_angular_joystick = pre_angular_joystick;
        }
        pre_linear_joystick = _actuator.linear_joystick;
        pre_angular_joystick = _actuator.angular_joystick; */
		return 1;
    }
	
		return flag;
}
#ifdef __cplusplus
}
#endif
