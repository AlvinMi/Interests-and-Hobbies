///**************************************************************************************
//1.读写NRF的寄存器时，必须按照：操作指令+寄存器地址的方式来访问。
//2.在读写NRF的寄存器时，CE必须拉低，使NRF处于待机或者掉电模式才可以。
//3.NRF的各状态机之间的切换，由CE，CONFIG寄存器中的PWR_UP位和PRIM_RX位的值共同决定
//4.当NRF处于待机模式2时（CE=1），只需将CE=0即可转为待机模式1。
//5.NRF有一个发射通道，6个接收通道。
//6.NRF会根据RF_CH寄存器中设置的频道来相互之间收发数据，即发射端和接收端都使用的是同一频道时，才可以相互通讯。
//7.收发端同处于同一频道时，还需要根据打包进数据包中的地址值来判断能被谁接收数据
//8.当NRF处于接收模式时，当接收到数据时，会触发中断，拉低IRQ，从而通知MCU收取数据
//9.NRF要改成发射模式前，必须先将要发送的数据写进TX端的FIFO中，然后再拉高CE，使NRF处于发射模式，以发送数据
//10.NRF的RX0的地址可以设置5个字节的自身地址，而RX1~5只能设置一个字节的自身地址，其余4个字节的地址为他们公用地址。
//11.NRF在发送一数据包的数据后，会自动转为接收模式，等待接收端发送的应答信号，当超过SETUP_RETR寄存器规定的等待时间还没收到应答
//	信号时，NRF会自动重发该数据包，并在此等待应答信号，以此重复，当重发的次数超过SETUP_RETR寄存器中设定的值时，还未收到应答
//	信号时，则会产生MAX_RT中断，注意此时不会自动清除TX端的FIFO中的数据，并且，必须清除MAX_RT中断，系统才能继续通讯；而若接收到
//	了应答信号，则会自动清除TX端的FIFO中的数据，并产生TX_DS中断，表明数据发送成功。
//12.OBSERVE_TX寄存器中的ARC_CNT位记录重发数据包的次数（当发送新的数据包后，此位自动复位），而PLOS_CNT位记录达到最大重发次数
//	后仍然没有收到应答信息的次数（相当于丢包次数，当写RX_CH寄存器时或者计数值达到15时自动复位）
//13.IRQ中断通过向STATUS寄存器中对应的中断标志位写1来清除相对应的中断，这个触发完相应的中断后，必须清除掉STATUS寄存器中的中断
//	标志位，否则会重复触发中断。
//14.当NRF处于发射模式时，在将TX端的FIFO中的数据发送完成并接受到应答信号后，若CE仍然处于高电平，则NRF会进入待机模式2，并且一但
//	有数据再次进入TX端的FIFO中时，NRF会立即进入发射状态，准备将数据发送出去
//15.当NRF进入接收模式时，至少等待130us后会开始检测空中信息
//16.状态寄存器中的RX_P_NO位会记录接收到的数据来自哪个通道
//17.当NRF接收到数据后，IRQ会产生中断（可以通过读取状态寄存器来得知是什么中断），通知MCU，此时MCU将CE拉低，使NRF进入待机模式1，
//	然后开始读取RX端的FIFO数据，读取完成后，NRF自动清除RX端的FIFO中的数据注意：NRF只有在接收到有效的数据，以及达到设定的接收
//	数据长度的时候才会触发IRQ的RX_DR中断
//18.NRF采用PID（数据包识别）和CRC来避免同一数据包被重复接收，NRF每发送一个新的数据包，PID值就会自动加一，并将此PID值和CRC校验值
//	打包进数据包中
//19.在掉电模式下转入发射模式，或者接收模式时，必须等待1.5ms的待机模式1
//20.只有在关闭电源时，配置好的寄存器中的值才会丢失，要重新配置，否则不用重新配置，这一点需要特别注意，因为系统复位，不会让NRF模块
//	寄存器中的数据丢失。
//21.发送端NRF发送的有效数据长度TX_PLOAD_WIDTH必须和接收端NRF中配置的RX_PLOAD_WIDTH长度一致，否则每次数据包发送后，会由于接收端
//	NRF的接收通道的FIFO中的数据量未能达到设定的值，从而不能触发RX_DS中断
//22.对于FIFO_STATUS寄存器中RX_FULL和TX_FULL位描述的RX_FIFO寄存器已满和TX_FIFO寄存器已满，指的是存放的数据量达到32字节（FIFO寄存器
//	的最大值），所以当配置的有效数据的宽度小于32时，则此配置下RX_FULL和TX_FULL位在任何时候都不可能为1
//23.NRF处于发射模式时流程：写数据进入TX端的FIFO->处于发射模式，开始发送数据->等待应答信号->接收到应答信号—>自动清除FIFO中数据，并
//	触发TX_DS中断->清除STATUS中的对应的中断标志位->设定此时NRF是处于发射模式，接收模式还是待机模式（->若没有接收到应答信号->自动
//	重发->重发次数超过设定值时，触发MAR_RT中断）
//24.NRF处于接收模式时流程：处于接收模式->接收到正确的数据->自动发送应答信号->触发RX_DR中断->读取RX端中的FIFO数据，后NRF会自动清除
//	FIFO中的数据->清除STATUS中对应的中断标志位->选择NRF处于发射模式，接收模式还是待机模式
//25.需要注意：NRF模块对电源的要求比较高，特别是大功率的NRF，要求电源不能有大幅度的振幅，所以这个要加上滤波电容，并且当NRF的发射功
//	率和NRF的输入电压有密切关系，电压越高，发射功率越高，当电源较低的时候可能会出现发射不成功的现象，或者接收端NRF只接收到一次数据后，
//	就不能再接收数据了
//**************************************************************************************/
///*************************************************************************************
//nRF无线收发程序，以接收模式为主，当有数据要发送时，则切换到发射模式，并且发送完成后自动
//切换回接收模式，等待接收数据，同时当接收到数据时，LED9会闪烁

