#include "imu.h"
#include "math.h"
#include "menu.h"
#include "pid.h"
#include "beep.h"

uint8_t acc_ration=4;    //加速度计置信度
uint8_t gyro_ration=4;   //陀螺仪置信度
float filtering_angle=0; //解算出的角度
float angle_temp;        //角度计算中间变量
float cycle_T=0.005;         //采样周期
imu_err_typdef imu_err={0,8,0}; //imu误差结构体
uint16 az_last=0; //上次加速度计Z轴数据

/**
* @brief   imu初始化
* @param   无
*/
void imu_init(void)
{
    ips200_show_string(0, 0, "IMU660RA init."); //显示初始化信息
    while(1)
    {
        if(imu660ra_init())
        {
            printf("\r\nIMU660RA init error.");                                 // IMU660RA 初始化失败
        }
        else
        {
            break;
        }
    }
    ips200_show_string(0, 16, "IMU660RA init success."); //显示初始化成功信息
    system_delay_ms(100); //延时100ms等待IMU稳定
    ips200_clear(); //清屏
}

/**
* @brief   一阶互补滤波解算姿态
* @param   无
*/
void first_order_filtering(void)
{
    int16 gx,gy,gz,ax,ay,az;
    //临时变量存储                                                      
    gx=imu660ra_gyro_x;
    gy=imu660ra_gyro_y;
    gz=imu660ra_gyro_z;                                                        
    ax=imu660ra_acc_x;
    ay=imu660ra_acc_y;
    az=imu660ra_acc_z;
    //去除静态误差
    gx+=imu_err.gx_err;
    gy+=imu_err.gy_err;
    gz+=imu_err.gz_err;
    if(gx<5 && gx>-5)
	{
		gx=0;
	}
	if(gy<5 && gy>-5)
	{
		gy=0;
	}
	if(gz<5 && gz>-5)
	{
		gz=0;
	} 
    //一阶互补滤波算角度
    float gyro_temp;
	float acc_temp;
	gyro_temp=gy*gyro_ration;
	acc_temp=(-ax-angle_temp)*acc_ration;
	angle_temp+=((gyro_temp+acc_temp)*cycle_T);
	filtering_angle=angle_temp;                                                     
}


/**
* @brief   提起保护处理函数
* @param   无
*/
void lift_protection(void)
{
    int16 gx,gy,gz,ax,ay,az;
    //临时变量存储                                                      
    gx=imu660ra_gyro_x;
    gy=imu660ra_gyro_y;
    gz=imu660ra_gyro_z;                                                        
    ax=imu660ra_acc_x;
    ay=imu660ra_acc_y;
    az=imu660ra_acc_z;

    uint8 acc_abnormal=abs(az-az_last)>8000; //加速度异常标志

    if(acc_abnormal) //如果加速度异常或角度异常
    {
        ips200_show_string(0, 0, "Lift protection!"); //显示提起保护信息
        system_delay_ms(20); 
        ips200_clear(); //清屏
        car_go=0;//停止
        main_menu_item=1;
        sec_menu_item=1;
        pid_clear_all();
        key_clear_all_state();//清除按键状态
        beep_on();//蜂鸣器响
    }

    az_last=az; //保存上次加速度计Z轴数据
}



