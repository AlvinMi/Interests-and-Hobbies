/***********************************************************************************
* ģ�� I2C
* ֻ�������Ҫ�޸�������޸�����: I2C_PORT,I2C_SCL,I2C_SDA,RCC_I2C_PORT
*
* ע��:
* 1.��ʼ�� I2C_Simulation() ����ǰ, ���ȳ�ʼ�� SysTick_init(), �Ա����ʹ����ʱ����
************************************************************************************/
#include "I2C_Simulation.h"
#include "systick.h"

/***********************************************************************************
* Code transplant modify:
* ֻ��Ҫ����ģ�� I2C ʹ�õĶ˿ڼ���
************************************************************************************/
#define I2C_PORT					GPIOB
#define I2C_SCL						GPIO_Pin_6
#define I2C_SDA						GPIO_Pin_7
#define RCC_I2C_PORT			RCC_APB2Periph_GPIOB

/*----------------------------------------------------------------------------------*/
// ���ƹܽŵĵ�ƽ״̬
#define I2C_SCL_LOW				(I2C_PORT->BRR  |= I2C_SCL)
#define I2C_SCL_HIGHT			(I2C_PORT->BSRR |= I2C_SCL)
#define I2C_SDA_LOW				(I2C_PORT->BRR	|= I2C_SDA)
#define I2C_SDA_HIGHT			(I2C_PORT->BSRR	|= I2C_SDA)

// ��ȡ�ܽŵĵ�ƽ״̬
#define I2C_SDA_READ			(I2C_PORT->IDR & I2C_SDA)

/*********************************** ������ ***************************************/

// I2C ��ʼ������
void I2C_SimulationConfig(void)
{
	GPIO_InitTypeDef GPIO_initStruct;
	
	RCC_APB2PeriphClockCmd(RCC_I2C_PORT,ENABLE);
	
	GPIO_initStruct.GPIO_Pin = I2C_SCL|I2C_SDA;
	GPIO_initStruct.GPIO_Mode = GPIO_Mode_Out_OD;		// ����Ϊͨ�ÿ�©���
	GPIO_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_PORT,&GPIO_initStruct);
	
	I2C_SCL_LOW;
	delay_us(1);
	I2C_SDA_HIGHT;
	delay_us(1);
}

// �ڲ�����, �ж������Ƿ��ڿ���״̬; ������, ��ѭ�����ṩ SCL ʱ������, ֱ���ӻ��ͷ� SDA ��
void _I2C_IsBusy(void)
{
	// ��ȡ SDA ���ϵĵ�ƽ״̬, ��Ϊ����˵�����߱��ӻ�����; ��Ϊ����˵�����߿���, ����׼�����Ϳ�ʼ����
	while(!I2C_SDA_READ)
	{
		I2C_SCL_LOW;
		delay_us(3);
		I2C_SCL_HIGHT;
		delay_us(3);
	}
}

// ������ʼ(start)�ź�
void I2C_Start(void)
{
	// �ж��������Ƿ��ڿ���״̬.
	_I2C_IsBusy();
	
	// ���� SCL �еĵ�ƽΪ��, ��ֹ��Ϊ SCL ���ڸߵ�ƽ��ʹ���潫 SDA ����ʱ, ���ܻᴥ��һ�� stop �ź�
	I2C_SCL_LOW;
	delay_us(1);
	
	I2C_SDA_HIGHT;
	delay_us(1);
	I2C_SCL_HIGHT;
	delay_us(1);
	I2C_SDA_LOW;
	delay_us(1);
	
	// �� SCL ����, ǯס SCL ��, ׼�����͵�ַ����
	I2C_SCL_LOW;
	delay_us(1);
}

// ����ֹͣ(stop)�ź�
void I2C_Stop(void)
{
	// ͬ��, ������ SCL ��, ��Ϊ SCL ��Ϊ�͵�ʱ��, SDA ����ı䶼�������Ӱ��
	I2C_SCL_LOW;
	delay_us(1);
	
	I2C_SDA_LOW;
	delay_us(1);
	I2C_SCL_HIGHT;
	delay_us(1);
	I2C_SDA_HIGHT;
	delay_us(1);
}

// ����Ӧ���źŻ��߷�Ӧ���ź�, ackState->Ϊ ENABLE ʱ, �����Ӧ���ź�
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

