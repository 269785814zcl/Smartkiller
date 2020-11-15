#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
	
typedef struct RxStruct
{
uint16_t index;
uint8_t *buffer_ptr;
uint16_t buffer_size;
}RxStruct_t;

typedef struct {
    uint64_t timestamp;
    float linear_test;
	float linear_joystick;
	float linear_gravity;
	float linear_auto_x;
	float linear_voice;
	float angular_auto_z;
	float angular_euler;
	float angular_joystick;
	float angular_gravity;
	float angular_voice;
    float angular_rotate;
	
} actuator_t;

extern actuator_t _actuator;
extern RxStruct_t RxStruct_data;
extern void RS485_Init(void);
extern void RS485_Send_Data(uint8_t *buf,uint8_t len);
extern void RS485_Send_Data_2(uint8_t *buf,uint8_t len);
extern void UART5_init(void);
//extern void RxStruct_init(RxStruct_t* RxStruct, uint8_t *pool, uint16_t size);


#ifdef __cplusplus
}
#endif