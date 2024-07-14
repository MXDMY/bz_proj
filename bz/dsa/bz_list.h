/***********************************************************************
 该头文件及其源文件实现列表数据结构
***********************************************************************/
#ifndef BZ_LIST_H
#define BZ_LIST_H

#include "bz_dsa.h"

//创建一个列表实例，失败返回 NULL
bz_list_o* bz_list_create(bz_uint count);

//销毁函数在 bz_dsa.h 中声明
//void bz_list_del(bz_list_o** plist);

//向列表中添加元素，成功返回 0，失败返回错误号
bz_int bz_list_append(bz_list_o* list , bz_ds_u ds_u);

//获取索引元素，成功会将 bz_errno 置 0，失败则置 错误号
bz_ds_u bz_list_index(bz_list_o* list , bz_uint index);


#endif