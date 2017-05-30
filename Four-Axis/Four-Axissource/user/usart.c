/**************************************************************************************************
���ڲ��Ե�ʱ��, ���ߴ�ӡ���ݵ�ʱ��:
���Դ���Ϊ:
USART_Config();
printf("hello world!");
***************************************************************************************************/
#include "usart.h"

// ��ֲ�����޸���:
//#define 	USART1			// ������ USART1, ��ע�͵�
#define USART					USART3
#define RCC_PORT			RCC_APB2Periph_GPIOB
#define RCC_USART			RCC_APB1Periph_USART3
#define PORT					GPIOB
#define TX						GPIO_Pin_10
#define RX						GPIO_Pin_11

// ��������ֲ:
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
	
	// ��Ϊ USART �� TX �˺� RX �˵����ų�ʼ��
	GPIO_initStructure.GPIO_Pin = TX;					// Pin_10
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_AF_PP;			// �����������
	GPIO_initStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PORT,&GPIO_initStructure);			// GPIOB
	
	GPIO_initStructure.GPIO_Pin = RX;					// Pin_11
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 // ��������
	GPIO_Init(PORT,&GPIO_initStructure);			// GPIOB
	
	// ���� USART
	USART_initStructure.USART_BaudRate = 115200;
	USART_initStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_initStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_initStructure.USART_Parity = USART_Parity_No;
	USART_initStructure.USART_StopBits = USART_StopBits_1;
	USART_initStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART,&USART_initStructure);

	USART_Cmd(USART,ENABLE);			// ʹ�ܴ��� 3	
}

// �ڲ���������, ע��Ҫ��ѡ OPTIONS �е� USE Micro LIB ѡ��
int fputc (int ch, FILE *f)
{
	USART_SendData(USART,(u8)ch);
	while(USART_GetFlagStatus(USART,USART_FLAG_TXE)==RESET);
	return ch;
}
