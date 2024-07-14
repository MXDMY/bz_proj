/***********************************************************************
 该头文件及其源文件包括扩展数学部分所需要的通用变量或函数
***********************************************************************/
#ifndef BZ_MH_H
#define BZ_MH_H

#include "../bz.h"

//框架扩展数学部分（局部）错误号，错误号预留位-899到-800
enum bz_mh_error_num
{
    BZ_MH_ERROR_NUM_HEAD = -899,           //错误号头部，确保所有错误号为负数

    BZ_MH_OUT_OF_BOUNDS_ERROR,             //越界错误
    BZ_MH_DIFF_DATA_TYPE_ERROR,            //相互运算的数据类型不一致
    BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR,      //不支持该数据类型的处理
    BZ_MH_TENSOR_DARR_NON_POS_VALUE_ERROR, //维度数组包含非正数
    BZ_MH_TENSOR_DIM_DIV_ERROR,            //张量维度划分不合规
    BZ_MH_TENSOR_SHAPE_ERROR,              //张量形状错误，张量形状不符合计算要求，如矩阵乘法中前面的列不等于后面的行
    BZ_MH_TENSOR_DATA_COUNT_ERROR,         //张量包含的元素的个数不符合计算要求

    BZ_MH_ERROR_NUM_TAIL = -800,           //错误号尾部，不使用，增强易读性
};

//数据类型
typedef enum bz_mh_data_type
{
    BZ_INT_TYPE,
    BZ_DOUBLE_TYPE,
}bz_mh_dtype;

//整型范围结构体
typedef struct bz_mh_int_range
{
    bz_int start;
    bz_int step;  //修正函数会对该值进行修正，例如：{7 , 1 , 5}会被修正为{7 , -1 , 5}；{1 , 0 , 5}修正为{1 , 1 , 5}
    bz_int end;
}bz_mh_irange;

//整形范围结构体修正函数，step 为0，默认修正为1或-1，成功返回0，失败返回错误号
bz_int bz_mh_irange_corr(bz_mh_irange* range , bz_uint count);


#endif