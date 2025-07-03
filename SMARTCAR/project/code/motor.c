#include "motor.h"


#define MAX_DUTY            (50 )                                               // 最大 MAX_DUTY% 占空比
#define DIR_L               (A0 )                                               // 左电机控制引脚
#define PWM_L               (TIM5_PWM_CH2_A1)                                   // 左电机 PWM 引脚

#define DIR_R               (A2 )                                               // 右电机控制引脚
#define PWM_R               (TIM5_PWM_CH4_A3)                                   // 右电机 PWM 引脚

/**
  * @brief  电机初始化
  * @param  无
  */
void motor_init(void)
{
    gpio_init(DIR_L, GPO, GPIO_HIGH, GPO_PUSH_PULL);                            // GPIO 初始化为输出 默认上拉输出高
    pwm_init(PWM_L, 17000, 0);                                                  // PWM 通道初始化频率 17KHz 占空比初始为 0

    gpio_init(DIR_R, GPO, GPIO_HIGH, GPO_PUSH_PULL);                            // GPIO 初始化为输出 默认上拉输出高
    pwm_init(PWM_R, 17000, 0);                                                  // PWM 通道初始化频率 17KHz 占空比初始为 0
}

/**
  * @brief  设置左右电机占空比和方向
  * @param  motor_left  左电机占空比（-MAX_DUTY ~ MAX_DUTY，正负表示方向，范围-10000到+10000）
  * @param  motor_right 右电机占空比（-MAX_DUTY ~ MAX_DUTY，正负表示方向，范围-10000到+10000）
  */
void motor_set_duty(int16 motor_left, int16 motor_right)
{
    // 左电机
    //输出限幅
    if(motor_left>10000)motor_left=10000;
    else if(motor_left<-10000)motor_left=-10000;
    
    if(motor_left >= 0)
    {
        gpio_set_level(DIR_L, GPIO_HIGH);                                       // 正转
        pwm_set_duty(PWM_L, motor_left * (PWM_DUTY_MAX / 10000));
    }
    else
    {
        gpio_set_level(DIR_L, GPIO_LOW);                                        // 反转
        pwm_set_duty(PWM_L, (-motor_left) * (PWM_DUTY_MAX / 10000));
    }

    // 右电机
    // 输出限幅
    if(motor_right>10000)motor_right=10000;
    else if(motor_right<-10000)motor_right=-10000;

    if(motor_right >= 0)
    {
        gpio_set_level(DIR_R, GPIO_HIGH);                                       // 正转
        pwm_set_duty(PWM_R, motor_right * (PWM_DUTY_MAX / 10000));
    }
    else
    {
        gpio_set_level(DIR_R, GPIO_LOW);                                        // 反转
        pwm_set_duty(PWM_R, (-motor_right) * (PWM_DUTY_MAX / 10000));
    }
}


