#ifndef _INTERRUPT_H_
#define	_INTERRUPT_H_


#ifdef __cplusplus
extern "C"{
#endif
#include "rosconfig.h"
#include "hardwareserial.h"

#ifdef  USE_SERIAL1
extern HardwareSerial *Serial1;
#endif  

#ifdef  USE_SERIAL2
extern HardwareSerial *Serial2;
#endif 

#ifdef  USE_SERIAL3
extern HardwareSerial *Serial3;
#endif


//void UART1_IRQHandler(void);
//void UART2_IRQHandler(void);
//void UART3_IRQHandler(void);	
	
#ifdef __cplusplus
}
#endif

#endif
