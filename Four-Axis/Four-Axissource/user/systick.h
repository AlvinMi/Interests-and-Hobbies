#ifndef _SYSTICK_H
#define _SYSTICK_H

#include "stm32f10x.h"

void SysTick_init(void);
void delay_us(u32 time);
void delay_ms(u32 time);

#endif
