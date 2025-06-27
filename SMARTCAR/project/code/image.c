#include "image.h"

#define DEAL_IMAGE_H 120
#define DEAL_IMAGE_W 160

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
  * @param  width 图像宽度
  * @param  height 图像高度
  * @retval 阈值
  */
uint8 otsu_get_threshold(uint8 *image, uint16 width, uint16 height) {
    uint32 histogram[256] = {0};
    uint32 total_pixels = width * height;

    // 计算直方图
    for (uint16 i = 0; i < height; i+=2) {
        for (uint16 j = 0; j < width; j+=2) {
            histogram[(int)image[i * width + j]]++;
        }
    }

    // 计算全局平均灰度值
    double sum = 0;
    for (int i = 0; i < 256; i++) {
        sum += i * histogram[i];
    }
    double global_mean = sum / total_pixels;

    double max_variance = 0;
    uint8 threshold = 0;

    //otsu计算阈值
    for (uint16 t = 0; t < 256; t++) {
        double w0 = 0, w1 = 0;
        double sum0 = 0, sum1 = 0;

        for (uint16 i = 0; i <= t; i++) {
            w0 += histogram[i];
            sum0 += i * histogram[i];
        }
        for (uint16 i = t + 1; i < 256; i++) {
            w1 += histogram[i];
            sum1 += i * histogram[i];
        }

        if (w0 == 0 || w1 == 0) continue;

        double mean0 = sum0 / w0;
        double mean1 = sum1 / w1;

        double variance = w0 * w1 * ((mean0 - mean1) * (mean0 - mean1));

        if (variance > max_variance) {
            max_variance = variance;
            threshold = t;
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
                *(out_image+temp+x) = 1;  // 白
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

