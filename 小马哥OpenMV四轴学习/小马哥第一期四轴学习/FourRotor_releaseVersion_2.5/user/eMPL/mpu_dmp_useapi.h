#ifndef _MPU_DMP_USEAPI_H
#define _MPU_DMP_USEAPI_H

#include "stm32f10x.h"

void gyro_data_ready_cb(void);
void SetReportFlag(u8 rawDataReportCmd,u8 quatDataReportCmd);

u8 mpu_dmp_init(void);
u8 dmp_getdata(void);


#endif
