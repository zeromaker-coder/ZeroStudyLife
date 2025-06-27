#ifndef _PID_H
#define _PID_H

#include "zf_common_headfile.h"
typedef struct {
    float gyro_kp;
    float gyro_ki;

    float angle_kp;
    float angle_kd;
    
    float speed_kp;
    float speed_ki;    
    
    float turn_kp;
    float turn_kp2;
    float turn_kd;
    float turn_kd2;

    float gyro_i_out_limit;
    float gyro_out_limit;

    float angle_out_limit;
    
    float speed_i_out_limit;
    float speed_out_limit;

}pid_typedef;

extern pid_typedef pid_param;

#endif