//注意：
//1.因为需要用到延迟函数，所以在NRF_Init()初始化前要先把systick_init()初始化
//2.因为在EXTI中要用到SYSTICK的中断函数，所以需要配置NVIC，进行中断优先级的配置

//主函数初始化代码为：
//	NRF_Config();
//***************************************************************************************/
//#include "nRF.h"
//#include "spi.h"
//#include "systick.h"
//#include "usart.h"
//#include "led.h"
//#include "deal_datapacket.h"

///**********************************************************************
//宏定义区
//***********************************************************************/
//#define CE_LOW					GPIOA->BRR |= GPIO_Pin_4
//#define CE_HIGH					GPIOA->BSRR |= GPIO_Pin_4

////==========================NRF24L01============================================
//#define TX_ADR_WIDTH    		5   		//这个地址是接收端的接收通道的地址的宽度
//#define RX_ADR_WIDTH    		5   		//这个是本机接收通道0的地址宽度
//#define TX_PLOAD_WIDTH  		11  		//要发送的有效数据长度，这个一般和接收端NRF的RX的FIFO设置值相对应
//#define RX_PLOAD_WIDTH  		12  		//要接收的有效数据长度，这个决定RX端FIFO达到多少数据量后触发中断
////=========================NRF24L01寄存器指令===================================
//#define READ_REG_CMD        	0x00  		// 读寄存器指令
//#define WRITE_REG_CMD       	0x20 		// 写寄存器指令
//#define RD_RX_PLOAD     		0x61  		// 读取接收数据指令
//#define WR_TX_PLOAD     		0xA0  		// 写待发数据指令
//#define FLUSH_TX        		0xE1 		// 冲洗发送 FIFO指令
//#define FLUSH_RX        		0xE2  		// 冲洗接收 FIFO指令
//#define REUSE_TX_PL     		0xE3  		// 定义重复装载数据指令
//#define NOP            			0xFF  		// 保留
////========================SPI(nRF24L01)寄存器地址===============================
//#define CONFIG          		0x00  		// 配置收发状态，CRC校验模式以及收发状态响应方式
//#define EN_AA           		0x01  		// 自动应答功能设置
//#define EN_RXADDR       		0x02  		// 可用信道设置
//#define SETUP_AW        		0x03  		// 收发地址宽度设置
//#define SETUP_RETR      		0x04  		// 自动重发功能设置
//#define RF_CH           		0x05  		// 工作频率设置
//#define RF_SETUP        		0x06  		// 发射速率、功耗功能设置
//#define STATUS          		0x07  		// 状态寄存器
//#define OBSERVE_TX      		0x08  		// 发送监测功能
//#define CD              		0x09  		// 地址检测           
//#define RX_ADDR_P0      		0x0A  		// 频道0接收数据地址
//#define RX_ADDR_P1      		0x0B  		// 频道1接收数据地址
//#define RX_ADDR_P2      		0x0C  		// 频道2接收数据地址
//#define RX_ADDR_P3      		0x0D  		// 频道3接收数据地址
//#define RX_ADDR_P4      		0x0E  		// 频道4接收数据地址
//#define RX_ADDR_P5      		0x0F  		// 频道5接收数据地址
//#define TX_ADDR         		0x10  		// 发送地址寄存器
//#define RX_PW_P0        		0x11 		// 接收频道0接收数据长度
//#define RX_PW_P1        		0x12  		// 接收频道0接收数据长度
//#define RX_PW_P2        		0x13  		// 接收频道0接收数据长度
//#define RX_PW_P3        		0x14  		// 接收频道0接收数据长度
//#define RX_PW_P4        		0x15  		// 接收频道0接收数据长度
//#define RX_PW_P5        		0x16  		// 接收频道0接收数据长度
//#define FIFO_STATUS     		0x17  		// FIFO栈入栈出状态寄存器设置
////=============================RF24l01状态=====================================

