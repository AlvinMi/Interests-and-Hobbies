/***********************************************************************************
* 模拟 I2C
* 只需根据需要修改下面的修改区的: I2C_PORT,I2C_SCL,I2C_SDA,RCC_I2C_PORT
*
* 注意:
* 1.初始化 I2C_Simulation() 函数前, 请先初始化 SysTick_init(), 以便可以使用延时函数
************************************************************************************/
#include "I2C_Simulation.h"
#include "systick.h"

/***********************************************************************************
* Code transplant modify:
* 只需要更改模拟 I2C 使用的端口即可
************************************************************************************/
#define I2C_PORT					GPIOB
#define I2C_SCL						GPIO_Pin_6
#define I2C_SDA						GPIO_Pin_7
#define RCC_I2C_PORT			RCC_APB2Periph_GPIOB

/*----------------------------------------------------------------------------------*/
// 控制管脚的电平状态
#define I2C_SCL_LOW				(I2C_PORT->BRR  |= I2C_SCL)
#define I2C_SCL_HIGHT			(I2C_PORT->BSRR |= I2C_SCL)
#define I2C_SDA_LOW				(I2C_PORT->BRR	|= I2C_SDA)
#define I2C_SDA_HIGHT			(I2C_PORT->BSRR	|= I2C_SDA)

// 读取管脚的电平状态
#define I2C_SDA_READ			(I2C_PORT->IDR & I2C_SDA)

/*********************************** 函数区 ***************************************/

// I2C 初始化配置
void I2C_SimulationConfig(void)
{
	GPIO_InitTypeDef GPIO_initStruct;
	
	RCC_APB2PeriphClockCmd(RCC_I2C_PORT,ENABLE);
	
	GPIO_initStruct.GPIO_Pin = I2C_SCL|I2C_SDA;
	GPIO_initStruct.GPIO_Mode = GPIO_Mode_Out_OD;		// 必须为通用开漏输出
	GPIO_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_PORT,&GPIO_initStruct);
	
	I2C_SCL_LOW;
	delay_us(1);
	I2C_SDA_HIGHT;
	delay_us(1);
}

// 内部函数, 判断总线是否处于空闲状态; 若不是, 则循环的提供 SCL 时钟驱动, 直到从机释放 SDA 线
void _I2C_IsBusy(void)
{
	// 读取 SDA 线上的电平状态, 若为低则说明总线被从机控制; 若为高则说明总线空闲, 可以准备发送开始条件
	while(!I2C_SDA_READ)
	{
		I2C_SCL_LOW;
		delay_us(3);
		I2C_SCL_HIGHT;
		delay_us(3);
	}
}

// 产生起始(start)信号
void I2C_Start(void)
{
	// 判断下总线是否处于空闲状态.
	_I2C_IsBusy();
	
	// 先让 SCL 中的电平为低, 防止因为 SCL 处于高电平而使后面将 SDA 拉高时, 可能会触发一个 stop 信号
	I2C_SCL_LOW;
	delay_us(1);
	
	I2C_SDA_HIGHT;
	delay_us(1);
	I2C_SCL_HIGHT;
	delay_us(1);
	I2C_SDA_LOW;
	delay_us(1);
	
	// 将 SCL 拉低, 钳住 SCL 线, 准备发送地址数据
	I2C_SCL_LOW;
	delay_us(1);
}

// 产生停止(stop)信号
void I2C_Stop(void)
{
	// 同理, 先拉低 SCL 线, 因为 SCL 线为低的时候, SDA 随意改变都不会造成影响
	I2C_SCL_LOW;
	delay_us(1);
	
	I2C_SDA_LOW;
	delay_us(1);
	I2C_SCL_HIGHT;
	delay_us(1);
	I2C_SDA_HIGHT;
	delay_us(1);
}

// 产生应答信号或者非应答信号, ackState->为 ENABLE 时, 则产生应答信号
void I2C_SetAck(FunctionalState		ackState)
{
	I2C_SCL_LOW;
	delay_us(1);
	
	if(ackState == ENABLE)
	{
		I2C_SDA_LOW;
		delay_us(1);
	}else{
		I2C_SDA_HIGHT;
		delay_us(1);
	}
	
	I2C_SCL_HIGHT;
	delay_us(1);
	
	I2C_SCL_LOW;
	delay_us(1);
	
	I2C_SDA_HIGHT;
	delay_us(1);
}

