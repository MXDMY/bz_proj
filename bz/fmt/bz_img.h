/***********************************************************************
 该头文件及其源文件实现图像格式处理
***********************************************************************/
#ifndef BZ_IMG_H
#define BZ_IMG_H

#include "bz_fmt.h"

//图像数据格式
typedef enum bz_img_format
{
    //YUV420P_PLANNAR_YV12,
    YUV420P_PLANNAR_YU12,  //每个元素均8bit，内容形式：YYYY...U...V...
    //YUV420SP_PLANNAR_NV12,
    //YUV420SP_PLANNAR_NV21,
    //YUV420P_PACKED_,       //保留

    RGB888,                //每个元素均8bit，内容形式：RGB...RGB...

    HSV_DBL,               //每个元素均 sizeof(bz_dbl) bit，内容形式：HSV...HSV...
}bz_img_fmt;

//图像格式转换，请确保 d 指向的内存空间足够，成功则返回转化后的数据个数，若返回0，表示转换失败，scount 为 s 指向的源数据的个数
bz_uint bz_img_cvrt(bz_u8* d , bz_img_fmt dformat , bz_u8* s , bz_img_fmt sformat , bz_uint scount);


#endif