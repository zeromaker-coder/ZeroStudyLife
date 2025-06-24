#ifndef IMAGE_H
#define IMAGE_H

#define DEAL_IMAGE_W    80  // 压缩图像的图像宽
#define DEAL_IMAGE_H    60  // 压缩图像的图像高

extern unsigned char Deal_Image_Gray[DEAL_IMAGE_H][DEAL_IMAGE_W];       // 压缩后的灰度图像
extern unsigned char Deal_Image_Binary[DEAL_IMAGE_H][DEAL_IMAGE_W];     // 二值化图像
extern unsigned char Car_State;                                         // 车状态,0:停车,1:正常

/**
*
* @brief    图像压缩
* @param    *in_image       原始图像
* @param    *out_image      压缩后的图像
* @return   void
* @notes
* Example:  Image_Compress(MT9V034_Image_Data[0],Deal_Image_Gray[0]);
*
**/
void Image_Compress(unsigned char *in_image,unsigned char *out_image);

/**
*
* @brief    大津法计算按图像二值化阈值
* @param    *image          需要计算阈值的图像
* @param    width           图像宽
* @param    height          图像高
* @param    pixel_threshold 图像阈值
* @return   unsigned char   大津法阈值
* @notes
* Example:  Image_Threshold(Deal_Image_Gray[0],DEAL_IMAGE_W,DEAL_IMAGE_H,150);
*
**/
unsigned char Image_Threshold(unsigned char *image, unsigned short width, unsigned short height, unsigned int image_max_threshold);

///**
// *
// * @brief    图像二值化
// * @param    *in_image       压缩后的图像
// * @param    *out_image      二值化后的图像
// * @return   void
// * @notes
// * Example:  Image_Binary(Deal_Image_Gray[0],Deal_Image_Binary[0],150);
// *
// **/
//
void Image_Binary(unsigned char *in_image,unsigned char *out_image);
 /**
  *
  * @brief    图像滤波
  * @param    *in_image       需要滤波的图像
  * @return   void
  * @notes
  * Example:  Image_Filter(Deal_Image_Binary[0]);
  *
  **/
 void Image_Filter(unsigned char *in_image);

 /**
 *
 * @brief    图像扫线
 * @param    *in_image       需要处理的图像
 * @return   void
 * @notes
 * Example:  Image_Find_Line(Deal_Image_Binary[0]);
 *
 **/
void Search_Line(unsigned char(*image)[DEAL_IMAGE_W]);

// /**
//  *
//  * @brief    图像处理
//  * @param    prospect            计算偏差的前瞻
//  * @param    error_offset        偏差补偿
//  * @return   void
//  * @notes
//  * Example:  Image_Deal(30,12.5);
//  *
//  **/
// float Image_Deal(unsigned char prospect, float error_offset);
#endif
