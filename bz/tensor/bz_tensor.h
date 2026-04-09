/***********************************************************************
 张量
***********************************************************************/
#ifndef BZ_TENSOR_H
#define BZ_TENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../bz.h"

#define BZ_TENSOR_FLAG_OWN_DATA   (1 << 0)  // 是否拥有 data 内存
#define BZ_TENSOR_FLAG_C_ORDER    (1 << 1)  // C 顺序（行优先），暂不支持 Fortran 顺序（列优先）
#define BZ_TENSOR_FLAG_EXT_DIM    (1 << 2)  // 使用外部维度数组

#define BZ_TENSOR_INLINE_DIM 3              // 内联维度数组的最大阶

typedef struct
{
    /* 高频访问字段优先 */
    void* data;

    union // 低地址为高阶
    {
        // 外部存储（高阶张量）
        struct
        {
            bz_uint* shape;
            bz_uint* strides;
        } ext;
        // 内联存储（低阶张量）
        struct
        {
            bz_uint shape[BZ_TENSOR_INLINE_DIM];
            bz_uint strides[BZ_TENSOR_INLINE_DIM];
        } inl;
    } dim;

    bz_uint ndim;     // 阶数
    bz_uint count;    // 元素总数
    bz_dtype dtype;   // 元素的数据类型
    bz_uint itemsize; // 元素的字节大小
    bz_uint32 flags;  // 标志位
} bz_tensor;

#define bz_tensor_shape(t)   (((t)->flags & BZ_TENSOR_FLAG_EXT_DIM) ? (t)->dim.ext.shape : (t)->dim.inl.shape)
#define bz_tensor_strides(t) (((t)->flags & BZ_TENSOR_FLAG_EXT_DIM) ? (t)->dim.ext.strides : (t)->dim.inl.strides)

/* 
 * 创建一个张量实例，该实例拥有独立 data 内存，行优先
 * @param errno_p 错误码将在所指地址处返回。传入 NULL，则忽略
 * @return 失败返回 NULL
*/
extern bz_tensor* bz_tensor_create(bz_uint* shape, bz_uint ndim, bz_dtype dtype, bz_int* errno_p);

/*
 * 销毁张量
 * @param t_2p 二阶指针，*t_2p 指向将变更为 NULL。传入 NULL 或 *t_2p 为 NULL，则忽略
*/
extern void bz_tensor_delete(bz_tensor** t_2p);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
