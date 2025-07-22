#include "image.h"
#include "math.h"
#include "menu.h"
#include "beep.h"
#include "pid.h"
#include "encoder.h"

#define DISPLAY_MODE                ( 1 )                                       // 显示模式 0-灰度显示 1-二值化显示
                                                                                // 0-灰度显示   就是正常显示的总钻风图像
                                                                                // 1-二值化显示 根据最后一个二值化阈值显示出对应的二值化图像
#define BINARIZATION_THRESHOLD      ( 64 )                                      // 二值化阈值 默认 64 需要设置 DISPLAY_MODE 为 1 才使用


#define IPS200_TYPE     (IPS200_TYPE_SPI)                                 // 双排排针 并口两寸屏 这里宏定义填写 IPS200_TYPE_PARALLEL8
                                                                                // 单排排针 SPI 两寸屏 这里宏定义填写 IPS200_TYPE_SPI

const unsigned char Image_Flags[9*32] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 
        0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,  
        0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 
        0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00,
        0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 
        0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 
        0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 
        0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 

        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 
        0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 
        0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00,
        0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 
        0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 
        0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  
};

//需要处理的图像的长宽
#define DEAL_IMAGE_H 120
#define DEAL_IMAGE_W 188


//一些常用变量
uint8 binary_image[MT9V03X_H][MT9V03X_W]; //二值化图像数组
uint8 image_threshold;//图像阈值
int16 left_line[MT9V03X_H];//左边界数组
int16 right_line[MT9V03X_H];//右边界数组
uint8 mid_line[MT9V03X_H];//中线数组
float line_err;//中线误差
uint8 white_count[MT9V03X_W];//每一列的白列长度
uint8 search_stop_line;//搜索截止行
uint8 boundary_start_left;//左边界起点
uint8 boundary_start_right;//右边界起点
uint8 left_start;
uint8 right_start;
int16 longest_white_left[2];//左边的最长白列，0为长度，1为下标
int16 longest_white_right[2];//右边的最长白列，0为长度，1为下标
uint8 left_lost_flag[MT9V03X_H];//左丢线数组
uint8 right_lost_flag[MT9V03X_H];//右丢线数组
uint8 left_lost_count;//左丢线计数
uint8 right_lost_count;//右丢线计数
uint8 left_right_lost_count;//左右同时丢线计数
uint8 left_down_point;//左下拐点
uint8 left_up_point;//左上拐点
uint8 right_down_point;//右下拐点
uint8 right_up_point;//右上拐点


//元素标志位
uint8 cross_flag;//十字标志位
uint8 circle_flag; //环岛标志位

uint8 circle_once_time=0; //环岛标志位单次触发
uint8 ramp_once_time=0; //坡道标志位单次触发
uint8 right_circle_flag; //右环岛标志

uint8 straight_flag=0; //直线标志位

uint8 ramp_flag=0; //坡道标志位
uint8 ramp_up_flag = 0;           // 上坡标志
uint8 ramp_top_flag = 0;          // 坡顶标志
uint8 ramp_down_flag = 0;         // 下坡标
uint8 ramp_protect = 0;           // 坡道保护标志志

uint8 zebra_count_total = 0;      // 斑马线总计数
uint8 zebra_detect_state = 0;     // 斑马线检测状态 0:未检测 1:检测中 2:已通过
uint16 zebra_clear_timer = 0;     // 斑马线清除计时器
uint8 zebra_last_flag = 0;        // 上次斑马线标志
uint8 zebra_flag = 0;          // 斑马线标志位

uint8 obstacle_state = 0;           // 路障状态 0:无 1:发现路障 2:避障中 3:回归中
uint8 obstacle_step = 0;            // 路障处理步骤
uint8 obstacle_counter1 = 0;        // 计数器1
uint8 obstacle_counter2 = 0;        // 计数器2 
uint8 obstacle_counter3 = 0;        // 计数器3
uint8 track_narrow_count = 0;       // 赛道变窄计数
uint8 obstacle_flag1 = 0;           // 路障标志1
uint8 obstacle_flag2 = 0;           // 路障标志2
uint8 obstacle_flag3 = 0;           // 路障标志3
uint8 obstacle_direction = 0;       // 避障方向 1:左避障 2:右避障
int32 obstacle_encoder_sum = 0;     // 路障编码器积分

uint8 obstacle_flag = 0;                   // 路障总标志

//右环岛处理中间变量
uint8 continuity_left_change_flag=0;//左边连续变化标志
uint8 continuity_right_change_flag=0;//右边连续变化标志
uint8 left_change_line=0;//左边突变点
uint8 right_change_line=0;//右边突变点
uint8 err_start_point; //误差起始点
uint8 err_end_point;   //误差终止点
int16 encoder_sum;//圆环状态编码器计数

//坡道相关变量
int16 ramp_xianzhi=0; //坡道计时器
int16 circle_xianzhi=0; //环岛计时器


extern uint8 data_buffer[100];//无线转串口数据缓冲区


//误差权重数组(后期使用)
const uint8 weight[DEAL_IMAGE_H]= 
{
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0
};

const uint8 road_wide[DEAL_IMAGE_H]=
{
41,42,43,45,46,47,49,49,51,53,
53,55,55,57,58,59,61,62,63,64,
65,67,68,69,70,72,73,74,76,76,
78,79,80,82,82,84,86,86,88,88,
90,91,92,94,95,96,97,98,100,100,
102,103,105,105,107,108,109,111,112,113,
114,116,117,118,119,120,122,123,124,126,
126,128,129,130,132,132,134,134,136,138,
138,140,140,142,144,144,146,146,148,149,
150,151,152,154,155,156,157,158,159,161,
162,163,164,165,166,167,169,170,171,172,
173,175,175,177,177,179,180,181,184,184
};

uint8 real_road_wide[DEAL_IMAGE_H];


/**
* @brief  摄像头初始化函数
* @param  无
*/
void image_init(void)
{
    ips200_show_string(0, 0, "mt9v03x init.");
    while(1)
    {
        if(mt9v03x_init())
        {
            ips200_show_string(0, 16, "mt9v03x reinit.");
        }
        else
        {
            break;
        }
        system_delay_ms(500);                                                   // 短延时快速闪灯表示异常
    }
    ips200_show_string(0, 16, "mt9v03x init success.");
    system_delay_ms(100);//延时100ms等待摄像头稳定                                                  
}

/**
* @brief  数据初始化
*/
void boundary_line_init(void)
{
    for(int i=0;i<MT9V03X_H;i++)
    {
        left_line[i]=0;
        right_line[i]=MT9V03X_W-1;
        mid_line[i]=(left_line[i]+right_line[i])/2;
        left_lost_flag[i]=0;
        right_lost_flag[i]=0;
    }
    boundary_start_left=0;
    boundary_start_right=MT9V03X_W-1;
    left_down_point=0;
    left_up_point=0;
    right_down_point=0;
    right_up_point=0;
}


/**
* @brief  摄像头原始图像显示函数
* @param  x 图像起始x
* @param  y 图像起始y
*/
void show_real_image(uint16 x, uint16 y)
{
    	if(mt9v03x_finish_flag)
		{
			uint8 image_copy[MT9V03X_H][MT9V03X_W];
            memcpy(image_copy, mt9v03x_image, MT9V03X_H*MT9V03X_W);
			ips200_show_gray_image(x, y, (const uint8 *)image_copy, MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);
			mt9v03x_finish_flag=0;
		}
}

/**
* @brief  摄像头二值化图像显示函数
* @param  x 图像起始x
* @param  y 图像起始y
*/
void show_binary_image(uint16 x, uint16 y,uint8 threshold)
{
        if(mt9v03x_finish_flag)
		{
			uint8 image_copy[MT9V03X_H][MT9V03X_W];
            memcpy(image_copy,mt9v03x_image , MT9V03X_H*MT9V03X_W);
			ips200_show_gray_image(x, y, (const uint8 *)image_copy, MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, threshold);
			mt9v03x_finish_flag=0;
		}
}

