#include "stm32f10x.h"
#include "led.h"

int main(void)
{
	LED_Config();
	LED_On(LED1);
	
	while(1)
	{
	
	}
}

