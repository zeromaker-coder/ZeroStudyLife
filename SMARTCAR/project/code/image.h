#ifndef _IMAGE_H
#define _IMAGE_H

#include "zf_common_headfile.h"

void image_init(void);

int otsu_get_threshold(uint8 *image, uint16 col, uint16 row);   

void show_real_image(uint16 x, uint16 y);


#endif