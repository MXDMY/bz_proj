/***********************************************************************
 该头文件及其源文件提供框架API的基础
***********************************************************************/
#ifndef BZ_API_H
#define BZ_API_H

#include "../bz/mh/bz_tensor.h"
#include "../bz/nn/bz_fc.h"
#include "../bz/fmt/bz_img.h"
#include "../bz/dsa/bz_list.h"

//框架 API（局部）错误号，错误号预留位-99到-0
enum bz_api_error_num
{
    BZ_API_ERROR_NUM_HEAD = -99, //错误号头部，确保所有错误号为负数

    BZ_APT_PARAM_STR_ERROR,      //字符串参数格式错误

    BZ_API_ERROR_NUM_TAIL = -0,  //错误号尾部，不使用，增强易读性
};

typedef struct __bz_api_tensor_func_s
{
    void (*del)(bz_tensor_o**);
    bz_int (*get)(bz_tensor_o* , void*);
    bz_tensor_o* (*hadamard_x)(bz_tensor_o* , bz_tensor_o*);
    bz_tensor_o* (*kronecker_x)(bz_tensor_o* , bz_tensor_o*);
    bz_tensor_o* (*mul)(bz_tensor_o* , bz_tensor_o*);
    bz_tensor_o* (*T)(bz_tensor_o*);
    bz_tensor_o* (*copy)(bz_tensor_o*);
    bz_tensor_o* (*eig)(bz_tensor_o*);
    bz_tensor_o** (*svd)(bz_tensor_o*);
    bz_tensor_o** (*svd_trunc)(bz_tensor_o* , bz_dbl);
    bz_tensor_o* (*mode_n_x)(bz_tensor_o* , bz_tensor_o* , bz_uint);
    bz_dbl (*norm_f)(bz_tensor_o*);
    bz_tensor_o* (*add)(bz_tensor_o* , bz_tensor_o*);
    bz_tensor_o* (*dec)(bz_tensor_o* , bz_tensor_o*);
    bz_int (*retype)(bz_tensor_o* , bz_mh_dtype);
    bz_tensor_o* (*create)(char* , bz_mh_dtype);
    bz_tensor_o* (*create_rand)(char* , bz_dbl , bz_dbl , bz_mh_dtype);
    bz_tensor_o** (*tucker)(bz_tensor_o* , bz_dbl);

    bz_int (*print)(bz_tensor_o*);
    bz_int (*print_shape)(bz_tensor_o*);
}__bz_tensor;

//打印张量，成功返回 0，失败返回错误号
bz_int bz_api_tensor_print(bz_tensor_o* obj);

//打印张量型状，成功返回 0，失败返回错误号
bz_int bz_api_tensor_print_shape(bz_tensor_o* obj);

//创建一个张量实例，失败返回 NULL
bz_tensor_o* bz_api_tensor_create(char* str , bz_mh_dtype type);

//创建张量，并初始化为随机数据，失败返回 NULL
bz_tensor_o* bz_api_tensor_create_rand(char* str , bz_dbl min , bz_dbl max , bz_mh_dtype type);


typedef struct __bz_api_list_func_s
{
    bz_list_o* (*create)(char*);
    void (*del)(bz_list_o**);
    bz_int (*append)(bz_list_o* , bz_ds_u);
    bz_ds_u (*index)(bz_list_o* , bz_uint);

    bz_int (*print)(bz_list_o*);
}__bz_list;

//打印列表，成功返回 0，失败返回错误号
bz_int bz_api_list_print(bz_list_o* list);

//创建列表，失败返回 NULL
bz_list_o* bz_api_list_create(char* str);


typedef struct __bz_api_func_s
{
    __bz_tensor tensor;
    __bz_list list;

    void (*srand)(bz_int);
    bz_uint (*rand)();
}__bz;

__bz bz_api_init();


#endif