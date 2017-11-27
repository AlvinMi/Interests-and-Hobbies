#ifndef _GLOBAL__H
#define _GLOBAL__H

#include "stm32f10x.h"

//指示任务是否开启结构体
typedef struct _TaskMask_s_{
	uint16_t checkSingle:		1;
	uint16_t angleDataReady:	1;
	uint16_t nrfDataReady:		1;
	uint16_t reserve:			13;
}TaskMask_s;


//接收到的数据包结构体
typedef struct _DataPacket_s_{
	u8 buttonFlag;						//指示哪个按键被按下，0x00的时候表示4个按键都没有被触发，0000 1111各表示KEY1~KEY4按键被按下
	vu16 accelerator;					//保存油门值
	vu16 yawSV;							//保存航向角修正值
	vu16 upDown;						//保存上下方向值
	vu16 leftRight;						//保存左右方向值
	u8 dataPID_Last;					//保存上一次接收到的数据包识别PID
	u8 dataPID;							//保存接收到的数据包识别PID值
}DataPacket_s;

//PID给定值结构体
typedef struct _SetValue_s_{
	volatile float pit;					//pitch角给定值，用于控制方向和保持平衡
	volatile float rol;					//roll角给定值，用于控制方向和保持平衡
	volatile float yaw;					//偏航角给定值，用于修正航向角和克服自转问题
	volatile float angularSpeed_X;		//内环x轴角速度给定值
	volatile float angularSpeed_Y;		//内环y轴角速度给定值
}SetValue_s;

//MPU6050测量值结构体
typedef struct _MeasuredValue_s_{
	float pit;							//俯仰角
	float rol;							//横滚角,
	float yaw;							//航向角
	vs16 angularSpeed_X;				//x轴的角速度值（这是转化后的实际值，不用原始数据值，起到一定的滤波的作用）
	vs16 angularSpeed_Y;				//y轴角速度值（这是转化后的实际值，不用原始数据值，起到一定的滤波的作用）
}MeasuredValue_s;

//PID运算后输出的PWM值
typedef struct _PID_PwmOutput_s_{
	vs16 pwm_X;							//内环PID运算后X轴输出的PWM值
	vs16 pwm_Y;							//内环PID运算后Y轴输出的PWM值
	vs16 pwm_Yaw;						//偏航修正PWM补偿
}PID_PwmOutput_s;


//MPU6050原始数据
typedef struct _RawData_MPU_s_{
	short gyro[3];
	short accel[3];
}RawData_MPU_s;


#endif 
