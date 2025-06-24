/*
 * image.c
 * author: xiayu
 * qq: 3401518894
 * detail: 快速八邻域
 * reference open source projects:
 * https://gitee.com/daimxa/DMX-TC264-Library?_from=gitee_search
 * https://pan.baidu.com/share/init?surl=UBrJpS4MbRPzk7uF6aIhoA
 * other:This open source uses the GPL3 protocol. Please comply with the relevant provisions of the open source protocol
 */
#include "common.h"
unsigned char Gray_Image[DEAL_IMAGE_H][DEAL_IMAGE_W];      // 压缩后的灰度图像
unsigned char Binarization_Image[DEAL_IMAGE_H][DEAL_IMAGE_W];    // 二值化图像

unsigned char Left_Line[DEAL_IMAGE_H];//左线数组
unsigned char Right_Line[DEAL_IMAGE_H];//右线数组
unsigned char Center_Line[DEAL_IMAGE_H];//中线数组

unsigned char Left_Line_Lost_Flag[DEAL_IMAGE_H];//丢线标志位
unsigned char Right_Line_Lost_Flag[DEAL_IMAGE_H];
unsigned char Left_Line_Lost;
unsigned char Right_Line_Lost;
unsigned char Line_Lost;
unsigned char Fix_Left_Line_Start;//补左线开始行
unsigned char Fix_Left_Line_End;
unsigned char Fix_Right_Line_Start;//补右线开始行
unsigned char Fix_Right_Line_End;
unsigned char Flag;//标志位,用来判断当前所处状态

unsigned char Trackwidth[DEAL_IMAGE_H];//赛道宽度
float Curve;//边线弧度
                                                            //下面为点坐标
unsigned char *Row_Pointers;                                // 行指针
unsigned char Line_Mid = DEAL_IMAGE_W/2 - 1;                // 行中点
unsigned char Line_Start=2;                                 //横向扫线开始点
unsigned char Line_End=DEAL_IMAGE_W-2;                      //横向扫线结束点
unsigned char Find_Line_Start=DEAL_IMAGE_H-2;               //竖向扫线开始点
unsigned char Find_Line_End=20;                              //竖向扫线结束点
unsigned char start_point_l[2];                             //左边起点的x，y值
unsigned char start_point_r[2];                             //右边起点的x，y值
#define All_Point (DEAL_IMAGE_H*2)                          //定义找点的数组成员个数
unsigned char Max=0,Max2=0;                                 //特征点,用于辅助找圆环
static int8 seeds_l[8][2] = { {0,  1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,  0},{1, 1}, };
static int8 seeds_r[8][2] = { {0,  1},{1,1},{1,0}, {1,-1},{0,-1},{-1,-1}, {-1,  0},{-1, 1}, };
 //存放点的x，y坐标
unsigned char points_l[All_Point][2];//左线
unsigned char points_r[All_Point][2];//右线
unsigned char dir_r[All_Point];//用来存储右边生长方向
unsigned char dir_l[All_Point];//用来存储左边生长方向
unsigned char data_stastics_l;//统计左边找到点的个数
unsigned char data_stastics_r;//统计右边找到点的个数
unsigned char hightest = 0;//最高点

//左边变量
unsigned char search_filds_l[8][2];//中心点旁八个点对应像素
unsigned char index_l;//找到的点索引
unsigned char temp_l[8][2];

unsigned char center_point_l[2];//八临域中点

