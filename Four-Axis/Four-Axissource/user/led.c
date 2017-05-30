#include "led.h"
#include "systick.h"

#define D1 			GPIO_Pin_13		// PC13
#define D2 			GPIO_Pin_14		// PC14
#define D3 			GPIO_Pin_15		// PC15

#define D4 			GPIO_Pin_15		// PB15
#define D5 			GPIO_Pin_14		// PB14
#define D6 			GPIO_Pin_13		// PB13
#define D7 			GPIO_Pin_9		// PB9
#define D8 			GPIO_Pin_8		// PB8
#define D9 			GPIO_Pin_12		// PB12 (LEDUSER)

// LED 函数相关
void LED_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructus;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructus.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructus.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructus.GPIO_Pin = D1|D2|D3;
	GPIO_Init(GPIOC, &GPIO_InitStructus);
	
	GPIO_InitStructus.GPIO_Pin = D4|D5|D6|D7|D8|D9;
	GPIO_Init(GPIOB,&GPIO_InitStructus);

	LED_OFF(LED1|LED2|LED3|LED4|LED5|LED6|LED7|LED8|LED9);
}

// 关 LED
void LED_OFF(u16 LEDx)
{
	if(LEDx & 1)
	{
		GPIO_ResetBits(GPIOC,D1);
	}
	if(LEDx & 2)
	{
		GPIO_ResetBits(GPIOC,D2);
	}
	if(LEDx & 4)
	{
		GPIO_ResetBits(GPIOC,D3);
	}
	if(LEDx & 8)
	{
		GPIO_ResetBits(GPIOB,D4);
	}
	if(LEDx & 16)
	{
		GPIO_ResetBits(GPIOB,D5);
	}
	if(LEDx & 32)
	{
		GPIO_ResetBits(GPIOB,D6);
	}
	if(LEDx & 64)
	{
		GPIO_ResetBits(GPIOB,D7);
	}
	if(LEDx & 128)
	{
		GPIO_ResetBits(GPIOB,D8);
	}
	if(LEDx & 256)
	{
		GPIO_ResetBits(GPIOB,D9);
	}
}

// 开 LED
void LED_ON(u16 LEDx)
{
	if(LEDx & 1)
	{
		GPIO_SetBits(GPIOC,D1);
	}
	if(LEDx & 2)
	{
		GPIO_SetBits(GPIOC,D2);
	}
	if(LEDx & 4)
	{
		GPIO_SetBits(GPIOC,D3);
	}
	if(LEDx & 8)
	{
		GPIO_SetBits(GPIOB,D4);
	}
	if(LEDx & 16)
	{
		GPIO_SetBits(GPIOB,D5);
	}
	if(LEDx & 32)
	{
		GPIO_SetBits(GPIOB,D6);
	}
	if(LEDx & 64)
	{
		GPIO_SetBits(GPIOB,D7);
	}
	if(LEDx & 128)
	{
		GPIO_SetBits(GPIOB,D8);
	}
	if(LEDx & 256)
	{
		GPIO_SetBits(GPIOB,D9);
	}
}

// LED 翻转
void LED_Toggle(u16 LEDx)
{
	// 异或, 开关模式输出高低电平
	if(LEDx & 1)
	{
		GPIOC->ODR ^= D1;
	}
	if(LEDx & 2)
	{
		GPIOC->ODR ^= D2;
	}
	if(LEDx & 4)
	{
		GPIOC->ODR ^= D3;
	}
	if(LEDx & 8)
	{
		GPIOB->ODR ^= D4;
	}
	if(LEDx & 16)
	{
		GPIOB->ODR ^= D5;
	}
	if(LEDx & 32)
	{
		GPIOB->ODR ^= D6;
	}
	if(LEDx & 64)
	{
		GPIOB->ODR ^= D7;
	}
	if(LEDx & 128)
	{
		GPIOB->ODR ^= D8;
	}
	if(LEDx & 256)
	{
		GPIOB->ODR ^= D9;
	}
}

// LED 循环闪烁, 延迟时间可选择
void LED_Circular(u32 delayms)
{
	while(1)
	{
		LED_ON(LED9);
		delay_ms(delayms);
		LED_OFF(LED9);
		delay_ms(delayms);
	}
}


