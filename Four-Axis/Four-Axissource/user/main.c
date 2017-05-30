#include "stm32f10x.h"
#include "led.h"
#include "systick.h"
#include "usart.h"
#include "I2C_Simulation.h"


int main(void)
{
	// 串口配置
	USART_Config();
	printf("usart is ready : hello world.\r\n");

	// 中断优先级配置
//	NVIC_PriorityConfig();

	// 系统滴答定时器初始化
	SysTick_init();
	
	// LED 配置
	LED_Config();
	LED_Circular(1000);			// User LED 1 秒闪烁
	LED_ON(LED1|LED2|LED3|LED4|LED5|LED6|LED7|LED8);		// 开启机臂的 LED 灯

	// 初始化 I2C, 并且将 systick 计时基准初始化.
	I2C_SimulationConfig();
	
	
	
	while(1);
}

