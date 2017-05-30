#ifndef	 __I2C_SIMULATION_H
#define  __I2C_SIMULATION_H

#include "stm32f10x.h"

void I2C_SimulationConfig(void);
void I2C_Start(void);
void I2C_Stop(void);

void _I2C_IsBusy(void);
void I2C_SetAck(FunctionalState		ackState);
FunctionalState	I2C_GetAck(void);
FunctionalState SimulationI2C_WriteByte(u8 data);
u8 SimulationI2C_ReadByte(FunctionalState ackState);
u8 SimulationI2C_WriteByteToSlave(u8 addr, u8 reg, u8 data);
u8 SimulationI2C_WriteSomeDataToSlave(u8 addr, u8 reg, u8 len, u8 *buf);
u8 SimulationI2C_ReadFromSlave(u8 addr, u8 reg, u8 *buf);
u8 SimulationI2C_ReadSomeDataFromSlave(u8 addr, u8 reg, u8 len, u8 *buf);
#endif

