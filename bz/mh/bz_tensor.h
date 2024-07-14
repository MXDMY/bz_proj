/***********************************************************************
 该头文件及其源文件实现框架的张量操作
***********************************************************************/
#ifndef BZ_TENSOR_H
#define BZ_TENSOR_H

#include "bz_mh.h"

//张量结构体
typedef struct bz_tensor_object
{
    void* data;          //内部数据指针
    bz_uint* darr;       //数组，储存张量每个维度的长度
    bz_uint* varr;       //数组，储存 相邻同级维 起始地址间隔 的数据数量
    bz_uint dim;         //张量总维度（阶数）
    bz_uint total_count; //张量内部数据总数量
    bz_mh_dtype type;    //内部数据类型
}bz_tensor_o;

//创建一个张量实例，失败返回 NULL
bz_tensor_o* bz_tensor_create(bz_uint* darr , bz_uint dim , bz_mh_dtype type);

/*
  销毁一个被创建的张量实例，并将原张量指针指向NULL
  参数 pobj 为 NULL 或 指向的指针（即 *pobj）为 NULL，则无操作
*/
void bz_tensor_del(bz_tensor_o** pobj);

/*
  从 data 指向的内存中获得张量数据，成功返回0，失败返回错误号
  该函数不确认 data 指向的数据类型，请至少确保 data 指向的空间足够
*/
bz_int bz_tensor_get(bz_tensor_o* obj , void* data);

//返回 coo数组元素 构成的坐标的数据地址，失败返回NULL
void* bz_tensor_coo(bz_tensor_o* obj , bz_uint* coo , bz_uint coo_dim);

//返回张量内部最大元素的地址，失败返回NULL
void* bz_tensor_max(bz_tensor_o* obj);

//张量 Hadamard 积，成功返回一个新张量，失败返回NULL
bz_tensor_o* bz_tensor_hadamard_product(bz_tensor_o* obj1 , bz_tensor_o* obj2);

//二阶张量（矩阵） Kronecker 积，成功返回一个新张量，失败返回NULL
bz_tensor_o* bz_tensor_kronecker_product(bz_tensor_o* obj1 , bz_tensor_o* obj2);

//二阶张量（矩阵）乘运算，成功返回一个新张量，失败返回NULL
bz_tensor_o* bz_tensor_mul(bz_tensor_o* obj1 , bz_tensor_o* obj2);

//更改张量的维度，成功返回0，失败返回错误号
bz_int bz_tensor_reshape(bz_tensor_o* obj , bz_uint* darr , bz_uint dim);

//张量填充，填充值为 init_v 指向的单个数据，成功返回填充生成的新张量，失败返回NULL
bz_tensor_o* bz_tensor_pad(bz_tensor_o* obj , bz_uint pad , void* init_v);

/*
  张量切片，成功返回切片生成的新张量，失败返回NULL
  range 数组储存每个维度切片的起、止范围（起、止位置本身均在切片范围内），大小为 count
*/
bz_tensor_o* bz_tensor_slice(bz_tensor_o* obj , bz_mh_irange* range , bz_uint count);

//二阶张量转置，成功返回转置后的新张量，失败返回NULL
bz_tensor_o* bz_tensor_T(bz_tensor_o* obj);

/*
  张量维度任意有序划分（矩阵化、模 n 展开...），成功返回新张量，失败返回 NULL
  参数 div_arr 数组采用单独的 0 分隔，维度从 1 开始计数，首尾不为 0
  格式例子：{2 , 1 , 0 , 3}
  上述表示将一个 3 阶张量前两维调换并划分到同一维，第 3 维单独作为一维，即：{2 , 1} x {3}
  当 NULL != div_data，函数执行有序划分的逆操作，将 div_data 中的数据进行有序划分的逆向操作复制到 obj->data
  成功后的返回值与参数 obj 相同
*/
bz_tensor_o* bz_tensor_dimdiv8(bz_tensor_o* obj , bz_uint* div_arr , bz_uint arr_count , void* div_data , bz_uint data_count);

/*
  模 n 积，成功返回新张量，失败返回 NULL
  obj1 为待模 n 展开的张量，obj2 为左乘的矩阵
*/
bz_tensor_o* bz_tensor_mode_n_product(bz_tensor_o* obj1 , bz_tensor_o* obj2 , bz_uint n);

//更改张量数据类型，成功返回 0，失败返回错误号
bz_int bz_tensor_retype(bz_tensor_o* obj , bz_mh_dtype type);

//成功返回一个张量副本，失败返回 NULL
bz_tensor_o* bz_tensor_copy(bz_tensor_o* obj);

/*
  求解特征值、特征向量
  成功返回一个 (N + 1) x N 矩阵，前 N 行即特征向量（列向量），最后一行即特征向量对应的特征值
  失败返回 NULL
*/
bz_tensor_o* bz_tensor_eig(bz_tensor_o* obj);

/*
  紧奇异值分解
  成功返回一个动态分配的 bz_tensor_o* 数组，包含 3 个元素，依次为 U diag(1...n) V
  成功获取结果后，可进行以下步骤分离结果：
  bz_tensor_o** svd_ret = bz_tensor_svd(obj);
  bz_tensor_o* U = svd_ret[0];
  bz_tensor_o* diag = svd_ret[1];
  bz_tensor_o* V = svd_ret[2];
  bz_mem_free(svd_ret);
  失败返回 NULL
*/
bz_tensor_o** bz_tensor_svd(bz_tensor_o* obj);

/*
  截断奇异值分解，低于 [最大奇异值 * tr] 的 奇异值及相应奇异向量 将被截断
  参数 tr 不易过大，否则将返回 NULL，并产生 BZ_FUNC_PARAM_ERROR 错误
*/
bz_tensor_o** bz_tensor_svd_trunc(bz_tensor_o* obj , bz_dbl tr);

/*
  态矢到矩阵乘积态，成功返回分解结果，分解后的矩阵的个数在 count_p 中返回，失败返回 NULL
  obj 无论形状如何，计算中都将视其为行向量，参数 tr 在截断奇异值分解中使用
*/
bz_tensor_o** bz_tensor_mps(bz_tensor_o* obj , bz_dbl tr , bz_uint* count_p);

//张量 F 范数，成功返回范数结果，失败返回负数
bz_dbl bz_tensor_norm_f(bz_tensor_o* obj);

//张量 加 运算，成功返回新张量，失败返回NULL
bz_tensor_o* bz_tensor_add(bz_tensor_o* obj1 , bz_tensor_o* obj2);

//张量 减 运算，成功返回新张量，失败返回NULL
bz_tensor_o* bz_tensor_dec(bz_tensor_o* obj1 , bz_tensor_o* obj2);

/*
  Tucker 分解，截断 HO-SVD 方法
  参数 tr 为截断率，范围 0 - 1，tr 越大，因子矩阵被截断的部分越多，但至少保留 1 列
  成功返回分解后的因子数组（包括核心张量且位于首元素），因子个数等于（输入张量阶数 + 1），失败返回 NULL
*/
bz_tensor_o** bz_tensor_tucker(bz_tensor_o* obj , bz_dbl tr);


#endif