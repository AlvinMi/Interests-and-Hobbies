/*************************************************************************
* 本 例 程 移 植 于 其 他
* Alvin.Mi--2017.5.20
**************************************************************************/
/*************************************************************************
系统滴答定时器
使用前，需要初始化计时基准
注意：systick_init()函数中就已经将systick中断打开了
**************************************************************************/
#include "systick.h"

u32 count;

void SysTick_init(void)
{
	// 设置重载值 (1us 时基)
	SysTick->LOAD  = (uint32_t)(SystemCoreClock/1000000 - 1UL);		// 1UL 无符号长整型
	
	// 清空计数器中的值
	SysTick->VAL   = 0UL;
	
	// 设置 systick 的时钟源和开启	systick 的中断
	/***** SysTick_CTRL_CLKSOURCE_Msk = 100; SysTick_CTRL_TICKINT_Msk = 10; 然后配置控制寄存器*****/ 
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_TICKINT_Msk;		
	
	// 先将 systick 关闭, 有需要计时的时候再打开
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

// 中断处理函数都是类似的, 在'startup '文件中会有相对应的中断函数名
// systick 中断处理函数		
void SysTick_Handler(void)
{
	if(count!=0){
		count--;
	}
}

