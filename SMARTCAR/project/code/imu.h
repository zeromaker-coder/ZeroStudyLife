#ifndef _IMU_H
#define _IMU_H

#include "zf_common_headfile.h"

typedef struct {
    uint8 gx_err;
    uint8 gy_err;
    uint8 gz_err;
}imu_err_typdef;

extern float filtering_angle;

#endif 