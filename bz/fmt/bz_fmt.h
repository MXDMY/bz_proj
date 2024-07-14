/***********************************************************************
 该头文件及其源文件包括图像、音频等格式处理部分所需要的通用变量或函数
***********************************************************************/
#ifndef BZ_FMT_H
#define BZ_FMT_H

#include "../bz.h"

//框架格式处理部分（局部）错误号，错误号预留位-799到-700
enum bz_fmt_error_num
{
    BZ_FMT_ERROR_NUM_HEAD = -799,      //错误号头部，确保所有错误号为负数

    BZ_FMT_THE_SAME_CVRT_ERROR,        //格式转换源格式与目标格式一致
    BZ_FMT_SCOUNT_ERROR,               //源数据的数量不符合操作规范
    BZ_FMT_CVRT_TYPE_NO_SUPPORT_ERROR, //不支持该类型的转换

    BZ_FMT_ERROR_NUM_TAIL = -700,      //错误号尾部，不使用，增强易读性
};


#endif