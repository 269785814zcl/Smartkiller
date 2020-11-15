#include "topics.h"
#include "control.h"
#include "rosconfig.h"
#include "modbus.h"
#include "RS485.h"
#include "pwm.h"
#include "gpio.h"
//#include "host.h"

#ifdef __cplusplus
extern "C" {
#endif


void disinfector_control()
{
	if(disinfect==1){
		pwm2_init(0.99,1000*1000*1000/87);
		pwm2_enable();
	}
	if(disinfect==0){
		pwm2_init(0,1000*1000*1000/87);
		pwm2_enable();
	}
}

void lamp_control()
{
	if(lamp==1){
		gpio_set_level(gpio1,gpio_level_low);
		gpio_set_level(gpio2,gpio_level_low);
		gpio_set_level(gpio3,gpio_level_high);
	}   //黄灯
	else if(lamp==0){
		gpio_set_level(gpio1,gpio_level_low);
		gpio_set_level(gpio2,gpio_level_high);
		gpio_set_level(gpio3,gpio_level_low);
	}	//红灯
	else if(lamp==2){
		gpio_set_level(gpio1,gpio_level_high);
		gpio_set_level(gpio2,gpio_level_low);
		gpio_set_level(gpio3,gpio_level_low); 
	} 	//绿灯
}

uint8_t Err_Limit_Flag = 0;//偏差限幅度标志位
uint8_t Integrate_Separation_Flag = 0;//积分分离标志位
uint8_t Integrate_Limit_Flag = 0;//积分限幅标志位


    float _vel_1 = 0; 
    float _vel_2 = 0;
	float pre_vel_1 = 0; 
	float d=0.156;
	float R;
	float C=0.43;

float getVel_1(float vel,int turn){
	if(turn==up){
		return -fabs(vel);
	}else if(turn==down){
		return fabs(vel);
	}
}

float getVel_2(float vel,int turn){
	if(turn==up){
		return fabs(vel);
	}else if(turn==down){
		return -fabs(vel);
	}
}
		
void rs485_write_single(uint8_t id, float linear_vel_x, float angular_vel_z)
{
	if(linear_vel_x!=0 && angular_vel_z==0){
		if(linear_vel_x>0){
			_vel_1 = getVel_1(linear_vel_x,up);
			_vel_2 = getVel_2(linear_vel_x,up);
		}else if(linear_vel_x<0){
			_vel_1 = getVel_1(linear_vel_x,down);
			_vel_2 = getVel_2(linear_vel_x,down);
		}
	}else if(linear_vel_x==0 && angular_vel_z!=0){
		if(angular_vel_z>0){
			_vel_1 = getVel_1(angular_vel_z*d,down);
			_vel_2 = getVel_2(angular_vel_z*d,up);;
		}else if(angular_vel_z<0){
			_vel_1 = getVel_1(angular_vel_z*d,up);
			_vel_2 = getVel_2(angular_vel_z*d,down);;
		}
	}else if(linear_vel_x==0 && angular_vel_z==0){
		_vel_1 = 0;
		_vel_2 = 0;
	}else{
		R=fabs(linear_vel_x)/fabs(angular_vel_z);
		if(R>=d){
			if(angular_vel_z>0){
				_vel_1=getVel_1((R-d)*angular_vel_z,up);
				_vel_2=getVel_2((R+d)*angular_vel_z,up);
			}else{
				_vel_1=getVel_1((R+d)*angular_vel_z,up);
				_vel_2=getVel_2((R-d)*angular_vel_z,up);
			}
		}else{
			if(angular_vel_z>0){
				_vel_1=getVel_1((d-R)*angular_vel_z,down);
				_vel_2=getVel_2((d+R)*angular_vel_z,up);
			}else{
				_vel_1=getVel_1((d+R)*angular_vel_z,up);
				_vel_2=getVel_2((d-R)*angular_vel_z,down);
			}
		}
	}
	if(id == 1) {
		SetRPM(0x01,_vel_1,C);
	}
	else if(id == 2) {
		SetRPM(0x02,_vel_2,C);
	}
}

 void angle_control(void)
{
// /************************偏差计算*********************/
	// _pid.err_prev = _pid.err_now;//保存上次偏差
	// _pid.err_now = _pid.euler_sp - _attitude.euler[2];//期望减去反馈得到偏差
// /***********************偏航角偏差超过+-180处理*****************************/
	// if(_pid.err_now < -PI)  _pid.err_now = _pid.err_now + PI*2;
	// if(_pid.err_now > PI)  _pid.err_now = _pid.err_now - PI*2;

	// if(Err_Limit_Flag==1)//偏差限幅度标志位
	// {
		// if(_pid.err_now >=  Err_Max)   _pid.err_now =  Err_Max;
		// if(_pid.err_now <= -Err_Max)   _pid.err_now = -Err_Max;
	// }

	// /*******积分计算*********************/
	// if(Integrate_Separation_Flag==1)//积分分离标志位
	// {
		// if(ABS(_pid.err_now) <= Integrate_Separation_Err)
		  // _pid.euler_int += _params.att_i * _pid.err_now;
	// }
	// else
	// {
	// _pid.euler_int += _params.att_i * _pid.err_now;
	// }
	// /*******积分限幅*********************/
	// if(Integrate_Limit_Flag==1)//积分限制幅度标志
	// {
		// if(_pid.euler_int >= Integrate_Max)
		  // _pid.euler_int = Integrate_Max;
		// if(_pid.euler_int <= -Integrate_Max)
		  // _pid.euler_int = -Integrate_Max ;
	// }
	// /*******总输出计算*********************/

	// _pid.output_angle =   _params.att_p * _pid.err_now//比例
	                 //// + _pid.euler_int//积分
	                 // + _params.att_d * (_pid.err_now - _pid.err_prev);//微分

	// ******总输出限幅*********************/
	// if(_pid.output_angle >=  Control_OutPut_Limit)
		// _pid.output_angle =  Control_OutPut_Limit;
	// if(_pid.output_angle <= -Control_OutPut_Limit)
		// _pid.output_angle = -Control_OutPut_Limit;
	
	// _actuator.angular_euler = _pid.output_angle;
}

float _value;
float test_speed = 0;
void speed_test(void)
{
	if(schedule_tick %4 == 0)//20ms
	{
		test_speed += _value;
		if(test_speed >= 120)
			_value = -2.7f;
		else if(test_speed <= 0)
			_value = 2.7f;
	}
	_actuator.linear_test = test_speed;
}

extern float required_angular_vel;
extern float required_linear_vel;
extern uint16_t rec_time;
extern uint16_t pre_rec_time;
float auto_rpm_x,auto_rpm_z;
uint8_t send_actuator[6];
float speed_ex = 2.0f;
void auto_control(void)
{
    //calculate for the target motor RPM and direction
    //front-left motor
//    auto_rpm_x = required_linear_vel * 40.0f * speed_ex;
//	if(auto_rpm_x >= 40 * speed_ex)
//		auto_rpm_x = 40 * speed_ex;
//    //front-right motor
//    auto_rpm_z =  required_angular_vel * 1.0f	* 2.4f * speed_ex;
//    auto_rpm_z = - required_angular_vel * 20.0f * 1.4f;
	_actuator.linear_auto_x = required_linear_vel;
	_actuator.angular_auto_z = required_angular_vel;
}
uint32_t voice_tick;
extern uint8_t mode_choose;
extern uint16_t meter;
float meter_rate=1.0f;
extern float initial_angle;
float err_ang;
int i_voice;
int voice_choose_flag = 1;
uint8_t voice_flag;
extern uint16_t usart2_tick;
uint16_t pre_usart2_tick;
void voice_control(void)
{
    
/*     meter_rate = meter == 1 ? 1.15 : 1; 
		if(i_voice == 0 && voice_flag == 1)//开始旋转
		{
			i_voice++;
		}
		if(ABS(_pid.err_now) <= 5*PI/180 && i_voice == 1)//旋转停止
		{
			i_voice++;
		}
		if(i_voice == 2)
		{
			voice_tick++;//开始计数
			if(voice_tick >= 120)//开始前进
			{
				_actuator.linear_voice = 18;
			}
		}
		if(voice_tick >= 450 * meter * meter_rate)//停止前进
		{
			_actuator.linear_voice = 0;
			if(voice_tick >= 520 * meter * meter_rate)
			{
				voice_tick = 0;
				i_voice = 0;
				meter = 0;
				initial_angle = _attitude.euler[2];//记录初始角度
				voice_flag = 0;
				pre_usart2_tick = usart2_tick;
			}
		} */
}

void control_task(void)
{
	auto_control();

/* 	if(mode_choose == 3)
	{
		if(usart2_tick != pre_usart2_tick)
			voice_flag = 1;

		angle_control();
		voice_control();
	} */
}

#ifdef __cplusplus
}
#endif
