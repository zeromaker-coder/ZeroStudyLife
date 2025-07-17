/*********************************************************************************************************************
* MM32F327X-G8P Opensourec Library 即（MM32F327X-G8P 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
* 
* 本文件是 MM32F327X-G8P 开源库的一部分
* 
* MM32F327X-G8P 开源库 是免费软件
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
* 开发环境          IAR 8.32.4 or MDK 5.37
* 适用平台          MM32F327X_G8P
* 店铺链接          https://seekfree.taobao.com/
* 
* 修改记录
* 日期              作者                备注
* 2022-08-10        Teternal            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"

// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project->clean  等待下方进度条走完

// 本例程是开源库移植用空工程

// **************************** 代码区域 ****************************
#include "beep.h"
#include "menu.h"
#include "image.h"
#include "imu.h"
#include "encoder.h"
#include "pid.h"
#include "motor.h"

//指示灯以及蜂鸣器
#define BEEP                    (D7 )       // 蜂鸣器接口

//角速度环定时器
#define PIT                     (TIM6_PIT )                                     // 使用的周期中断编号 如果修改 需要同步对应修改周期中断编号与 isr.c 中的调用
#define PIT_PRIORITY            (TIM6_IRQn)                                     // 对应周期中断的中断编号 在 mm32f3277gx.h 头文件中查看 IRQn_Type 枚举体

//菜单角度信息获取
#define menu_imu_PIT            (TIM2_PIT )                                     
#define menu_imu_PIT_PRIORITY   (TIM2_IRQn)                                     


uint32 system_count;//系统计数器
uint32 image_count;//图像采样计数器
uint8 data_buffer[32];//无线转串口数据缓冲区

int main(void)
{
    // 此处编写用户代码 例如外设初始化代码等
    clock_init(SYSTEM_CLOCK_120M);                                              // 初始化芯片时钟 工作频率为 120MHz
    debug_init();                                                               // 初始化默认 Debug UART

    menu_init();//菜单初始化
    pid_init();//PID初始化
    image_init();//图像采样初始化
    encoder_init();//编码器初始化
    motor_init();//电机初始化
    imu_init();//IMU初始化
    wireless_uart_init();//无线转串口初始化
    boundary_line_init();//边界线初始化

    
    ips200_show_rgb565_image(0, 120, (const uint16 *)Image_Flags, 32, 9, 240, 80, 0);        // 显示一个RGB565色彩图片 原图240*80 显示240*80 低位在前
    system_delay_ms(500);
    ips200_clear();//清屏

    
    gpio_init(BEEP, GPO, GPIO_LOW, GPO_PUSH_PULL);//蜂鸣器初始化

    pit_ms_init(PIT,1);//周期中断初始化，1ms周期
    pit_ms_init(menu_imu_PIT,5);//5ms获取一次角度数据

    //设置中断优先级，0为最高
    interrupt_set_priority(PIT_PRIORITY,0);
    interrupt_set_priority(menu_imu_PIT_PRIORITY,0);
    
    // gpio_set_level(BEEP, GPIO_HIGH);                                            // BEEP 响
    // system_delay_ms(100);
    // gpio_set_level(BEEP, GPIO_LOW);                                             // BEEP 停
    // system_delay_ms(100);


    while(1)
    {
        image_count++;

        key_scanner();//按键扫描

        image_binary(mt9v03x_image,binary_image);//图像二值化

        longest_white_sweep_line(binary_image);//图像扫线

        line_err=err_sum_average(err_start_point, err_end_point);//计算误差

        if(image_count%5==0)
        {
			image_threshold=otsu_get_threshold(mt9v03x_image, MT9V03X_W, MT9V03X_H);//图像获取阈值
        }

        if(!car_go)
        {
            menu();
            err_start_point=user_param.err_start;//错误起始点
            err_end_point=user_param.err_end;//错误终止点
            // sprintf(data_buffer,"%.1f\r\n",filtering_angle);
            // wireless_uart_send_string(data_buffer);//发送角度信息
        }
        else
        {
            ramp_xianzhi++;//坡道计时器
            if(KEY_SHORT_PRESS==key_get_state(KEY_1)||image_out_of_bounds(mt9v03x_image)||zebra_flag)//按键1短按
            {
                car_go=0;//停止
                main_menu_item=1;
                sec_menu_item=1;
                pid_clear_all();
                key_clear_all_state();//清除按键状态
                // beep_on();//蜂鸣器响
                encoder_data_left_last=0;
                encoder_data_right_last=0; // 重置编码器数据
                encoder_data_left = 0;
                encoder_data_right = 0; // 重置编码器数据
                ramp_xianzhi=0;//重置坡道计时器
                ramp_once_time=0;//重置坡道单次保护
                circle_once_time=0;//重置环岛单次保护
            }
            // sprintf(data_buffer,"%d\r\n",right_circle_flag);
            // wireless_uart_send_string(data_buffer);//发送右圆环状态
            // sprintf(data_buffer,"%d\r\n",encoder_sum);
            // wireless_uart_send_string(data_buffer);//发送编码器计数
        }

        beep_cycle();
    }
}
// **************************** 代码区域 ****************************
void pit_handler(void)
{
    system_count++;//系统计数器

    imu660ra_get_acc();                                                         // 获取 IMU660RA 的加速度测量数值
    imu660ra_get_gyro();                                                        // 获取 IMU660RA 的角速度测量数值

    turn_pid_location();//转向环

    gyro_pid_location();//角速度环
    
    motor_set_duty(gyro_pid_out-turn_pid_out,gyro_pid_out+turn_pid_out);//电机输出

    if(system_count%5==0)
    {
        first_order_filtering();                                                    // 一阶互补滤波解算姿态
        angle_pid_location();//角度环
    }
    
    if(system_count%20==0)
    {
        encoder_read();//编码器读取
        motor_speed_protection();//电机速度保护
        speed_pid_loacation();//速度环
    }
}


void menu_imu_pit_handler(void)
{
    imu660ra_get_acc();                                                         // 获取 IMU660RA 的加速度测量数值
    imu660ra_get_gyro();                                                        // 获取 IMU660RA 的角速度测量数值
    encoder_read();                                                        // 编码器读取
    first_order_filtering();                                                    // 一阶互补滤波解算姿态
}