/**
* @brief  边线数组以及中线数组显示函数
*/
void show_boundary_line(void)
{
    for(int i=30;i<30+MT9V03X_H;i++)
    {
        if((i-30)!=left_down_point&&(i-30)!=left_up_point)
        {
            ips200_draw_point(left_line[i-30],i,RGB565_RED);
            ips200_draw_point(left_line[i-30]+1,i,RGB565_RED);
            ips200_draw_point(left_line[i-30]+2,i,RGB565_RED);  
        }
        else
        {
            if((i-30)==left_down_point)
            {
                ips200_draw_point(left_line[i-30],i-1,RGB565_BLUE);
                ips200_draw_point(left_line[i-30]+1,i-1,RGB565_BLUE);
                ips200_draw_point(left_line[i-30]+2,i-1,RGB565_BLUE);
                ips200_draw_point(left_line[i-30],i,RGB565_BLUE);
                ips200_draw_point(left_line[i-30]+1,i,RGB565_BLUE);
                ips200_draw_point(left_line[i-30]+2,i,RGB565_BLUE);
                ips200_draw_point(left_line[i-30],i+1,RGB565_BLUE);
                ips200_draw_point(left_line[i-30]+1,i+1,RGB565_BLUE);
                ips200_draw_point(left_line[i-30]+2,i+1,RGB565_BLUE);
            }
            else
            {
                ips200_draw_point(left_line[i-30],i-1,RGB565_BLUE);
                ips200_draw_point(left_line[i-30]+1,i-1,RGB565_BLUE);
                ips200_draw_point(left_line[i-30]+2,i-1,RGB565_BLUE);
                ips200_draw_point(left_line[i-30],i,RGB565_BLUE);
                ips200_draw_point(left_line[i-30]+1,i,RGB565_BLUE);
                ips200_draw_point(left_line[i-30]+2,i,RGB565_BLUE);
                ips200_draw_point(left_line[i-30],i+1,RGB565_BLUE);
                ips200_draw_point(left_line[i-30]+1,i+1,RGB565_BLUE);
                ips200_draw_point(left_line[i-30]+2,i+1,RGB565_BLUE);
            }
        }

        if((i-30)!=right_down_point&&(i-30)!=right_up_point)
        { 
            ips200_draw_point(right_line[i-30],i,RGB565_RED);
            ips200_draw_point(right_line[i-30]-1,i,RGB565_RED);
            ips200_draw_point(right_line[i-30]-2,i,RGB565_RED);
        }
        else
        {
            if((i-30)==right_down_point)
            {
                ips200_draw_point(right_line[i-30],i-1,RGB565_BLUE);
                ips200_draw_point(right_line[i-30]-1,i-1,RGB565_BLUE);
                ips200_draw_point(right_line[i-30]-2,i-1,RGB565_BLUE);
                ips200_draw_point(right_line[i-30],i,RGB565_BLUE);
                ips200_draw_point(right_line[i-30]-1,i,RGB565_BLUE);
                ips200_draw_point(right_line[i-30]-2,i,RGB565_BLUE);
                ips200_draw_point(right_line[i-30],i+1,RGB565_BLUE);
                ips200_draw_point(right_line[i-30]-1,i+1,RGB565_BLUE);
                ips200_draw_point(right_line[i-30]-2,i+1,RGB565_BLUE);
            }
            else
            {
                ips200_draw_point(right_line[i-30],i-1,RGB565_BLUE);
                ips200_draw_point(right_line[i-30]-1,i-1,RGB565_BLUE);
                ips200_draw_point(right_line[i-30]-2,i-1,RGB565_BLUE);
                ips200_draw_point(right_line[i-30],i,RGB565_BLUE);
                ips200_draw_point(right_line[i-30]-1,i,RGB565_BLUE);
                ips200_draw_point(right_line[i-30]-2,i,RGB565_BLUE);
                ips200_draw_point(right_line[i-30],i+1,RGB565_BLUE);
                ips200_draw_point(right_line[i-30]-1,i+1,RGB565_BLUE);
                ips200_draw_point(right_line[i-30]-2,i+1,RGB565_BLUE);
            }
        }

        ips200_draw_point(mid_line[i-30],i,RGB565_GREEN);
        ips200_draw_point(mid_line[i-30]-1,i,RGB565_GREEN);
        ips200_draw_point(mid_line[i-30]+1,i,RGB565_GREEN);
    }
}

/**
* @brief   出界判断,选取图像底部中间10*3区域进行判断
* @param   *in_image  原始图像
* @retval   0:正常 1:出界
*/
uint8 image_out_of_bounds(unsigned char in_image[DEAL_IMAGE_H][DEAL_IMAGE_W])
{
    int sum=0;
    for(int i=0;i<10;i++)
    {
        for(int j=0;j<3;j++)
        {
            sum+=in_image[DEAL_IMAGE_H-1-j][DEAL_IMAGE_W/2-5+i];
        }
    }
    int average = sum / 30; // 计算平均值
    if(average<140)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
* @brief   图像压缩
* @param   *in_image  原始图像
* @param   *out_image 压缩后图像
*/
void Image_Compress(unsigned char *in_image, unsigned char *out_image)
{
    float Hight_Ratio = (float)MT9V03X_H / DEAL_IMAGE_H;
    float Width_Ratio = (float)MT9V03X_W / DEAL_IMAGE_W;

    for (unsigned int CompressY = 0; CompressY < DEAL_IMAGE_H; CompressY++) {
        unsigned int OriginalY = (unsigned int)(CompressY * Hight_Ratio);
        unsigned int CompressTemp = DEAL_IMAGE_W * CompressY;
        unsigned int OriginalTemp = MT9V03X_W * OriginalY;

        for (unsigned int CompressX = 0; CompressX < DEAL_IMAGE_W; CompressX++) {
            unsigned int OriginalX = (unsigned int)(CompressX * Width_Ratio);
            *(out_image + CompressTemp + CompressX) = *(in_image + OriginalTemp + OriginalX);
        }
    }
}

/**
  * @brief  otsu算法获取阈值
  * @param  *image 图像数组指针 
  * @param  col 图像宽度
  * @param  row 图像高度
  * @retval 阈值
  */
int otsu_get_threshold(uint8 *image, uint16 col, uint16 row)   
{
    #define GrayScale 256
    uint16 width = col;
    uint16 height = row;
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j;
    int pixelSum = width * height/4;
    int threshold = 0;
    uint8* data = image;  
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }
    uint32 gray_sum=0;
    //统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height; i+=2)
    {
        for (j = 0; j < width; j+=2)
        {
            pixelCount[(int)data[i * width + j]]++;  //将当前的点的像素值作为计数数组的下标
            gray_sum+=(int)data[i * width + j];       //灰度值总和
        }
    }
    //计算每个像素值的点在整幅图像中的比例
    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }
    //遍历灰度级[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = 0; j < GrayScale; j++)
    {
        w0 += pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
        u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值
        w1=1-w0;
        u1tmp=gray_sum/pixelSum-u0tmp;
        u0 = u0tmp / w0;              //背景平均灰度
        u1 = u1tmp / w1;              //前景平均灰度
        u = u0tmp + u1tmp;            //全局平均灰度
        deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = j;
        }
        if (deltaTmp < deltaMax)
        {
            break;
        }
    }
    return threshold;
}

/**
* @brief   图像二值化
* @param   *in_image  压缩后图像
* @param   *out_image 二值化图像
*/
void image_binary(uint8 *in_image,uint8 *out_image)
{
    uint8 i, j;
    for (i = 0; i < DEAL_IMAGE_H; i++)
    {
        for (j = 0; j < DEAL_IMAGE_W; j++)
        {
            if (*(in_image + DEAL_IMAGE_W * i + j) >= image_threshold)
            {
                *(out_image + DEAL_IMAGE_W * i + j) = 1; //白色
            }
            else
            {
                *(out_image + DEAL_IMAGE_W * i + j) = 0; //黑色
            }
        }
    }
}

/**
*
* @brief  给图像上左右画两列黑框
* @param image[DEAL_IMAGE_H][DEAL_IMAGE_W] 压缩后图像
*
**/
void image_draw_black(unsigned char image[DEAL_IMAGE_H][DEAL_IMAGE_W])
{
    uint8 i = 0;
    for (i = 0; i < DEAL_IMAGE_H; i++)
    {
        image[i][0] = 0;
        image[i][1] = 0;
        image[i][DEAL_IMAGE_W - 1] = 0;
        image[i][DEAL_IMAGE_W - 2] = 0;
    }
    for (i = 0; i < DEAL_IMAGE_W; i++)
    {
        image[0][i] = 0;
        image[1][i] = 0;
    }
}




