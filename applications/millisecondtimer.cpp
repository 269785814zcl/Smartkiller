#ifdef __cplusplus
extern "C" {
#endif

#include <rtthread.h>
#include "millisecondtimer.h"
#define TickClock   72     // MHz
#define TickPeriod  50000   // us
volatile uint32_t _counter;

void initialise(void) 
{
	_counter = 0;
//	SysTick_Config(TickClock * TickPeriod);
	// SysTick_Config(96000000 /1000);

	// NVIC_SetPriority (SysTick_IRQn,0);
}

/* uint64_t time()
{
    uint64_t tick = _counter;
    uint64_t val = SysTick->VAL;

    if (tick != _counter) {
        tick = _counter;
        val = SysTick->VAL;
    }

    return (tick + 1) * TickPeriod - val / TickClock;
} */

 
void delay(uint32_t millis) 
{
	uint32_t target;

	target = rt_tick_get() + millis;
	while(rt_tick_get() < target);
} 

/* void SysTick_Handler(void) 
{
	_counter++;
} */

uint32_t millis(void) 
{
	return rt_tick_get();
}

// void usleep(uint32_t us)
// {
    // uint64_t timestamp = time();

    // while (time() < timestamp + us);
// }

void reset(void) 
{
	rt_tick_set	(0);

}

#ifdef __cplusplus
}
#endif

