/**************************************************************************************************
串口测试的时候, 或者打印数据的时候:
测试代码为:
USART_Config();
printf("hello world!");
***************************************************************************************************/
#include "usart.h"

// 移植代码修改区:
//#define 	USART1			// 若不是 USART1, 则注释掉
#define USART					USART3
#define RCC_PORT			RCC_APB2Periph_GPIOB
#define RCC_USART			RCC_APB1Periph_USART3
#define PORT					GPIOB
#define TX						GPIO_Pin_10
#define RX						GPIO_Pin_11

// 函数区移植:
void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_initStructure;
	USART_InitTypeDef USART_initStructure;
	
	RCC_APB2PeriphClockCmd(RCC_PORT,ENABLE);			// GPIOB, ENABLE

	#ifdef USE_USART1
		RCC_APB2PeriphClockCmd(RCC_USART,ENABLE);
	#else
		RCC_APB1PeriphClockCmd(RCC_USART,ENABLE);
	#endif
	
	// 作为 USART 的 TX 端和 RX 端的引脚初始化
	GPIO_initStructure.GPIO_Pin = TX;					// Pin_10
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_AF_PP;			// 复用推挽输出
	GPIO_initStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PORT,&GPIO_initStructure);			// GPIOB
	
	GPIO_initStructure.GPIO_Pin = RX;					// Pin_11
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 // 浮空输入
	GPIO_Init(PORT,&GPIO_initStructure);			// GPIOB
	
	// 配置 USART
	USART_initStructure.USART_BaudRate = 115200;
	USART_initStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_initStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_initStructure.USART_Parity = USART_Parity_No;
	USART_initStructure.USART_StopBits = USART_StopBits_1;
	USART_initStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART,&USART_initStructure);

	USART_Cmd(USART,ENABLE);			// 使能串口 3	
}

// 内部函数调用, 注意要勾选 OPTIONS 中的 USE Micro LIB 选项
int fputc (int ch, FILE *f)
{
	USART_SendData(USART,(u8)ch);
	while(USART_GetFlagStatus(USART,USART_FLAG_TXE)==RESET);
	return ch;
}
