/***********************************************************************
 该头文件及其源文件定义基础实现
***********************************************************************/
#ifndef BZ_H
#define BZ_H

#include "bz_conf.h"

#if BZ_IO_MODE == 0 || BZ_DEV_RW_MODE == 0
#include <stdio.h>
#endif

#if BZ_MEM_ALLOC_MODE == 0
#include <stdlib.h>
#include <string.h>
#elif BZ_MEM_ALLOC_MODE == 1
#define USER_CUSTOM_ALLOC_MEM
#else
bz_u8 bz_mem[BZ_MEM_SIZE];
#endif

#if BZ_MH_MODE == 0
#include <math.h>
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

// 全局错误号，错误号预留位 -99 到 -0（错误号编写格式均参考该枚举类型）
enum bz_errcode
{
    BZ_ERRCODE_HEAD = -99, // 错误号头部，确保所有错误号为负数

    BZ_FUNC_PARAM_ERROR,   // 函数参数错误，参数不满足函数要求
    BZ_MEM_ALLOC_ERROR,    // 内存分配错误

    BZ_ERRCODE_TAIL = -0,  // 错误号尾部，不使用，增强易读性
};
// 局部错误号（张量计算）
enum bz_tensor_errcode
{
    BZ_TENSOR_ERRCODE_HEAD = -199,


    BZ_TENSOR_ERRCODE_TAIL = -100,
};

// 部分函数不直接返回错误号，可通过该全局变量查阅，用于与 C 标准库函数对齐
// extern bz_int bz_errno; 暂时注释，便于重构

/* 用于指示某些复杂结构体（如张量）的基础数据类型 */
typedef enum bz_data_type
{
    BZ_INT_TYPE = 0, // 枚举的值同时表示数组索引，不要随意修改
    BZ_FLOAT_TYPE,

    /* 此处添加新的基础数据类型 */

    /************************/

    BZ_DTYPE_END,
}bz_dtype;
// 数据类型的字节大小映射表，便于直接通过 bz_dtype 枚举型获得对应的字节大小，注意：新增映射时，确保类型顺序一致
extern bz_uint bz_dtype_size_table[BZ_DTYPE_END];
/******************************************************************************************************/

/*******************************************************************************************************
 框架内存分配相关，允许自定义修改内部实现，自定义修改时需保证接口功能与 C 语言标准内存函数一致
*******************************************************************************************************/
// 内存分配
void* bz_mem_alloc(bz_uint size);

// 内存分配，初始化 0 值
void* bz_mem_calloc(bz_uint nitems , bz_uint size);

// 内存重分配
void* bz_mem_ralloc(void* ptr , bz_uint size);

// 内存拷贝
void* bz_mem_cpy(void* str1 , const void* str2 , bz_uint n);

// 内存赋值
void* bz_mem_set(void* str , bz_int c , bz_uint n);

// 内存比较
bz_int bz_mem_cmp(const void* str1 , const void* str2 , bz_uint n);

// 内存释放
void bz_mem_free(void* ptr);

/*******************************************************************************************************
 框架数学函数相关，允许自定义修改内部实现，自定义修改时需保证接口功能与 C 语言标准数学函数一致
*******************************************************************************************************/
// 返回 e 的 x 次幂的值
bz_float bz_mh_exp(bz_float x);

// 返回 x 的 y 次幂
bz_float bz_mh_pow(bz_float x , bz_float y);

// 返回 x 的平方根
bz_float bz_mh_sqrt(bz_float x);

// 返回 浮点数 的绝对值
bz_float bz_mh_fabs(bz_float x);

// 根据种子初始化随机数发生器
void bz_mh_srand(bz_int seed);
// 返回取值在 0 ~ BZ_RAND_MAX 之间的随机数
bz_uint bz_mh_rand();

/*******************************************************************************************************
 框架设备读写相关，允许自定义修改内部实现，自定义修改时需保证接口功能与 C 语言标准文件函数一致
*******************************************************************************************************/
// 打开设备，以模式 mode 打开设备 dev，并返回设备数据流
void* bz_dev_open(const void* dev , const void* mode);

// 以二进制只读方式打开设备，并返回设备数据流
void* bz_dev_readonly_open(const void* dev);

// 关闭设备
bz_int bz_dev_close(void* stream);

// 读取设备数据流
bz_uint bz_dev_read(void* ptr , bz_uint size , bz_uint nmemb , void* stream);

// 写入设备数据流
bz_uint bz_dev_write(const void* ptr , bz_uint size , bz_uint nmemb , void* stream);

// 设备数据流错误检测
bz_int bz_dev_error(void* stream);

// 设备数据流结束检测
bz_int bz_dev_eof(void* stream);

// 清除设备数据流错误与结束指示
void bz_dev_clearerr(void* stream);


#endif