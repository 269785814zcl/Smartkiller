#pragma once

#ifdef __cplusplus

extern "C" {
#endif

#include <stdint.h>
#define HI(n) ((n)>>8)
#define LOW(n) ((n)&0xff)
	

typedef struct {

	uint8_t addr;
	uint8_t command;
	uint8_t first_register[2];
	uint16_t value;
	uint8_t crc[2];
	uint8_t len;
	uint8_t err;

} tx_data_frame_t;

typedef struct {

	uint8_t index;
	uint8_t addr;
	uint8_t command;
	uint8_t total_bytes;
	uint16_t rpm;
    int32_t pos;
	uint8_t crc[2];
	uint8_t len;
	uint8_t err;

} rx_data_frame_t;

typedef struct {
	uint8_t addr;
	uint8_t command;
	uint8_t crc[2];
	uint8_t err; //0:����
			//0x01:�Ƿ�������
			//0x02:�Ƿ����ݵ�ַ
			//0x03:�Ƿ�����ֵ
			//0x04:��վ�豸����
			//0x05:�����ѱ�ȷ�ϣ�����Ҫ�ϳ�ʱ������������
			//0x06:���豸æ
			//0x08:�洢��ż�Բ��
			//0x0A:�����õ�����
			//0x0B:����Ŀ���豸��Ӧʧ��
			//0x40:��ֹ����
			//0x60:��δѧϰ�������
			//0xFF:δ�������
} err_frame_t;

typedef struct {
	uint16_t plugging;
	uint16_t pwm;
	float current;
	float speed;
	float voltage1;
	float voltage2;
	float voltage3;
}realtime_status_t;

typedef enum {
	normal = 0,
	emergency = 1,
	_free = 2
}stop_t;

typedef enum {
	up = 0,
	down = 1,
}acceleration_t;

extern rx_data_frame_t Rx_data_frame1;
extern rx_data_frame_t Rx_data_frame2;
extern err_frame_t _err_frame;
extern realtime_status_t _realtime_status;
extern stop_t _stop;
extern acceleration_t _acceleration;
extern void modbus_rtu(void);
extern void SendCommand(uint8_t addr,uint8_t command, uint16_t srart_register, int16_t value);
extern void SendCommand_2(uint8_t addr,uint8_t command, uint16_t srart_register, int16_t value);
void SetAcceleration(uint8_t addr, acceleration_t mode, float acc);
extern void ModbusRtuReceive();
extern void Stop(uint8_t addr, stop_t mode);
extern void SetRPM(uint8_t addr, float v,float C);
extern void SetRPM0();
extern void SetRPM00();
extern void SetRPM100(); 
extern unsigned short CRC16_Modbus ( unsigned char *pdata, int len);

#ifdef __cplusplus
}
#endif