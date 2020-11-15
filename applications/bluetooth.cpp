#include "pwm.h"
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

#include "bluetooth.h"
#include "modbus.h"
#include "topics.h"

extern uint16_t usart2_tick;
extern uint16_t pre_usart2_tick;
extern uint8_t Rx_APP_data[100];
extern uint8_t Rx1_update_flag;
uint8_t Rx_app_cnt;
extern uint8_t mode_choose ;

float pause_flag = 1.0f;
float disinfector_duty = 0;
//uint8_t Rx_mode_data[9];
uint8_t i=0;
extern int rotate_flag;
extern int gear;
void mode_election(void)
{
  if(Rx1_update_flag)
	{
		switch(Rx_APP_data[0])
		{
			case 'v':
				mode_choose=3;
				pre_usart2_tick = usart2_tick;
				Rx1_update_flag=0;
				break;
			case 'g':
				mode_choose=2;
				Rx1_update_flag=0;
				break;
			case 'k':
				mode_choose=1;
				Rx1_update_flag=0;
				break;
			case 'o':
				mode_choose=4;
				Rx1_update_flag=0;
				break;
			case 'r':
				mode_choose=4;
				Rx1_update_flag=0;
				break;
			case 'a':
				gear = 5;
				Rx1_update_flag=0;
				break;
			case 'b':
				gear = 10;
				Rx1_update_flag=0;
				break;
			case 'c':
				gear = 20;
				Rx1_update_flag=0;
				break;
			case 'd':
				gear = 35;
				Rx1_update_flag=0;
				break;
			case 'e':
				gear = 50;
				Rx1_update_flag=0;
				break;
            case 'z':
                rotate_flag = rotate_flag == 0 ? 1 : 0;
                Rx1_update_flag=0;
            case 'p':
                pause_flag = pause_flag == 0 ? 1 : 0;
                Rx1_update_flag=0;
            case 'f':
            	disinfector_duty = disinfector_duty == 0 ? 0.99 : 0;
                pwm2_init(disinfector_duty,1000*1000*1000/87);
				pwm2_enable();
			default:

				break;
		}
//        if(Rx_mode_data[0] != 'a')
//        {
//            i=8;
//            Rx_mode_data[0]=0;
//        }
//        if(Rx_APP_data[0] != Rx_mode_data[i])
//        {
//            i = i==8 ? 0 : i+1;
//            Rx_mode_data[i] = Rx_APP_data[0];
//        }
	}
}
void bluetooth_handle(void) 
{

	Rx_APP_data[Rx_app_cnt++] = bluetooth_rx();
	rt_kprintf("%d\n",Rx_APP_data[Rx_app_cnt]);
	if(Rx_APP_data[Rx_app_cnt-1]=='.')		// 结束位
	{
		Rx_APP_data[Rx_app_cnt-1] = '\0';	// 把'\r'换成'\0'
		Rx_app_cnt = 0;
		Rx1_update_flag = 1;		// 更新数据
		mode_election();
	}
	
}



#ifdef __cplusplus
}
#endif
