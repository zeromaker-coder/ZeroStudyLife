#include "motor.h"
#include "pid.h"
#include "menu.h"
#include "beep.h"
#include "encoder.h"
#include "math.h"

uint8 motor_speed_count = 0;                                                   // 电机速度计数器


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
    pwm_init(PWM_L, 10000, 0);                                                  // PWM 通道初始化频率 17KHz 占空比初始为 0

    gpio_init(DIR_R, GPO, GPIO_HIGH, GPO_PUSH_PULL);                            // GPIO 初始化为输出 默认上拉输出高
    pwm_init(PWM_R, 10000, 0);                                                  // PWM 通道初始化频率 17KHz 占空比初始为 0
}

/**
  * @brief  设置左右电机占空比和方向
  * @param  motor_left  左电机占空比（-MAX_DUTY ~ MAX_DUTY，正负表示方向，范围-10000到+10000）
  * @param  motor_right 右电机占空比（-MAX_DUTY ~ MAX_DUTY，正负表示方向，范围-10000到+10000）
  */
void motor_set_duty(int16 motor_left, int16 motor_right)
{
    //电机极性反转
    motor_left = -motor_left;                                                      
    motor_right = -motor_right;
      
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


/**
  * @brief  电机过快保护
  * @param  无
  */
void motor_speed_protection(void)
{
    if(abs(gyro_pid_out) > 4500||abs(encoder_data_left-encoder_data_left_last)>35||abs(encoder_data_right-encoder_data_right_last)>35) // 如果 PID 输出大于最大占空比
    {
        motor_speed_count++; // 电机速度计数器加 1
        if(motor_speed_count > 2)
        {
            car_go=0;//停止
            main_menu_item=1;
            sec_menu_item=1;
            pid_clear_all();
            key_clear_all_state();//清除按键状态
            motor_speed_count=0;
            beep_on();//蜂鸣器响
            encoder_data_left_last=0;
            encoder_data_right_last=0; // 重置编码器数据
            encoder_data_left = 0;
            encoder_data_right = 0; // 重置编码器数据
        }
    }
    else
    {
        motor_speed_count = 0; // 重置电机速度计数器
    }
}


