#include "pid.h"

pid_typedef pid_param; // PID参数结构体

/**
* @brief  初始化PID参数
* @param   无
*/
void pid_init(void)
{
    pid_param.gyro_kp = 0.0f;   
    pid_param.gyro_ki = 0.0f;

    pid_param.angle_kp = 0.0f;
    pid_param.angle_kd = 0.0f;

    pid_param.speed_kp = 0.0f;
    pid_param.speed_ki = 0.0f;

    pid_param.turn_kp = 0.0f;
    pid_param.turn_kp2 = 0.0f;
    pid_param.turn_kd = 0.0f;
    pid_param.turn_kd2 = 0.0f;

    pid_param.gyro_i_out_limit = 0.0f;
    pid_param.gyro_out_limit = 0.0f;

    pid_param.angle_out_limit = 0.0f;

    pid_param.speed_i_out_limit = 0.0f;
    pid_param.speed_out_limit = 0.0f;
}