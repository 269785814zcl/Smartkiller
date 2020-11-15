#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <rtthread.h>
	
extern void mode_election(void);
extern uint8_t bluetooth_rx(void);
extern void bluetooth_handle(void);

#ifdef __cplusplus
}
#endif