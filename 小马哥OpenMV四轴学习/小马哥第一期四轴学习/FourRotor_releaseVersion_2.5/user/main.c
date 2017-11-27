/***************************************************************************************
									声明
本项目代码仅供个人学习使用，可以自由移植修改，但必须保留此声明信息。移植过程中出现其他不可
估量的BUG，修远智控不负任何责任。请勿商用！

程序版本号：	2.5
日期：			2017-5-1
作者：			东方萧雨、小马哥
版权所有：		修远智控N0.1实验室
****************************************************************************************/
#include "stm32f10x.h"
#include "mpu_dmp_useapi.h"
#include "usart.h"
#include "i2c_moni.h"
#include "dmp_exti.h"
#include "nvic.h"
#include "led.h"
#include "moto_pwm.h"
#include "systick.h"
#include "nRF.h"
#include "checksignal.h"
#include "task.h"
#include "global.h"


extern TaskMask_s TaskMask;

int main(void)
{
	USART_Config();
	printf("usart is ready\r\n");
	
	//各中断优先级配置
	NVIC_PriorityConfig();
	
	SysTick_init();
	
	LED_Config();
	LED_On(LED1|LED2|LED3|LED4|LED5|LED6|LED7|LED8);
	
	//初始化I2C，并且将systick计时基准的初始化
	I2C_MoniConfig();
	
	//初始化MPU
	mpu_dmp_init();
	
	//设置哪些数据上报给上位机
	SetReportFlag(RESET,RESET);
	
	MOTO_PwmConfig();
	
	NRF_Config();
	
	CheckSignal_Config();
	
	//接收DMP中断完成信号的EXTI初始化
	DMP_EXTIConfig();
 
	while(1){
		//循环执行任务
		if(TaskMask.angleDataReady){
			Task_PIDControl();
		}
		
		if(TaskMask.checkSingle){
			Task_CheckSingle();
		}
		
		if(TaskMask.nrfDataReady){
			Task_NrfDataReady();
		}
	}
}

