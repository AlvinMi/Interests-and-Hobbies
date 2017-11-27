/***************************************************************************************
									声明
本项目代码仅供个人学习使用，可以自由移植修改，但必须保留此声明信息。移植过程中出现其他不可
估量的BUG，修远智控不负任何责任。请勿商用！

程序版本号：	2.5
日期：			2017-5-1
作者：			东方萧雨、小马哥
版权所有：		修远智控N0.1实验室
****************************************************************************************/
/***********************************************************************
利用TIM2定时器作为计时，每隔1秒钟检查一次飞机和遥控器是否有数据交互，防止
飞机脱离遥控器的控制。
计算周期的公式为：psc/72*period/1000000（单位为秒）
************************************************************************/
#include "checksignal.h"

void CheckSignal_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_timeBaseStucture;
	TIM_OCInitTypeDef TIM_ocInitStructure;
	
	u16 period 	= 10000;											//设置PWM周期值（即ARR值）
	u16 pluse 	= 9000;												//设置PWM的脉冲宽度值（即CRR值）
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIM_timeBaseStucture.TIM_ClockDivision = TIM_CKD_DIV1;      	//分频因子，输出给定时器的ETRP数字滤波器提供时钟
	TIM_timeBaseStucture.TIM_Prescaler = 7200-1;                	//预分频，给TIMx_CNT驱动的时钟，注意：实际的预分频值是0+1
	TIM_timeBaseStucture.TIM_CounterMode = TIM_CounterMode_Up;		//向上计数
	TIM_timeBaseStucture.TIM_Period = period;			
	TIM_TimeBaseInit(TIM2,&TIM_timeBaseStucture);
	
	TIM_ocInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				//PWM输出模式为PWM1
	TIM_ocInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//设置有效电平的极性
	TIM_ocInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能通道输出PWM
	TIM_ocInitStructure.TIM_Pulse =  pluse;							//设置PWM的脉冲宽度值，即CRR值
	TIM_OC1Init(TIM2,&TIM_ocInitStructure);
	
	TIM_ARRPreloadConfig(TIM2,ENABLE);								//使能TIM2的寄存器ARR的预装载功能，DISABLE时将会使改变ARR值时立即生效
	TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);				//使能TIM2通道1的CCR的预装载功能，DISABLE时将会使改变CRR值时立即生效
	
	TIM_Cmd(TIM2,ENABLE);
	
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);							//先清除定时器更新标志位，防止一开启中断就进入中断处理函数中
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	NVIC_EnableIRQ(TIM2_IRQn);
}






