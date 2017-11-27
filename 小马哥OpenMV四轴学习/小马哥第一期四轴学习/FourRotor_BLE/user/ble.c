/***************************************************************************************
									声明
本项目代码仅供个人学习使用，可以自由移植修改，但必须保留此声明信息。移植过程中出现其他不可
估量的BUG，修远智控不负任何责任。请勿商用！

程序版本号：	2.0
日期：			2017-1-1
作者：			东方萧雨
版权所有：		修远智控N0.1实验室
****************************************************************************************/
/*******************************************************
蓝牙收到数据后通过USART2和DMA将数据传输给STM32
*********************************************************/
#include "ble.h"
#include "deal_datapacket.h"
#include "systick.h"


//============================全局变量定义区==========================================
u8 RxBuf[7];									//数据接收缓冲区

extern u8 buttonFlag;							//指示哪个按键被按下，原始定义在deal_datapacket.c文件中
//====================================================================================



//==================================函数区=====================================================
void BLE_Config(void)
{
	DMA_Config();
	USART2_Config();
}


//配置DMA
void DMA_Config(void)
{
	DMA_InitTypeDef DMA_initStructure;
	
	//开启DMA1时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	
	//配置DMA1通道1，将ADC采样转换得到的数据传输到内存数组中
	DMA_initStructure.DMA_BufferSize = 7;										//每次传输的数据的个数
	DMA_initStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							//传输方向为：外设->内存
	DMA_initStructure.DMA_M2M = DMA_M2M_Disable;								//失能内存到内存的传输方式
	DMA_initStructure.DMA_MemoryBaseAddr = (u32)RxBuf;							//数据保存到内存中数组的首地址（这里因为RxBuf是数组名，所以不用加&）
	DMA_initStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;				//以一个字节为单位进行数据的传输
	DMA_initStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						//内存地址自增（这里地址是每次增加1，因为是以字节作为单位传输的）
	DMA_initStructure.DMA_Mode = DMA_Mode_Circular;								//循环传输的方式
	DMA_initStructure.DMA_PeripheralBaseAddr = ((u32)&USART2->DR);				//&USART2->DR的第一个字节的地址是DR中的0~7位
	DMA_initStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		//以一个字节为单位进行数据的传输
	DMA_initStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//外设地址自增
	DMA_initStructure.DMA_Priority = DMA_Priority_Medium;						//DMA通道6的优先级设置为中级，（这个优先级是当同一个DMA的不同通道同时有传输数据的要求时，优先级高的先进行传输）
	DMA_Init(DMA1_Channel6,&DMA_initStructure);
	
	DMA_ITConfig(DMA1_Channel6,DMA_IT_TC,ENABLE);								//打开DMA通道6数据传输完成中断
	NVIC_EnableIRQ(DMA1_Channel6_IRQn);											//打开NVIC中对应的DMA通道6的中断通道
	
	//开启DMA1的通道6
	DMA_Cmd(DMA1_Channel6,ENABLE);
}


//配置串口2并打开串口DMA请求通道
void USART2_Config(void)
{
	GPIO_InitTypeDef GPIO_initStructure;
	USART_InitTypeDef USART_initStructure;
	
	//时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	GPIO_initStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_initStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_initStructure);
	
	GPIO_initStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_initStructure);
	
	
	//配置串口USART2
	USART_initStructure.USART_BaudRate = 9600;										//蓝牙模块默认的波特率是9600
	USART_initStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_initStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_initStructure.USART_Parity = USART_Parity_No;
	USART_initStructure.USART_StopBits = USART_StopBits_1;
	USART_initStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2,&USART_initStructure);
	
	//使能USART
	USART_Cmd(USART2,ENABLE);
	
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);			//使能USART的RX端向DMA请求数据传输
}


//DMA1通道6中断通道处理函数,当触发此中断时，说明一个数据包（7字节）已经发送过来了
void DMA1_Channel6_IRQHandler(void){
	if(DMA_GetITStatus(DMA1_IT_TC6)==SET){
//		if(RxBuf[0]!=255){
//			DMA_Cmd(DMA1_Channel6,DISABLE);
//			delay_us(10);
//			DMA_Config();
//		}else{
			UnpackData();
//		}
		
		DMA_ClearITPendingBit(DMA1_IT_TC6);					//清除DMA1通道6传输完成中断
	}
}


