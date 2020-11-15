#ifdef __cplusplus
extern "C" {
#endif
#include <rtthread.h>
#include <stdlib.h>  
#include "../libraries/ls1c_public.h"
#include "../libraries/ls1c_gpio.h"
#include "../libraries/ls1c_delay.h"
#ifdef __cplusplus
}
#endif
#include "gpio.h"

void gpio1_init(void)
{
	gpio_init(gpio1, gpio_mode_output);
}
void gpio2_init(void)
{
	gpio_init(gpio2, gpio_mode_output);
}
void gpio3_init(void)
{
	gpio_init(gpio3, gpio_mode_output);
}
void gpio_set_level(unsigned int gpio,gpio_level_t level)
{
    gpio_set(gpio, level);
}