//右边变量
unsigned char search_filds_r[8][2];
unsigned char center_point_r[2];//中心坐标点
unsigned char index_r;//索引下标
unsigned char temp_r[8][2];
void Delay(uint n)
{
uint x;
while (n--)
{
x = 5000;
while (x--);
}
}
/**
*
* @brief    补线函数
* @return   void
*
**/
void draw_line (uint16 x_start, uint16 y_start, uint16 x_end, uint16 y_end,int state)
{
    if(x_start>DEAL_IMAGE_W)x_start=DEAL_IMAGE_W;
    if(x_start<0)x_start=0;
    if(x_end>DEAL_IMAGE_W)x_end=DEAL_IMAGE_W;
    if(x_end<0)x_end=0;
    if(y_start>DEAL_IMAGE_H)y_start=DEAL_IMAGE_H;
    if(y_start<0)y_start=0;
    if(y_end>DEAL_IMAGE_H)y_end=DEAL_IMAGE_H;
    if(y_end<0)y_end=0;
    int16 x_dir = (x_start < x_end ? 1 : -1);
    int16 y_dir = (y_start < y_end ? 1 : -1);
    float temp_rate = 0;
    float temp_b = 0;

    do
    {
        if(x_start != x_end)
        {
            temp_rate = (float)(y_start - y_end) / (float)(x_start - x_end);
            temp_b = (float)y_start - (float)x_start * temp_rate;
        }
        else
        {
            while(y_start != y_end)// 在图像中写入边线数据,左边线为2,中线为3,右边线为4
            {
                Binarization_Image[y_start][x_start]=state;
                switch(state){
                case 2: Left_Line[y_start]=x_start; break;
                case 3: Center_Line[y_start]=x_start; break;
                case 4: Right_Line[y_start]=x_start; break;
                }
                y_start += y_dir;
            }
            break;
        }
        if(func_abs(y_start - y_end) > func_abs(x_start - x_end))
        {
            while(y_start != y_end)
            {
                Binarization_Image[y_start][x_start]=state;
                switch(state){
                case 2: Left_Line[y_start]=x_start; break;
                case 3: Center_Line[y_start]=x_start; break;
                case 4: Right_Line[y_start]=x_start; break;
                }
                y_start += y_dir;
                x_start = (int16)(((float)y_start - temp_b) / temp_rate);
            }
        }
        else
        {
            while(x_start != x_end)
            {
                Binarization_Image[y_start][x_start]=state;
                switch(state){
                case 2: Left_Line[y_start]=x_start; break;
                case 3: Center_Line[y_start]=x_start; break;
                case 4: Right_Line[y_start]=x_start; break;
                }
                x_start += x_dir;
                y_start = (int16)((float)x_start * temp_rate + temp_b);
            }
        }
    }while(0);
}
/**
*
* @brief    边线曲率
* @param    *in_line        需要计算曲率的边线
* @param    len             边线的长度
* @param    start           开始计算的长度
* @param    end             结束计算的长度
* @return   void
* @notes
* Example:  Line_Curvature(Deal_Image_Mid_Line[0],DEAL_IMAGE_H,End_Line,DEAL_IMAGE_H - 5);
*
**/
float Line_Curvature(unsigned char in_line[DEAL_IMAGE_H],unsigned char len,unsigned char start,unsigned char end)
{
    int i;
    float sumX = 0, sumY = 0, avrX = 0, avrY = 0 ;
    float B = 0, num = 0, B_up1 = 0, B_up2 = 0, B_up = 0, B_down = 0;
    if(end > len-5)
        end = len - 5;
    for(i = start; i <= end; i++)
    {
        num++;
        sumX += i;
        sumY += (in_line[i] + i);
    }
    avrX = sumX / num;
    avrY = sumY / num;
    for(i = start; i <= end; i++)
    {
        B_up1 = ((in_line[i] + i) - avrY);
        B_up2 = i - avrX;
        B_up += (int)(10 * (B_up1 * B_up2));
        B_up = B_up / 100 * 100;
        B_down += (int)(10 * ((i - avrX) * (i - avrX)));
    }
    if(B_down == 0)
        B = 0;
    else
        B = B_up * 16 / B_down;
    return B;
}