/**
*
* @brief  图像扫线，根据网上的教程选用双最长白列
* @param  image 图像数组
*
**/
void longest_white_sweep_line(uint8 image[DEAL_IMAGE_H][DEAL_IMAGE_W])
{
    int i,j;
    int start_point = 20;//起搜下标
    int end_point = DEAL_IMAGE_W - 20;//结束下标
    int left_border = 0, right_border = 0;//存储赛道临时位置
    //变量清零
    longest_white_left[0] = 0;
    longest_white_left[1] = 0;
    longest_white_right[0] = 0;
    longest_white_right[1] = 0;
    left_start = 0;
    right_start = 0;
    for(i=0;i<=DEAL_IMAGE_H-1;i++)
    {
        left_line[i]=0;
        right_line[i]=DEAL_IMAGE_W-1;
        left_lost_flag[i]=0;
        right_lost_flag[i]=0;
        real_road_wide[i]=0;
    }
    for(i=0;i<=DEAL_IMAGE_W-1;i++)
    {
        white_count[i]=0;
    }
    //记录每列白点数量
    for(j=start_point;j<end_point;j++)
    {
        for(i=DEAL_IMAGE_H-1;i>=0;i--)
        {
            if(image[i][j] == 0)break;//黑点跳出
            else white_count[j]++;//白点计数
        }
    }
    //寻找左最长白列
    for(i=start_point;i<end_point;i+=1)
    {
        if( longest_white_left[0] < white_count[i])//找最长的那一列，寻到右边界
        {
            longest_white_left[0] = white_count[i];
            longest_white_left[1] = i;
        }
    }
    //寻找右最长白列
    for(i=end_point;i>=longest_white_left[1];i-=1)//从右往左，找到右最长白列，寻到左边最长白列位置
    {
        if( longest_white_right[0] < white_count[i])//找最长的那一列
        {
            longest_white_right[0] = white_count[i];
            longest_white_right[1] = i;
        }
    }
    
    search_stop_line = (longest_white_left[0]>longest_white_right[0])?longest_white_left[0]:longest_white_right[0];//非常重要，搜索截止行存储

    //巡线
    for(i = DEAL_IMAGE_H-1;i>=DEAL_IMAGE_H-search_stop_line&&i>=0;i--)
    {
        for(j = longest_white_right[1]; j<=DEAL_IMAGE_W-1-2;j++)//从右最长白列找右边
        {
            if(image[i][j] == 1 && image[i][j+1] == 0 && image[i][j+2] == 0)//白黑黑，找到右边界
            {
                if(right_start==0)//左边界起点
                {
                    right_start = j;
                }
                right_border = j;
                right_lost_flag[i]=0;
                break;
            }
            else if(j>=DEAL_IMAGE_W-1-2)//右边界丢失
            {
                if(right_start==0)//左边界起点
                {
                    right_start = j;
                }
                right_border = DEAL_IMAGE_W-1;
                right_lost_flag[i]=1;//右边界丢线记录
                break;
            }
        }
        for(j = longest_white_left[1]; j>=2;j--)//从左最长白列找左边
        {
            if(image[i][j] == 1 && image[i][j-1] == 0 && image[i][j-2] == 0)//白黑黑，找到右边界
            {
                if(left_start==0)//左边界起点
                {
                    left_start = j;
                }
                left_border = j;
                left_lost_flag[i]=0;
                break;
            }
            else if(j<=2)//左边界丢失
            {
                if(left_start==0)//左边界起点
                {
                    left_start = j;
                }
                left_border = 0;
                left_lost_flag[i]=1;//左边界丢线记录
                break;
            }
        }
        left_line[i]=left_border;//存储左边线
        right_line[i]=right_border;//存储右边线
        real_road_wide[i]=right_border-left_border;//存储赛道宽度
        // printf("%d\r\n",right_border-left_border);//获取赛道宽度
    }

    //边界丢线清零
    left_lost_count=0;
    right_lost_count=0;
    left_right_lost_count=0;
    //记录边界起点
    boundary_start_left=0;
    boundary_start_right=0;
    //记录丢边情况
    for(i=DEAL_IMAGE_H-1;i>=DEAL_IMAGE_H-search_stop_line;i--)
    {
        if(boundary_start_left==0&&left_lost_flag[i]==0)
        {
            boundary_start_left=i;//记录左边界起点
        }

        if(boundary_start_right==0&&right_lost_flag[i]==0)
        {
            boundary_start_right=i;//记录右边界起点
        }

        if(left_lost_flag[i]==1&&right_lost_flag[i]==0)left_lost_count++;//左丢
        if(left_lost_flag[i]==0&&right_lost_flag[i]==1)right_lost_count++;//�1��丢
        if(left_lost_flag[i]==1&&right_lost_flag[i]==1)left_right_lost_count++;//丢双边
    }

    if(car_go)
    {
        zebra_judge_multi();//判断斑马线
    }

    if(ramp_xianzhi>100&&!ramp_once_time)
    {
        ramp_judge();//判断坡道
    }
    
    // obstacle_detect();//路障检测
    
    if(circle_once_time&& circle_xianzhi>250)
    {
        circle_judge();//判断环岛
    }


    cross_judge();//判断十字

    if(straight_judge())//判断直线
    {
        straight_flag=1;//直线标志位
    }
    else
    {
        straight_flag=0;//不是直线
    }


    for(i = DEAL_IMAGE_H-1;i>=DEAL_IMAGE_H-search_stop_line&&i>=0;i--)
    {
        mid_line[i]=(left_line[i]+right_line[i])/2;//存储中线
    }
}

/**
*
* @brief  计算某几行的平均误差(我打算前期用这个调前瞻范围)
* @param  start_point
* @param  end_point
* @retval err 误差值
**/
float err_sum_average(uint8 start_point,uint8 end_point)
{
    //防止参数输入错误
    if(end_point<start_point)
    {
        uint8 t=end_point;
        end_point=start_point;
        start_point=t;
    }

    if(start_point<DEAL_IMAGE_H-search_stop_line)start_point=DEAL_IMAGE_H-search_stop_line-1;//防止起点越界
    if(end_point<DEAL_IMAGE_H-search_stop_line)end_point=DEAL_IMAGE_H-search_stop_line-2;//防止终点越界

    float err=0;
    for(int i=start_point;i<end_point;i++)
    {
        err+=(DEAL_IMAGE_W/2-((left_line[i]+right_line[i])>>1));//位操作等效除以2
    }
    err=err/(end_point-start_point);
    return err;
}

/**
*
* @brief  计算加权平均误差(后期使用)
* @param  无
* @retval err 误差值
**/
float err_sum_weight(void)
{
    float err;
    uint8 weight_sum;
    for(int i=DEAL_IMAGE_H-1;i>DEAL_IMAGE_H-search_stop_line;i--)
    {
        err+=(DEAL_IMAGE_W/2-((left_line[i]+right_line[i])>>1)*weight[i]);//位操作等效除以2
        weight_sum+=weight[i];
    }
    err=err/weight_sum;
    return err;
}


/**
*
* @brief  左边补线
* @param  x1 起点x坐标
* @param  x2 终点x坐标
* @param  y1 起点y坐标
* @param  y2 终点y坐标
**/
void left_draw_line(uint8 x1,uint8 y1,uint8 x2,uint8 y2)
{
    uint8 hx;
    uint8 a1=y1;
    uint8 a2=y2;
    //防止越界以及参数输入错误
    if(y1>y2)
    {
        uint8 t=y1;
        y1=y2;
        y2=t;
    }

    if(x1>=DEAL_IMAGE_W-1)x1=DEAL_IMAGE_W-1;
    else if(x1<=0)x1=0;
    if(y1>=DEAL_IMAGE_H-1)y1=DEAL_IMAGE_H-1;
    else if(y1<=0)y1=0;

    if(x2>=DEAL_IMAGE_W-1)x2=DEAL_IMAGE_W-1;
    else if(x2<=0)x2=0;
    if(y2>=DEAL_IMAGE_H-1)y2=DEAL_IMAGE_H-1;
    else if(y2<=0)y2=0;

    for(uint8 i=a1;i<a2;i++)
    {
        hx=x1+(i-y1)*(x2-x1)/(y2-y1);//使用斜率补线
        //防止补线越界
        if(hx>=DEAL_IMAGE_W-1)hx=DEAL_IMAGE_W-1;
        else if(hx<=0)hx=0;
        left_line[i]=hx;
    }
}

/**
*
* @brief  右边补线(虽然和左边补线是一样的，但还是要分开，因为会有单边补线)
* @param  x1 起点x坐标
* @param  x2 终点x坐标
* @param  y1 起点y坐标
* @param  y2 终点y坐标
**/
void right_draw_line(uint8 x1,uint8 y1,uint8 x2,uint8 y2)
{
    uint8 hx;
    uint8 a1=y1;
    uint8 a2=y2;
    //防止越界以及参数输入错误
    if(y1>y2)
    {
        uint8 t=y1;
        y1=y2;
        y2=t;
    }

    if(x1>=DEAL_IMAGE_W-1)x1=DEAL_IMAGE_W-1;
    else if(x1<=0)x1=0;
    if(y1>=DEAL_IMAGE_H-1)y1=DEAL_IMAGE_H-1;
    else if(y1<=0)y1=0;

    if(x2>=DEAL_IMAGE_W-1)x2=DEAL_IMAGE_W-1;
    else if(x2<=0)x2=0;
    if(y2>=DEAL_IMAGE_H-1)y2=DEAL_IMAGE_H-1;
    else if(y2<=0)y2=0;

    for(uint8 i=a1;i<a2;i++)
    {
        hx=x1+(i-y1)*(x2-x1)/(y2-y1);//使用斜率补线
        //防止补线越界
        if(hx>=DEAL_IMAGE_W-1)hx=DEAL_IMAGE_W-1;
        else if(hx<=0)hx=0;
        right_line[i]=hx;
    }
}

