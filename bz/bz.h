/***********************************************************************
 作者：中华人民共和国-非985非211普通本科 安徽工业大学-非全日制研究生-毕洁
 
                     ##                 ##      ############        
                    ##                   ##       ##    ##
                   ##                     ##        ####
            ################                       ##  ##
            ##            ##            ##       ##      ##
            ##            ##             ##          ##
            ##            ##              ##      ########
            ################                         ##
            ##            ##              ##   ##############
            ##            ##             ##          ##
            ##            ##            ##           ##
            ################           ##            ##

 框架名称：白泽
 命名来源：白泽，为华夏古代神话故事中的一种神兽，能人言，聪慧，并通达知晓万物之情理。

          东晋《抱朴子‧极言》：穷神奸则记白泽之辞。
          南朝梁《宋书·志·卷二十九·符瑞下》：泽兽，黄帝时巡狩至于东滨，泽兽出，能言，
                            达知万物之精，以戒于民，为时除害。贤君明德幽远则来。

 简介：该框架为搭建人工智能模型的通用机器学习框架，开发最初是为了学习，后因作者接手了一
      个嵌入式项目，需要在嵌入式平台构建用于手势识别的人工智能模型，而重新恢复开发。框
      架采用C语言实现，力图构建一个全平台可用的机器学习框架，持续开发中。

***********************************************************************/
/***********************************************************************
 该头文件及其源文件实现框架的基础变量与函数等基础配置
***********************************************************************/
#ifndef BZ_H
#define BZ_H

#include "bz_conf.h"

#if BZ_IO_MODE == 1 || BZ_DEV_RW_MODE == 1
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

#if BZ_MH_MODE == 1
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

//框架全局错误号，错误号预留位-999到-900，每个类型（全局、局部）错误号的值域都应不同
enum bz_error_num
{
    BZ_ERROR_NUM_HEAD = -999,      //错误号头部，确保所有错误号为负数

    BZ_FUNC_RETURN_ABNORMAL_ERROR, //函数返回异常，例如框架内存函数自定义修改时出现不应当有的返回值
    BZ_FUNC_PARAM_ERROR,           //函数参数错误
    BZ_MEM_ALLOC_ERROR,            //内存分配错误
    BZ_DEV_OPEN_ERROR,             //设备打开错误
    BZ_DEV_CLOSE_ERROR,            //设备关闭错误
    BZ_DEV_READ_ERROR,             //设备读取错误
    BZ_DEV_WRITE_ERROR,            //设备写入错误

    BZ_ERROR_NUM_TAIL = -900,      //错误号尾部，不使用，增强易读性
};

//框架部分函数不直接返回错误号，可通过该全局变量查阅
extern bz_int bz_errno;

/*******************************************************************************************************
 框架内存分配相关，允许自定义修改内部实现，自定义修改时需保证接口功能与C语言标准内存函数一致
*******************************************************************************************************/
//内存分配
void* bz_mem_alloc(bz_uint size);

//内存分配，初始化0值
void* bz_mem_calloc(bz_uint nitems , bz_uint size);

//内存重分配
void* bz_mem_ralloc(void* ptr , bz_uint size);

//内存拷贝
void* bz_mem_cpy(void* str1 , const void* str2 , bz_uint n);

//内存赋值
void* bz_mem_set(void* str , bz_int c , bz_uint n);

//内存比较
bz_int bz_mem_cmp(const void* str1 , const void* str2 , bz_uint n);

//内存释放
void bz_mem_free(void* ptr);

/*******************************************************************************************************
 框架数学函数相关，允许自定义修改内部实现，自定义修改时需保证接口功能与C语言标准数学函数一致
*******************************************************************************************************/
//返回 e 的 x 次幂的值
bz_dbl bz_mh_exp(bz_dbl x);

//返回 x 的 y 次幂
bz_dbl bz_mh_pow(bz_dbl x , bz_dbl y);

//返回 x 的平方根
bz_dbl bz_mh_sqrt(bz_dbl x);

//返回 浮点数 的绝对值
bz_dbl bz_mh_fabs(bz_dbl x);

//根据种子初始化随机数发生器
void bz_mh_srand(bz_int seed);
//返回取值在 0 ~ BZ_RAND_MAX 之间的随机数
bz_uint bz_mh_rand();

/*******************************************************************************************************
 框架设备读写相关，允许自定义修改内部实现，自定义修改时需保证接口功能与C语言标准文件函数一致
*******************************************************************************************************/
//打开设备，以模式 mode 打开设备 dev，并返回设备数据流
void* bz_dev_open(const void* dev , const void* mode);

//以二进制只读方式打开设备，并返回设备数据流
void* bz_dev_readonly_open(const void* dev);

//关闭设备
bz_int bz_dev_close(void* stream);

//读取设备数据流
bz_uint bz_dev_read(void* ptr , bz_uint size , bz_uint nmemb , void* stream);

//写入设备数据流
bz_uint bz_dev_write(const void* ptr , bz_uint size , bz_uint nmemb , void* stream);

//设备数据流错误检测
bz_int bz_dev_error(void* stream);

//设备数据流结束检测
bz_int bz_dev_eof(void* stream);

//清除设备数据流错误与结束指示
void bz_dev_clearerr(void* stream);


#endif