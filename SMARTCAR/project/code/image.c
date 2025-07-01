#include "image.h"
#include "math.h"
#define DISPLAY_MODE                ( 1 )                                       // 显示模式 0-灰度显示 1-二值化显示
                                                                                // 0-灰度显示   就是正常显示的总钻风图像
                                                                                // 1-二值化显示 根据最后一个二值化阈值显示出对应的二值化图像
#define BINARIZATION_THRESHOLD      ( 64 )                                      // 二值化阈值 默认 64 需要设置 DISPLAY_MODE 为 1 才使用


#define IPS200_TYPE     (IPS200_TYPE_SPI)                                 // 双排排针 并口两寸屏 这里宏定义填写 IPS200_TYPE_PARALLEL8
                                                                                // 单排排针 SPI 两寸屏 这里宏定义填写 IPS200_TYPE_SPI

//需要处理的图像的长宽
#define DEAL_IMAGE_H 120
#define DEAL_IMAGE_W 188

//一些常用变量
uint8 binary_image[MT9V03X_H][MT9V03X_W]; //二值化图像数组
uint8 image_threshold;//图像阈值
uint8 left_line[MT9V03X_H];//左边界数组
uint8 right_line[MT9V03X_H];//右边界数组
uint8 mid_line[MT9V03X_H];//中线数组
uint8 white_count[MT9V03X_W];//每一列的白列长度
uint8 search_stop_line;//搜索截止行
uint8 boundary_start_left;//左边界起点
uint8 boundary_start_right;//右边界起点
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

