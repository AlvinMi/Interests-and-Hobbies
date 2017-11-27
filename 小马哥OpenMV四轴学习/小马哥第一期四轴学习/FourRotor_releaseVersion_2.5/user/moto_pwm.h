#ifndef _MOTO_PWM_H
#define _MOTO_PWM_H

#include "stm32f10x.h"

#define PERIOD 		(SystemCoreClock/10000)				//ARR计数值为：7200，不分频时，则pwm频率为10KHz，注意这里要用小括号括起来


void MOTO_PwmConfig(void);
void MOTO_Start(void);

void MOTO1_SetDuty(u8 precent);
void MOTO2_SetDuty(u8 precent);
void MOTO3_SetDuty(u8 precent);
void MOTO4_SetDuty(u8 precent);

void MOTO1_SetPulse(u16 pulse);
void MOTO2_SetPulse(u16 pulse);
void MOTO3_SetPulse(u16 pulse);
void MOTO4_SetPulse(u16 pulse);

#endif
