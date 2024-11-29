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

// 创建张量，失败返回 NULL，若 errcode_p 不为 NULL，错误号将在所指地址处返回（注意：函数成功不会在此有任何修改）
bz_tensor* bz_tensor_create(bz_uint* shape , bz_uint ndim , bz_dtype dtype , bz_int* errcode_p);

// 销毁张量，*dblptr 指向将变更为 NULL，若 dblptr 为 NULL 或 *dblptr 为 NULL，则无操作
void bz_tensor_delete(bz_tensor** dblptr);


#endif