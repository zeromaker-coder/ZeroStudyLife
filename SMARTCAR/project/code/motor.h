#ifndef _MOTOR_H
#define _MOTOR_H

#include "zf_common_headfile.h"

void motor_init(void);

void motor_set_duty(int8 motor_left, int8 motor_right);

#endif