///******************************************************************************
//收发地址的说明：
//	发送端NRF在发送数据给接收端NRF，由接收端NRF的某个接收通道（一共有RX0~5个通道）接收发送的数据（以RX1为例说明），
//这样发送端NRF发送数据的时候需将接收端RX1通道的地址打包进数据包中，以便可以根据地址确定由接收端NRF哪个通道接收这个数据包。
//	同时，当接收端NRF使能了自动应答后，接收端NRF会自动切换到发送模式，并且它的TX端发送的应答数据包中也会将RX1通道的地址
//打包进数据包中，发送给发送端NRF，由于NRF接收应答信号的通道只能为RX0，所以，此时发送端NRF的RX0的地址也必须配置成接收端RX1
//通道的地址一致才能正确的接收应答信号
//******************************************************************************/
//u8 TX_ADDRESS[TX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};	//此地址用来识别接收端哪个RX通道可以接收发送出去的数据包
//u8 RX_ADDRESS[RX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};	//此地址用来配置本机NRF的RX0通道的地址，同时为了能正常收到应答信号，此地址一般都和上面的地址配置相同


////发生中断时，根据STATUS寄存器中的值来判断是哪个中断源触发了IRQ中断
//#define TX_DS			0x20								//数据发送完成中断
//#define RX_DR			0x40								//数据接收完成中断
//#define MAX_RT			0x10								//数据包重发次数超过设定值中断


////============================全局变量定义区==========================================
//vu8 sta;													//接收从STATUS寄存器中返回的值
//u8 RxBuf[RX_PLOAD_WIDTH];									//数据接收缓冲区

//extern u8 buttonFlag;										//指示哪个按键被按下，原始定义在deal_datapacket.c文件中
////====================================================================================



///**********************************************************************
//初始化NRF，及其IRQ引脚对应的EXTI中断
//***********************************************************************/
//void NRF_Config(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructus;
//	EXTI_InitTypeDef EXTI_initStructure;
//	
//	//初始化SPI接口
//	SPI_Config();
//	
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
//	
//	//初始化CE引脚
//	GPIO_InitStructus.GPIO_Pin = GPIO_Pin_4;
//	GPIO_InitStructus.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructus.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA,&GPIO_InitStructus);

//	//配置IRQ引脚对应的IO口
//	GPIO_InitStructus.GPIO_Pin = GPIO_Pin_0;
//	GPIO_InitStructus.GPIO_Mode = GPIO_Mode_IPU;							//nRF中断产生时，IRQ引脚会被拉低，所以这里要配置成上拉输入
//	GPIO_Init(GPIOB,&GPIO_InitStructus);

