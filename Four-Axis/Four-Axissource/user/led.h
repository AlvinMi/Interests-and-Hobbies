#ifndef _LED_H
#define _LED_H

#include "stm32f10x.h"
#define LED1 	GPIO_Pin_13
#define LED2 	GPIO_Pin_14
#define LED3 	GPIO_Pin_15

#define LED4 	GPIO_Pin_15
#define LED5 	GPIO_Pin_14
#define LED6 	GPIO_Pin_13
#define LED7 	GPIO_Pin_9
#define LED8 	GPIO_Pin_8
#define LED9 	GPIO_Pin_12


void LED_Config(void);
void LED_On(u16 LEDx);
void LED_Off(u16 LEDx);

#endif
