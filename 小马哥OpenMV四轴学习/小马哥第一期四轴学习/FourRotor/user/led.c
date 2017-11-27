/***************************************************************************************
									声明
本项目代码仅供个人学习使用，可以自由移植修改，但必须保留此声明信息。移植过程中出现其他不可
估量的BUG，修远智控不负任何责任。请勿商用！

程序版本号：	2.0
日期：			2017-1-1
作者：			东方萧雨
版权所有：		修远智控N0.1实验室
****************************************************************************************/
/********************************************************************************
点亮LED灯

注意：
1.当要使用LED_CyclieOn(...)函数循环点亮LED9灯时，请先初始化SysTick_init()

主函数初始化代码：
	LED_Config();
	
测试代码为：
	SysTick_init();
	LED_Init();
	LED_CyclieOn(1000);
	
	while(1){}
*******************************************************************************/
#include "led.h"
#include "systick.h"

#define D1 			GPIO_Pin_13
#define D2 			GPIO_Pin_14
#define D3 			GPIO_Pin_15

#define D4 			GPIO_Pin_15
#define D5 			GPIO_Pin_14
#define D6 			GPIO_Pin_13
#define D7 			GPIO_Pin_9
#define D8 			GPIO_Pin_8
#define D9 			GPIO_Pin_12



void LED_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructus;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructus.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructus.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructus.GPIO_Pin = D1|D2|D3;
	GPIO_Init(GPIOC,&GPIO_InitStructus);
	
	GPIO_InitStructus.GPIO_Pin = D4|D5|D6|D7|D8|D9;
	GPIO_Init(GPIOB,&GPIO_InitStructus);
	
	LED_Off(LED1|LED2|LED3|LED4|LED5|LED6|LED7|LED8|LED9);
}


//开灯程序
void LED_On(u16 LEDx)
{
	if(LEDx & 1){
		GPIO_ResetBits(GPIOC,D1);
	}
	if(LEDx & 2){
		GPIO_ResetBits(GPIOC,D2);
	}
	if(LEDx & 4){
		GPIO_ResetBits(GPIOC,D3);
	}
	if(LEDx & 8){
		GPIO_ResetBits(GPIOB,D4);
	}
	if(LEDx & 16){
		GPIO_ResetBits(GPIOB,D5);
	}
	if(LEDx & 32){
		GPIO_ResetBits(GPIOB,D6);
	}
	if(LEDx & 64){
		GPIO_ResetBits(GPIOB,D7);
	}
	if(LEDx & 128){
		GPIO_ResetBits(GPIOB,D8);
	}
	if(LEDx & 256){
		GPIO_ResetBits(GPIOB,D9);
	}
}

void LED_Off(u16 LEDx)
{
	if(LEDx & 1){
		GPIO_SetBits(GPIOC,D1);
	}
	if(LEDx & 2){
		GPIO_SetBits(GPIOC,D2);
	}
	if(LEDx & 4){
		GPIO_SetBits(GPIOC,D3);
	}
	if(LEDx & 8){
		GPIO_SetBits(GPIOB,D4);
	}
	if(LEDx & 16){
		GPIO_SetBits(GPIOB,D5);
	}
	if(LEDx & 32){
		GPIO_SetBits(GPIOB,D6);
	}
	if(LEDx & 64){
		GPIO_SetBits(GPIOB,D7);
	}
	if(LEDx & 128){
		GPIO_SetBits(GPIOB,D8);
	}
	if(LEDx & 256){
		GPIO_SetBits(GPIOB,D9);
	}
}


//开关LED
void LED_Toggle(u16 LEDx)
{
	//异或，开关模式输出高低电平
	if(LEDx & 1){
		GPIOC->ODR ^= D1;
	}
	if(LEDx & 2){
		GPIOC->ODR ^= D2;
	}
	if(LEDx & 4){
		GPIOC->ODR ^= D3;
	}
	if(LEDx & 8){
		GPIOB->ODR ^= D4;
	}
	if(LEDx & 16){
		GPIOB->ODR ^= D5;
	}
	if(LEDx & 32){
		GPIOB->ODR ^= D6;
	}
	if(LEDx & 64){
		GPIOB->ODR ^= D7;
	}
	if(LEDx & 128){
		GPIOB->ODR ^= D8;
	}
	if(LEDx & 256){
		GPIOB->ODR ^= D9;
	}
}


//循环点亮LED9
void LED_CyclieOn(u32 delayms)
{
	while(1){
		LED_On(LED9);
		delay_ms(delayms);
		LED_Off(LED9);
		delay_ms(delayms);
	}
}


