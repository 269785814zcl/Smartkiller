
#include "interrupt.h"

#ifdef  USE_SERIAL1
HardwareSerial *Serial1=0 ;
#endif  

#ifdef  USE_SERIAL2
HardwareSerial *Serial2=0 ;
#endif 

#ifdef  USE_SERIAL3
HardwareSerial *Serial3=0 ;
#endif 

//void UART1_IRQHandler(void) 
//{
//	Serial1->irq();
//}

//void USART2_IRQHandler(void) 
//{
//#ifdef  USE_SERIAL2
//		Serial2->irq();
//#endif
//}

//void USART3_IRQHandler(void) 
//{
//#ifdef  USE_SERIAL3
//		Serial3->irq();
//#endif
//}
