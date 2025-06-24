#ifndef IMAGE_H
#define IMAGE_H

#define DEAL_IMAGE_W    80  // ѹ��ͼ���ͼ���
#define DEAL_IMAGE_H    60  // ѹ��ͼ���ͼ���

extern unsigned char Deal_Image_Gray[DEAL_IMAGE_H][DEAL_IMAGE_W];       // ѹ����ĻҶ�ͼ��
extern unsigned char Deal_Image_Binary[DEAL_IMAGE_H][DEAL_IMAGE_W];     // ��ֵ��ͼ��
extern unsigned char Car_State;                                         // ��״̬,0:ͣ��,1:����

/**
*
* @brief    ͼ��ѹ��
* @param    *in_image       ԭʼͼ��
* @param    *out_image      ѹ�����ͼ��
* @return   void
* @notes
* Example:  Image_Compress(MT9V034_Image_Data[0],Deal_Image_Gray[0]);
*
**/
void Image_Compress(unsigned char *in_image,unsigned char *out_image);

/**
*
* @brief    ��򷨼��㰴ͼ���ֵ����ֵ
* @param    *image          ��Ҫ������ֵ��ͼ��
* @param    width           ͼ���
* @param    height          ͼ���
* @param    pixel_threshold ͼ����ֵ
* @return   unsigned char   �����ֵ
* @notes
* Example:  Image_Threshold(Deal_Image_Gray[0],DEAL_IMAGE_W,DEAL_IMAGE_H,150);
*
**/
unsigned char Image_Threshold(unsigned char *image, unsigned short width, unsigned short height, unsigned int image_max_threshold);

///**
// *
// * @brief    ͼ���ֵ��
// * @param    *in_image       ѹ�����ͼ��
// * @param    *out_image      ��ֵ�����ͼ��
// * @return   void
// * @notes
// * Example:  Image_Binary(Deal_Image_Gray[0],Deal_Image_Binary[0],150);
// *
// **/
//
void Image_Binary(unsigned char *in_image,unsigned char *out_image);
 /**
  *
  * @brief    ͼ���˲�
  * @param    *in_image       ��Ҫ�˲���ͼ��
  * @return   void
  * @notes
  * Example:  Image_Filter(Deal_Image_Binary[0]);
  *
  **/
 void Image_Filter(unsigned char *in_image);

 /**
 *
 * @brief    ͼ��ɨ��
 * @param    *in_image       ��Ҫ�����ͼ��
 * @return   void
 * @notes
 * Example:  Image_Find_Line(Deal_Image_Binary[0]);
 *
 **/
void Search_Line(unsigned char(*image)[DEAL_IMAGE_W]);

// /**
//  *
//  * @brief    ͼ����
//  * @param    prospect            ����ƫ���ǰհ
//  * @param    error_offset        ƫ���
//  * @return   void
//  * @notes
//  * Example:  Image_Deal(30,12.5);
//  *
//  **/
// float Image_Deal(unsigned char prospect, float error_offset);
#endif
