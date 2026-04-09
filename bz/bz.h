/***********************************************************************
 基础实现
***********************************************************************/
#ifndef BZ_H
#define BZ_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bz_conf.h"

#if BZ_USE_C_STD_LIB
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef EOF
#define EOF (-1)
#endif

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

// 全局错误码，预留位 -99 到 0
enum bz_errcode
{
    BZ_ERRCODE_HEAD = -99, // 错误码头部，确保所有错误码为负数

    BZ_FUNC_PARAM_ERROR,   // 函数参数错误
    BZ_MEM_ALLOC_ERROR,    // 内存分配错误

    BZ_ERRCODE_TAIL = 0    // 错误码尾部，不使用，增强易读性
};
// 局部错误码（张量计算）
enum bz_tensor_errcode
{
    BZ_TENSOR_ERRCODE_HEAD = -199,


    BZ_TENSOR_ERRCODE_TAIL = -100,
};

/* 用于指示某些复杂结构体（如张量）的基础数据类型 */
typedef enum bz_meta_data_type
{
    BZ_INT_TYPE = 0, // 枚举的值同时表示数组索引，不要随意修改
    BZ_FLT_TYPE,
    BZ_INT8_TYPE,
    BZ_INT16_TYPE,
    BZ_INT32_TYPE,

    BZ_DTYPE_END,
} bz_dtype;
// 数据类型的字节大小映射表，便于直接通过 bz_dtype 枚举型获得对应的字节大小，注意：新增映射时，确保类型顺序一致
extern const bz_uint bz_dtype_size_table[BZ_DTYPE_END];
/******************************************************************************************************/

/*******************************************************************************************************
 修改时需保证接口功能与 C 标准库一致
*******************************************************************************************************/
extern bz_int bz_errno; // 用于与 C 标准库 errno 对齐

#ifndef bz_mem_alloc
extern void* bz_mem_alloc(bz_uint size);
#endif

#ifndef bz_mem_calloc
extern void* bz_mem_calloc(bz_uint nmemb, bz_uint size);
#endif

#ifndef bz_mem_ralloc
extern void* bz_mem_ralloc(void* ptr, bz_uint size);
#endif

#ifndef bz_mem_cpy
extern void* bz_mem_cpy(void* dest, const void* src, bz_uint n);
#endif

#ifndef bz_mem_set
extern void* bz_mem_set(void* s, bz_int c, bz_uint n);
#endif

#ifndef bz_mem_cmp
extern bz_int bz_mem_cmp(const void* s1, const void* s2, bz_uint n);
#endif

#ifndef bz_mem_free
extern void bz_mem_free(void* ptr);
#endif

#ifndef bz_mh_exp
extern bz_flt bz_mh_exp(bz_flt x);
#endif

#ifndef bz_mh_pow
extern bz_flt bz_mh_pow(bz_flt x, bz_flt y);
#endif

#ifndef bz_mh_sqrt
extern bz_flt bz_mh_sqrt(bz_flt x);
#endif

#ifndef bz_mh_fabs
extern bz_flt bz_mh_fabs(bz_flt x);
#endif

#ifndef bz_mh_srand
extern void bz_mh_srand(bz_uint seed);
extern bz_int bz_mh_rand(void);
#endif

#ifndef bz_fs_open
extern void* bz_fs_open(const char* path, const char* mode);
#endif

#ifndef bz_fs_close
extern bz_int bz_fs_close(void* stream);
#endif

#ifndef bz_fs_read
extern bz_uint bz_fs_read(void* ptr, bz_uint size, bz_uint nmemb, void* stream);
#endif

#ifndef bz_fs_write
extern bz_uint bz_fs_write(const void* ptr, bz_uint size, bz_uint nmemb, void* stream);
#endif

#ifndef bz_fs_error
extern bz_int bz_fs_error(void* stream);
#endif

#ifndef bz_fs_eof
extern bz_int bz_fs_eof(void* stream);
#endif

#ifndef bz_fs_clearerr
extern void bz_fs_clearerr(void* stream);
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