//	EXTI_initStructure.EXTI_Line = EXTI_Line0;
//	EXTI_initStructure.EXTI_LineCmd = ENABLE;
//	EXTI_initStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_initStructure.EXTI_Trigger = EXTI_Trigger_Falling;					//下降沿触发
//	EXTI_Init(&EXTI_initStructure);
//	
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);				//开启GPIO管脚的中断线路
//	NVIC_EnableIRQ(EXTI0_IRQn);
//	
//	
//	CE_LOW;																	//拉低CE，注意：读/写nRF寄存器均需要将CE拉低，使其进入待机或者掉电模式才可以
//	
//	//初始化NRF
//	SPI_Write_Byte(WRITE_REG_CMD + SETUP_AW, 0x03);							//配置通信地址的长度，默认值时0x03,即地址长度为5字节
//	SPI_Write_Buf(WRITE_REG_CMD + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    	//发送的数据包中被一块打包进去的接收端NRF的接收通道的地址
//	SPI_Write_Buf(WRITE_REG_CMD + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); 	//配置本机接收通道0的接收数据的地址
//	SPI_Write_Byte(WRITE_REG_CMD + SETUP_RETR, 0x1a); 						//自动重发延迟为500+86us，重发次数10次
//	SPI_Write_Byte(WRITE_REG_CMD + EN_AA, 0x01);      						//接收数据后，只允许频道0自动应答
//	SPI_Write_Byte(WRITE_REG_CMD + EN_RXADDR, 0x01);  						//只允许频道0接收数据
//	
//	//测试NRF作为发射端的时候是否能发出数据的测试代码，程序正常运行时，可以注释掉
////	SPI_Write_Byte(WRITE_REG_CMD + SETUP_RETR, 0x00);
////	SPI_Write_Byte(WRITE_REG_CMD + EN_AA, 0x00); 
////	SPI_Write_Byte(WRITE_REG_CMD + EN_RXADDR, 0x00); 
//	
//	SPI_Write_Byte(WRITE_REG_CMD + RF_SETUP, 0x07);   						//设置发射速率为1MHZ，发射功率为最大值0dB
//	SPI_Write_Byte(WRITE_REG_CMD + RF_CH, 30);        						//设置通道通信频率，工作通道频率可由以下公式计算得出：Fo=（2400+RF-CH）MHz.并且射频收发器工作的频率范围从2.400-2.525GHz
//	SPI_Write_Byte(WRITE_REG_CMD + RX_PW_P0, RX_PLOAD_WIDTH); 				//设置接收数据长度，本次设置为5字节，只有接收的数据达到此长度时，才会触发RX_DS中断
//	
//	SPI_Write_Byte(WRITE_REG_CMD + CONFIG, 0x0f);   						//默认处于接收模式

//	//读回配置信息，防止配置出错（调试代码的时候可以使用，程序正常运行代码时，注释掉这些打印代码）
////	printf("SETUP_AW:%x\r\n",SPI_Read_Byte(READ_REG_CMD+SETUP_AW));
////	printf("SETUP_RETR:%x\r\n",SPI_Read_Byte(READ_REG_CMD+SETUP_RETR));
////	printf("EN_RXADDR:%x\r\n",SPI_Read_Byte(READ_REG_CMD+EN_RXADDR));
////	printf("EN_AA:%x\r\n",SPI_Read_Byte(READ_REG_CMD+EN_AA));
////	printf("RF_SETUP:%x\r\n",SPI_Read_Byte(READ_REG_CMD+RF_SETUP));
////	printf("RF_CH:%x\r\n",SPI_Read_Byte(READ_REG_CMD+RF_CH));
////	printf("RX_PW_P0:%x\r\n",SPI_Read_Byte(READ_REG_CMD+RX_PW_P0));
////	printf("CONFIG:%x\r\n",SPI_Read_Byte(READ_REG_CMD+CONFIG));

//	CE_HIGH;
//}

