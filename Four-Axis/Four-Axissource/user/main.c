#include "stm32f10x.h"
#include "led.h"
#include "systick.h"
#include "usart.h"
#include "I2C_Simulation.h"


int main(void)
{
	// ��������
	USART_Config();
	printf("usart is ready : hello world.\r\n");

	// �ж����ȼ�����
//	NVIC_PriorityConfig();

	// ϵͳ�δ�ʱ����ʼ��
	SysTick_init();
	
	// LED ����
	LED_Config();
	LED_Circular(1000);			// User LED 1 ����˸
	LED_ON(LED1|LED2|LED3|LED4|LED5|LED6|LED7|LED8);		// �������۵� LED ��

	// ��ʼ�� I2C, ���ҽ� systick ��ʱ��׼��ʼ��.
	I2C_SimulationConfig();
	
	
	
	while(1);
}

