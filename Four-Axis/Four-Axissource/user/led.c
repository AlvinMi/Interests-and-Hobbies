#include "led.h"

void LED_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructus;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructus.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructus.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructus.GPIO_Pin = LED1|LED2|LED3;
	GPIO_Init(GPIOC, &GPIO_InitStructus);
	
	GPIO_InitStructus.GPIO_Pin = LED4|LED5|LED6|LED7|LED8|LED9;
	GPIO_Init(GPIOC, &GPIO_InitStructus);

	LED_Off(LED1|LED2|LED3|LED4|LED5|LED6|LED7|LED8|LED9);
}

void LED_On(u16 LEDx)
{
	
}

void LED_Off(u16 LEDx)
{

}