//误差权重数组(后期使用)
const uint8 weight[DEAL_IMAGE_H]= 
{
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0
};


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
    ips200_show_string(0, 16, "init success.");
    ips200_clear();
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
            memcpy(image_copy, binary_image, MT9V03X_H*MT9V03X_W);
			ips200_show_gray_image(0, 0, (const uint8 *)image_copy, MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, threshold);
			mt9v03x_finish_flag=0;
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
    if(average<50)
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
void image_binary(uint8 *in_image,unsigned char *out_image)
{
    unsigned char x, y;
    unsigned char final_threshold,threshold;
    unsigned int  temp;

    threshold = (otsu_get_threshold(in_image,DEAL_IMAGE_W, DEAL_IMAGE_H));
    for (y = 0; y < DEAL_IMAGE_H; y++)
    {
        temp = DEAL_IMAGE_W*y;
        for (x = 0; x < DEAL_IMAGE_W; x++)
        {
            // if (y <= 15)
            //      final_threshold = threshold + 20;
            //  else if (y >= 40)
            //      final_threshold = threshold + 10;
            //  else if (y >= 50)
            //      final_threshold = threshold - 10;
            //  else
            //      final_threshold = threshold;
            final_threshold = threshold;
            // if(final_threshold<80) final_threshold=80;
            if ( *(in_image+temp+x) > final_threshold)
                *(out_image+temp+x) = 255;  // 白
            else
                *(out_image+temp+x) = 0;  // 黑
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
    boundary_start_left=0;
    boundary_start_right=0;
    for(i=0;i<DEAL_IMAGE_H-1;i++)
    {
        left_line[i]=0;
        right_line[i]=DEAL_IMAGE_W-1;
        left_lost_flag[i]=0;
        right_lost_flag[i]=0;
    }
    for(i=0;i<DEAL_IMAGE_W-1;i++)
    {
        white_count[i]=0;
    }
    //记录每列白点数量
    for(j=start_point;j<end_point;j++)
    {
        for(i=DEAL_IMAGE_H-1;i>=0;i--)
        {
            if(image[i][j] == 1)break;//黑点跳出
            else white_count[j]++;//白点计数
        }
    }
    //寻找左最长白列
    for(i=start_point;i<end_point;i+=2)
    {
        if( longest_white_left[0] < white_count[i])//找最长的那一列，寻到右边界
        {
            longest_white_left[0] = white_count[i];
            longest_white_left[1] = i;
        }
    }
    //寻找右最长白列
    for(i=end_point;i>=longest_white_left[1];i-=2)//从右往左，找到右最长白列，寻到左边最长白列位置
    {
        if( longest_white_right[0] < white_count[i])//找最长的那一列
        {
            longest_white_right[0] = white_count[i];
            longest_white_right[1] = i;
        }
    }
    
    search_stop_line = (longest_white_left[0]>longest_white_right[0])?longest_white_left[0]:longest_white_right[0];//非常重要，搜索截止行存储

    //巡线
    for(i = DEAL_IMAGE_H-1;i>=DEAL_IMAGE_H-search_stop_line;i--)
    {
        for(j = longest_white_right[1]; j<=DEAL_IMAGE_W-1-2;j++)//从右最长白列找右边
        {
            if(image[i][j] == 1 && image[i][j+1] == 0 && image[i][j+2] == 0)//白黑黑，找到右边界
            {
                right_border = j;
                right_lost_flag[i]=0;
                break;
            }
            else if(j>=DEAL_IMAGE_W-1-2)//右边界丢失
            {
                right_border = j;
                right_lost_flag[i]=1;//右边界丢线记录
                break;
            }
        }
        for(j = longest_white_left[1]; j>=2;j--)//从左最长白列找左边
        {
            if(image[i][j] == 1 && image[i][j-1] == 0 && image[i][j-2] == 0)//白黑黑，找到右边界
            {
                left_border = j;
                left_lost_flag[i]=0;
                break;
            }
            else if(j<=2)//左边界丢失
            {
                left_border = j;
                left_lost_flag[i]=1;//左边界丢线记录
                break;
            }
        }
        left_line[i]=left_border;//存储左边线
        right_line[i]=right_border;//存储右边线
        
    }
    //记录丢边情况
    for(i=DEAL_IMAGE_H-1;i>=DEAL_IMAGE_H-search_stop_line;i--)
    {
        if(left_lost_flag[i]==1&&right_lost_flag[i]==0)left_lost_count++;//左丢
        if(left_lost_flag[i]==0&&right_lost_flag[i]==1)right_lost_count++;//右丢
        if(left_lost_flag[i]==1&&right_lost_flag[i]==1)left_right_lost_count++;//丢双边
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
        if((i+2-DEAL_IMAGE_H)*sizeof(uint8)>sizeof(weight))
        {
            break;//越界跳出
        }
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
* @brief  找下拐点
* @param  start_point 搜索起点 
* @param  end_point    搜索终点
**/

void find_down_point(uint8 start_point,uint8 end_point)
{
    //参数清零
    left_down_point=0;
    right_down_point=0;
    if(start_point>DEAL_IMAGE_H-5)
    {
        start_point=DEAL_IMAGE_H-5;
    }
    if(end_point<DEAL_IMAGE_H-search_stop_line)
    {
        end_point=DEAL_IMAGE_H-search_stop_line;
    }
    for(int i=start_point;i>=end_point;i--)
    {
        //点i下面2个连续相差不大并且点i与上面边3个点分别相差很大，认为有下左拐点
        if(left_down_point==0&&
            abs(left_line[i]-left_line[i+1])<=5&&
            abs(left_line[i+1]-left_line[i+2])<=5&&
            abs(left_line[i+2]-left_line[i+3])<=5&&
            (left_line[i]-left_line[i-2])>=5&&
            (left_line[i]-left_line[i-3])>=10&&
            (left_line[i]-left_line[i-4])>=10)
            {
                left_down_point=i;
            }
        if(right_down_point==0&&
            abs(right_line[i]-right_line[i+1])<=5&&
            abs(right_line[i+1]-right_line[i+2])<=5&&
            abs(right_line[i+2]-right_line[i+3])<=5&&
            (right_line[i]-right_line[i-2])<=-5&&
            (right_line[i]-right_line[i-3])<=-10&&
            (right_line[i]-right_line[i-4])<=-10)
            {
                right_down_point=i;
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
    if(start_point<DEAL_IMAGE_H-search_stop_line+5)
    {
        start_point=DEAL_IMAGE_H-search_stop_line+5;
    }
    if(end_point>DEAL_IMAGE_H-5)
    {
        end_point=DEAL_IMAGE_H-5;
    }
    for(int i=start_point;i<end_point;i++)
    {
        if(left_up_point==0&&
            abs(left_line[i]-left_line[i-1])<5&&
            abs(left_line[i-1]-left_line[i-2])<5&&
            abs(left_line[i-2]-left_line[i-3])<5&&
            left_line[i+2]-left_line[i]<-5&&
            left_line[i+3]-left_line[i]<-10&&
            left_line[i+4]-left_line[i]<-10)
            {
                left_up_point=i;
            }
        if(right_up_point==0&&
            abs(right_line[i]-right_line[i-1])<5&&
            abs(right_line[i-1]-right_line[i-2])<5&&
            abs(right_line[i-2]-right_line[i-3])<5&&
            right_line[i+2]-right_line[i]>5&&
            right_line[i+3]-right_line[i]>10&&
            right_line[i+4]-right_line[i]>10)
            {
                right_up_point=i;
            }
        if(left_up_point!=0&&right_down_point!=0)
        {
            break;
        }
    }
}
