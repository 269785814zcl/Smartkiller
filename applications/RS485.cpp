#include <rtthread.h>
#include "RS485.h"
// #include "bluetooth.h"
//#include "timer.h"
#include "modbus.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t RS485_TX_CNT;//发送的数据长度
extern uint8_t RS485_RX_CNT;//接收到的数据长度
extern uint8_t RS485_RX_BUFF[64];
extern uint8_t RS485_RxFlag;
extern rx_data_frame_t Rx_data_frame1;
extern rx_data_frame_t Rx_data_frame2;
uint8_t Rx_APP_data[100];
uint8_t err;
uint8_t addr = 0;
uint8_t addr_2 = 0;
uint8_t _rpm[2] = { 0 };
uint8_t _pos[4] = { 0 };
uint8_t _crc[2] = { 0 };
uint8_t _rpm_2[2] = { 0 };
uint8_t _pos_2[4] = { 0 };
uint8_t _crc_2[2] = { 0 };
RxStruct_t RxStruct_data = { 0 };
RxStruct_t RxStruct_data_2 = { 0 };
actuator_t _actuator = { 0 };

void RxStruct_init(RxStruct_t* RxStruct, uint8_t *pool, uint16_t size)
{
/* 初始化头 */
RxStruct->index = 0;
/* 初始化缓冲区 */
RxStruct->buffer_ptr = pool;
RxStruct->buffer_size = size;
}

//RS485发送len个字节
//buf:发送区首地址
//len:发送的字节数
/* void RS485_Send_Data(uint8_t *buf,uint8_t len)
{
	uint8_t t;
	
	rt_device_write	(,0,buf,len 

	
  	// for(t=0;t<len;t++)
	// {
	    // while(UART_GetFlagStatus(UART2,UART_IT_TXIEN)==RESET){};	
		// UART_SendData(UART2,buf[t]); //发送数据
	// }
	// while(UART_GetFlagStatus(UART2,UART_IT_TXIEN)==RESET); //等待发送结束		
	// RS485_RX_CNT=0;
} */

void RS485_Send_Data_2(uint8_t *buf,uint8_t len)
{
	uint8_t t;
/*   	for(t=0;t<len;t++)
	{
	    while(UART_GetFlagStatus(UART2,UART_IT_TXIEN)==RESET){};	
		UART_SendData(UART2,buf[t]); //发送数据
	}
	while(UART_GetFlagStatus(UART2,UART_IT_TXIEN)==RESET); //等待发送结束		
	RS485_RX_CNT=0; */
}


//void RS485_Init(void)
//{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	UART_InitTypeDef UART_InitStructure;
//	NVIC_InitTypeDef  NVIC_InitStructure;

//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART2, ENABLE); 
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//	//初始化使能485芯片管脚
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
//	GPIO_InitStructure.GPIO_Pin    =  GPIO_Pin_10;
//	GPIO_InitStructure.GPIO_Speed  =  GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode   =  GPIO_Mode_AF_PP;
//	GPIO_Init(GPIOC,&GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;           
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
//	GPIO_Init(GPIOC, &GPIO_InitStructure); 

//	UART_InitStructure.UART_BaudRate	 = 9600;				
//	UART_InitStructure.UART_WordLength = UART_WordLength_8b;	
//	UART_InitStructure.UART_StopBits 	 = UART_StopBits_1;	
//	UART_InitStructure.UART_Parity	 = UART_Parity_No;		
//	UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
//	UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;

//	UART_Init(UART2, &UART_InitStructure); 				

//	NVIC_InitStructure.NVIC_IRQChannel = UART2_IRQn;       
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0; 
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
//	NVIC_Init(&NVIC_InitStructure);						   
//	UART_ITConfig(UART2, UART_IT_RXNE, ENABLE);

//	UART_Cmd(UART2, ENABLE);

//}

uint32_t usart3_tick = 0;



