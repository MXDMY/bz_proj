/***********************************************************************
 该头文件及其源文件包括神经网络部分所需要的通用变量或函数
***********************************************************************/
#ifndef BZ_NN_H
#define BZ_NN_H

#include "../bz.h"

//框架神经网络部分（局部）错误号，错误号预留位-699到-600
enum bz_nn_error_num
{
    BZ_NN_ERROR_NUM_HEAD = -699,       //错误号头部，确保所有错误号为负数

    //BZ_NN_DIFF_DARR_IN_COV_LIST_ERROR, //同一层卷积层的卷积核的维度数组不一致
    //BZ_NN_NO_OPERABLE_COV_ERROR,       //没有可操作的卷积
    //BZ_NN_COV_DIM_DARR_ERROR,          //卷积核的维度不合法
    //BZ_NN_NO_OPERABLE_POOL_ERROR,      //没有可操作的池化
    //BZ_NN_POOL_WIDTH_ERROR,            //池化区域的范围不合法
    //BZ_NN_POOL_DIM_ERROR,              //被池化的张量维度不合法
    BZ_NN_MAP_DERI_FUNC_ERROR,         //查找不到原函数对应的导函数
    BZ_NN_CPRS_INCOR_PARAM_FILE_ERROR, //压缩模型加载的参数文件不正确

    BZ_NN_ERROR_NUM_TAIL = -600,       //错误号尾部，不使用，增强易读性
};

/*******************************************************************************************************
 激活函数以及对应导函数、辅助函数
*******************************************************************************************************/
//Relu函数
bz_dbl bz_nn_relu(bz_dbl input);

//Relu函数的批处理版本，避免处理大量数据时多次调用，结果修改在原数组上，成功返回0，失败返回错误号
bz_int bz_nn_relu_bat(bz_dbl* input , bz_uint input_count);

//Relu函数对应导函数
bz_dbl bz_nn_relu_d(bz_dbl input);

//Relu函数对应导函数的批处理版本，结果修改在原数组上，成功返回0，失败返回错误号
bz_int bz_nn_relu_d_bat(bz_dbl* input , bz_uint input_count);

//Sigmoid函数
bz_dbl bz_nn_sigmoid(bz_dbl input);

//Sigmoid函数的批处理版本，结果修改在原数组上，成功返回0，失败返回错误号
bz_int bz_nn_sigmoid_bat(bz_dbl* input , bz_uint input_count);

//Sigmoid函数对应导函数
bz_dbl bz_nn_sigmoid_d(bz_dbl input);

//Sigmoid函数对应导函数的批处理版本，结果修改在原数组上，成功返回0，失败返回错误号
bz_int bz_nn_sigmoid_d_bat(bz_dbl* input , bz_uint input_count);

//根据提供的函数地址查找对应的导函数地址，查找不到则返回NULL
void* bz_nn_map_deri_func(void* prim_func);


#endif