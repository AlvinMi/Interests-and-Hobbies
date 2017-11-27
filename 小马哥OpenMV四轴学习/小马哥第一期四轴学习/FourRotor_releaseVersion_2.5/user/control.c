/***************************************************************************************
									声明
本项目代码仅供个人学习使用，可以自由移植修改，但必须保留此声明信息。移植过程中出现其他不可
估量的BUG，修远智控不负任何责任。请勿商用！

程序版本号：	2.5
日期：			2017-5-1
作者：			东方萧雨、小马哥
版权所有：		修远智控N0.1实验室
****************************************************************************************/
#include "control.h"
#include "moto_pwm.h"
#include "usart.h"
#include "global.h"
#include "mpu_dmp_useapi.h"

#define GYRO_XISHU			(2000.0/65535.0)		//用来将GYRO原始值转换为实际角速度的比例系数，2000为量程

/*********************************************************************************
全局变量定义区
************************************************************************************/
vs16 MotoPwm[4];					//保存输出控制四个电机的PWM值
float e_I_Y,e_I_X;					//内环积分累计偏差值

extern SetValue_s SV;
extern MeasuredValue_s MV;
extern PID_PwmOutput_s PwmOutPut;
extern RawData_MPU_s RawData;		
extern DataPacket_s DataPacket;
//=====================================================================


/*****************************************************************
姿态角更新
******************************************************************/
void Get_Angle(void)
{
	dmp_getdata();
	
	MV.angularSpeed_X = RawData.gyro[0]*GYRO_XISHU;
	MV.angularSpeed_Y = RawData.gyro[1]*GYRO_XISHU;
	
	#if 0
		printf("gyro_y:%d\r\n",gyro_y);
	#endif
}


/*****************************************************************
前后左右方向控制
******************************************************************/
void Direction_Control(void)
{
	//根据遥控器传过来的左右方向值，改变SV.pit的给定值
	SV.pit = -(DataPacket.leftRight-15);
	
	//根据遥控器传过来的前后方向值，改变SV.rol的给定值
	SV.rol = DataPacket.upDown-15;
}


/*****************************************************************
外环角度控制
******************************************************************/
void Outter_PID(void)
{
	static float kp1=2;								//绕X轴比例系数
	static float kp2=1;								//绕Y轴比例系数，kp=1时系统性能较好
	static float e_pit,e_rol;						//本次偏差，前一次偏差
	
	//计算X轴和Y轴角度偏差值
	e_pit = SV.pit-MV.pit;
	e_rol = SV.rol-MV.rol;

	//外环PID运算
	SV.angularSpeed_Y = kp2*e_pit;
	SV.angularSpeed_X = kp1*e_rol;
	
	#if 0
		printf("%f\r\n",angular_speed_y);
	#endif
}


/*****************************************************************
内环角速度控制
******************************************************************/
void Inner_PID(void)
{
	static float kp1=30,ki1=0.5,kd1=30;				//绕X轴PID系数
	static float kp2=25,ki2=0.5,kd2=22;				//绕Y轴PID系数,kp=25,ki=0.5,kd=22,此组参数性能较好
	static float e_X[2],e_Y[2];						//本次偏差，前一次偏差
	static u8 flag_Y=0,flag_X=0;					//积分项参不参与运算的标志
	
	//计算X轴和Y轴角速度偏差
	e_Y[0] = SV.angularSpeed_Y - MV.angularSpeed_Y;
	e_X[0] = SV.angularSpeed_X - MV.angularSpeed_X;
	
	#if 0
		printf("e_Y:%f\r\n",e_Y[0]);					//可大致估算下角速度偏差值最大一般不会超过500dps，一般在100左右
	#endif
	
	//===========================绕Y轴内环PID运算========================================
	//积分分离，以便在偏差较大的时候可以快速的缩减偏差，在偏差较小的时候，才加入积分，消除误差
	if(e_Y[0]>=150.0||e_Y[0]<=-150.0){
		flag_Y = 0;
	}else{
		flag_Y = 1;
		e_I_Y += e_Y[0];
	}
	
	//积分限幅
	if(e_I_Y>1000)
		e_I_Y=1000;
	if(e_I_Y<-1000)
		e_I_Y=-1000;
	
	//位置式PID运算
	PwmOutPut.pwm_Y = kp2*e_Y[0]+flag_Y*ki2*e_I_Y+kd2*(e_Y[0]-e_Y[1]);
	
	
	//===========================绕X轴内环PID运算============================================
	//积分分离，以便在偏差较大的时候可以快速的缩减偏差，在偏差较小的时候，才加入积分，消除误差
	if(e_X[0]>=150.0||e_X[0]<=150.0){
		flag_X = 0;
	}else{
		flag_X = 1;
		e_I_X += e_X[0];
	}
	
	//积分限幅
	if(e_I_X>1000)
		e_I_X=1000;             					
	if(e_I_X<-1000)	
		e_I_X=-1000;              					
	
	//位置式PID运算
	PwmOutPut.pwm_X = kp1*e_X[0]+flag_X*ki1*e_I_X+kd1*(e_X[0]-e_X[1]);
	
	//=======================================================================================
	//记录本次偏差
	e_Y[1] = e_Y[0];								//用本次偏差值替换上次偏差值
	e_X[1] = e_X[0];								//用本次偏差值替换上次偏差值
	
}