//void UART3_IRQHandler(void)
//{
//	uint8_t res;
//	usart3_tick++;
//	if(UART_GetITStatus(UART3, UART_IT_RXIEN) != RESET)//接收到数据
//	{
////		if(UART_GetFlagStatus(UART3,UART_FLAG_NE|UART_FLAG_FE|UART_FLAG_PE)) {err=1;}//检测到噪音、帧错误或校验错误
////            else err=0;
//		if(err == 0)
//		{
//			res =UART_ReceiveData(UART3);//;读取接收到的数据
//			switch(RxStruct_data.index)
//			{
//			case 0://地址
//				if((res == 0x01 || res == 0x02))
//				{
//					addr = res;
//					RxStruct_data.index++;
//				}
//				break;
//			case 1://读
//				if(res == 0x03 )
//					RxStruct_data.index++;
//				else
//				{
//					RxStruct_data.index = 0;
//				}

//				break;
//			case 2://数目
//				if(res == 0x02 ) {
//					RxStruct_data.index++;
//					if(addr == 0x01)
//						Rx_data_frame1.total_bytes = res;
//					else if(addr == 0x02)
//						Rx_data_frame2.total_bytes = res;
//						
//				}
//				else
//				{
//					RxStruct_data.index = 0;
//					
//				}
//				
//				break;
//			case 3:
//					if(addr == 0x01)
//						Rx_data_frame1.addr = addr;
//					else if(addr == 0x02)
//						Rx_data_frame2.addr = addr;
//					if(addr == 0x01)
//						Rx_data_frame1.command = 0x03;
//					else if(addr == 0x02)
//						Rx_data_frame2.command = 0x03;
//					
//					_pos[0] = res;
//					RxStruct_data.index++;
//				break;
//            case 4:
//					if(addr == 0x01)
//						Rx_data_frame1.addr = addr;
//					else if(addr == 0x02)
//						Rx_data_frame2.addr = addr;
//					if(addr == 0x01)
//						Rx_data_frame1.command = 0x03;
//					else if(addr == 0x02)
//						Rx_data_frame2.command = 0x03;
//					
//					_pos[1] = res;
//					RxStruct_data.index++;
//				break;
//            case 5:
//					if(addr == 0x01)
//						Rx_data_frame1.addr = addr;
//					else if(addr == 0x02)
//						Rx_data_frame2.addr = addr;
//					if(addr == 0x01)
//						Rx_data_frame1.command = 0x03;
//					else if(addr == 0x02)
//						Rx_data_frame2.command = 0x03;
//					
//					_pos[2] = res;
//					RxStruct_data.index++;
//				break;
//			case 6://值
//					_pos[3] = res;
//					if(addr == 0x01)
//						Rx_data_frame1.pos = (s32)((uint32_t)_pos[3] | (((uint32_t)_pos[2]) << 8)| (((uint32_t)_pos[1]) << 16)| (((uint32_t)_pos[0]) << 24));
//					else if(addr == 0x02)
//						Rx_data_frame2.pos = (s32)((uint32_t)_pos[3] | (((uint32_t)_pos[2]) << 8)| (((uint32_t)_pos[1]) << 16)| (((uint32_t)_pos[0]) << 24));
//					RxStruct_data.index++;
//				break;
//			case 7:
//					_crc[0] = res;
//					RxStruct_data.index++;
//				break;
//			case 8:
//					_crc[1] = res;
//					RxStruct_data.index = 0;
//				break;
//			default:break;
//			}

//		}
//		UART_ClearITPendingBit(UART3, UART_IT_RXIEN);
//	}
//}
uint32_t usart4_tick = 0;
//void UART2_IRQHandler(void)
//{
//	uint8_t res;
//	usart4_tick++;
//	if(UART_GetITStatus(UART2, UART_IT_RXNE) != RESET)//接收到数据
//	{
////		if(UART_GetFlagStatus(UART2,UART_FLAG_NE|UART_FLAG_FE|UART_FLAG_PE)) {err=1;}//检测到噪音、帧错误或校验错误
////            else err=0;
//		if(err == 0)
//		{
//			res =UART_ReceiveData(UART2);//;读取接收到的数据
//			switch(RxStruct_data.index)
//			{
//			case 0://地址
//				if((res == 0x01 || res == 0x02))
//				{
//					addr = res;
//					RxStruct_data.index++;
//				}
//				break;
//			case 1://读
//				if(res == 0x03 )
//					RxStruct_data.index++;
//				else
//				{
//					RxStruct_data.index = 0;
//				}