// 获取应答信号(ENABLE)或者非应答信号(DISABLE)
FunctionalState	I2C_GetAck(void)
{
	FunctionalState ask;	 // 记录 STM32 收到的信号是 ENABLE or DISENABLE 的局部变量
	
	I2C_SCL_HIGHT;
	delay_us(1);
	
	// 读取 SDA 线上的电平状态
	if(I2C_SDA_READ)
	{
			ask = DISABLE;
	}else{
			ask = ENABLE;
	}
	
	I2C_SCL_LOW;		// 拉低之后, 读取数据
	delay_us(1);
	
	return ask;
}

// 写出数据给从机, 并返回应答或者非应答信号
FunctionalState SimulationI2C_WriteByte(u8 data)
{
	u8 i;
	
	// 类似位移寄存器的功能, 将数据通过 I/O 发送除去
	for(i=0; i<8; i++)
	{
		I2C_SCL_LOW;		// 首先拉低
		delay_us(1);
		
		if(data & 0x80)	// data & 1000 0000; 为了判断
		{
			I2C_SDA_HIGHT;
		}else{
			I2C_SDA_LOW;
		}
		data <<= 1;
		delay_us(1);
		
		I2C_SCL_HIGHT;		// 将 SCL 拉高, 读取数据
		delay_us(2);
	}
	
	// 主机释放 SDA 线, 使得总线空闲, 以便 MPU6050 能够发出响应信息, 并钳住 SCL 线
	I2C_SCL_LOW;		// 首先得拉低 SCL 线
	delay_us(1);
	I2C_SDA_HIGHT;	// SDA 为高, 即释放掉, 总线空闲
	delay_us(1);
	
	return I2C_GetAck();
}

// 读取从机发送的数据, 并决定是应答信号还是非应答信号
u8 SimulationI2C_ReadByte(FunctionalState ackState)
{
	u8 i;
	u8 data = 0x00;
	
	// 类似位移寄存器的功能, 将数据从 I/O 口中读取进来
	for(i=0; i<8; i++)
	{
		I2C_SCL_HIGHT;
		delay_us(1);
		
		// 先移位, 后赋值
		data <<= 1;
		
		if(I2C_SDA_READ)
		{
			data |= 0X01;
		}
		
		I2C_SCL_LOW;
		delay_us(2);
	}
	
	// 发送响应 (或者非响应) 给 MPU6050
	I2C_SetAck(ackState);
	return data;
}

/*****************************************************************************
* 往从机中的寄存器写入一个字节的数据
* addr: 设备地址
* reg:  寄存器地址
* data: 要写入的数据
* 返回值: 0 为正常写入, 1 为写入异常
******************************************************************************/
u8 SimulationI2C_WriteByteToSlave(u8 addr, u8 reg, u8 data)
{
	FunctionalState	state;		// 用来记录 ACK 还是 NACK
	
	I2C_Start();					//　产生起始信号
	state = SimulationI2C_WriteByte(addr<<1|0);	// 将从机地址和写方向写入总线
	if(state == ENABLE)													// 判断从机是否
	{
		state = SimulationI2C_WriteByte(reg);			// 将要写入数据的寄存器地址
		if(state == ENABLE)
		{
			SimulationI2C_WriteByte(data);					// 将数据发送给从机
			
			// 正常写入, 产生停止信号, 并返回 0
			I2C_Stop();
			return 0;
		}
	}
	
	// 写入异常, 产生停止信号, 并返回 1
	I2C_Stop();
	return 1;
}

