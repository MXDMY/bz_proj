/***********************************************************************
 该头文件及其源文件包括基础数据结构与算法部分所需要的通用变量或函数
***********************************************************************/
#ifndef BZ_DSA_H
#define BZ_DSA_H

#include "../bz.h"

//（局部）错误号，错误号预留位-599到-500
enum bz_dsa_error_num
{
    BZ_DSA_ERROR_NUM_HEAD = -599, //错误号头部，确保所有错误号为负数

    BZ_DSA_INDEX_ERROR,           //索引错误

    BZ_DSA_ERROR_NUM_TAIL = -500, //错误号尾部，不使用，增强易读性
};

//数据结构使用的数据类型
typedef enum bz_ds_data_type
{
    BZ_DS_NONE_TYPE, //无类型

    //以下为定长数据类型
    BZ_DS_INT_TYPE,
    BZ_DS_UINT_TYPE,
    BZ_DS_U8_TYPE,
    BZ_DS_DOUBLE_TYPE,

    //以下为变长数据类型
    BZ_DS_LIST_TYPE,
}bz_ds_dtype;

//数据结构单元
typedef struct bz_ds_data_unit
{
    union 
    {
        bz_int i;
        bz_uint u;
        bz_u8 b;
        bz_dbl d;
        void* v;   //变长数据类型
    }data;
    bz_ds_dtype type;
}bz_ds_u;

//列表单元
typedef struct bz_list_unit
{
    bz_ds_u ds_u;
    struct bz_list_unit* pre;
    struct bz_list_unit* next;
}bz_list_u;
//列表
typedef struct bz_list_object
{
    bz_list_u* head;
    bz_list_u* tail;
    bz_uint count;
}bz_list_o;
/*
  销毁一个被创建的列表实例，并将原列表指针指向NULL
  参数 plist 为 NULL 或 指向的指针（即 *plist）为 NULL，则无操作
*/
void bz_list_del(bz_list_o** plist);


/*******************************************************************************************************
 以下为排序相关算法的函数声明
*******************************************************************************************************/
/*
  堆排序，成功返回 0，失败返回错误号
  参数 order：大于 0 顺序，小于 0 逆序，等于 0 不排序
  前 4 个参数 若出现 NULL 或 0，则表示不排，不会造成 BZ_FUNC_PARAM_ERROR 参数错误
*/
void bz_sort_heap(bz_int* int_data , bz_uint int_count , bz_dbl* dbl_data , bz_uint dbl_count , bz_int order);


#endif