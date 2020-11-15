#ifdef __cplusplus
 extern "C" {
#endif
#ifndef __USART_H__
#define __USART_H__

#include "rosconfig.h"
#include "RingBuffer.h"

class HardwareSerial {
public:
	HardwareSerial(Serial_TypeDef _Serial=SERIAL1);
	~HardwareSerial(){};
	void begin(uint32_t baud);

	uint32_t available(void);
	uint8_t read(void);
	void flush(void);
	uint32_t write(uint8_t ch);
	void print(const char *format, ...);
	void putstr(const char *str);
	void irq();
	
protected:
	RingBuffer rx_buffer;
	Serial_TypeDef Serial;
};

extern void serial1_irq(void);

extern uint8_t HardwareSerial_rx(void);
extern void HardwareSerial_tx(uint8_t ch);

#endif //__USART_H__
#ifdef __cplusplus
}
#endif