/************************************************************************************************
* 往从机中的寄存器写入多个数据
* addr: 设备的地址
* reg:  首个写入数据的寄存器地址
* len:  要写入的数据的个数
* buf： 要写入的数据区的首地址
* 返回值:　0为正常写入, 1 为写入异常
*************************************************************************************************/
u8 SimulationI2C_WriteSomeDataToSlave(u8 addr, u8 reg, u8 len, u8 *buf)
{
	FunctionalState state;				// 用来记录 ACK 还是 NACK
	u8 i;
	
	I2C_Start();
	state = SimulationI2C_WriteByte(addr<<1|0);			// 将从机地址和写方向写入总线
	if(state == ENABLE)															// 判断从机是否响应了地址
	{
		state = SimulationI2C_WriteByte(reg);					// 将要写入的数据首个寄存器地址发送给从机
		if(state == ENABLE)
		{
			for(i=0; i<len; i++)												// 开始将数据区的数据发送给从机
			{
				state = SimulationI2C_WriteByte(*(buf+i));// 将数据发送给从机
				if(state == DISABLE)
				{
					I2C_Stop();					// 从机未应答, 产生停止信号结束数据传输, 并返回1
					return 1;
				}
			}			
			// 正常产生停止信号, 返回0
			I2C_Stop();
			return 0;
		}
	}
	// 写入异常, 产生结束信号, 返回1
	I2C_Stop();
	return 1;
}

/************************************************************************************************
* 往从机中的寄存器读取一个字节的数据
* addr: 设备的地址
* reg:  寄存器地址
* buf： 读取到的数据储存的内存区
* 返回值:　0为正常写入, 1 为写入异常
*************************************************************************************************/
u8 SimulationI2C_ReadFromSlave(u8 addr, u8 reg, u8 *buf)
{
	FunctionalState state;
	
	I2C_Start();																			// 产生起始信号
	state = SimulationI2C_WriteByte(addr<<1|0);				// 发送从机地址和写方向
	if(state == ENABLE)
	{
		state = SimulationI2C_WriteByte(reg);					// 发送要读取数据的寄存器地址
		if(state == ENABLE)
		{
			I2C_Start();																// 发送起始信号
			state = SimulationI2C_WriteByte(addr<<1|1);	// 发送从机地址和读方向
			if(state == ENABLE)
			{
				*buf = SimulationI2C_ReadByte(DISABLE);		// 读取完一个数据后, 向从机发送一个 NACK 信号, 结束从机发送数据
				
				// 正常读取数据,  产生停止信号, 并返回 0
				I2C_Stop();
				return 0;
			}
		}
	}
	
	// 读取数据异常, 产生 STOP 信号, 并返回0
	I2C_Stop();
	return 1;
}

/************************************************************************************************
* 从从机中读取采样到的高 8 位和低 8 位数据
* addr: 器件的地址
* reg:  要读取数据的首个数据的寄存器地址
* len:  读取的数据个数
* buf： 读取到的 8 位数据存放区的地址, 即通过指针的方式将读取到的数据返回给调用者
* 返回值:　0为正常写入, 1 为写入异常
*************************************************************************************************/
u8 SimulationI2C_ReadSomeDataFromSlave(u8 addr, u8 reg, u8 len, u8 *buf)
{
	FunctionalState state;
	
	I2C_Start();
	state = SimulationI2C_WriteByte(addr<<1|0);										// 写入从机地址和写方向
	if(state == ENABLE)
	{
		state = SimulationI2C_WriteByte(reg);												// 写入要读取数据的首个寄存器的地址
		if(state == ENABLE)
		{
			I2C_Start();
			state = SimulationI2C_WriteByte(addr<<1|1);								// 重新发送起始信号, 转为读方向, 开始读取数据
			if(state == ENABLE)
			{
				while(len)
				{
					if(len == 1)
					{
						*buf = SimulationI2C_ReadByte(DISABLE);							// 当最后一个数据接收时, 给从机发送一个 NACK 信号
					}else{
						*buf = SimulationI2C_ReadByte(ENABLE);							// 当一个数据接收时, 给从机发送一个 ACK 信号
					}
					
					len--;
					buf++;
				}
				
				// 数据正常读取, 产生停止信号, 结束此次数据的传输, 并返回 0 
				I2C_Stop();
				return 0;
			}
		}
	}
	
	// 数据读取异常, 产生 stop 信号, 结束此次数据读取
	I2C_Stop();
	return 1;
}

