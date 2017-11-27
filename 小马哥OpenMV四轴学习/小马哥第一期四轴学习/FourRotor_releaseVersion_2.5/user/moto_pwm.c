/***************************************************************************************
									声明
本项目代码仅供个人学习使用，可以自由移植修改，但必须保留此声明信息。移植过程中出现其他不可
估量的BUG，修远智控不负任何责任。请勿商用！

程序版本号：	2.5
日期：			2017-5-1
作者：			东方萧雨、小马哥
版权所有：		修远智控N0.1实验室
****************************************************************************************/
/***************************************************************************************
高级定时器TIM1输出PWM波控制四个电机

主函数测试代码为：
	MOTO_PwmConfig();
	MOTO_Start();
	
	while(1);
	
****************************************************************************************/
#include "moto_pwm.h"


/***********************************函数区***********************************************/
//定时器TIM1初始化配置
void MOTO_PwmConfig(void)
{
	GPIO_InitTypeDef GPIO_initStructure;
	TIM_TimeBaseInitTypeDef TIM_timeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	
	//配置TIM1输出通道的引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_initStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_initStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_initStructure);
	
	//使能TIM1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	//配置时基
	TIM_timeBaseInitStructure.TIM_ClockDivision = 0;
	TIM_timeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;		//向上计数
	TIM_timeBaseInitStructure.TIM_Period = PERIOD;						//设置ARR值
	TIM_timeBaseInitStructure.TIM_Prescaler = 0;						//时钟预分频值，不分频
	TIM_timeBaseInitStructure.TIM_RepetitionCounter = 0;				//重复计数值，每次计数溢出，该值都将减一，直到该值减到0时，才会产生溢出事件
	TIM_TimeBaseInit(TIM1,&TIM_timeBaseInitStructure);
	
	//配置OC输出通道（注意就算没有用到互补输出，也要配置CHxN，只不过最后将TIM_OutputNState_Disable关闭即可，不配置将发生意想不到的事情发生）
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;					//采用PWM模式1输出波形
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;		//设置CH通道的空闲状态的电平（这个和刹车有关，刹车后通道的电平状态就取决于这个）
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;			//设置CH通道的有效电平
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;		//设置CHN通道的空闲状态的电平
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;			//设置CHN通道的有效电平
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		//使能CH通道
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;	//关闭CHN通道
	TIM_OCInitStructure.TIM_Pulse = 0;									//设置TIM1的CCR值（注意同一TIM的各个通道共用ARR）
	
	TIM_OC1Init(TIM1,&TIM_OCInitStructure);
	TIM_OC2Init(TIM1,&TIM_OCInitStructure);
	TIM_OC3Init(TIM1,&TIM_OCInitStructure);
	TIM_OC4Init(TIM1,&TIM_OCInitStructure);
	
	//使能TIM的ARR和CRR，以及使能TIM定时器,开启pwm输出
	TIM_ARRPreloadConfig(TIM1,ENABLE);									//使能TIM1的寄存器ARR的预装载功能，DISABLE时将会使改变ARR值时立即生效
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Disable);                   //使能TIM1通道1的CCR的预装载功能，DISABLE时将会使改变CRR值时立即生效
	TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Disable);
	TIM_OC3PreloadConfig(TIM1,TIM_OCPreload_Disable);
	TIM_OC4PreloadConfig(TIM1,TIM_OCPreload_Disable);
	
	TIM_Cmd(TIM1,ENABLE);
	
	//开始启动定时器输出pwm,这句是高级定时器才有的，输出pwm必须打开
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
}


//方便测试启动电机用的
void MOTO_Start(void)
{
	MOTO1_SetDuty(10);
	MOTO2_SetDuty(10);
	MOTO3_SetDuty(10);
	MOTO4_SetDuty(10);
}



/***********************************************
更改MOTO1的占空比
输入参数为pwm占空比的百分比数值
************************************************/
void MOTO1_SetDuty(u8 precent)
{
	//防止传进来的参数超限，并且由于precent是无符号值，所以不用判断负数的情况
	if(precent >= 100){
		precent = 100;
	}
	
	TIM_SetCompare1(TIM1,(u16)(PERIOD*precent/100));
}

/************************************************
更改MOTO2的占空比
输入参数为pwm占空比的百分比数值
************************************************/
void MOTO2_SetDuty(u8 precent)
{
	//防止传进来的参数超限，并且由于precent是无符号值，所以不用判断负数的情况
	if(precent >= 100){
		precent = 100;
	}
	
	TIM_SetCompare2(TIM1,(u16)(PERIOD*precent/100));
}

/************************************************
更改MOTO3的占空比
输入参数为pwm占空比的百分比数值
************************************************/
void MOTO3_SetDuty(u8 precent)
{
	//防止传进来的参数超限，并且由于precent是无符号值，所以不用判断负数的情况
	if(precent >= 100){
		precent = 100;
	}
	
	TIM_SetCompare3(TIM1,(u16)(PERIOD*precent/100));
}

/************************************************
更改MOTO4的占空比
输入参数为pwm占空比的百分比数值
************************************************/
void MOTO4_SetDuty(u8 precent)
{
	//防止传进来的参数超限，并且由于precent是无符号值，所以不用判断负数的情况
	if(precent >= 100){
		precent = 100;
	}
	
	TIM_SetCompare4(TIM1,(u16)(PERIOD*precent/100));
}


//=================================================================
/***********************************************
更改MOTO1的pulse
输入参数为pwm的占空数值
************************************************/
void MOTO1_SetPulse(u16 pulse)
{
//	//防止传进来的参数超限
//	if(pulse >= PERIOD-50){
//		pulse = PERIOD-50;
//	}else if(pulse <= 5){
//		pulse = 1;
//	}
	
	TIM_SetCompare1(TIM1,pulse);
}

/************************************************
更改MOTO2的pulse
输入参数为pwm占空数值
************************************************/
void MOTO2_SetPulse(u16 pulse)
{
	TIM_SetCompare2(TIM1,pulse);
}

/************************************************
更改MOTO2的pulse
输入参数为pwm占空数值
************************************************/
void MOTO3_SetPulse(u16 pulse)
{
	TIM_SetCompare3(TIM1,pulse);
}

/************************************************
更改MOTO2的pulse
输入参数为pwm占空数值
************************************************/
void MOTO4_SetPulse(u16 pulse)
{
	TIM_SetCompare4(TIM1,pulse);
}