//				break;
//			case 2://数目
//				if(res == 0x04 ) {
//					RxStruct_data.index++;
//					if(addr == 0x01)
//						Rx_data_frame1.total_bytes = res;
//					else if(addr == 0x02)
//						Rx_data_frame2.total_bytes = res;
//						
//				}
//				else
//				{
//					RxStruct_data.index = 0;
//					
//				}
//				
//				break;
//			case 3:
//					if(addr == 0x01)
//						Rx_data_frame1.addr = addr;
//					else if(addr == 0x02)
//						Rx_data_frame2.addr = addr;
//					if(addr == 0x01)
//						Rx_data_frame1.command = 0x03;
//					else if(addr == 0x02)
//						Rx_data_frame2.command = 0x03;
//					
//					_pos[0] = res;
//					RxStruct_data.index++;
//				break;
//            case 4:
//					if(addr == 0x01)
//						Rx_data_frame1.addr = addr;
//					else if(addr == 0x02)
//						Rx_data_frame2.addr = addr;
//					if(addr == 0x01)
//						Rx_data_frame1.command = 0x03;
//					else if(addr == 0x02)
//						Rx_data_frame2.command = 0x03;
//					
//					_pos[1] = res;
//					RxStruct_data.index++;
//				break;
//            case 5:
//					if(addr == 0x01)
//						Rx_data_frame1.addr = addr;
//					else if(addr == 0x02)
//						Rx_data_frame2.addr = addr;
//					if(addr == 0x01)
//						Rx_data_frame1.command = 0x03;
//					else if(addr == 0x02)
//						Rx_data_frame2.command = 0x03;
//					
//					_pos[2] = res;
//					RxStruct_data.index++;
//				break;
//			case 6://值
//					_pos[3] = res;
//					if(addr == 0x01)
//						Rx_data_frame1.pos = (s32)((uint32_t)_pos[3] | (((uint32_t)_pos[2]) << 8)| (((uint32_t)_pos[1]) << 16)| (((uint32_t)_pos[0]) << 24));
//					else if(addr == 0x02)
//						Rx_data_frame2.pos = (s32)((uint32_t)_pos[3] | (((uint32_t)_pos[2]) << 8)| (((uint32_t)_pos[1]) << 16)| (((uint32_t)_pos[0]) << 24));
//					RxStruct_data.index++;
//				break;
//			case 7:
//					_crc[0] = res;
//					RxStruct_data.index++;
//				break;
//			case 8:
//					_crc[1] = res;
//					RxStruct_data.index = 0;
//				break;
//			default:break;
//			}

//		}
//		UART_ClearITPendingBit(UART2, UART_IT_RXNE);
//	}
//}
uint32_t uart4_tick = 0;
uint8_t ressss;
//void UART2_IRQHandler(void)
//{
//	uint8_t res;
//	uart4_tick++;
//	if(UART_GetITStatus(UART2, UART_IT_RXIEN) != RESET)//接收到数据
//	{
////		if(UART_GetFlagStatus(UART2,UART_FLAG_NE|UART_FLAG_FE|UART_FLAG_PE)) {err=1;}//检测到噪音、帧错误或校验错误
////            else err=0;
//		if(err == 0)
//		{
//			res =UART_ReceiveData(UART2);//;读取接收到的数据
//			ressss = res;
//			switch(RxStruct_data_2.index)
//			{
//			case 0://地址
//				if((res == 0x01 || res == 0x02))
//				{
//					addr_2 = res;
//					RxStruct_data_2.index++;
//				}
//				break;
//			case 1://读
//				if(res == 0x03 )
//					RxStruct_data_2.index++;
//				else
//				{
//					RxStruct_data_2.index = 0;
//				}