/**
*
* @brief    图像压缩
* @param    *in_image       原始图像
* @param    *out_image      压缩后的图像
* @return   void
* @notes
* Example:  Image_Compress(MT9V034_Image_Data[0],Gray_Image[0]);
*
**/
void Image_Compress(unsigned char *in_image,unsigned char *out_image)
{
    unsigned int CompressY, CompressX;
    unsigned int OriginalY, OriginalX;
    unsigned int CompressTemp,OriginalTemp;
    float Hight_Ratio = (float)((MT9V03X_H * 1.0 / DEAL_IMAGE_H));
    float Width_Ratio = (float)(MT9V03X_W* 1.0 / DEAL_IMAGE_W);

    for (CompressY = 0; CompressY < DEAL_IMAGE_H; CompressY++)
    {
        OriginalY = (unsigned int)(CompressY * Hight_Ratio);
        CompressTemp = DEAL_IMAGE_W*CompressY;
        OriginalTemp = MT9V03X_W*OriginalY;

        for (CompressX = 0; CompressX < DEAL_IMAGE_W; CompressX++)
        {
            OriginalX = (unsigned int)(CompressX * Width_Ratio);
            *(out_image+CompressTemp+CompressX) = *(in_image+OriginalTemp+OriginalX);
        }
    }
}
/**
*
* @brief  动态阈值
* @notes
* Example:
*
**/
uint8 otsuThreshold(uint8 *image, uint16 col, uint16 row)
{
#define GrayScale 256
    uint16 Image_Width  = col;
    uint16 Image_Height = row;
    int X; uint16 Y;
    uint8* data = image;
    int HistGram[GrayScale] = {0};

    uint32 Amount = 0;
    uint32 PixelBack = 0;
    uint32 PixelIntegralBack = 0;
    uint32 PixelIntegral = 0;
    int32 PixelIntegralFore = 0;
    int32 PixelFore = 0;
    double OmegaBack=0, OmegaFore=0, MicroBack=0, MicroFore=0, SigmaB=0, Sigma=0; // 类间方差;
    uint8 MinValue=0, MaxValue=0;
    uint8 Threshold = 0;


    for (Y = 0; Y <Image_Height; Y++) //Y<Image_Height改为Y =Image_Height；以便进行 行二值化
    {
        //Y=Image_Height;
        for (X = 0; X < Image_Width; X++)
        {
        HistGram[(int)data[Y*Image_Width + X]]++; //统计每个灰度值的个数信息
        }
    }
    for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++) ;        //获取最小灰度的值
    for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--) ; //获取最大灰度的值

    if (MaxValue == MinValue)
    {
        return MaxValue;          // 图像中只有一个颜色
    }
    if (MinValue + 1 == MaxValue)
    {
        return MinValue;      // 图像中只有二个颜色
    }

    for (Y = MinValue; Y <= MaxValue; Y++)
    {
        Amount += HistGram[Y];        //  像素总数
    }

    PixelIntegral = 0;
    for (Y = MinValue; Y <= MaxValue; Y++)
    {
        PixelIntegral += HistGram[Y] * Y;//灰度值总数
    }
    SigmaB = -1;
    for (Y = MinValue; Y < MaxValue; Y++)
    {
          PixelBack = PixelBack + HistGram[Y];    //前景像素点数
          PixelFore = Amount - PixelBack;         //背景像素点数
          OmegaBack = (double)PixelBack / Amount;//前景像素百分比
          OmegaFore = (double)PixelFore / Amount;//背景像素百分比
          PixelIntegralBack += HistGram[Y] * Y;  //前景灰度值
          PixelIntegralFore = PixelIntegral - PixelIntegralBack;//背景灰度值
          MicroBack = (double)PixelIntegralBack / PixelBack;//前景灰度百分比
          MicroFore = (double)PixelIntegralFore / PixelFore;//背景灰度百分比
          Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//g
          if (Sigma > SigmaB)//遍历最大的类间方差g
          {
              SigmaB = Sigma;
              Threshold = (uint8)Y;
          }
    }
   return Threshold;
}
/**
*
* @brief    图像二值化
* @param    *in_image       压缩后的图像
* @param    *out_image      二值化后的图像
* @return   void
* @notes
* Example:  Image_Binary(Gray_Image[0],Binarization_Image[0]);
*
**/
void Image_Binary(unsigned char *in_image,unsigned char *out_image)
{
    unsigned char x, y;
    unsigned char final_threshold,threshold;
    unsigned int  temp;

//    threshold = Image_Threshold(in_image, DEAL_IMAGE_W, DEAL_IMAGE_H, image_max_threshold) + 20;
    threshold = (otsuThreshold(in_image,DEAL_IMAGE_W, DEAL_IMAGE_H));
    for (y = 0; y < DEAL_IMAGE_H; y++)
    {
        temp = DEAL_IMAGE_W*y;
        for (x = 0; x < DEAL_IMAGE_W; x++)
        {
            if (y <= 15)
                 final_threshold = threshold + 20;
             else if (y >= 40)
                 final_threshold = threshold + 10;
             else if (y >= 50)
                 final_threshold = threshold - 10;
             else
                 final_threshold = threshold;

            if(final_threshold<80) final_threshold=80;
            if ( *(in_image+temp+x) > final_threshold)
                *(out_image+temp+x) = 1;  // 白
            else
                *(out_image+temp+x) = 0;  // 黑
        }
    }
}
/**
*
* @brief  圆环补线
* @notes
* Example:  Cricle(Flag);
*
**/
void Cricle(unsigned char Flag){
    switch(Flag){
    case 0:draw_line(Left_Line[Fix_Left_Line_Start+2],Fix_Left_Line_Start+2,Left_Line[Find_Line_End],Find_Line_End, 2);break;
    case 1:draw_line(Left_Line[Fix_Left_Line_End], Fix_Left_Line_End, Right_Line[Fix_Left_Line_Start], Fix_Left_Line_Start,4); break;
    case 2:break;
    case 3:{
//        if(Fix_Left_Line_End<35)Fix_Left_Line_End=36;
        draw_line(Left_Line[Fix_Left_Line_End], Fix_Left_Line_End, Right_Line[Fix_Right_Line_Start],DEAL_IMAGE_H,4); break;
    }
    case 4:break;
    }
}
/**
*
* @brief  八邻域扫线
* @notes
* Example:  Search_Line(Binarization_Image[0]);
*
**/
void Search_Line(unsigned char(*image)[DEAL_IMAGE_W]){

     unsigned char x,y,i,j;
     //初始化数据
     for(y = 0,data_stastics_l=0,data_stastics_r=0,Left_Line_Lost=0,Right_Line_Lost=0,Line_Lost=0,Fix_Left_Line_Start=0,Fix_Left_Line_End=0,Fix_Right_Line_Start=0,Fix_Right_Line_End=0; y < Line_End; y++)
     {
         Left_Line[y] = Line_Start;           // 初始化左边线
         Right_Line[y] = DEAL_IMAGE_W - 1;    // 初始化右边线
         Center_Line[y] = DEAL_IMAGE_W / 2;   // 初始化中线
         Trackwidth[y] = DEAL_IMAGE_W - 1;    // 初始化宽度
     }
     //初始化所有需要的数组
     memset(Left_Line_Lost_Flag, 0, sizeof(Left_Line_Lost_Flag));
     memset(Right_Line_Lost_Flag, 0, sizeof(Right_Line_Lost_Flag));

     memset(search_filds_l, 0, sizeof(search_filds_l));
     memset(search_filds_r, 0, sizeof(search_filds_r));

     memset(points_l, 0, sizeof(points_l));
     memset(points_r, 0, sizeof(points_r));

     memset(dir_l, 0, sizeof(dir_l));
     memset(dir_r, 0, sizeof(dir_r));

     memset(temp_l, 0, sizeof(temp_l));
     memset(temp_r, 0, sizeof(temp_r));

     memset(center_point_l, 0, sizeof(center_point_l));
     memset(center_point_r, 0, sizeof(center_point_r));

     memset(center_point_l, 0, sizeof(center_point_l));
     memset(center_point_r, 0, sizeof(center_point_r));

     memset(start_point_l, 0, sizeof(center_point_l));
     memset(start_point_r, 0, sizeof(center_point_r));
     //行地址
     Row_Pointers = (*image + DEAL_IMAGE_W * Find_Line_Start);
     //底部行的中点为白点
     if((*(Row_Pointers + Line_Mid)) == 1){
     // 中点向左找到黑点或者找到最左边则为底部行左边线
       for(x = Line_Mid; x > 0; x--)
       {
           if(*(Row_Pointers + x - 1) == 0 && *(Row_Pointers + x) == 0 )
           {
               start_point_l[0] = x;//x
               start_point_l[1] = Find_Line_Start;//y
               break;
           }
       }
       // 中点向右找到黑点或者找到最右边则为底部行右边线
       for(x = Line_Mid; x < DEAL_IMAGE_W; x++)
       {
           if(*(Row_Pointers + x + 1) == 0 && *(Row_Pointers + x) == 0 )
           {
               start_point_r[0] = x;//x
               start_point_r[1] = Find_Line_Start;//y
               break;
           }
       }
    }else{//如果中点是黑点
        start_point_l[1] = Find_Line_Start;
        start_point_r[1] = Find_Line_Start;
      for(x = 0;x < Line_Mid; x++)
      {
          // 左边找到白点退出此for循环,此时x的值为该白点的x坐标
          if(*(Row_Pointers + Line_Mid - x) == 1 )
              break;
          // 右边找到白点退出此for循环,此时x的值为该白点的x坐标
          if(*(Row_Pointers + Line_Mid + x) == 1 )
              break;
      }
      // 判断上面for循环退出时的x坐标的点在中点左边还是右边
      // 中点减去x坐标为白则在左边
      if(*(Row_Pointers + Line_Mid - x) == 1 )
      {
          // 该点可看作底部行的右边界
          start_point_r[0] = Line_Mid - x + 1;
          for(x = start_point_r[0]; x > 0; x--)
          {
              if(*(Row_Pointers + x - 1) == 0 && *(Row_Pointers + x) == 0 )
              {
                  start_point_l[0] = x;
                  break;
              }
          }
      }
      // 中点加上x坐标为白则在右边
      else if(*(Row_Pointers + Line_Mid + x) ==1)
      {
          // 该点可看作底部行的左边界
          start_point_l[0] = Line_Mid + x - 1;
          for(x = start_point_l[0]; x < DEAL_IMAGE_W - 1; x++)
          {
              if(*(Row_Pointers + x + 1) == 0 && *(Row_Pointers + x) == 0 )
              {
                  start_point_r[0] = x;
                  break;
              }
          }
      }
    }

     //第一次更新坐标点
     center_point_l[0] = start_point_l[0];//x
     center_point_l[1] = start_point_l[1];//y
     center_point_r[0] = start_point_r[0];//x
     center_point_r[1] = start_point_r[1];//y
     index_l=0;
     index_r=0;
     unsigned char breakflag=DEAL_IMAGE_H*2;
     unsigned char num;
     //开启邻域循环
     while (breakflag--)
     {
         //左边 获得8个点坐标
         for (i = 0; i < 8; i++)
         {
             search_filds_l[i][0] = center_point_l[0] + seeds_l[i][0];//x
             search_filds_l[i][1] = center_point_l[1] + seeds_l[i][1];//y
         }
         //中心坐标点填充到已经找到的点内
         points_l[data_stastics_l][0] = center_point_l[0];//x
         points_l[data_stastics_l][1] = center_point_l[1];//y
         data_stastics_l++;//迭代

         //右边
         for (i = 0; i < 8; i++)
         {
             search_filds_r[i][0] = center_point_r[0] + seeds_r[i][0];//x
             search_filds_r[i][1] = center_point_r[1] + seeds_r[i][1];//y
         }
         points_r[data_stastics_r][0] = center_point_r[0];
         points_r[data_stastics_r][1] = center_point_r[1];

         index_l = 0;//清零
         memset(temp_l, 0, sizeof(temp_l));
         memset(temp_r, 0, sizeof(temp_r));
         //左边判断
         for (i = 0; i < 8; i++)
         {
             if (image[search_filds_l[i][1]][search_filds_l[i][0]] == 0
                 && image[search_filds_l[(i + 1) & 7][1]][search_filds_l[(i + 1) & 7][0]] == 1)
             {
                 temp_l[index_l][0] = search_filds_l[(i)][0];
                 temp_l[index_l][1] = search_filds_l[(i)][1];
                 index_l++;
                 dir_l[data_stastics_l - 1] = (i);//记录生长方向
             }
             if (index_l)
             {
                 //更新坐标点
                 center_point_l[0] = temp_l[0][0];//x
                 center_point_l[1] = temp_l[0][1];//y
                 for (j = 0; j < index_l; j++)
                 {
                     if (center_point_l[1] > temp_l[j][1])
                     {
                         center_point_l[0] = temp_l[j][0];//x
                         center_point_l[1] = temp_l[j][1];//y
                     }
                 }
             }
         }
         if ((points_r[data_stastics_r][0]== points_r[data_stastics_r-1][0]&& points_r[data_stastics_r][0] == points_r[data_stastics_r - 2][0]
             && points_r[data_stastics_r][1] == points_r[data_stastics_r - 1][1] && points_r[data_stastics_r][1] == points_r[data_stastics_r - 2][1])
             ||(points_l[data_stastics_l-1][0] == points_l[data_stastics_l - 2][0] && points_l[data_stastics_l-1][0] == points_l[data_stastics_l - 3][0]
                 && points_l[data_stastics_l-1][1] == points_l[data_stastics_l - 2][1] && points_l[data_stastics_l-1][1] == points_l[data_stastics_l - 3][1]))
         {
             break; //三次进入同一个点，退出
         }
         if (abs(points_r[data_stastics_r][0] - points_l[data_stastics_l - 1][0]) < 2
             && abs(points_r[data_stastics_r][1] - points_l[data_stastics_l - 1][1] < 2)
             )
         {
             hightest = (points_r[data_stastics_r][1] + points_l[data_stastics_l - 1][1]) >> 1;//取出最高点
             //在hightest处退出
             break;
         }
         if ((points_r[data_stastics_r][1] < points_l[data_stastics_l - 1][1]))
         {
             continue;//如果左边比右边高了，左边等待右边
         }
         if (dir_l[data_stastics_l - 1] == 7
             && (points_r[data_stastics_r][1] > points_l[data_stastics_l - 1][1]))//左边比右边高且已经向下生长了
         {
             //左边开始向下了，等待右边，等待中
             center_point_l[0] = points_l[data_stastics_l - 1][0];//x
             center_point_l[1] = points_l[data_stastics_l - 1][1];//y
             data_stastics_l--;
         }
         data_stastics_r++;//索引加一
         index_r = 0;//先清零，后使用
//         for (i = 0; i < 8; i++)
//         {
//             temp_r[i][0] = 0;//先清零，后使用
//             temp_r[i][1] = 0;//先清零，后使用
//         }

         //右边判断
         for (i = 0; i < 8; i++)
         {
             if (image[search_filds_r[i][1]][search_filds_r[i][0]] == 0
                 && image[search_filds_r[(i + 1) & 7][1]][search_filds_r[(i + 1) & 7][0]] == 1)
             {
                 temp_r[index_r][0] = search_filds_r[(i)][0];
                 temp_r[index_r][1] = search_filds_r[(i)][1];
                 index_r++;//索引加一
                 dir_r[data_stastics_r - 1] = (i);//记录生长方向
                 //printf("dir[%d]:%d\n", data_stastics_r - 1, dir_r[data_stastics_r - 1]);
             }
             if (index_r)
             {
                 //更新坐标点
                 center_point_r[0] = temp_r[0][0];//x
                 center_point_r[1] = temp_r[0][1];//y
                 for (j = 0; j < index_r; j++)
                 {
                     if (center_point_r[1] > temp_r[j][1])
                     {
                         center_point_r[0] = temp_r[j][0];//x
                         center_point_r[1] = temp_r[j][1];//y
                     }
                 }
             }
         }
     }

     num=0;
     unsigned char h,Left_End,Right_End;
     float K,slope;
     //左边
     for (j = 0,h=Find_Line_Start; j < data_stastics_l; j++)
     {
       if (points_l[j][1] == h)
        {
            Left_End=h;
            Left_Line[h] = points_l[j][0]+1;
        }
       else continue; //每行只取一个点，没到下一行就不记录
        h--;
       if (h == 0) break;
     }
     //右边
     for (j = 0,h=Find_Line_Start; j < data_stastics_r; j++)
     {
         if (points_r[j][1] == h)
         {
             Right_End=h;
             Right_Line[h] = points_r[j][0] - 1;
         }
         else continue;//每行只取一个点，没到下一行就不记录
         h--;
         if (h == 0)break;//到最后一行退出
     }
     //重新计算扫线结束行
     Find_Line_End=(Left_End+Right_End)/2;
     //补线开始
     for(h=Find_Line_Start;h>Find_Line_End;h--){
       num++;
         //记录赛道宽度
         Trackwidth[h]=Right_Line[h]-Left_Line[h];
//       Binarization_Image[h][start_point_l[0]+num]=2;
         //拟合赛道不丢线情况算距离偏差
         if(abs(Left_Line[h]-(start_point_l[0]+num))>5&&Left_Line[h]-Line_Start<=3){
             Left_Line_Lost_Flag[h]=1;
             Left_Line_Lost++;
         if(Fix_Left_Line_Start==0) Fix_Left_Line_Start=h;
         }else{
          if(Fix_Left_Line_Start!=0&&Fix_Left_Line_End==0)
          Fix_Left_Line_End=h;
         }
         //确认是否是为弯道丢线
         if(Fix_Left_Line_Start==Find_Line_Start&&Left_Line[h]-Line_Start<=1)
             Left_Line_Lost_Flag[h]=2;

         if(abs(Right_Line[h]-(start_point_r[0]-num))>5&&Line_End-Right_Line[h]<=3){
             Right_Line_Lost_Flag[h]=1;
             Right_Line_Lost++;
          if(Fix_Right_Line_Start==0) Fix_Right_Line_Start=h;
         }else{
          if(Fix_Right_Line_Start!=0&&Fix_Right_Line_End==0)
          Fix_Right_Line_End=h;
         }
         //确认是否是为弯道丢线
         if(Fix_Right_Line_Start==Find_Line_Start&&Line_End-Right_Line[h]<=1)
             Right_Line_Lost_Flag[h]=2;
         if(Left_Line_Lost_Flag[h]==1&&Right_Line_Lost_Flag[h]==1)
             Line_Lost++;
     }

     //边线缺失,从底部画延长线补线
     if(Line_Lost>=10){
       if(Fix_Left_Line_End>=Find_Line_End){
        K = ((float)(Left_Line[Fix_Left_Line_Start] -start_point_l[0]))*1.0 / ((float)(Fix_Left_Line_Start - start_point_l[1]));
//        ips114_show_int(200,80,(int)K,4);
        if(K<0.8)slope=K;
        for (y =Find_Line_Start; y>Fix_Left_Line_End; y--) {
            int key=start_point_l[0]+(start_point_l[1]-y)*slope;
            image[y][key] = 2;
          }
         }
//     draw_line(start_point_l[0],Find_Line_Start,Left_Line[Fix_Left_Line_End-1],Fix_Left_Line_End-1, 2);
//     draw_line(start_point_r[0],Find_Line_Start,Right_Line[Fix_Right_Line_End-1],Fix_Right_Line_End-1,4);
     }else{
     if(Flag==4){
     if(Fix_Left_Line_Start!=0&&Fix_Left_Line_End!=0)
     draw_line(Left_Line[Fix_Left_Line_Start+2],Fix_Left_Line_Start+2,Left_Line[Fix_Left_Line_End-4],Fix_Left_Line_End-4, 2);
     if(Fix_Right_Line_Start!=0&&Fix_Right_Line_End!=0)
     draw_line(Right_Line[Fix_Right_Line_Start+2],Fix_Right_Line_Start+2,Right_Line[Fix_Right_Line_End-4],Fix_Right_Line_End-4,4);
    }
   }
     //避障判断
//     num=0;
//     if(data_stastics_l<35&&data_stastics_r<35&&Car_State!=3){
//         Car_State=2;
//     }
     num=0;
     //保护程序
     if((*(Row_Pointers + Line_Mid)) == 0&&start_point_l[0]==0&&start_point_r[0]==0){
     for(y=0;y<DEAL_IMAGE_H/2;y+=5){
       for(x=0;x<DEAL_IMAGE_W;x+=5){
          if(*(*image + x * y)==0){
             num++;
         }
       }
      }
    }
//     if (num>90) {
//      Car_State=1;
//      breakflag=0;
//     }

   //圆环开始
   unsigned char X1,X2,Y1,Y2;

   X1=Left_Line[Fix_Left_Line_Start];
   X2=Right_Line[Fix_Left_Line_End];
   Y1=Fix_Left_Line_Start;
   Y2=Fix_Left_Line_End;

   ips114_show_int(200,0,X1,4);
   ips114_show_int(200,20,Y1,4);
   ips114_show_int(200,40,X2,4);
   ips114_show_int(200,60,Max,4);
//   ips114_show_int(200,20,Right_Line[Fix_Left_Line_Start],4);
//   ips114_show_int(200,40,Fix_Left_Line_Start,4);
//   ips114_show_int(200,60,Left_Line_Lost,4);
   ips114_show_int(200,80,Left_Line_Lost,4);
   ips114_show_int(200,100,Flag,4);

   if(X2>Max) Max=X2;
   if(Right_Line_Lost>Max2) Max2=Right_Line_Lost;
//   if(X1!=0&&Left_Line_Lost>0&&Flag!=2&&Flag!=3&&Flag!=4){//&&Right_Line_Lost<=1
//           Flag=1;
//    }
   if(Max>50&&X2>40&&X2<50&&Flag!=2&&Flag!=3&&Flag!=4){//&&Right_Line_Lost<=1
        Flag=1;
    }
   if(Max2>=25&&Right_Line_Lost<=1&&Flag==1){
       Flag=2;
   }
   if(Line_Lost>=1&&Flag==2){
       Flag=3;
   }
   if(X2>30&&X2<45&&Left_Line_Lost>0&&Flag==3){//&&Right_Line_Lost<=1
       Flag=4;
   }
   Cricle(Flag);

}
/**
*
* @brief
* @notes
* Example: Image_Filter(Binarization_Image[0]);
*
**/
void Show_Line(unsigned char *image){
    unsigned int i;
    for (i = 0; i < data_stastics_l; i++)
    {
        Binarization_Image[points_l[i][1]+2][points_l[i][0]]=2;
    }
    for (i = 0; i < data_stastics_r; i++)
    {
        Binarization_Image[points_r[i][1]][points_r[i][0]-2]=4;
    }

    for (i = hightest; i < DEAL_IMAGE_H-1; i++)
    {
        Center_Line[i] = (Left_Line[i] + Right_Line[i]) >> 1;//求中线
        Binarization_Image[i][Center_Line[i]]=3;
    }
}


