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

//指示灯以及蜂鸣器
#define LED1                    (H2 )
#define LED2                    (B13)
#define BEEP                    (D7 )       // 蜂鸣器接口

//角速度环定时器
#define GYRO_PIT                     (TIM6_PIT )                                     // 使用的周期中断编号 如果修改 需要同步对应修改周期中断编号与 isr.c 中的调用
#define GYRO_PIT_PRIORITY            (TIM6_IRQn)                                     // 对应周期中断的中断编号 在 mm32f3277gx.h 头文件中查看 IRQn_Type 枚举体
//角度环定时器
#define ANGLE_PIT                    (TIM5_PIT)
#define ANGLE_PIT_PRIORITY           (TIM5_IRQn)   
//速度环定时器
#define SPEED_PIT                    (TIM2_PIT)
#define SPEED_PIT_PRIORITY           (TIM2_IRQn) 

uint8 gyro_pit_state;//角速度环定时器标志位
uint8 angle_pit_state;//角度环定时器标志位
uint8 speed_pit_state;//速度环定时器标志位

int main(void)
{
    // 此处编写用户代码 例如外设初始化代码等
    clock_init(SYSTEM_CLOCK_120M);                                              // 初始化芯片时钟 工作频率为 120MHz
    debug_init();                                                               // 初始化默认 Debug UART

    menu_init();//菜单初始化
    image_init();//图像采样初始化
    encoder_init();//编码器初始化 
    
    gpio_init(BEEP, GPO, GPIO_LOW, GPO_PUSH_PULL);//蜂鸣器初始化
    gpio_init(LED1, GPO, GPIO_HIGH, GPO_PUSH_PULL);//指示灯初始化
    gpio_init(LED2, GPO, GPIO_HIGH, GPO_PUSH_PULL);

    pit_ms_init(GYRO_PIT,3);//3ms一次角速度解算
    pit_ms_init(ANGLE_PIT,5);//5ms一次角度解算
    pit_ms_init(SPEED_PIT,20);//20ms一次速度解算

    //设置中断优先级，0为最高
    interrupt_set_priority(GYRO_PIT_PRIORITY,0);
    interrupt_set_priority(ANGLE_PIT_PRIORITY,0);
    interrupt_set_priority(SPEED_PIT_PRIORITY,0);
    
    gpio_set_level(BEEP, GPIO_HIGH);                                            // BEEP 响
    system_delay_ms(100);
    gpio_set_level(BEEP, GPIO_LOW);                                             // BEEP 停
    system_delay_ms(100);
    while(1)
    {
        
		// 此处编写需要循环执行的代码
        if(!car_go)
        {
            menu();
        }
        
        if(gyro_pit_state)
        {
            gyro_pit_state=0;
        }

        if(angle_pit_state)
        {
            angle_pit_state=0;
        }

        if(speed_pit_state)
        {
            speed_pit_state=0;
        }
        beep_cycle();
    }
}
// **************************** 代码区域 ****************************
//角速度环中断触发函数
void gyro_pit_handler(void)
{
    gyro_pit_state=1;
}

//角度环中断触发函数
void angle_pit_handler(void)
{
    angle_pit_state=1;
}

//速度环中断触发函数
void speed_pit_handler(void)
{
    encoder_read();
    speed_pit_state=1;
}

