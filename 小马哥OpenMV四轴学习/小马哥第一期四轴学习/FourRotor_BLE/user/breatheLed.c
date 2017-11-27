/***************************************************************************************
									声明
本项目代码仅供个人学习使用，可以自由移植修改，但必须保留此声明信息。移植过程中出现其他不可
估量的BUG，修远智控不负任何责任。请勿商用！

程序版本号：	2.0
日期：			2017-1-1
作者：			东方萧雨
版权所有：		修远智控N0.1实验室
****************************************************************************************/
/**********************************************************
使用PWM控制LED灯，实现呼吸灯的效果

主函数测试代码为：
	SysTick_init();
	LED_Config();
	
	PWM_Config();
	
	while(1){}
***********************************************************/
#include "breatheLed.h"
#include "led.h"

u16 i = 11;
u8 flag = 0;
void PWM_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_timeBaseStucture;
	TIM_OCInitTypeDef TIM_ocInitStructure;
	
	u16 period 	= 10000;											//设置PWM周期值（即ARR值）
	u16 pluse 	= 9000;												//设置PWM的脉冲宽度值（即CRR值）
	
	//使能TIM3的时钟，TIM3是挂在APB1总线上的
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	//配置TIM3的时基
	TIM_timeBaseStucture.TIM_ClockDivision = TIM_CKD_DIV1;      	//分频因子，输出给定时器的ETRP数字滤波器提供时钟
	TIM_timeBaseStucture.TIM_Prescaler = 0;                			//预分频，给TIMx_CNT驱动的时钟，注意：实际的预分频值是0+1
	TIM_timeBaseStucture.TIM_CounterMode = TIM_CounterMode_Up;		//向上计数
	TIM_timeBaseStucture.TIM_Period = period;						
	TIM_TimeBaseInit(TIM3,&TIM_timeBaseStucture);
	
	//配置TIM通道1的PWM的输出
	TIM_ocInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				//PWM输出模式为PWM1
	TIM_ocInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//设置有效电平的极性
	TIM_ocInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能通道输出PWM
	TIM_ocInitStructure.TIM_Pulse =  pluse;							//设置PWM的脉冲宽度值，即CRR值
	TIM_OC1Init(TIM3,&TIM_ocInitStructure);
	
	TIM_ARRPreloadConfig(TIM3,ENABLE);								//使能TIM3的寄存器ARR的预装载功能，DISABLE时将会使改变ARR值时立即生效
	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);				//使能TIM3通道1的CCR的预装载功能，DISABLE时将会使改变CRR值时立即生效
	
	//使能TIM3定时器
	TIM_Cmd(TIM3,ENABLE);
	
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	
	TIM_ITConfig(TIM3,TIM_IT_CC1|TIM_IT_Update,ENABLE);
	NVIC_EnableIRQ(TIM3_IRQn);
}


//定时器3中断处理函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_CC1)==SET){
		LED_On(LED1|LED2|LED3|LED4|LED5|LED6|LED7|LED8|LED9);
		TIM_ClearITPendingBit(TIM3,TIM_IT_CC1);
	}else if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET){
		LED_Off(LED1|LED2|LED3|LED4|LED5|LED6|LED7|LED8|LED9);
		if(i<10){
			i=10;
			flag = 0;
		}else if(i>9990){
			i=9990;
			flag = 1;
		}
		if(flag == 0){
			TIM_SetCompare1(TIM3,i++);
		}else{
			TIM_SetCompare1(TIM3,i--);
		}
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
}




