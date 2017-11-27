/***************************************************************************************
									声明
本项目代码仅供个人学习使用，可以自由移植修改，但必须保留此声明信息。移植过程中出现其他不可
估量的BUG，修远智控不负任何责任。请勿商用！

程序版本号：	2.5
日期：			2017-5-1
作者：			东方萧雨、小马哥
版权所有：		修远智控N0.1实验室
****************************************************************************************/
/*************************************************************************
系统滴答定时器
使用前，需要初始化计时基准
注意：systick_init()函数中就已经将systick中断打开了
**************************************************************************/
#include "systick.h"

u32 count;

void SysTick_init(void)
{
	//设置重载值
	SysTick->LOAD  = (uint32_t)(SystemCoreClock/1000000 - 1UL);

	//清空计数器中的值
	SysTick->VAL   = 0UL;

	//设置systick的时钟源和开启systick的中断
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_TICKINT_Msk;
	
	//先将systick关闭，有需要计时的时候再打开
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}


void delay_us(u32 time)
{
	if(time<=0)
		return;

	count = time;
	SysTick->VAL = 0;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	while(count!=0);
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void delay_ms(u32 time)
{
	if(time<=0)
		return;

	count = time*1000;
	SysTick->VAL = 0;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	while(count!=0);
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

//systick中断处理函数
void SysTick_Handler(void)
{
	if(count!=0){
		count--;
	}
}

