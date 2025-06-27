#include "imu.h"

uint8_t acc_ration=4;    //加速度计置信度
uint8_t gyro_ration=4;   //陀螺仪置信度
float filtering_angle=0; //解算出的角度
float angle_temp;        //角度计算中间变量
float cycle_T=0;         //采样周期

imu_err_typdef imu_err; //imu误差结构体

/**
* @brief   imu初始化
* @param   无
*/
void imu_init(void)
{
    imu_err.gx_err = 0;
    imu_err.gy_err = 0;
    imu_err.gz_err = 0;
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
}

/**
* @brief   一阶互补滤波解算姿态
* @param   无
*/
void first_order_filtering(void)
{
    int16 gx,gy,gz,ax,ay,az;
    //临时变量存储                                                      // 获取 IMU660RA 的角速度测量数值
    gx=imu660ra_gyro_x;
    gy=imu660ra_gyro_y;
    gz=imu660ra_gyro_z;                                                        // 获取 IMU660RA 的角速度测量数值
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
	gyro_temp=gx*gyro_ration;
	acc_temp=(ay-angle_temp)*acc_ration;
	angle_temp+=((gyro_temp+acc_temp)*cycle_T);
	filtering_angle=angle_temp;                                                       // 获取 IMU660RA 的加速度测量数值
}

