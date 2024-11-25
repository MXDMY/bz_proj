/***********************************************************************
 该头文件及其所属源文件实现张量操作
***********************************************************************/
#ifndef BZ_TENSOR_H
#define BZ_TENSOR_H

#include "../bz.h"

// 张量
typedef struct bz_tensor_object
{
    void* data;       // 指向内部数据
    bz_uint* shape;   // 储存每个阶的长度，即形状
    bz_uint* offsets; // 储存每个阶的相邻两跳的偏移
    bz_uint ndim;     // 阶数
    bz_uint count;    // 元素总数
    bz_dtype dtype;   // 元素的数据类型
    bz_uint itemsize; // 元素的字节大小
}bz_tensor;

// 创建张量，失败返回 NULL，错误号将在 errcode_p 返回
bz_tensor* bz_tensor_create(bz_uint* shape , bz_uint ndim , bz_dtype dtype , bz_int* errcode_p);


#endif