// ��ȡӦ���ź�(ENABLE)���߷�Ӧ���ź�(DISABLE)
FunctionalState	I2C_GetAck(void)
{
	FunctionalState ask;	 // ��¼ STM32 �յ����ź��� ENABLE or DISENABLE �ľֲ�����
	
	I2C_SCL_HIGHT;
	delay_us(1);
	
	// ��ȡ SDA ���ϵĵ�ƽ״̬
	if(I2C_SDA_READ)
	{
			ask = DISABLE;
	}else{
			ask = ENABLE;
	}
	
	I2C_SCL_LOW;		// ����֮��, ��ȡ����
	delay_us(1);
	
	return ask;
}

// д�����ݸ��ӻ�, ������Ӧ����߷�Ӧ���ź�
FunctionalState SimulationI2C_WriteByte(u8 data)
{
	u8 i;
	
	// ����λ�ƼĴ����Ĺ���, ������ͨ�� I/O ���ͳ�ȥ
	for(i=0; i<8; i++)
	{
		I2C_SCL_LOW;		// ��������
		delay_us(1);
		
		if(data & 0x80)	// data & 1000 0000; Ϊ���ж�
		{
			I2C_SDA_HIGHT;
		}else{
			I2C_SDA_LOW;
		}
		data <<= 1;
		delay_us(1);
		
		I2C_SCL_HIGHT;		// �� SCL ����, ��ȡ����
		delay_us(2);
	}
	
	// �����ͷ� SDA ��, ʹ�����߿���, �Ա� MPU6050 �ܹ�������Ӧ��Ϣ, ��ǯס SCL ��
	I2C_SCL_LOW;		// ���ȵ����� SCL ��
	delay_us(1);
	I2C_SDA_HIGHT;	// SDA Ϊ��, ���ͷŵ�, ���߿���
	delay_us(1);
	
	return I2C_GetAck();
}

// ��ȡ�ӻ����͵�����, ��������Ӧ���źŻ��Ƿ�Ӧ���ź�
u8 SimulationI2C_ReadByte(FunctionalState ackState)
{
	u8 i;
	u8 data = 0x00;
	
	// ����λ�ƼĴ����Ĺ���, �����ݴ� I/O ���ж�ȡ����
	for(i=0; i<8; i++)
	{
		I2C_SCL_HIGHT;
		delay_us(1);
		
		// ����λ, ��ֵ
		data <<= 1;
		
		if(I2C_SDA_READ)
		{
			data |= 0X01;
		}
		
		I2C_SCL_LOW;
		delay_us(2);
	}
	
	// ������Ӧ (���߷���Ӧ) �� MPU6050
	I2C_SetAck(ackState);
	return data;
}

/*****************************************************************************
* ���ӻ��еļĴ���д��һ���ֽڵ�����
* addr: �豸��ַ
* reg:  �Ĵ�����ַ
* data: Ҫд�������
* ����ֵ: 0 Ϊ����д��, 1 Ϊд���쳣
******************************************************************************/
u8 SimulationI2C_WriteByteToSlave(u8 addr, u8 reg, u8 data)
{
	FunctionalState	state;		// ������¼ ACK ���� NACK
	
	I2C_Start();					//��������ʼ�ź�
	state = SimulationI2C_WriteByte(addr<<1|0);	// ���ӻ���ַ��д����д������
	if(state == ENABLE)													// �жϴӻ��Ƿ�
	{
		state = SimulationI2C_WriteByte(reg);			// ��Ҫд�����ݵļĴ�����ַ
		if(state == ENABLE)
		{
			SimulationI2C_WriteByte(data);					// �����ݷ��͸��ӻ�
			
			// ����д��, ����ֹͣ�ź�, ������ 0
			I2C_Stop();
			return 0;
		}
	}
	
	// д���쳣, ����ֹͣ�ź�, ������ 1
	I2C_Stop();
	return 1;
}

