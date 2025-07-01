#ifndef _ENCODER_H
#define _ENCODER_H

#include "zf_common_headfile.h"

void encoder_init(void);

void encoder_read(void);

extern int16 encoder_data_left;

extern int16 encoder_data_right;

#endif