/**
*
* @brief  左边界延长
* @param  start_point 延长起点
* @param  end_point   延长终点
**/
void lenthen_left_line(uint8 start_point,uint8 end_point)
{
    float k;
    //防止越界
    if(start_point>=DEAL_IMAGE_H-1)start_point=DEAL_IMAGE_H-1;
    if(start_point<0)start_point=0;
    if(end_point>=DEAL_IMAGE_H-1)end_point=DEAL_IMAGE_H-1;
    if(end_point<0)end_point=0;
    
    if(end_point<start_point)
    {
        uint8 t=start_point;
        start_point=end_point;
        end_point=t;
    }

    if(start_point<=5)//起点过于靠上，直接连线
    {
        left_draw_line(left_line[start_point],start_point,left_line[end_point],end_point);
    }
    else
    {
        k=(float)(left_line[start_point]-left_line[start_point-4])/5.0;//斜率
        for(uint8 i=start_point;i<=end_point;i++)
        {
            left_line[i]=left_line[start_point]+(int)(i-start_point)*k;//使用斜率延长

            if(left_line[i]<1)//防止越界
            {
                left_line[i]=1;
            }

            if(left_line[i]>=DEAL_IMAGE_W-2)//防止越界
            {
                left_line[i]=DEAL_IMAGE_W-2;
            }
        }
    }
}


/**
* @brief  左边界延长（从下往上）
* @param  start_point 延长起点（下方点）
* @param  end_point   延长终点（上方点）
**/
void lenthen_left_line_up(uint8 start_point, uint8 end_point)
{
    float k;
    // 防止越界
    if (start_point >= DEAL_IMAGE_H - 1) start_point = DEAL_IMAGE_H - 1;
    if (start_point < 0) start_point = 0;
    if (end_point >= DEAL_IMAGE_H - 1) end_point = DEAL_IMAGE_H - 1;
    if (end_point < 0) end_point = 0;

    // 确保起点大于终点
    if (start_point < end_point) {
        uint8 t = start_point;
        start_point = end_point;
        end_point = t;
    }

    // 如果起点过于靠下，直接连线
    if (start_point >= DEAL_IMAGE_H - 6) {
        left_draw_line(left_line[start_point], start_point, left_line[end_point], end_point);
    } else {
        // 计算斜率
        k = (float)(left_line[start_point] - left_line[start_point + 4]) / 5.0;
        
        // 从起点向上延长
        for (int16_t i = start_point; i >= end_point; i--) {
            left_line[i] = left_line[start_point] + (int)((i - start_point) * (-k));// 使用斜率延长(负斜率)
            
            // 防止越界
            if (left_line[i] < 1) {
                left_line[i] = 1;
            }
            if (left_line[i] >= DEAL_IMAGE_W - 2) {
                left_line[i] = DEAL_IMAGE_W - 2;
            }
        }
    }
}



/**
*
* @brief  右边界延长
* @param  start_point 延长起点
* @param  end_point   延长终点
**/
void lenthen_right_line(uint8 start_point,uint8 end_point)
{
    float k;
    //防止越界
    if(start_point>=DEAL_IMAGE_H-1)start_point=DEAL_IMAGE_H-1;
    if(start_point<0)start_point=0;
    if(end_point>=DEAL_IMAGE_H-1)end_point=DEAL_IMAGE_H-1;
    if(end_point<0)end_point=0;
    
    if(end_point<start_point)
    {
        uint8 t=start_point;
        start_point=end_point;
        end_point=t;
    }

    if(start_point<=5)//起点过于靠上，直接连线
    {
        left_draw_line(right_line[start_point],start_point,right_line[end_point],end_point);
    }
    else
    {
        k=(float)(right_line[start_point]-right_line[start_point-4])/5.0;//斜率
        for(uint8 i=start_point;i<=end_point;i++)
        {
            right_line[i]=right_line[start_point]+(int)(i-start_point)*k;//使用斜率延长

            if(right_line[i]<1)//防止越界
            {
                right_line[i]=1;
            }
            
            if(right_line[i]>=DEAL_IMAGE_W-2)//防止越界
            {
                right_line[i]=DEAL_IMAGE_W-2;
            }
        }
    }
}


/**
*
* @brief  道宽半边补左线
**/
void road_wide_draw_left_line(void)
{
    for(int i=0;i<DEAL_IMAGE_H-1;i++)
    {
        left_line[i]=right_line[i]-road_wide[i];
        if(left_line[i]<1)//防止越界
        {
            left_line[i]=1;
        }
    }
}

/**
*
* @brief  道宽半边补右线
**/
void road_wide_draw_right_line(void)
{
    for(int i=0;i<DEAL_IMAGE_H-1;i++)
    {
        right_line[i]=left_line[i]+road_wide[i];
        if(right_line[i]>=DEAL_IMAGE_W-2)//防止越界
        {
            right_line[i]=DEAL_IMAGE_W-2;
        }
    }
}


/**
*
* @brief  找下拐点
* @param  start_point 搜索起点 
* @param  end_point    搜索终点
**/

void find_down_point(uint8 start_point,uint8 end_point)
{
    //参数清零
    left_down_point=0;
    right_down_point=0;
    if(start_point<end_point)
    {
        uint8 t=start_point;
        start_point=end_point;
        end_point=t;
    }
    if(start_point>DEAL_IMAGE_H-5-1)
    {
        start_point=DEAL_IMAGE_H-5-1;
    }
    if(end_point<DEAL_IMAGE_H-search_stop_line)
    {
        end_point=DEAL_IMAGE_H-search_stop_line;
    }
    if(end_point<5)
    {
        end_point=5;
    }
    for(int i=start_point;i>=end_point;i--)
    {
        //点i下面2个连续相差不大并且点i与上面边3个点分别相差很大，认为有下左拐点
        if(left_down_point==0&&
            abs(left_line[i]-left_line[i+1])<=7&&
            abs(left_line[i+1]-left_line[i+2])<=7&&
            abs(left_line[i+2]-left_line[i+3])<=7&&
            (left_line[i]-left_line[i-2])>=8&&
            (left_line[i]-left_line[i-3])>=8&&
            (left_line[i]-left_line[i-4])>=8)
            {
                left_down_point=i+3;
            }
        if(right_down_point==0&&
            abs(right_line[i]-right_line[i+1])<=6&&
            abs(right_line[i+1]-right_line[i+2])<=6&&
            abs(right_line[i+2]-right_line[i+3])<=6&&
            (right_line[i]-right_line[i-2])<=-8&&
            (right_line[i]-right_line[i-3])<=-8&&
            (right_line[i]-right_line[i-4])<=-8)
            {
                right_down_point=i+3;
            }
        if(left_down_point!=0&&right_down_point!=0)
        {
            break;
        }       
    }
}

/**
*
* @brief  找上拐点
* @param  start_point 搜索起点 
* @param  end_point    搜索终点
**/
void find_up_point(uint8 start_point,uint8 end_point)
{
    left_up_point=0;
    right_up_point=0;
    if(start_point<end_point)
    {
        uint8 t=start_point;
        start_point=end_point;
        end_point=t;
    }

    if(start_point>DEAL_IMAGE_H-5-1)
    {
        start_point=DEAL_IMAGE_H-5-1;
    }

    if(end_point<DEAL_IMAGE_H-search_stop_line)
    {
        end_point=DEAL_IMAGE_H-search_stop_line;
    }

    if(end_point<5)
    {
        end_point=5;
    }

    for(int i=start_point;i>=end_point;i--)
    {
        //点i下面2个连续相差不大并且点i与上面边3个点分别相差很大，认为有上左拐点
        if(left_up_point==0&&
            abs(left_line[i]-left_line[i-1])<=5&&
            abs(left_line[i-1]-left_line[i-2])<=5&&
            abs(left_line[i-2]-left_line[i-3])<=5&&
            (left_line[i]-left_line[i+2])>=8&&
            (left_line[i]-left_line[i+3])>=15&&
            (left_line[i]-left_line[i+4])>=15)
            {
                left_up_point=i-3;
            }
        if(right_up_point==0&&
            abs(right_line[i]-right_line[i-1])<=3&&
            abs(right_line[i-1]-right_line[i-2])<=3&&
            abs(right_line[i-2]-right_line[i-3])<=3&&
            (right_line[i]-right_line[i+2])<=-8&&
            (right_line[i]-right_line[i+3])<=-15&&
            (right_line[i]-right_line[i+4])<=-15)
            {
                right_up_point=i-3;
            }
        if(left_up_point!=0&&right_up_point!=0)
        {
            break;
        }       
    }
}

