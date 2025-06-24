#include "common.h"
#include "motor.h"

void Motor_Init(void)
{
    pwm_init(LEFT_MOTOR_PWM, 2000, 0);
    pwm_init(LEFT_MOTOR_DIR, 2000, 0);
    pwm_init(RIGHT_MOTOR_PWM, 2000, 0);
    pwm_init(RIGHT_MOTOR_DIR , 2000, 0);
}
void Set_Left_Motor_Duty(int duty)
{
    if(duty >=0)
      {
          if(duty > MOTOR_LEFT_DUTY)
              duty = MOTOR_LEFT_DUTY;

          pwm_set_freq(LEFT_MOTOR_PWM, PWM,duty);
          pwm_set_freq(LEFT_MOTOR_DIR,PWM, 0);
      }
      else
      {
          if(duty < -MOTOR_Right_DUTY)
              duty = -MOTOR_Right_DUTY;

          pwm_set_freq(LEFT_MOTOR_PWM,PWM, 0);
          pwm_set_freq(LEFT_MOTOR_DIR,PWM, -duty);
      }

}

void Set_Right_Motor_Duty(int duty)
{
    if(duty >=0)
       {
           if(duty > MOTOR_LEFT_DUTY)
               duty = MOTOR_LEFT_DUTY;

           pwm_set_freq(RIGHT_MOTOR_PWM,PWM, duty);
           pwm_set_freq(RIGHT_MOTOR_DIR,PWM, 0);
       }
       else
       {
           if(duty < -MOTOR_Right_DUTY)
               duty = -MOTOR_Right_DUTY;

           pwm_set_freq(RIGHT_MOTOR_PWM,PWM, 0);
           pwm_set_freq(RIGHT_MOTOR_DIR,PWM, -duty);
       }
}

int Pid_Left_Motor(int ERR)
{
    int PWM_ERROR = PWM_SPACE+(KP*ERR + KD*(ERR-ERR_LAST_L));
    // 结果限幅
    if(PWM_ERROR > 10000)
        PWM_ERROR = 10000;
    else if(PWM_ERROR < -10000)
        PWM_ERROR = -10000;
    // 误差迭代
    ERR_LAST_L = ERR;
    return (int)PWM_ERROR;
}

int Pid_Right_Motor(int ERR)
{
    int PWM_ERROR = PWM_SPACE-(KP*ERR + KD*(ERR-ERR_LAST_R));  //舵机参数计算
    // 结果限幅
    if(PWM_ERROR > 10000)
        PWM_ERROR = 10000;
    else if(PWM_ERROR < -10000)
        PWM_ERROR = -10000;
    // 误差迭代
    ERR_LAST_R = ERR;
    return (int)PWM_ERROR;
}