//				break;
//			case 2://数目
//				if(res == 0x02 ) {
//					RxStruct_data_2.index++;
//					if(addr_2 == 0x01)
//						Rx_data_frame1.total_bytes = res;
//					else if(addr_2 == 0x02)
//						Rx_data_frame2.total_bytes = res;
//						
//				}
//				else
//				{
//					RxStruct_data_2.index = 0;
//					
//				}
//				
//				break;
//            case 3:
//					if(addr_2 == 0x01)
//						Rx_data_frame1.addr = addr_2;
//					else if(addr_2 == 0x02)
//						Rx_data_frame2.addr = addr_2;
//					if(addr_2 == 0x01)
//						Rx_data_frame1.command = 0x03;
//					else if(addr_2 == 0x02)
//						Rx_data_frame2.command = 0x03;
//					
//					_pos_2[0] = res;
//					RxStruct_data_2.index++;
//				break;
//            case 4:
//					if(addr_2 == 0x01)
//						Rx_data_frame1.addr = addr_2;
//					else if(addr_2 == 0x02)
//						Rx_data_frame2.addr = addr_2;
//					if(addr_2 == 0x01)
//						Rx_data_frame1.command = 0x03;
//					else if(addr_2 == 0x02)
//						Rx_data_frame2.command = 0x03;
//					
//					_pos_2[1] = res;
//					RxStruct_data_2.index++;
//				break;
//            case 5:
//					if(addr_2 == 0x01)
//						Rx_data_frame1.addr = addr_2;
//					else if(addr_2 == 0x02)
//						Rx_data_frame2.addr = addr_2;
//					if(addr_2 == 0x01)
//						Rx_data_frame1.command = 0x03;
//					else if(addr_2 == 0x02)
//						Rx_data_frame2.command = 0x03;
//					
//					_pos_2[2] = res;
//					RxStruct_data_2.index++;
//				break;
//			case 6://值
//					_pos_2[3] = res;
//					if(addr_2 == 0x01)
//						Rx_data_frame1.pos = (s32)((uint32_t)_pos_2[3] | (((uint32_t)_pos_2[2]) << 8)| (((uint32_t)_pos_2[1]) << 16)| (((uint32_t)_pos_2[0]) << 24));
//					else if(addr_2 == 0x02)
//						Rx_data_frame2.pos = (s32)((uint32_t)_pos_2[3] | (((uint32_t)_pos_2[2]) << 8)| (((uint32_t)_pos_2[1]) << 16)| (((uint32_t)_pos_2[0]) << 24));
//					RxStruct_data_2.index++;
//				break;
//			case 7:
//					_crc_2[0] = res;
//					RxStruct_data_2.index++;
//				break;
//			case 8:
//					_crc_2[1] = res;
//					RxStruct_data_2.index = 0;
//				break;
//			default:break;
//			}

//		}
//		UART_ClearITPendingBit(UART2, UART_IT_RXIEN);
//	}
//}
 uint16_t usart2_tick;
extern uint16_t pre_usart2_tick;
extern uint8_t Rx1_update_flag;
extern uint8_t Rx_app_cnt;
extern uint8_t mode_choose ;
//void UART2_IRQHandler(void) 
//{
//	usart2_tick++;
//	if (UART_GetITStatus(UART3, UART_IT_RXIEN) != RESET) {
//		Rx_APP_data[Rx_app_cnt++] = UART_ReceiveData(UART3);
//	}
//	if(Rx_APP_data[Rx_app_cnt-1]=='.')		// 结束位
//	{
//		Rx_APP_data[Rx_app_cnt-1] = '\0';	// 把'\r'换成'\0'
//		Rx_app_cnt = 0;
//		Rx1_update_flag = 1;		// 更新数据
//		mode_election();
//	}
//	UART_ClearITPendingBit(UART3, UART_IT_RXIEN); 
//	
//}

#ifdef __cplusplus
}
#endif