/**
*
* @brief  找圆环右上拐点
* @param  start_point 搜索起点 
* @param  end_point    搜索终点
**/
void circle_right_up_point(uint8 start_point,uint8 end_point)
{
    right_up_point=0;
    if(start_point<end_point)
    {
        uint8 t=start_point;
        start_point=end_point;
        end_point=t;
    }

    if(start_point>DEAL_IMAGE_H-5-1)
    {
        start_point=DEAL_IMAGE_H-5-1;
    }

    if(end_point<5)
    {
        end_point=5;
    }

    for(int i=start_point;i>=end_point;i--)
    {
        if(right_up_point==0&&
            abs(right_line[i]-right_line[i-1])<=5&&
            abs(right_line[i-1]-right_line[i-2])<=5&&
            abs(right_line[i-2]-right_line[i-3])<=5&&
            (right_line[i]-right_line[i+2])<=-6&&
            (right_line[i]-right_line[i+3])<=-8&&
            (right_line[i]-right_line[i+4])<=-8)
            {
                right_up_point=i-3;
            }
        if(right_up_point!=0)
        {
            break;
        }       
    }
}

/**
* @brief  找圆环右下拐点
* @param  start_point 搜索起点
* @param  end_point    搜索终点
**/
void circle_right_down_point(uint8 start_point,uint8 end_point)
{
    //参数清零
    right_down_point=0;
    if(start_point<end_point)
    {
        uint8 t=start_point;
        start_point=end_point;
        end_point=t;
    }
    if(start_point>DEAL_IMAGE_H-5-1)
    {
        start_point=DEAL_IMAGE_H-5-1;
    }
    if(end_point<5)
    {
        end_point=5;
    }
    for(int i=start_point;i>=end_point;i--)
    {
        if(right_down_point==0&&
            abs(right_line[i]-right_line[i+1])<=10&&
            abs(right_line[i+1]-right_line[i+2])<=10&&
            abs(right_line[i+2]-right_line[i+3])<=10&&
            (right_line[i]-right_line[i-2])<=-3&&
            (right_line[i]-right_line[i-3])<=-3&&
            (right_line[i]-right_line[i-4])<=-3)
            {
                right_down_point=i+3;
            }
        if(right_down_point!=0)
        {
            break;
        }       
    }
}

/**
* @brief  找圆环左下拐点
* @param  start_point 搜索起点
* @param  end_point    搜索终点
**/
void circle_left_down_point(uint8 start_point,uint8 end_point)
{
    //参数清零
    left_down_point=0;
    if(start_point<end_point)
    {
        uint8 t=start_point;
        start_point=end_point;
        end_point=t;
    }
    if(start_point>DEAL_IMAGE_H-5-1)
    {
        start_point=DEAL_IMAGE_H-5-1;
    }
    if(end_point<5)
    {
        end_point=5;
    }
    for(int i=start_point;i>=end_point;i--)
    {
        if(right_down_point==0&&
            abs(right_line[i]-right_line[i+1])<=7&&
            abs(right_line[i+1]-right_line[i+2])<=7&&
            abs(right_line[i+2]-right_line[i+3])<=7&&
            (right_line[i]-right_line[i-2])>=8&&
            (right_line[i]-right_line[i-3])>=10&&
            (right_line[i]-right_line[i-4])>=10)
            {
                left_down_point=i+3;
            }
        if(left_down_point!=0)
        {
            break;
        }       
    }
}

/**
*
* @brief  判断右边界连续性
* @retval 连续返回0，不连续返回断裂点
**/
uint8 right_countinuity_detect(uint8 start_point,uint8 end_point)
{
    uint8 continuity_line=0;//连续标志

    if(start_point<end_point)//从下往上扫
    {
        uint8 t=start_point;
        start_point=end_point;
        end_point=t;
    }

    if(right_lost_count>DEAL_IMAGE_H*0.95)//如果右边丢线超过95%，直接返回1
    {
        return 1;
    }

    if(start_point>DEAL_IMAGE_H-5)//防止起点越界
    {
        start_point=DEAL_IMAGE_H-5;
    }

    if(end_point<5)//防止终点越界
    {
        end_point=5;
    }

    for(uint8 i=start_point;i>end_point;i--)
    {
        if(abs(right_line[i]-right_line[i-1])>=3)
        {
            continuity_line=i;//如果当前点与前一个点相差大于4，认为不连续
            break;
        }
    }
    return continuity_line;//返回断裂点坐标，如果返回0，表示连续
}

/**
*
* @brief  判断左边界连续性
* @retval 连续返回0，不连续返回断裂点
**/
uint8 left_countinuity_detect(uint8 start_point,uint8 end_point)
{
    uint8 continuity_line=0;//连续标志

    if(start_point<end_point)//从下往上扫
    {
        uint8 t=start_point;
        start_point=end_point;
        end_point=t;
    }

    if(left_lost_count>DEAL_IMAGE_H*0.95)//如果左边丢线超过95%，直接返回1
    {
        return 1;
    }

    if(search_stop_line<=5)//如果搜索截止行小于5，直接返回1
    {
        return 1;
    }

    if(start_point>DEAL_IMAGE_H-5)//防止起点越界
    {
        start_point=DEAL_IMAGE_H-5;
    }

    if(end_point<5)//防止终点越界
    {
        end_point=5;
    }

    for(uint8 i=start_point;i>end_point;i--)
    {
        if(abs(left_line[i]-left_line[i-1])>=3)
        {
            continuity_line=i;//如果当前点与前一个点相差大于4，认为不连续
            break;
        }
    }
    return continuity_line;//返回断裂点坐标，如果返回0，表示连续
}


/**
*
* @brief  找到右边单调性突变点
* @retval 拐点坐标
**/
uint8 find_right_change(uint8 start_point,uint8 end_point)
{
    uint8 right_change_line=0;//右边突变标志

    if(start_point<end_point)//从下往上扫
    {
        return right_change_line;//如果起点小于终点，直接返回0
    }

    if(start_point>=DEAL_IMAGE_H-5)//防止起点越界
    {
        start_point=DEAL_IMAGE_H-5;
    }

    if(end_point<5)//防止终点越界
    {
        end_point=5;
    }

    if(right_lost_count>DEAL_IMAGE_H*0.95)
    {
        return right_change_line;//如果右边丢线超过95%，直接返回0
    }

    for(uint8 i=start_point;i>end_point;i--)
    {
        if(abs(right_line[i]-right_line[i-5])<=10&&abs(right_line[i]-right_line[i+5])<=14)//如果当前点与前后5个点相差小于10
        {
            if(right_line[i]==right_line[i-5]&&right_line[i]==right_line[i+5]&&
            right_line[i]==right_line[i-4]&&right_line[i]==right_line[i+4]&&
            right_line[i]==right_line[i-3]&&right_line[i]==right_line[i+3]&&
            right_line[i]==right_line[i-2]&&right_line[i]==right_line[i+2]&&
            right_line[i]==right_line[i-1]&&right_line[i]==right_line[i+1])
            {
                continue;//如果当前点与前后5个点相等，继续
            }
            else if(right_line[i]<=right_line[i-5]&&right_line[i]<=right_line[i+5]&&
                    right_line[i]<=right_line[i-4]&&right_line[i]<=right_line[i+4]&&
                    right_line[i]<=right_line[i-3]&&right_line[i]<=right_line[i+3]&&
                    right_line[i]<=right_line[i-2]&&right_line[i]<=right_line[i+2]&&
                    right_line[i]<=right_line[i-1]&&right_line[i]<=right_line[i+1])
            {
                right_change_line=i;//如果当前点大于前后5个点，认为是突变点
                break;
            }
        }
    }

    return right_change_line;//返回突变点坐标
}





