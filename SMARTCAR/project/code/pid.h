#ifndef _PID_H
#define _PID_H

#include "zf_common_headfile.h"

void gyro_pid_location(void);

void angle_pid_location(void);

void speed_pid_loacation(void);

void turn_pid_location(void);

void pid_init(void);

void pid_clear_all(void);


typedef struct
{
  float kp;                     
  float ki;                       
  float kd;                       
  float ek;                      
  float ek1;                                            
  float location_sum;            
  float out;
  float PID_I_LIMIT_MAX;
  float PID_OUT_LIMIT_MAX;											
}PID_LocTypeDef;

typedef struct
{
  float kp;
  float kd;
  float kp2;
  float kd2;
  float ek;                      
  float ek1;                      
  float location_sum;            
  float out;
  float PID_I_LIMIT_MAX;
  float PID_OUT_LIMIT_MAX;	
}Turn_PPDD_LocTypeDef;

typedef struct
{
  float mid_angle;
  float target_speed;
}USER_PARAM_LocTypeDef;


extern PID_LocTypeDef gyro_pid_param;
extern PID_LocTypeDef angle_pid_param;
extern PID_LocTypeDef speed_pid_param;
extern Turn_PPDD_LocTypeDef turn_pid_param;
extern USER_PARAM_LocTypeDef user_param;
extern float gyro_pid_out;
extern float angle_pid_out;
extern float speed_pid_out;
extern float turn_pid_out;
extern uint8 straight_flag;



#endif