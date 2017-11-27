#ifndef _TASK_H
#define _TASK_H

#include "stm32f10x.h"

void Task_PIDControl(void);
void Task_CheckSingle(void);
void Task_NrfDataReady(void);

#endif
