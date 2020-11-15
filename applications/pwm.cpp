#ifdef __cplusplus
extern "C" {
#endif
#include <rtthread.h>
#include <rthw.h>
#include <ls1c.h>
#include <stdlib.h> 

#include "../libraries/ls1c_public.h"
#include "../libraries/ls1c_gpio.h"
#include "../libraries/ls1c_delay.h"
#include "../libraries/ls1c_pwm.h"
#ifdef __cplusplus
}
#endif
#include "pwm.h"

pwm_info_t pwm2_info;

void pwm2_init(float duty,unsigned long period)
{


    pwm2_info.gpio = LS1C_PWM2_GPIO46;               // pwm引脚位gpio46
    pwm2_info.mode = PWM_MODE_NORMAL;                // 正常模式--连续输出pwm波形
    pwm2_info.duty = duty;                           // pwm占空比
    pwm2_info.period_ns = period;               // pwm周期5ms

    // pwm初始化，初始化后立即产生pwm波形
    pwm_init(&pwm2_info);

    return ;
}

void pwm2_enable(void)
{
	    // 使能pwm
    pwm_enable(&pwm2_info);
}

void pwm2_disable(void)
{
	     // 禁止pwm
    pwm_disable(&pwm2_info);
}
