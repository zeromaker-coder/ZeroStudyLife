#ifndef COMMON_H
#define COMMON_H
typedef unsigned char uchar;
#include "zf_common_headfile.h"
#include "image.h"
double pow();
void road_upspeed(int state);
void Motor_Init();
float Image_Process(unsigned char prospect, float error_offset);
int Pid_Left_Motor(int hope_speed);
int Pid_Right_Motor(int hope_speed);
void Set_Left_Motor_Duty(int duty);
void Set_Right_Motor_Duty(int duty);
void Delay(uint n);
#endif

