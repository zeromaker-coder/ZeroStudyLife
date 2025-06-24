/*********************************************************************************************************************
* CH32V307VCT6 Opensourec Library 即（CH32V307VCT6 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是CH32V307VCT6 开源库的一部分
*
* CH32V307VCT6 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          main
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          MounRiver Studio V1.8.1
* 适用平台          CH32V307VCT6
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期                                      作者                             备注
* 2022-09-15        大W            first version
********************************************************************************************************************/
#include "common.h"
int main (void)
{
    clock_init(SYSTEM_CLOCK_120M);      // 初始化芯片时钟 工作频率为 120MHz
    debug_init();                       // 务必保留，本函数用于初始化MPU 时钟 调试串口
    Motor_Init();
    ips114_init();     //初始化屏幕
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
        //右拐避障,自己改下看看能不能用
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