/*************************************************************************
自转修正补偿PD
**************************************************************************/
void Rotation_Correction(void)
{
	static float kp1=40,kd1=60;				//自转修正系数，kp为40，kd为60较合适的参数
	static float e_Yaw[2];					//本次偏差，前一次偏差
	
	//yaw的偏差值
	e_Yaw[0] = SV.yaw - MV.yaw;
	#if 0
		printf("yaw_zhongzhi:%f\r\n",yaw_zhongzhi);
	#endif
	
	//位置式PD运算
	PwmOutPut.pwm_Yaw = kp1*e_Yaw[0]+kd1*(e_Yaw[0]-e_Yaw[1]);
	#if 0
		printf("pwm_Yaw:%d\r\n",pwm_Yaw);
	#endif
	
	//记录本次偏差
	e_Yaw[1] = e_Yaw[0];							//用本次偏差值替换上次偏差值
}


/**********************************************************************************************
pwm输出限速
***********************************************************************************************/
void DealPwm(void)
{
	//先判断是否按下KEY1，指示即将启动飞机，用于解锁和加锁飞机，防止油门误操作
	if(!(DataPacket.buttonFlag & 0x01)){							//飞机仍然处于加锁状态时	

		//将外环角速度输出值重置为0，记录此时yaw角度值并覆盖yaw_zhongzhi中的值
		SV.angularSpeed_Y = 0;
		SV.angularSpeed_X = 0;
		SV.yaw = MV.yaw;
		
		//将积分累计偏差值，和输出的PWM值全重置为0
		PwmOutPut.pwm_X = 0;
		PwmOutPut.pwm_Y = 0;
		PwmOutPut.pwm_Yaw = 0;
		e_I_X = 0;
		e_I_Y = 0;
		
		//将油门值改为1
		DataPacket.accelerator = 1;
		return;
	}
	
	//当飞机已经解锁时，但油门值小于100时，说明飞机处于待飞状态，把各PID运算输出值和积分累计项重置0
	if(DataPacket.accelerator<=100){
		SV.angularSpeed_Y = 0;
		SV.angularSpeed_X = 0;
		SV.yaw = MV.yaw;
		
		PwmOutPut.pwm_X = 0;
		PwmOutPut.pwm_Y = 0;
		PwmOutPut.pwm_Yaw = 0;
		e_I_X = 0;
		e_I_Y = 0;
	}
}


/*****************************************************************
设置电机的pwm值
******************************************************************/
void Set_Pwm(void)
{
	//注意：这里必须为s16，因为moto_pwm[i]为有符号数，这里也必须是有符号数，否则会导致比较大小错误
	static s16 max = 7150;
	static s16 min = 5;
	u8 i;
	
	//装配给各电机的PWM值
	MotoPwm[0] = DataPacket.accelerator+PwmOutPut.pwm_X-PwmOutPut.pwm_Y+PwmOutPut.pwm_Yaw;
	MotoPwm[1] = DataPacket.accelerator+PwmOutPut.pwm_X+PwmOutPut.pwm_Y-PwmOutPut.pwm_Yaw;
	MotoPwm[2] = DataPacket.accelerator-PwmOutPut.pwm_X+PwmOutPut.pwm_Y+PwmOutPut.pwm_Yaw;
	MotoPwm[3] = DataPacket.accelerator-PwmOutPut.pwm_X-PwmOutPut.pwm_Y-PwmOutPut.pwm_Yaw;
	
	#if 0
		moto_pwm[0] = +pwm_Yaw;
		moto_pwm[1] = -pwm_Yaw;
		moto_pwm[2] = +pwm_Yaw;
		moto_pwm[3] = -pwm_Yaw;
		
		printf("moto1:%d\r\n",moto_pwm[0]);
		printf("moto2:%d\r\n",moto_pwm[1]);
		printf("moto3:%d\r\n",moto_pwm[2]);
		printf("moto4:%d\r\n",moto_pwm[3]);
		printf("==============================\r\n");
	#endif
	
	//PWM限幅和防止出现负值
	for(i=0;i<4;i++){
		if(MotoPwm[i] >= max){
			MotoPwm[i] = max;
		}else if(MotoPwm[i] <= min){
			MotoPwm[i] = 1;
		}
	}
	
	#if 0
		printf("moto_pwm1:%d\r\n",moto_pwm[0]);
		printf("moto_pwm2:%d\r\n",moto_pwm[1]);
		printf("moto_pwm3:%d\r\n",moto_pwm[2]);
		printf("moto_pwm4:%d\r\n",moto_pwm[3]);
		printf("================================\r\n");
	#endif
	
	MOTO1_SetPulse(MotoPwm[0]);
	MOTO2_SetPulse(MotoPwm[1]);
	MOTO3_SetPulse(MotoPwm[2]);
	MOTO4_SetPulse(MotoPwm[3]);
}

