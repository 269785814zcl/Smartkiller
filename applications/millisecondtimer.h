#ifdef __cplusplus
extern "C" {
#endif
#ifndef _MILLISECONDTIMER_H_
#define _MILLISECONDTIMER_H_
#include <stdint.h>

extern void initialise(void);
extern void delay(uint32_t millis_);
extern uint32_t millis(void);
extern void reset(void);

#endif
#ifdef __cplusplus
}
#endif



        