/***************************************************************************************
									声明
本项目代码仅供个人学习使用，可以自由移植修改，但必须保留此声明信息。移植过程中出现其他不可
估量的BUG，修远智控不负任何责任。请勿商用！

程序版本号：	2.0
日期：			2017-1-1
作者：			东方萧雨
版权所有：		修远智控N0.1实验室
****************************************************************************************/
/***********************************************************************
利用TIM2定时器作为计时，每隔1秒钟检查一次飞机和遥控器是否有数据交互，防止
飞机脱离遥控器的控制。
计算周期的公式为：psc/72*period/1000000（单位为秒）
************************************************************************/
#include "checksignal.h"
#include "led.h"

u8 dataPID_Last = 250;				//保存上一次接收到的数据包识别PID

extern u8 dataPID;					//数据包识别PID，原始定义在：deal_datapacket.c文件中
extern vu16 remoteControl[4];		//保存从遥控器中发送过来的油门和方向数据，原始定义在control.h文件中
//==================================================================================================


void CheckSignal_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_timeBaseStucture;
	TIM_OCInitTypeDef TIM_ocInitStructure;
	
	u16 period 	= 10000;											//设置PWM周期值（即ARR值）
	u16 pluse 	= 9000;												//设置PWM的脉冲宽度值（即CRR值）
	
	//使能TIM3的时钟，TIM3是挂在APB1总线上的
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	//配置TIM3的时基
	TIM_timeBaseStucture.TIM_ClockDivision = TIM_CKD_DIV1;      	//分频因子，输出给定时器的ETRP数字滤波器提供时钟
	TIM_timeBaseStucture.TIM_Prescaler = 7200-1;                	//预分频，给TIMx_CNT驱动的时钟，注意：实际的预分频值是0+1
	TIM_timeBaseStucture.TIM_CounterMode = TIM_CounterMode_Up;		//向上计数
	TIM_timeBaseStucture.TIM_Period = period;			
	TIM_TimeBaseInit(TIM2,&TIM_timeBaseStucture);
	
	//配置TIM通道1的PWM的输出
	TIM_ocInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				//PWM输出模式为PWM1
	TIM_ocInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//设置有效电平的极性
	TIM_ocInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//使能通道输出PWM
	TIM_ocInitStructure.TIM_Pulse =  pluse;							//设置PWM的脉冲宽度值，即CRR值
	TIM_OC1Init(TIM2,&TIM_ocInitStructure);
	
	TIM_ARRPreloadConfig(TIM2,ENABLE);								//使能TIM3的寄存器ARR的预装载功能，DISABLE时将会使改变ARR值时立即生效
	TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);				//使能TIM3通道1的CCR的预装载功能，DISABLE时将会使改变CRR值时立即生效
	
	//使能TIM3定时器
	TIM_Cmd(TIM2,ENABLE);
	
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);							//先清除定时器更新标志位，防止一开启中断就进入中断处理函数中
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	NVIC_EnableIRQ(TIM2_IRQn);
}


//定时器2中断处理函数
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET){
		
		//这个非常重要，防止飞机脱离遥控器的控制而无限制的飞行
		if(dataPID_Last == dataPID){					//这里表明飞机已经和遥控器脱离控制了
			//当飞机脱离遥控器控制时，自动把油门输出值限定到1250
			if(remoteControl[1]-1250>=800.0){
				remoteControl[1] -= 800;
			}else{
				remoteControl[1] = 1250;
			}
			
			remoteControl[2] = 15;						//当飞机脱离遥控器时，自动把左右方向值给15
			remoteControl[3] = 15;						//当飞机脱离遥控器时，自动把前后方向值给15
			LED_Off(LED9);								//LED9灯灭，表明和遥控器通讯失败
		}else{
			dataPID_Last = dataPID;						//这里表明飞机和遥控器通讯正常，将接收到的dataPID覆盖掉dataPID_Last之前的值
		}
		
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}