/**
*
* @brief  找到左边单调性突变点
* @retval 拐点坐标
**/
uint8 find_left_change(uint8 start_point,uint8 end_point)
{
    uint8 left_change_line=0;//右边突变标志

    if(start_point<end_point)//从下往上扫
    {
        return left_change_line;//如果起点小于终点，直接返回0
    }

    if(start_point>=DEAL_IMAGE_H-5)//防止起点越界
    {
        start_point=DEAL_IMAGE_H-5;
    }

    if(end_point<5)//防止终点越界
    {
        end_point=5;
    }

    if(right_lost_count>DEAL_IMAGE_H*0.95)
    {
        return left_change_line;//如果右边丢线超过95%，直接返回0
    }

    for(uint8 i=start_point;i>end_point;i--)
    {
        if(abs(left_line[i]-left_line[i-5])<=10&&abs(left_line[i]-left_line[i+5])<=12)//如果当前点与前后5个点相差小于10
        {
            if(left_line[i]==left_line[i-5]&&left_line[i]==left_line[i+5]&&
            left_line[i]==left_line[i-4]&&left_line[i]==left_line[i+4]&&
            left_line[i]==left_line[i-3]&&left_line[i]==left_line[i+3]&&
            left_line[i]==left_line[i-2]&&left_line[i]==left_line[i+2]&&
            left_line[i]==left_line[i-1]&&left_line[i]==left_line[i+1])
            {
                continue;//如果当前点与前后5个点相等，继续
            }
        else if(left_line[i]>=left_line[i-5]&&left_line[i]>=left_line[i+5]&&
                left_line[i]>=left_line[i-4]&&left_line[i]>=left_line[i+4]&&
                left_line[i]>=left_line[i-3]&&left_line[i]>=left_line[i+3]&&
                left_line[i]>=left_line[i-2]&&left_line[i]>=left_line[i+2]&&
                left_line[i]>=left_line[i-1]&&left_line[i]>=left_line[i+1])
            {
                left_change_line=i;//如果当前点大于前后5个点，认为是突变点
                break;
            }
        }
    }
    return left_change_line;//返回突变点坐标
}


/**
*
* @brief  判断直道
* @retval 直道返回1，非直道返回0
**/
uint8 straight_judge(void)
{
    if(search_stop_line>=110)
    {
        if(boundary_start_left>=115&&boundary_start_right>=115&&left_lost_count<10&&right_lost_count<10&&left_right_lost_count<10)
        {
            if(abs(line_err)<=7)
            {
                return 1;//直道
            }
            else
            {
                return 0;//非直道
            }
        }
    }
    else
    {
        return 0;//非直道
    }
}





/**
*
* @brief  判断十字路口并补线
**/
void cross_judge(void)
{

    if(!circle_flag)
    {
        if(left_right_lost_count>10)
        {
            find_up_point(MT9V03X_H-1,0);//寻找上拐点

            find_down_point(MT9V03X_H-1,(left_up_point+right_up_point)/2);//寻找下拐点

            // if(car_go)
            // {
            //     if(right_up_point||left_up_point)//如果找到了上拐点
            //     {
            //         beep_on();//蜂鸣器响
            //     } 
            // }

            if(right_up_point&&left_up_point)
            {
                cross_flag=1;//十字标志置1

                if(left_down_point&&right_down_point)//如果四个拐点都存在
                {
                    left_draw_line(left_line[left_up_point],left_up_point,left_line[left_down_point],left_down_point);//左边补线
                    right_draw_line(right_line[right_up_point],right_up_point,right_line[right_down_point],right_down_point);//右边补线
                }
                else if(left_down_point&&!right_down_point)//如果左边有下拐点，右边没有
                {
                    left_draw_line(left_line[left_up_point],left_up_point,left_line[left_down_point],left_down_point);//左边补线
                    lenthen_right_line(right_up_point-1,DEAL_IMAGE_H-1);//右边延长
                }
                else if(!left_down_point&&right_down_point)//如果右边有下拐点，左边没有
                {
                    right_draw_line(right_line[right_up_point],right_up_point,right_line[right_down_point],right_down_point);//右边补线
                    lenthen_left_line(left_up_point-1,DEAL_IMAGE_H-1);//左边延长
                }
                else if(!left_down_point&&!right_down_point)//如果四个拐点都不存在
                {
                    lenthen_left_line(left_up_point-1,DEAL_IMAGE_H-1);//左边延长
                    lenthen_right_line(right_up_point-1,DEAL_IMAGE_H-1);//右边延长
                }
            }
            else
            {
                cross_flag=0;//十字标志清零
            } 
        }
    }
}



/**
*
* @brief  判断斑马线状态（支持多次识别）
**/
void zebra_judge_multi(void)
{
    uint8 zebra_count = 0;
    uint8 zebra_detected = 0;  // 当前帧是否检测到斑马线
    
    // 基本条件检查
    if(longest_white_left[1] > 20 && longest_white_right[1] < DEAL_IMAGE_W - 20 &&
       longest_white_right[1] > 20 && longest_white_left[1] < DEAL_IMAGE_W - 20 &&
       search_stop_line >= 110 &&
       boundary_start_left >= DEAL_IMAGE_H - 20 &&
       boundary_start_right >= DEAL_IMAGE_H - 20)
    {
        // 检测斑马线特征
        for(int i = DEAL_IMAGE_H - 1; i >= DEAL_IMAGE_H - 3; i--) 
        {
            zebra_count = 0;  // 每行重新计数
            for(int j = 0; j <= DEAL_IMAGE_W - 1 - 3; j++)
            {
                // 检测白黑黑模式
                if(binary_image[i][j] == 1 && binary_image[i][j+1] == 0 && binary_image[i][j+2] == 0)
                {
                    zebra_count++;
                }
            }
            
            // 如果某一行的跳变次数足够多，认为检测到斑马线
            if(zebra_count >= 8)  // 适当降低阈值提高检测率
            {
                zebra_detected = 1;
                break;
            }
        }
    }
    
    // 状态机处理斑马线识别
    switch(zebra_detect_state)
    {
        case 0:  // 未检测状态
            if(zebra_detected)
            {
                zebra_detect_state = 1;  // 进入检测中状态
                zebra_clear_timer = 0;
                zebra_flag = 1;
                zebra_count_total++;     // 斑马线计数加1
                
                if(car_go)
                {
                    beep_on();  // 蜂鸣器提示
                }
            }
            else
            {
                zebra_flag = 0;
            }
            break;
            
        case 1:  // 检测中状态
            if(zebra_detected)
            {
                zebra_flag = 1;
                zebra_clear_timer = 0;  // 重置计时器
            }
            else
            {
                zebra_clear_timer++;
                if(zebra_clear_timer >= 5)  // 连续5帧未检测到，进入已通过状态
                {
                    zebra_detect_state = 2;
                    zebra_flag = 0;
                    zebra_clear_timer = 0;
                }
            }
            break;
            
        case 2:  // 已通过状态
            zebra_flag = 0;
            zebra_clear_timer++;
            if(zebra_clear_timer >= 20)  // 等待20帧后恢复到未检测状态
            {
                zebra_detect_state = 0;
                zebra_clear_timer = 0;
            }
            break;
    }
     
    zebra_last_flag = zebra_flag;
}



