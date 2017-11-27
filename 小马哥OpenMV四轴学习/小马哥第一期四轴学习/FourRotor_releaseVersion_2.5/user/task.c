/***************************************************************************************
									声明
本项目代码仅供个人学习使用，可以自由移植修改，但必须保留此声明信息。移植过程中出现其他不可
估量的BUG，修远智控不负任何责任。请勿商用！

程序版本号：	2.5
日期：			2017-5-1
作者：			东方萧雨、小马哥
版权所有：		修远智控N0.1实验室
****************************************************************************************/
#include "task.h"
#include "mpu_dmp_useapi.h"
#include "control.h"
#include "global.h"
#include "led.h"
#include "nrf.h"

//========================全局变量定义区=======================================
extern TaskMask_s TaskMask;
extern DataPacket_s DataPacket;


//PID运算及其控制输出任务
void Task_PIDControl(void)
{
	//更新角度数据
	Get_Angle();
	
	//PID运算
	Direction_Control();
	Outter_PID();
	Inner_PID();
	Rotation_Correction();
	
	//处理PWM数值，根据油门和是否解锁飞机来判断是否正常起飞，以及进行限幅
	DealPwm();
	
	//将处理好的PWM数值设置给4个电机
	Set_Pwm();
	
	//关闭此任务
	TaskMask.angleDataReady = 0;
}


//检测飞机和遥控器的连接是否断开任务
void Task_CheckSingle(void)
{
	//这个非常重要，防止飞机脱离遥控器的控制而无限制的飞行
	if(DataPacket.dataPID_Last == DataPacket.dataPID){					//这里表明飞机已经和遥控器脱离控制了
		//当飞机脱离遥控器控制时，自动把油门输出值限定到1250
		if(DataPacket.accelerator-1250>=800.0){
			DataPacket.accelerator -= 800;
		}else{
			DataPacket.accelerator = 1250;
		}
		
		DataPacket.leftRight = 15;						//当飞机脱离遥控器时，自动把左右方向值给15
		DataPacket.upDown = 15;							//当飞机脱离遥控器时，自动把前后方向值给15
		LED_Off(LED9);									//LED9灯灭，表明和遥控器通讯失败
	}else{
		DataPacket.dataPID_Last = DataPacket.dataPID;						//这里表明飞机和遥控器通讯正常，将接收到的dataPID覆盖掉dataPID_Last之前的值
	}
	
	//关闭此任务
	TaskMask.checkSingle = 0;
}


//nrf无线数据收发处理任务
void Task_NrfDataReady(void)
{
	NRF_DataReady();
	TaskMask.nrfDataReady = 0;
}


