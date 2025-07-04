#include "encoder.h"

#define ENCODER_1                   (TIM3_ENCODER)
#define ENCODER_1_A                 (TIM3_ENCODER_CH1_B4)
#define ENCODER_1_B                 (TIM3_ENCODER_CH2_B5)

#define ENCODER_2                   (TIM4_ENCODER)
#define ENCODER_2_A                 (TIM4_ENCODER_CH1_B6)
#define ENCODER_2_B                 (TIM4_ENCODER_CH2_B7)

int16 encoder_data_left = 0;
int16 encoder_data_right = 0;

/**
  * @brief  编码器初始化
  * @param  无
  * @retval 无
  */
void encoder_init(void)
{
    encoder_quad_init(ENCODER_1, ENCODER_1_A, ENCODER_1_B);                     // 初始化编码器模块与引脚 正交解码编码器模式
    encoder_quad_init(ENCODER_2, ENCODER_2_A, ENCODER_2_B);                     // 初始化编码器模块与引脚 正交解码编码器模式
}

/**
  * @brief  读取编码器数据
  * @param  无
  * @retval 无
  */
void encoder_read(void)
{
    encoder_data_left  = encoder_get_count(ENCODER_1);                              // 获取左编码器计数
    encoder_clear_count(ENCODER_1);                                             // 清空左编码器计数

    encoder_data_right  = -encoder_get_count(ENCODER_2);                              // 获取右编码器计数
    encoder_clear_count(ENCODER_2);                                             // 清空右编码器计数
}