/**
*
* @brief  图像滤波
* @notes
* Example: Image_Filter(Binarization_Image[0]);
*
**/
void Image_Filter(unsigned char *in_image)
{
    unsigned char x, y;
    for (y = 2; y < DEAL_IMAGE_H-2; y++)
    {
        for (x = 2; x < DEAL_IMAGE_W-2; x++)
        {
            if ( (*(in_image+DEAL_IMAGE_W*y+x)==0) && ((*(in_image+DEAL_IMAGE_W*(y-1)+x) + *(in_image+DEAL_IMAGE_W*(y+1)+x) + *(in_image+DEAL_IMAGE_W*y+x+1) + *(in_image+DEAL_IMAGE_W*y+x-1)) >= 3) )
                *(in_image+DEAL_IMAGE_W*y+x) = 1;
            else if( (*(in_image+DEAL_IMAGE_W*y+x)==1) && ((*(in_image+DEAL_IMAGE_W*(y-1)+x) + *(in_image+DEAL_IMAGE_W*(y+1)+x) + *(in_image+DEAL_IMAGE_W*y+x+1) + *(in_image+DEAL_IMAGE_W*y+x-1)) <= 1) )
                *(in_image+DEAL_IMAGE_W*y+x) = 0;
        }
    }
}
/**
*
* @brief  给图像上左右画两列黑框
* @notes
* Example:  Image_Process();
*
**/
void image_draw_rectan(unsigned char(*image)[DEAL_IMAGE_W])
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
* @brief  图像处理
* @notes
* Example:  Image_Process();
*
**/
float Image_Process(unsigned char prospect, float error_offset)
{
    // 图像压缩
    Image_Compress(mt9v03x_image[0],Gray_Image[0]);
    // 图像二值化
    Image_Binary(Gray_Image[0],Binarization_Image[0]);
    //赛道边界滤波
    Image_Filter(Binarization_Image[0]);
    //在图像上,左,右,加两行黑框
    image_draw_rectan(Binarization_Image);
    //扫线
    Search_Line(Binarization_Image);
    //保存边线到图像数组
    Show_Line(Binarization_Image[0]);
    //弧度计算
    Curve=Line_Curvature(Center_Line,DEAL_IMAGE_H,Find_Line_End,Find_Line_Start);
    ///偏差计算
    float error = 1.1 * (Center_Line[prospect] - DEAL_IMAGE_W/2) - 0.75 *Curve  + error_offset;
    //显示图像
    ips114_show_gray_image(0,0,Binarization_Image[0], DEAL_IMAGE_W, DEAL_IMAGE_H,188,120,0);
   return error;
}