/************************************************************************************************
* ���ӻ��еļĴ���д��������
* addr: �豸�ĵ�ַ
* reg:  �׸�д�����ݵļĴ�����ַ
* len:  Ҫд������ݵĸ���
* buf�� Ҫд������������׵�ַ
* ����ֵ:��0Ϊ����д��, 1 Ϊд���쳣
*************************************************************************************************/
u8 SimulationI2C_WriteSomeDataToSlave(u8 addr, u8 reg, u8 len, u8 *buf)
{
	FunctionalState state;				// ������¼ ACK ���� NACK
	u8 i;
	
	I2C_Start();
	state = SimulationI2C_WriteByte(addr<<1|0);			// ���ӻ���ַ��д����д������
	if(state == ENABLE)															// �жϴӻ��Ƿ���Ӧ�˵�ַ
	{
		state = SimulationI2C_WriteByte(reg);					// ��Ҫд��������׸��Ĵ�����ַ���͸��ӻ�
		if(state == ENABLE)
		{
			for(i=0; i<len; i++)												// ��ʼ�������������ݷ��͸��ӻ�
			{
				state = SimulationI2C_WriteByte(*(buf+i));// �����ݷ��͸��ӻ�
				if(state == DISABLE)
				{
					I2C_Stop();					// �ӻ�δӦ��, ����ֹͣ�źŽ������ݴ���, ������1
					return 1;
				}
			}			
			// ��������ֹͣ�ź�, ����0
			I2C_Stop();
			return 0;
		}
	}
	// д���쳣, ���������ź�, ����1
	I2C_Stop();
	return 1;
}

/************************************************************************************************
* ���ӻ��еļĴ�����ȡһ���ֽڵ�����
* addr: �豸�ĵ�ַ
* reg:  �Ĵ�����ַ
* buf�� ��ȡ�������ݴ�����ڴ���
* ����ֵ:��0Ϊ����д��, 1 Ϊд���쳣
*************************************************************************************************/
u8 SimulationI2C_ReadFromSlave(u8 addr, u8 reg, u8 *buf)
{
	FunctionalState state;
	
	I2C_Start();																			// ������ʼ�ź�
	state = SimulationI2C_WriteByte(addr<<1|0);				// ���ʹӻ���ַ��д����
	if(state == ENABLE)
	{
		state = SimulationI2C_WriteByte(reg);					// ����Ҫ��ȡ���ݵļĴ�����ַ
		if(state == ENABLE)
		{
			I2C_Start();																// ������ʼ�ź�
			state = SimulationI2C_WriteByte(addr<<1|1);	// ���ʹӻ���ַ�Ͷ�����
			if(state == ENABLE)
			{
				*buf = SimulationI2C_ReadByte(DISABLE);		// ��ȡ��һ�����ݺ�, ��ӻ�����һ�� NACK �ź�, �����ӻ���������
				
				// ������ȡ����,  ����ֹͣ�ź�, ������ 0
				I2C_Stop();
				return 0;
			}
		}
	}
	
	// ��ȡ�����쳣, ���� STOP �ź�, ������0
	I2C_Stop();
	return 1;
}

/************************************************************************************************
* �Ӵӻ��ж�ȡ�������ĸ� 8 λ�͵� 8 λ����
* addr: �����ĵ�ַ
* reg:  Ҫ��ȡ���ݵ��׸����ݵļĴ�����ַ
* len:  ��ȡ�����ݸ���
* buf�� ��ȡ���� 8 λ���ݴ�����ĵ�ַ, ��ͨ��ָ��ķ�ʽ����ȡ�������ݷ��ظ�������
* ����ֵ:��0Ϊ����д��, 1 Ϊд���쳣
*************************************************************************************************/
u8 SimulationI2C_ReadSomeDataFromSlave(u8 addr, u8 reg, u8 len, u8 *buf)
{
	FunctionalState state;
	
	I2C_Start();
	state = SimulationI2C_WriteByte(addr<<1|0);										// д��ӻ���ַ��д����
	if(state == ENABLE)
	{
		state = SimulationI2C_WriteByte(reg);												// д��Ҫ��ȡ���ݵ��׸��Ĵ����ĵ�ַ
		if(state == ENABLE)
		{
			I2C_Start();
			state = SimulationI2C_WriteByte(addr<<1|1);								// ���·�����ʼ�ź�, תΪ������, ��ʼ��ȡ����
			if(state == ENABLE)
			{
				while(len)
				{
					if(len == 1)
					{
						*buf = SimulationI2C_ReadByte(DISABLE);							// �����һ�����ݽ���ʱ, ���ӻ�����һ�� NACK �ź�
					}else{
						*buf = SimulationI2C_ReadByte(ENABLE);							// ��һ�����ݽ���ʱ, ���ӻ�����һ�� ACK �ź�
					}
					
					len--;
					buf++;
				}
				
				// ����������ȡ, ����ֹͣ�ź�, �����˴����ݵĴ���, ������ 0 
				I2C_Stop();
				return 0;
			}
		}
	}
	
	// ���ݶ�ȡ�쳣, ���� stop �ź�, �����˴����ݶ�ȡ
	I2C_Stop();
	return 1;
}

