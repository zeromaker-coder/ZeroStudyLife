#ifndef _IMAGE_H
#define _IMAGE_H

#include "zf_common_headfile.h"

void image_init(void);

int otsu_get_threshold(uint8 *image, uint16 col, uint16 row);   

void show_real_image(uint16 x, uint16 y);

void show_binary_image(uint16 x, uint16 y,uint8 threshold);

void image_binary(uint8 *in_image,unsigned char *out_image);

extern uint8 binary_image[MT9V03X_H][MT9V03X_W];

extern uint8 image_threshold;

#endif