/**
*
* @brief  判断环岛状态并补线
**/
void circle_judge(void)
{
    
    continuity_left_change_flag=0;//左边连续变化标志
    continuity_right_change_flag=0;//右边连续变化标志
    left_change_line=0;//左边突变点
    right_change_line=0;//右边突变点

    if(cross_flag==0&&obstacle_flag==0)//避开十字
    {
        continuity_left_change_flag=left_countinuity_detect(DEAL_IMAGE_H-1-60,15);//判断左边连续性
        continuity_right_change_flag=right_countinuity_detect(DEAL_IMAGE_H-1-15,15);//判断右边连续性
        left_change_line=find_left_change(DEAL_IMAGE_H-1-40,15);//寻找左边突变点
        right_change_line=find_right_change(DEAL_IMAGE_H-1-5,5);//寻找右边突变点
        find_down_point(DEAL_IMAGE_H-20,60);//寻找下拐点
        circle_right_down_point(DEAL_IMAGE_H-5,80);//寻找右下拐点
        find_up_point(DEAL_IMAGE_H-5,10);//寻找上拐点
        // sprintf(data_buffer,"%d,%d,%d,%d,%d,%d,%d,%d,%d,\r\n",continuity_left_change_flag,continuity_right_change_flag,left_lost_count,right_lost_count,left_right_lost_count,boundary_start_left,boundary_start_right,search_stop_line,right_down_point);
        // wireless_uart_send_string(data_buffer);
        if(right_circle_flag==0)//处理右圆环
        {
            if(continuity_left_change_flag<=35&&
            continuity_right_change_flag!=0&&
            right_lost_count>=8&&right_lost_count<=110&&
            left_right_lost_count<=25&&
            boundary_start_left>=DEAL_IMAGE_H-15&&
            boundary_start_right>=DEAL_IMAGE_H-15&&
            search_stop_line>=115&&
            right_down_point
            )
            {
                if(car_go&&circle_flag==0&&right_circle_flag==0)//如果车在行驶中
                {
                    beep_on();//蜂鸣器响
                }
                right_circle_flag=1;//右圆环标志置1
                circle_flag=1;//环岛标志置1
                err_start_point=user_param.err_start+16;//错误起始点
                err_end_point=user_param.err_end+16;//错误终止点
            }
            else
            {
                right_circle_flag=0;//右圆环标志置0
                circle_flag=0;//环岛标志置0
            }
        }
        else if(circle_flag==1)
        {
            if(right_circle_flag==1)
            {    
                road_wide_draw_right_line();//右边道宽补线
                if(right_change_line>35&&right_up_point)//右边突点坐标过大并且有右上拐点
                {
                    right_circle_flag=2;//右圆环标志置2
                    if(car_go)
                    {
                        beep_on();//蜂鸣器响
                    }
                }
            }
            else if(right_circle_flag==2)
            {
                circle_right_up_point(DEAL_IMAGE_H-10,10);//寻找右上拐点
                if(right_up_point)
                {
                    left_draw_line(right_line[right_up_point],right_up_point,left_line[boundary_start_left],boundary_start_left);//左边补线
                }
                else
                {
                    road_wide_draw_left_line();//左边道宽补线
                }

                if(encoder_sum>=17000)
                {
                    right_circle_flag=3;//右圆环标志置0
                    if(car_go)
                    {
                        beep_on();//蜂鸣器响
                    }
                    encoder_sum=0;//编码器积分清零
                }

            }
            else if(right_circle_flag==3)
            {
                if(left_down_point>=3)
                {
                    right_circle_flag=4;//右圆环标志置4
                    if(car_go)
                    {
                        beep_on();//蜂鸣器响
                    }
                }
            }
            else if(right_circle_flag==4)
            {
                find_down_point(DEAL_IMAGE_H-10,10);//寻找下拐点
                lenthen_left_line_up(left_down_point,0);
                if(left_down_point>50)
                {
                    right_circle_flag=5;//右圆环标志置0
                    if(car_go)
                    {
                        beep_on();//蜂鸣器响
                    }
                }
            }
            else if(right_circle_flag==5)
            {
                if(right_up_point)
                {
                    right_circle_flag=6;
                    if(car_go)
                    {
                        beep_on();//蜂鸣器响
                    }
                }
                else
                {
                    road_wide_draw_left_line();//左边道宽补线
                }
            }
            else if(right_circle_flag==6)
            {
                find_up_point(DEAL_IMAGE_H-5,5);//寻找上拐点
                if(right_up_point)
                {
                    lenthen_right_line(right_up_point,DEAL_IMAGE_H-1);//右边延长
                }
                else
                {
                    road_wide_draw_right_line();//右边道宽补线
                }
                if(right_up_point==0)
                {
                    circle_flag=0;//环岛标志置0
                    right_circle_flag=0;//右圆环标志置0
                    encoder_sum=0;//编码器积分清零
                    circle_once_time=1;//环岛一次标志置1
                    err_start_point=user_param.err_start;//错误起始点
                    err_end_point=user_param.err_end;//错误终止点
                    if(car_go)
                    {
                        beep_on();//蜂鸣器响
                    }
                }
            }
        }
    }
}


/**
*
* @brief  计算边界连续性
* @param  side 1-左边界 2-右边界
* @param  start_row 起始行
* @param  end_row 结束行
* @retval 连续性长度
**/
uint8 calculate_line_continuity(uint8 side, uint8 start_row, uint8 end_row)
{
    uint8 continuity_count = 0;
    uint8 max_continuity = 0;
    uint8 current_continuity = 0;
    
    for (int i = start_row; i < end_row && i < DEAL_IMAGE_H - 1; i++)
    {
        int diff;
        if (side == 1)  // 左边界
        {
            diff = abs(left_line[i] - left_line[i + 1]);
        }
        else  // 右边界
        {
            diff = abs(right_line[i] - right_line[i + 1]);
        }
        
        if (diff <= 3)  // 连续性阈值
        {
            current_continuity++;
        }
        else
        {
            if (current_continuity > max_continuity)
            {
                max_continuity = current_continuity;
            }
            current_continuity = 0;
        }
    }
    
    if (current_continuity > max_continuity)
    {
        max_continuity = current_continuity;
    }
    
    return max_continuity;
}


/**
*
* @brief  计算边界斜率
* @param  side 1-左边界 2-右边界
* @param  start_row 起始行
* @param  end_row 结束行
* @retval 斜率值
**/
float calculate_line_slope(uint8 side, uint8 start_row, uint8 end_row)
{
    float sum_x = 0, sum_y = 0, sum_xy = 0, sum_x2 = 0;
    uint8 count = 0;
    
    for (int i = start_row; i <= end_row && i < DEAL_IMAGE_H; i++)
    {
        float x = i;  // 行号作为x
        float y;
        
        if (side == 1)  // 左边界
        {
            y = left_line[i];
        }
        else  // 右边界
        {
            y = right_line[i];
        }
        
        sum_x += x;
        sum_y += y;
        sum_xy += x * y;
        sum_x2 += x * x;
        count++;
    }
    
    if (count > 1)
    {
        float slope = (count * sum_xy - sum_x * sum_y) / (count * sum_x2 - sum_x * sum_x);
        return slope;
    }
    
    return 0;
}

/**
*
* @brief  计算边界方差
* @param  side 1-左边界 2-右边界
* @param  start_row 起始行
* @param  end_row 结束行
* @retval 方差值
**/
float calculate_line_variance(uint8 side, uint8 start_row, uint8 end_row)
{
    float sum = 0;
    float mean = 0;
    float variance = 0;
    uint8 count = 0;
    
    // 计算均值
    for (int i = start_row; i <= end_row && i < DEAL_IMAGE_H; i++)
    {
        if (side == 1)  // 左边界
        {
            sum += left_line[i];
        }
        else  // 右边界
        {
            sum += right_line[i];
        }
        count++;
    }
    
    if (count > 0)
    {
        mean = sum / count;
        
        // 计算方差
        for (int i = start_row; i <= end_row && i < DEAL_IMAGE_H; i++)
        {
            float diff;
            if (side == 1)
            {
                diff = left_line[i] - mean;
            }
            else
            {
                diff = right_line[i] - mean;
            }
            variance += diff * diff;
        }
        variance = variance / count;
    }
    
    return variance;
}

/**
*
* @brief  坡道判断函数
**/
void ramp_judge(void)
{
    static uint32 ramp_timer = 0;            // 坡道计时器
    
    uint8 judge_time = 0;                    // 判断计时
    float left_variance = 0;                 // 左边界方差
    float right_variance = 0;                // 右边界方差
    uint8 left_continuity = 0;               // 左边界连续性
    uint8 right_continuity = 0;              // 右边界连续性
    float left_slope = 0;                    // 左边界斜率
    float right_slope = 0;                   // 右边界斜率
    
    
    // 计算边界方差
    left_variance = calculate_line_variance(1, 0, 59);    // 左边界方差
    right_variance = calculate_line_variance(2, 0, 59);   // 右边界方差
    
    // 计算边界连续性
    left_continuity = calculate_line_continuity(1, 0, 119);
    right_continuity = calculate_line_continuity(2, 0, 119);
    
    // 计算边界斜率
    left_slope = calculate_line_slope(1, 0, 35);
    right_slope = calculate_line_slope(2, 0, 35);
    
    // 上坡检测
    if (ramp_protect == 0 && ramp_down_flag == 0 && ramp_flag == 0 && 
        left_continuity >= 55 && right_continuity >= 55 && 
        right_variance >= 0 && right_variance < 120 && 
        left_variance >= 0 && left_variance < 120)
    {
        // 检查赛道宽度特征
        if (real_road_wide[40] >= road_wide[40] && real_road_wide[45] >= road_wide[45] && 
            real_road_wide[50] >= road_wide[50] && real_road_wide[55] >= road_wide[55] && 
            search_stop_line > 80 && 
            !(left_line[50] < (DEAL_IMAGE_W - 3) && right_line[50] > 0))
        {
            judge_time = 1;
        }
        else if (real_road_wide[40] >= road_wide[40]+10 && real_road_wide[45] >= road_wide[45]+10 && 
                 real_road_wide[50] >= road_wide[50]+10 && real_road_wide[55] >= road_wide[55]+10 && 
                 search_stop_line > 80 && 
                 left_line[50] < (DEAL_IMAGE_W - 3) && right_line[50] > 0)
        {
            judge_time = 1;
        }
        
        // 斜率检测
        if (judge_time >= 1 && left_slope > -0.9 && left_slope < 0 && 
            right_slope > 0 && right_slope < 0.9)
        {
            ramp_up_flag = 1;
            ramp_flag = 1;
            ramp_timer = 0;
            
            if(car_go)
            {
                beep_on();  // 蜂鸣器提示
            }
        }
    }
    
    // 坡顶检测
    if (ramp_flag == 1 && ramp_up_flag == 1 && 
        left_continuity > 105 && right_continuity > 105)
    {
        ramp_up_flag = 0;
        ramp_top_flag = 1;
        ramp_timer = 0;
        encoder_sum = 0;  // 清零编码器积分
        if(car_go)
        {
            beep_on();  // 蜂鸣器提示
        }
    }
    
    // 下坡检测
    if (ramp_top_flag == 1 && search_stop_line >= 55 && 
        (left_continuity >= 40 || right_continuity >= 40) && 
        real_road_wide[10] >= 99 && real_road_wide[20] >= 95 && 
        real_road_wide[25] >= 90)
    {
        ramp_flag = 0;
        ramp_up_flag = 0;
        ramp_top_flag = 0;
        ramp_down_flag = 1;
        ramp_timer = 0;
        
        if(car_go)
        {
            beep_on();  // 蜂鸣器提示
        }
    }
    
    // 坡道结束检测
    if (ramp_down_flag == 1)
    {
        ramp_timer++;
        if (ramp_timer > 30)  // 下坡一定时间后结束
        {
            ramp_down_flag = 0;
            ramp_protect = 1;
            ramp_timer = 0;
        }
    }
    
    // 坡道保护计时
    if (ramp_protect == 1)
    {
        ramp_timer++;
        if (ramp_timer > 50)  // 保护时间2秒
        {
            ramp_protect = 0;
            ramp_once_time=1;
            ramp_flag=0;
            ramp_timer = 0;
            if(car_go)
            {
                beep_on();  // 蜂鸣器提示
            }
        }
    }

    
    // 设置坡道标志
    if (ramp_up_flag || ramp_top_flag || ramp_down_flag)
    {
        ramp_flag = 1;
    }
}


