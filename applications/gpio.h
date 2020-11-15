#ifndef __GPIO_H
#define __GPIO_H

#define gpio1 52	//PA10
#define gpio2 54	//PA9
#define gpio3 56	//PA8

#include "../libraries/ls1c_public.h"
#include "../libraries/ls1c_gpio.h"
#include "../libraries/ls1c_delay.h"

void gpio1_init(void);
void gpio2_init(void);
void gpio3_init(void);
void gpio_set_level(unsigned int gpio,gpio_level_t level);

#endif