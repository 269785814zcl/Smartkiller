#ifdef __cplusplus
extern "C" {
#endif

#include "Android.h"
#include "modbus.h"
#include "millisecondtimer.h"
#include "topics.h"
#include "rosconfig.h"
#include "RS485.h"

uint8_t Rx1_update_flag;
extern uint8_t Rx_APP_data[100];
uint8_t mode_choose = 1;
//extern uint8_t Rx_mode_data[9];
uint8_t pre_mode;
uint8_t start_stop = 0;
uint32_t key_count = 0;
uint32_t android_tick = 0;

float g_x_rate = 3.5f;
float g_z_rate = 1.8f;
float g_motor1 = 0, g_motor2 = 0;
uint8_t SetAcc_flag = 0;

uint8_t turn_flag = 0;
uint8_t pre_turn_flag = 0;
float turn_angle = PI/180*30.0f;
int gear = 1;
int rotate_flag = -1;

float k_x_rate = 0.02;
float k_z_rate = 0.05;

void KeyControl(void)
{
	key_count++;
    if(mode_choose==1)
    {
        if(Rx1_update_flag)
        {
            switch(Rx_APP_data[0])
            {
                case '1'://前

                    pre_turn_flag = 1;

                    _actuator.linear_joystick = 15 + 1.1 * gear;
					_actuator.linear_joystick *= k_x_rate;
                    _actuator.angular_joystick = 0;
                    _actuator.angular_joystick *= k_z_rate;
                    Rx1_update_flag=0;
                    break;
                case '2'://前右
                    pre_turn_flag = 2;

                    _actuator.linear_joystick = 10 + gear;
					_actuator.linear_joystick *= k_x_rate;
                    _actuator.angular_joystick = 8;
					_actuator.angular_joystick *= k_z_rate;
                    Rx1_update_flag=0;
                    break;
                case '3'://右
                    pre_turn_flag = 3;

                    _actuator.linear_joystick = 0;
					_actuator.linear_joystick *= k_x_rate;
                    _actuator.angular_joystick =  -10 - 0.1 * gear;
					_actuator.angular_joystick *= k_z_rate;
                    Rx1_update_flag=0;
                    break;

                case '5'://后
                    _actuator.linear_joystick = -15 - gear * 0.2f;
					_actuator.linear_joystick *= k_x_rate;
                    _actuator.angular_joystick = 0;
					_actuator.angular_joystick *= k_z_rate;
                    pre_turn_flag = 5;

                    Rx1_update_flag=0;
                    break;
                
                case '7'://左
                    pre_turn_flag = 7;
                
                    _actuator.linear_joystick = 0;
					_actuator.linear_joystick *= k_x_rate;
                    _actuator.angular_joystick = 10 + 0.1 * gear;
					_actuator.angular_joystick *= k_z_rate;

                    Rx1_update_flag=0;
                    break;

                case '8'://前左
                    pre_turn_flag = 8;

                    _actuator.linear_joystick = 10 + gear;
					_actuator.linear_joystick *= k_x_rate;
                    _actuator.angular_joystick = - 8;
					_actuator.angular_joystick *= k_z_rate;

                    Rx1_update_flag=0;

                    break;

                default:
                    Rx1_update_flag=0;
                    _actuator.linear_joystick = 0;
    			    _actuator.angular_joystick = 0;
//                    Stop(0x01,emergency);
//                    Stop(0x02,emergency);
                    break;
            }
        }
    }
    else
    {
        _actuator.linear_joystick = 0;
        _actuator.angular_joystick = 0;
    }
//    if(     Rx_mode_data[0] == 'a'
//        &&  Rx_mode_data[1] == 'b'
//        &&  Rx_mode_data[2] == 'c'
//        &&  Rx_mode_data[3] == 'b'
//        &&  Rx_mode_data[4] == 'c'
//        &&  Rx_mode_data[5] == 'b'
//        &&  Rx_mode_data[6] == 'c'
//        &&  Rx_mode_data[7] == 'b'
//        &&  Rx_mode_data[8] == 'a')
//    {
//        _actuator.linear_joystick = 0;
//        _actuator.angular_joystick = -5;
//    }
}

float x_vel=0,z_vel=0,zz;
void gravity_control(void)
{
	uint8_t p=0;
	float x,z;
    if(mode_choose==2)
    {
        if(Rx1_update_flag)
        {
            if(Rx_APP_data[0]=='n')  //收到n则表示停止指示
            {
                start_stop=0;   //设置成stop状态
                _actuator.linear_gravity = 0;
                _actuator.angular_gravity = 0;
                Rx1_update_flag=0;
            }
            else if(Rx_APP_data[0]=='y')  //收到y则表示开始指示
            {
                start_stop=1;   //设置成start状态
                Rx1_update_flag=0;
            }
            if(start_stop)
            {
                while(Rx_APP_data[p]!=',')
                {
                  z*=10.0f;
                    z+=Rx_APP_data[p]-'0';
                    p++;
                }
                p++;  //跳过逗号
                while(Rx_APP_data[p]!='s')
                {
                    x*=10.0f;
                    x+=Rx_APP_data[p]-'0';
                    p++;
                }
                Rx1_update_flag=0; //标志位清零
                zz=z;
                x_vel = g_x_rate * ( x >= 10 ? (x - 10.0f) : (x > 7.0f ? 0 : x - 7.0f));
                z_vel = g_z_rate * (10.0f - z);
                 if (x_vel > 40)
                     x_vel = 40;
                 if (z_vel > 40)
                     z_vel = 40;
                 if (z_vel < -40)
                     z_vel = -40;
                _actuator.linear_gravity = x_vel*0.01;
                
                _actuator.angular_gravity = - z_vel*0.01;
            }
            
        }
    }
    else
    {
        _actuator.linear_gravity = 0;
        _actuator.angular_gravity = 0;
    }
}

void start_control(void)
{
  if(Rx1_update_flag)
	{
    if(Rx_APP_data[0]=='y')  //收到y则表示开始指示
		{
			start_stop=1;   //设置成start状态
			Rx1_update_flag=0;
		}
	}
}

void stop_control(void)
{
  if(Rx1_update_flag)
	{
		if(Rx_APP_data[0]=='n')  //收到n则表示停止指示
		{
			start_stop=0;   //设置成stop状态
			_actuator.linear_gravity = 0;
			_actuator.angular_gravity = 0;
			Rx1_update_flag=0;
		}
	}
}

void rotate_control()
{
//    if(rotate_flag)
//        _actuator.angular_rotate = 2.0f;
//    else
//        _actuator.angular_rotate = 0;
//    Rx1_update_flag=0;
}

uint16_t voice_angular;
uint16_t angg;
uint16_t meter;
float initial_angle;
void voice_receive(void)
{
	int p=0;
	voice_angular=0;
	if(Rx1_update_flag)
	{
		if(Rx_APP_data[p]=='d')
		{
			p++;
			while(Rx_APP_data[p]>='0' && Rx_APP_data[p]<='9')
			{
				voice_angular*=10;
				voice_angular+=Rx_APP_data[p]-'0';
				p++;
			}
			switch(Rx_APP_data[p++])
			{
				case 'l':__pid.euler_sp -= voice_angular * PI/180;Rx1_update_flag=0;break;
				case 'r':__pid.euler_sp += voice_angular * PI/180;Rx1_update_flag=0;break;
				default:Rx1_update_flag=0;
			}
		}
		if(Rx_APP_data[p]=='m')
		{
			p++;
			while(Rx_APP_data[p]>='0' && Rx_APP_data[p]<='9')
			{
				meter = Rx_APP_data[p]-'0';
				p++;
			}
			switch(Rx_APP_data[p])
			{
				case 'f':Rx1_update_flag=0;break;
				case 'b':Rx1_update_flag=0;break;
				default:Rx1_update_flag=0;
			}
		}
	}
}
void reset_actuator(void)
{
	_actuator.linear_test = 0;
	_actuator.linear_joystick = 0;
	_actuator.angular_joystick = 0;
	_actuator.linear_test = 0;
	_actuator.linear_auto_x = 0;
	_actuator.linear_gravity = 0;
	_actuator.linear_voice = 0;
	_actuator.angular_auto_z = 0;
	_actuator.angular_euler = 0;
	_actuator.angular_gravity = 0;
    _actuator.angular_rotate = 0;
    rotate_flag = 0;
}

void Emergency_stop(void)
{
	if(Rx_APP_data[0] == 0x30)//急停
	{
		_actuator.linear_test = 0;
		_actuator.linear_joystick = 0;
		_actuator.angular_joystick = 0;
		_actuator.linear_test = 0;
		_actuator.linear_auto_x = 0;
		_actuator.linear_gravity = 0;
		_actuator.linear_voice = 0;
		_actuator.angular_auto_z = 0;
		_actuator.angular_euler = 0;
		_actuator.angular_gravity = 0;
        _actuator.angular_rotate = 0;
        rotate_flag = 0;
		Rx1_update_flag=0;
        meter = 0;
	}
}
extern uint32_t schedule_tick;

void AndroidTask(void)
{
	android_tick++;

	if(mode_choose != 4 && pre_mode == 4)//切换手动控制模式时加速度
	{
			SetAcceleration(0x01,up,100);
			SetAcceleration(0x02,up,100);
			SetAcceleration(0x01,down,700);
			SetAcceleration(0x02,down,700);
		if(schedule_tick %16 == 0)
				SetAcceleration(0x01,up,100);
			else if(schedule_tick %16 == 4)
				SetAcceleration(0x02,up,100);
			else if(schedule_tick %16 == 8)
				SetAcceleration(0x01,down,700);
			else if(schedule_tick %16 == 12)
				SetAcceleration(0x02,down,700);
	}

	if(mode_choose == 4 && pre_mode != 4)//切换自动导航模式时加速度
	{
			if(schedule_tick %16 == 0)
				SetAcceleration(0x01,up,200);
			else if(schedule_tick %16 == 4)
				SetAcceleration(0x02,up,200);
			else if(schedule_tick %16 == 8)
				SetAcceleration(0x01,down,30);
			else if(schedule_tick %16 == 12)
				SetAcceleration(0x02,down,30);
	}
	if(mode_choose == 2 && pre_mode != 2)//切换重力感应模式时加速度
	{
			SetAcceleration(0x01,up,200);
			SetAcceleration(0x02,up,200);
			SetAcceleration(0x01,down,200);
			SetAcceleration(0x02,down,200);
	}

	if(mode_choose==1)//按键控制
	{
		if(pre_mode != 1)
		{
			// __pid.euler_sp = _attitude.euler[2];//防止模式切换突转
			
			//切换模式急停
			_actuator.linear_test = 0;
			_actuator.linear_joystick = 0;
			_actuator.angular_joystick = 0;
			_actuator.linear_test = 0;
			_actuator.linear_auto_x = 0;
			_actuator.linear_gravity = 0;
			_actuator.linear_voice = 0;
			_actuator.angular_auto_z = 0;
			_actuator.angular_euler = 0;
			_actuator.angular_gravity = 0;
            _actuator.angular_rotate = 0;
            rotate_flag = 0;
		}

		KeyControl();
		pre_mode = 1;
	}
	else if(mode_choose==2)//重力感应控制
	{
		if(pre_mode != 2)
		{
			// __pid.euler_sp = _attitude.euler[2];//防止模式切换突转

			//切换模式急停
			_actuator.linear_test = 0;
			_actuator.linear_joystick = 0;
			_actuator.angular_joystick = 0;
			_actuator.linear_test = 0;
			_actuator.linear_auto_x = 0;
			_actuator.linear_gravity = 0;
			_actuator.linear_voice = 0;
			_actuator.angular_auto_z = 0;
			_actuator.angular_euler = 0;
			_actuator.angular_gravity = 0;
            _actuator.angular_rotate = 0;
            rotate_flag = 0;
		}


		gravity_control();

		pre_mode = 2;
	}
	else if(mode_choose==3)//语音控制
	{
		if(pre_mode != 3)
		{
			// __pid.euler_sp = _attitude.euler[2];//防止模式切换突转

			//切换模式急停
			_actuator.linear_test = 0;
			_actuator.linear_joystick = 0;
			_actuator.angular_joystick = 0;
			_actuator.linear_test = 0;
			_actuator.linear_auto_x = 0;
			_actuator.linear_gravity = 0;
			_actuator.linear_voice = 0;
			_actuator.angular_auto_z = 0;
			_actuator.angular_euler = 0;
			_actuator.angular_gravity = 0;
            _actuator.angular_rotate = 0;
            meter = 0;
            initial_angle = _attitude.euler[2];
            rotate_flag = 0;
		}
		pre_mode = 3;
		voice_receive();
	}
	else if(mode_choose == 4)//自动导航
	{
		if(pre_mode != 4)
		{
			// __pid.euler_sp = _attitude.euler[2];//防止模式切换突转

			//切换模式急停
			_actuator.linear_test = 0;
			_actuator.linear_joystick = 0;
			_actuator.angular_joystick = 0;
			_actuator.linear_test = 0;
			_actuator.linear_auto_x = 0;
			_actuator.linear_gravity = 0;
			_actuator.linear_voice = 0;
			_actuator.angular_auto_z = 0;
			_actuator.angular_euler = 0;
			_actuator.angular_gravity = 0;
            _actuator.angular_rotate = 0;
            rotate_flag = 0;
		}
		
		pre_mode = 4;
	}
                  rotate_control();  
//    else if(mode_choose ==5)//自动旋转
//    {
//        if(pre_mode != 5)
//		{
//			__pid.euler_sp = _attitude.euler[2];//防止模式切换突转

//			//切换模式急停
//			_actuator.linear_test = 0;
//			_actuator.linear_joystick = 0;
//			_actuator.angular_joystick = 0;
//			_actuator.linear_test = 0;
//			_actuator.linear_auto_x = 0;
//			_actuator.linear_gravity = 0;
//			_actuator.linear_voice = 0;
//			_actuator.angular_auto_z = 0;
//			_actuator.angular_euler = 0;
//			_actuator.angular_gravity = 0;
//            _actuator.angular_rotate = 0;
//		}
//        pre_mode = 5;
//    }
}
#ifdef __cplusplus
}
#endif