/**
*
* @brief  路障识别函数
**/
void obstacle_detect(void)
{
    // 只在直道且无其他元素时检测路障
    if(circle_flag || cross_flag || ramp_flag || zebra_flag)
    {
        return;  // 有其他元素时不检测路障
    }
    
    // 基本条件检查：视野足够远，边界完整
    if(search_stop_line <110 || 
       left_lost_count > 15 || right_lost_count > 15 || 
       left_right_lost_count > 15)
    {
        return;  // 条件不满足，不检测路障
    }
    
    // 检测赛道是否为直道状态
    if(!(boundary_start_left <= DEAL_IMAGE_H - 15 && 
         boundary_start_right <= DEAL_IMAGE_H - 15 && 
         abs(line_err) > 15))
    {
        return;  // 不是直道，不检测路障
    }
    
    obstacle_detect_process();
}

/**
*
* @brief  路障识别处理函数
**/
void obstacle_detect_process(void)
{
    // 遍历图像检测赛道宽度变化
    for(int i = DEAL_IMAGE_H - 3; i > DEAL_IMAGE_H - search_stop_line + 10; i--)
    {
        // 检查当前行是否有有效的标准宽度
        if(road_wide[i] == 0 || i < 20)
        {
            continue;
        }
        
        // 检查边界是否正常
        if(left_line[i] < 10 || right_line[i] > DEAL_IMAGE_W - 10)
        {
            continue;
        }
        
        float current_width = real_road_wide[i];  // 当前赛道宽度
        float standard_width = road_wide[i];  // 标准赛道宽度
        
        // 状态机处理路障识别
        switch(obstacle_state)
        {
            case 0:  // 无路障状态
                obstacle_detect_entry(i, current_width, standard_width);
                break;
                
            case 1:  // 发现路障状态
                obstacle_detect_narrow(i, current_width, standard_width);
                break;
                
            case 2:  // 避障中状态
                obstacle_avoid_process();
                break;
                
            case 3:  // 回归中状态
                obstacle_return_process(i, current_width, standard_width);
                break;
        }
        
        if(obstacle_state > 0)  // 如果检测到路障，跳出循环
        {
            break;
        }
    }
    
    // 设置路障标志
    obstacle_flag = (obstacle_state > 0) ? 1 : 0;
}

/**
*
* @brief  路障入口检测
**/
void obstacle_detect_entry(int row, float current_width, float standard_width)
{
    // 检测赛道宽度正常（作为基准）
    if(current_width >= standard_width * 0.85f)
    {
        obstacle_flag1 = 1;
        obstacle_counter1++;
        
        if(obstacle_counter1 >= 15)  // 连续15行宽度正常
        {
            obstacle_state = 1;  // 进入发现路障状态
            obstacle_counter1 = 0;      
        }
    }
    else
    {
        obstacle_flag1 = 0;
        obstacle_counter1 = 0;
    }
    if(car_go)
    {
        beep_on();  // 蜂鸣器提示
    }
}

/**
*
* @brief  路障变窄检测
**/
void obstacle_detect_narrow(int row, float current_width, float standard_width)
{
    // 检测赛道宽度骤减
    if(current_width <= standard_width * 0.9f)
    {
        obstacle_flag2 = 1;
        track_narrow_count++;
        
        if(track_narrow_count >= 10)  // 连续10行宽度变窄（路障长度）
        {
            obstacle_state = 2;  // 进入避障状态
            obstacle_step = 1;
            track_narrow_count = 0;
            obstacle_encoder_sum = 0;

            continuity_left_change_flag=left_countinuity_detect(DEAL_IMAGE_H-1-20,15);//判断左边连续性
            continuity_right_change_flag=right_countinuity_detect(DEAL_IMAGE_H-1-20,15);//判断右边连续性

            // 判断避障方向（根据边界特征）
            if(continuity_right_change_flag)
            {
                obstacle_direction = 1;  // 左避障
            }
            else if(continuity_left_change_flag)
            {
                obstacle_direction = 2;  // 右避障
            }
            
            if(car_go)
            {
                beep_on();  // 蜂鸣器提示
            }
        }
    }
    else if(current_width >= standard_width * 0.95f && obstacle_step == 0)
    {
        obstacle_flag2 = 0;
        track_narrow_count = 0;
    }
}

/**
*
* @brief  避障处理
**/
void obstacle_avoid_process(void)
{
    // 根据方向进行避障补线
    if(obstacle_direction == 1)  // 左避障
    {
        // 左边界向外扩展避障
        for(int i = DEAL_IMAGE_H - 1; i >= DEAL_IMAGE_H - search_stop_line; i--)
        {
            if(left_line[i] > 20)
            {
                left_line[i] -= 15;  // 向左偏移避障
            }
            else
            {
                left_line[i] = 5;
            }
        }
    }
    else  // 右避障
    {
        // 右边界向外扩展避障
        for(int i = DEAL_IMAGE_H - 1; i >= DEAL_IMAGE_H - search_stop_line; i--)
        {
            if(right_line[i] < DEAL_IMAGE_W - 20)
            {
                right_line[i] += 15;  // 向右偏移避障
            }
            else
            {
                right_line[i] = DEAL_IMAGE_W - 5;
            }
        }
    }
    
    // 检测是否完成避障
    obstacle_encoder_sum += abs(encoder_data_left) + abs(encoder_data_right);
    
    if(obstacle_encoder_sum > 14000 || search_stop_line > 110)  // 编码器积分足够或视野变好
    {
        obstacle_state = 3;  // 进入回归状态
        obstacle_encoder_sum = 0;
        
        if(car_go)
        {
            beep_on();  // 蜂鸣器提示
        }
    }
}

/**
*
* @brief  回归正常赛道
**/
void obstacle_return_process(int row, float current_width, float standard_width)
{
    // 检测赛道宽度恢复正常
    if(current_width >= standard_width * 0.95f)
    {
        obstacle_counter3++;
        obstacle_flag3 = 1;
        
        if(obstacle_counter3 >= 8)  // 连续8行宽度正常
        { 
            reset_obstacle_detect();  // 重置路障状态   
            if(car_go)
            {
                beep_on();  // 蜂鸣器提示
            }
        }
    }
    else
    {
        obstacle_counter3 = 0;
        obstacle_flag3 = 0;
    }
}

/**
*
* @brief  获取路障状态
* @retval 路障状态
**/
uint8 get_obstacle_state(void)
{
    return obstacle_state;
}

/**
*
* @brief  获取避障方向
* @retval 避障方向 1:左避障 2:右避障
**/
uint8 get_obstacle_direction(void)
{
    return obstacle_direction;
}

/**
*
* @brief  重置路障识别
**/
void reset_obstacle_detect(void)
{
    obstacle_state = 0;
    obstacle_step = 0;
    obstacle_counter1 = 0;
    obstacle_counter2 = 0;
    obstacle_counter3 = 0;
    track_narrow_count = 0;
    obstacle_flag1 = 0;
    obstacle_flag2 = 0;
    obstacle_flag3 = 0;
    obstacle_direction = 0;
    obstacle_encoder_sum = 0;
    obstacle_flag = 0;
}




