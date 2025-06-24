/*********************************************************************************************************************
* CH32V307VCT6 Opensourec Library ����CH32V307VCT6 ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ���CH32V307VCT6 ��Դ���һ����
*
* CH32V307VCT6 ��Դ�� ��������
* �����Ը��������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
*
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
*
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
*
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ���֤Э�� �����������Ϊ���İ汾
* �������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ���֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
*
* �ļ�����          main
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          MounRiver Studio V1.8.1
* ����ƽ̨          CH32V307VCT6
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����                                      ����                             ��ע
* 2022-09-15        ��W            first version
********************************************************************************************************************/
#include "common.h"
int main (void)
{
    clock_init(SYSTEM_CLOCK_120M);      // ��ʼ��оƬʱ�� ����Ƶ��Ϊ 120MHz
    debug_init();                       // ��ر��������������ڳ�ʼ��MPU ʱ�� ���Դ���
    Motor_Init();
    ips114_init();     //��ʼ����Ļ
    ips114_show_string(0,0,"OK");
    mt9v03x_init();
    float final_error;
//    timer_init(TIM_1, TIMER_MS);
//    float time=0;
    while(1)
    if(mt9v03x_finish_flag)
      {
        final_error=Image_Process(40,10);
        Set_Left_Motor_Duty (Pid_Left_Motor(final_error));
        Set_Right_Motor_Duty(Pid_Right_Motor(final_error));
        //�ҹձ���,�Լ����¿����ܲ�����
//        if (Car_State==0||Car_State==3){
//         final_error=Image_Process(40,10);
//         Set_Left_Motor_Duty (Pid_Left_Motor(final_error));
//         Set_Right_Motor_Duty(Pid_Right_Motor(final_error));
//        }else if (Car_State==1) {
//         Set_Left_Motor_Duty(0);
//         Set_Right_Motor_Duty(0);
//        }else if (Car_State==2) {
//         Set_Left_Motor_Duty(9500);
//         Set_Right_Motor_Duty(1000);
//         Delay(1000);
//         Set_Left_Motor_Duty(1000);
//         Set_Right_Motor_Duty(9500);
//         Delay(500);
//         Set_Left_Motor_Duty(9500);
//         Set_Right_Motor_Duty(9500);
//         Delay(1000);
//         Set_Left_Motor_Duty(1000);
//         Set_Right_Motor_Duty(9500);
//         Delay(2000);
//         Set_Left_Motor_Duty(9500);
//         Set_Right_Motor_Duty(1000);
//         Delay(500);
//         Car_State=3;
//        }

//        ips114_show_int(200,80,Pid_Left_Motor((int)final_error),4);

//        time=timer_get(TIM_1);
        mt9v03x_finish_flag = 0;

  }
}
