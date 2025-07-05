#ifndef _MOTOR_H
#define _MOTOR_H

#include "zf_common_headfile.h"

void motor_init(void);

void motor_set_duty(int16 motor_left, int16 motor_right);

void motor_speed_protection(void);

#endif