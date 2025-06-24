#ifndef MOTOR_H_
#define MOTOR_H_

// ���PWM����
#define LEFT_MOTOR_PWM      TIM3_PWM_MAP2_CH1_B4
#define LEFT_MOTOR_DIR      TIM9_PWM_MAP3_CH1_D9
#define RIGHT_MOTOR_PWM     TIM3_PWM_MAP0_CH3_B0
#define RIGHT_MOTOR_DIR     TIM3_PWM_MAP0_CH4_B1
// ����޷�
#define MOTOR_LEFT_DUTY        10000
#define MOTOR_Right_DUTY        10000
char ERR,ERR_LAST_L,ERR_LAST_R;//ǰհƫ��
unsigned char Car_State=0;//������־λ
int PWM=2000;
int PWM_SPACE=8000;
float KP = 10.5*10; //����ǰհ���Ʊ���ϵ�� 7.9*10
float KD = 1.0*10; //����ǰհ����΢��ϵ��
/**
*
* @brief    �����ʼ��
* @param
* @return   void
* @notes
* Example:  Init_Motor();
*
**/
void Motor_Init(void);


void Set_Left_Motor_Duty(int duty);
void Set_Right_Motor_Duty(int duty);


int Pid_Left_Motor(int hope_speed);
int Pid_Right_Motor(int hope_speed);

#endif /* CODE_DMX_MOTOR_H_ */