///*****************************************************************************************
//IRQ引脚对应的EXTI中断处理函数
//*******************************************************************************************/
//void EXTI0_IRQHandler(void)
//{
//	CE_LOW;													//拉低CE，以便读取NRF中STATUS中的数据
//	sta = SPI_Read_Byte(READ_REG_CMD+STATUS);				//读取STATUS中的数据，以便判断是由什么中断源触发的IRQ中断
//	
//	if(sta & TX_DS){										//数据发送成功，并且收到了应答信号
//		RX_Mode();											//将NRF的模式改为接收模式，等待接收数据
//	}else if(sta & RX_DR){									//数据接收成功
//		NRF_ReceivePacket();								//将数据从RX端的FIFO中读取出来，
//		if(buttonFlag & 0x01){
//			LED_On(LED9);									//LED9常亮表示飞机解锁成功，并且和遥控器正在进行数据通讯
//		}else{
//			LED_Toggle(LED9);								//LED9闪烁表示飞机处于加锁模式，但是和遥控器通讯成功
//		}
//	}else if(sta & MAX_RT){									//触发了最大重发MAX_RT中断
//		RX_Mode();											//将NRF的模式改为接收模式
//		SPI_Write_Byte(WRITE_REG_CMD+STATUS,sta);			//清除MAX_RT中断
//	}
//	
//	EXTI_ClearITPendingBit(EXTI_Line0);
//}



///**********************************************************************
//配置NRF为RX模式，准备开始接收数据
//***********************************************************************/
//void RX_Mode(void)
//{
//	CE_LOW;													//拉低CE，进入待机模式，准备开始往NRF中的寄存器中写入数据
//	
//	SPI_Write_Byte(WRITE_REG_CMD + CONFIG, 0x0f); 			//配置为接收模式
//	SPI_Write_Byte(WRITE_REG_CMD + STATUS, 0x7e);			//写0111 xxxx 给STATUS，清除所有中断标志，防止一进入接收模式就触发中断
//	
//	CE_HIGH; 												//拉高CE，准备接受从外部发送过来的数据
//}


///**********************************************************************
//从NRF的RX的FIFO中读取一组数据包
//输入参数rx_buf:保存从FIFO中读取到的数据的区域首地址
//***********************************************************************/
//void NRF_ReceivePacket(void)
//{
//	CE_LOW;
//	
//	SPI_Read_Buf(RD_RX_PLOAD,RxBuf,RX_PLOAD_WIDTH);			//从RX端的FIFO中读取数据，并存入指定的区域，注意：读取完FIFO中的数据后，NRF会自动清除其中的数据
//	SPI_Write_Byte(WRITE_REG_CMD+STATUS,sta);   			//清楚中断标志
//	
//	UnpackData();											//将接收到的数据解包，处理数据，并分装
//	
//	CE_HIGH;												//重新拉高CE，让其重新处于接收模式，准备接收下一个数据
//}


///**********************************************************************
//配置NRF为TX模式，并发送一个数据包
//输入参数tfbuf:即将要发送出去的数据区首地址
//***********************************************************************/
//void NRF_SendPacket(u8* tfbuf)
//{
//	CE_LOW;																	//拉低CE，进入待机模式，准备开始往NRF中的寄存器中写入数据
//	
//	//SPI_Write_Buf(WRITE_REG_CMD + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); 	//装载接收端地址，由于这里只有一个通道通讯，不用改变接收端的NRF的接收通道地址,所以，这句可以注释掉
//	SPI_Write_Buf(WR_TX_PLOAD, tfbuf, TX_PLOAD_WIDTH); 						//将数据写入TX端的FIFO中,写入的个数与TX_PLOAD_WIDTH设置值相同
//	
//	SPI_Write_Byte(WRITE_REG_CMD + CONFIG, 0x0e); 							//将NRF配置成发射模式
//	SPI_Write_Byte(WRITE_REG_CMD + STATUS, 0x7e);							//写0111 xxxx 给STATUS，清除所有中断标志，防止一进入发射模式就触发中断
//	
//	CE_HIGH;																//拉高CE，准备发射TX端FIFO中的数据
//	
//	delay_ms(1);															//CE拉高后，需要延迟至少130us
//}





