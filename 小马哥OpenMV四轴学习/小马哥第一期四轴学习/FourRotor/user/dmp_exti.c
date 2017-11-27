/***************************************************************************************
									声明
本项目代码仅供个人学习使用，可以自由移植修改，但必须保留此声明信息。移植过程中出现其他不可
估量的BUG，修远智控不负任何责任。请勿商用！

程序版本号：	2.0
日期：			2017-1-1
作者：			东方萧雨
版权所有：		修远智控N0.1实验室
****************************************************************************************/
/*************************************************************************
初始化配置和MPU6050的INT引脚相连的EXTI

主函数初始化代码为：
	DMP_EXTIConfig();
*************************************************************************/
#include "dmp_exti.h"

void DMP_EXTIConfig(void)
{
	GPIO_InitTypeDef GPIO_initStructure;
	EXTI_InitTypeDef EXTI_initStructure;
	
	//因为要用到EXTI，所以必须打开AFIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	
	//初始化GPIOA_PIN_15脚，用他们作为硬件输入(下拉输入)，接收来自MPU的INT脚的电平触发信号
	GPIO_initStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA,&GPIO_initStructure);
	
	//给所用到的中断线路配置相应的触发模式和触发时机
	EXTI_initStructure.EXTI_Line = EXTI_Line15;
	EXTI_initStructure.EXTI_LineCmd = ENABLE;
	EXTI_initStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_initStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_initStructure);
	
	//配置中断
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);		//开启GPIO管脚的中断线路
	NVIC_EnableIRQ(EXTI15_10_IRQn);									//开启NVIC中EXTI的中断通道
}



