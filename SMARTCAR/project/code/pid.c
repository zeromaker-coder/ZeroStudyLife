#include "pid.h"

//定义结构体
PID_LocTypeDef gyro_pid_param;//角速度环结构体
PID_LocTypeDef angle_pid_param;//角度环结构体
PID_LocTypeDef speed_pid_param;//速度环结构体
Turn_PPDD_LocTypeDef turn_pid_param;//转向环结构体
USER_PARAM_LocTypeDef user_param;//常用参数

//定义结构体指针
PID_LocTypeDef* gyro_pid_pin;//角速度环结构体指针
PID_LocTypeDef* angle_pid_pin;//角度环结构体指针
PID_LocTypeDef* speed_pid_pin;//速度环结构体指针
Turn_PPDD_LocTypeDef* turn_pid_pin;//转向环结构体指针
USER_PARAM_LocTypeDef* user_param_pin;//常用参数



//定义中间变量
float gyro_pid_out;//角速度环输出
float angle_pid_out;//角度环输出
float speed_pid_out;//速度环输出




/**
* @brief  初始化PID参数
* @param   无
*/
void pid_init(void)
{
    //角速度环参数初始化
    gyro_pid_param.kp=0.0;
    gyro_pid_param.ki=0.0;
    gyro_pid_param.kd=0.0;
    gyro_pid_param.PID_I_LIMIT_MAX=0.0;
    gyro_pid_param.PID_OUT_LIMIT_MAX=0.0;
    //角度环参数初始化
    angle_pid_param.kp=0.0;
    angle_pid_param.ki=0.0;
    angle_pid_param.kd=0.0;
    angle_pid_param.PID_I_LIMIT_MAX=0.0;
    angle_pid_param.PID_OUT_LIMIT_MAX=0.0;
    //速度环参数初始化
    speed_pid_param.kp=0.0;
    speed_pid_param.ki=0.0;
    speed_pid_param.kd=0.0;
    speed_pid_param.PID_I_LIMIT_MAX=0.0;
    speed_pid_param.PID_OUT_LIMIT_MAX=0.0;
    //转向环参数初始化
    turn_pid_param.kp=0.0;
    turn_pid_param.kp2=0.0;
    turn_pid_param.kd=0.0;
    turn_pid_param.kd2=0.0;
    turn_pid_param.PID_OUT_LIMIT_MAX=0.0;
}



/**
* @brief  位置式PID
* @param   setvalue 设定值
* @param   actualvalue 实际值
* @param   PID PID参数结构体
*/
float PID_location(float setvalue, float actualvalue, PID_LocTypeDef *PID)
{ 
	PID->ek =setvalue-actualvalue;
	PID->location_sum += PID->ek;//计算累计误差值                         
	if((PID->ki!=0)&&(PID->location_sum*PID->ki>PID->PID_I_LIMIT_MAX)) PID->location_sum=PID->PID_I_LIMIT_MAX;
	if((PID->ki!=0)&&(PID->location_sum*PID->ki<-PID->PID_I_LIMIT_MAX)) PID->location_sum=-PID->PID_I_LIMIT_MAX;
	//out计算
    PID->out=PID->kp*PID->ek+(PID->ki*PID->location_sum)+PID->kd*(PID->ek-PID->ek1);
    PID->ek1 = PID->ek;  
    //PID限幅
	if(PID->out<-PID->PID_OUT_LIMIT_MAX)PID->out=-PID->PID_OUT_LIMIT_MAX;
	if(PID->out>PID->PID_OUT_LIMIT_MAX)PID->out=PID->PID_OUT_LIMIT_MAX;
	
	return PID->out;
}

/**
* @brief  D先行PID
* @param   setvalue 设定值
* @param   actualvalue 实际值
* @param   GY  角速度
* @param   PID PID参数结构体
*/
float PID_D_Pre_location(float setvalue, float actualvalue, float GY ,PID_LocTypeDef *PID)
{
    PID->ek =setvalue-actualvalue;
	PID->location_sum += PID->ek;//计算累计误差值                         
	if((PID->ki!=0)&&(PID->location_sum*PID->ki>PID->PID_I_LIMIT_MAX)) PID->location_sum=PID->PID_I_LIMIT_MAX;
	if((PID->ki!=0)&&(PID->location_sum*PID->ki<-PID->PID_I_LIMIT_MAX)) PID->location_sum=-PID->PID_I_LIMIT_MAX;
	//out计算
    PID->out=PID->kp*PID->ek+(PID->ki*PID->location_sum)+PID->kd*GY;//极性调整处
    //PID限幅
	if(PID->out<-PID->PID_OUT_LIMIT_MAX)PID->out=-PID->PID_OUT_LIMIT_MAX;
	if(PID->out>PID->PID_OUT_LIMIT_MAX)PID->out=PID->PID_OUT_LIMIT_MAX;
	
	return PID->out;
}







