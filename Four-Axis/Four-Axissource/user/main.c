#include "stm32f10x.h"
#include "led.h"
#include "systick.h"


int main(void){
	
	//u8 value;
	//USART_Config();
	SysTick_init();
	LED_Config();
	LED_Circular(1000);
	
	while(1){}
	//Printf("value is %d\r\n", value);
}

