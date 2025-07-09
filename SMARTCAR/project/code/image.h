#ifndef _IMAGE_H
#define _IMAGE_H

#include "zf_common_headfile.h"

#define DEAL_IMAGE_H 120
#define DEAL_IMAGE_W 188

void image_init(void);

int otsu_get_threshold(uint8 *image, uint16 col, uint16 row);   

void show_real_image(uint16 x, uint16 y);

void show_binary_image(uint16 x, uint16 y,uint8 threshold);

void image_binary(uint8 *in_image,unsigned char *out_image);

void show_boundary_line(void);

void boundary_line_init(void);

void longest_white_sweep_line(uint8 image[DEAL_IMAGE_H][DEAL_IMAGE_W]);

uint8 image_out_of_bounds(unsigned char in_image[DEAL_IMAGE_H][DEAL_IMAGE_W]);

float err_sum_average(uint8 start_point,uint8 end_point);

void find_down_point(uint8 start_point,uint8 end_point);

void find_up_point(uint8 start_point,uint8 end_point);

void left_draw_line(uint8 x1,uint8 y1,uint8 x2,uint8 y2);

void right_draw_line(uint8 x1,uint8 y1,uint8 x2,uint8 y2);

void circle_judge(void);

void cross_judge(void);

extern uint8 left_down_point;

extern uint8 left_up_point;

extern uint8 right_down_point;

extern uint8 right_up_point;

extern uint8 binary_image[MT9V03X_H][MT9V03X_W];

extern uint8 image_threshold;

extern uint8 left_line[MT9V03X_H];

extern uint8 right_line[MT9V03X_H];

extern float line_err;

extern uint8 left_lost_count;

extern uint8 right_lost_count;

extern uint8 left_right_lost_count;

extern uint8 left_change_line;

extern uint8 right_change_line;

extern uint8 continuity_left_change_flag;

extern uint8 continuity_right_change_flag;

extern uint8 search_stop_line;

extern uint8 boundary_start_left;

extern uint8 boundary_start_right;


#endif