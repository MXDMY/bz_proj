#include "bz_tensor.h"

bz_tensor_o* bz_tensor_create(bz_uint* darr , bz_uint dim , bz_mh_dtype type)
{
    //参数检查并计算张量数据的总数量
    if(NULL == darr || 0 == dim)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }
    bz_uint total = 1;
    for(bz_uint i = 0 ; i < dim ; i++)
    {
        if(0 == darr[i])
        {
            bz_errno = BZ_MH_TENSOR_DARR_NON_POS_VALUE_ERROR;
            return NULL;
        }
        else
            total *= darr[i];
    }

    //根据不同的数据类型分配不同的数据内存
    void* data = NULL;
    if(BZ_INT_TYPE == type)
        data = bz_mem_alloc(total * sizeof(bz_int));
    else if(BZ_DOUBLE_TYPE == type)
        data = bz_mem_alloc(total * sizeof(bz_dbl));
    else
    {
        bz_errno = BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;
        return NULL;
    }

    if(NULL == data)
    {
        bz_errno = BZ_MEM_ALLOC_ERROR;
        return NULL;
    }

    //分配张量结构体内存
    bz_tensor_o* obj = (bz_tensor_o*)bz_mem_alloc(sizeof(bz_tensor_o));
    if(NULL == obj)
        goto BZ_TENSOR_CREATE_FREE_DATA;

    //分配张量维度数组darr内存
    obj->darr = (bz_uint*)bz_mem_alloc(dim * sizeof(bz_uint));
    if(NULL == obj->darr)
        goto BZ_TENSOR_CREATE_FREE_OBJ;

    //分配varr内存
    obj->varr = (bz_uint*)bz_mem_alloc(dim * sizeof(bz_uint));
    if(NULL == obj->varr)
        goto BZ_TENSOR_CREATE_FREE_OBJ_DARR;

    //darr赋值
    for(bz_uint i = 0 ; i < dim ; i++)
        obj->darr[i] = darr[i];
    //计算varr并赋值，相比正向遍历计算O(n^2)，该方法只需要O(n)
    obj->varr[dim - 1] = 1;
    if(dim > 1)
    {
        bz_uint i = dim - 2;
        while(1)
        {
            obj->varr[i] = obj->varr[i + 1] * obj->darr[i + 1];
            if(0 == i)
                break;
            else
                i--;
        }    
    }
    //张量其余属性赋值
    obj->data = data;
    obj->dim = dim;
    obj->type = type;
    obj->total_count = total;
    return obj;

BZ_TENSOR_CREATE_FREE_OBJ_DARR:
    bz_mem_free(obj->darr);
BZ_TENSOR_CREATE_FREE_OBJ:
    bz_mem_free(obj);
BZ_TENSOR_CREATE_FREE_DATA:
    bz_mem_free(data);
    bz_errno = BZ_MEM_ALLOC_ERROR;
    return NULL;
}

void bz_tensor_del(bz_tensor_o** pobj)
{
    if(NULL == pobj)
        return;
    bz_tensor_o* obj = *pobj;
    if(NULL != obj)
    {
        bz_mem_free(obj->darr);
        bz_mem_free(obj->data);
        bz_mem_free(obj->varr);
        bz_mem_free(obj);
        *pobj = NULL;
    }
}

bz_int bz_tensor_get(bz_tensor_o* obj , void* data)
{
    if(NULL == obj || NULL == data)
        return BZ_FUNC_PARAM_ERROR;

    bz_uint total_count = obj->total_count;

#define DTYPE_HANDLE(dtype) \
do{ \
    dtype* obj_data = (dtype*)(obj->data); \
    dtype* int_data = (dtype*)data; \
    for(bz_uint i = 0 ; i < total_count ; i++) \
        obj_data[i] = int_data[i]; \
}while(0)

    if(BZ_INT_TYPE == obj->type)
        DTYPE_HANDLE(bz_int);
    else if(BZ_DOUBLE_TYPE == obj->type)
        DTYPE_HANDLE(bz_dbl);
    else
        return BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;

#undef DTYPE_HANDLE

    return 0;
}

void* bz_tensor_coo(bz_tensor_o* obj , bz_uint* coo , bz_uint coo_dim)
{
    if(NULL == obj || NULL == coo || coo_dim != obj->dim)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }
    for(bz_uint i = 0 ; i < coo_dim ; i++)
    {
        //coo坐标越界
        if(coo[i] >= obj->darr[i])
        {
            bz_errno = BZ_MH_OUT_OF_BOUNDS_ERROR;
            return NULL;
        }
    }
    
    //将coo坐标降至一维坐标
    bz_uint index = 0;
    for(bz_uint i = 0 ; i < coo_dim - 1 ; i++)
        index += coo[i] * obj->varr[i];
    index += coo[coo_dim - 1];

    //坐标一样，数据类型不同，地址也不同
    if(BZ_INT_TYPE == obj->type)
        return (void*)( (bz_int*)(obj->data) + index );
    else if(BZ_DOUBLE_TYPE == obj->type)
        return (void*)( (bz_dbl*)(obj->data) + index );
    else
    {
        bz_errno = BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;
        return NULL;
    }
}

void* bz_tensor_max(bz_tensor_o* obj)
{
    if(NULL == obj)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }

#define DTYPE_HANDLE(dtype) \
do{ \
    dtype* data = (dtype*)(obj->data); \
    dtype* max = data + 0; \
    for(bz_uint i = 1 ; i < obj->total_count ; i++) \
    { \
        if(data[i] > (*max)) \
            max = data + i; \
    } \
    return (void*)max; \
}while(0)

    if(BZ_INT_TYPE == obj->type)
        DTYPE_HANDLE(bz_int);
    else if(BZ_DOUBLE_TYPE == obj->type)
        DTYPE_HANDLE(bz_dbl);
    else
    {
        bz_errno = BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;
        return NULL;
    }

#undef DTYPE_HANDLE
}

bz_tensor_o* bz_tensor_hadamard_product(bz_tensor_o* obj1 , bz_tensor_o* obj2)
{
    if(NULL == obj1 || NULL == obj2)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }
    if(obj1->type != obj2->type)
    {
        bz_errno = BZ_MH_DIFF_DATA_TYPE_ERROR;
        return NULL;
    }
    if(obj1->total_count != obj2->total_count)
    {
        bz_errno = BZ_MH_TENSOR_DATA_COUNT_ERROR;
        return NULL;
    }

    bz_tensor_o* obj = bz_tensor_create(obj1->darr , obj1->dim , obj1->type);
    if(NULL == obj)
        return NULL;
    
    bz_uint total_count = obj->total_count;

#define DTYPE_HANDLE(dtype) \
do{ \
    dtype* obj_data = (dtype*)(obj->data); \
    dtype* obj1_data = (dtype*)(obj1->data); \
    dtype* obj2_data = (dtype*)(obj2->data); \
    for(bz_uint i = 0 ; i < total_count ; i++) \
        obj_data[i] = obj1_data[i] * obj2_data[i]; \
}while(0)

    if(BZ_INT_TYPE == obj->type)
        DTYPE_HANDLE(bz_int);
    else if(BZ_DOUBLE_TYPE == obj->type)
        DTYPE_HANDLE(bz_dbl);
    else
    {
        bz_errno = BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;
        bz_tensor_del(&obj); //obj会被置为NULL
    }

#undef DTYPE_HANDLE

    return obj;
}

bz_tensor_o* bz_tensor_kronecker_product(bz_tensor_o* obj1 , bz_tensor_o* obj2)
{
    if(NULL == obj1 || NULL == obj2)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }
    if(obj1->type != obj2->type)
    {
        bz_errno = BZ_MH_DIFF_DATA_TYPE_ERROR;
        return NULL;
    }
    if(2 != obj1->dim || 2 != obj2->dim)
    {
        bz_errno = BZ_MH_TENSOR_SHAPE_ERROR;
        return NULL;
    }

    bz_uint new_darr[2];
    new_darr[0] = obj1->darr[0] * obj2->darr[0];
    new_darr[1] = obj1->darr[1] * obj2->darr[1];

    bz_tensor_o* new_obj = bz_tensor_create(new_darr , 2 , obj1->type);
    if(NULL == new_obj)
        return NULL;

    bz_uint Row2 = obj2->darr[0];
    bz_uint Col1 = obj1->darr[1];
    bz_uint Col2 = obj2->darr[1];
    bz_uint temp;

#define DTYPE_HANDLE(dtype) \
do{ \
    dtype* data1 = (dtype*)(obj1->data); \
    dtype* data2 = (dtype*)(obj2->data); \
    dtype* new_data = (dtype*)(new_obj->data); \
    for(bz_uint i = 0 ; i < new_darr[0] ; i++) \
    { \
        temp = i * new_darr[1]; \
        for(bz_uint j = 0 ; j < new_darr[1] ; j++) \
            new_data[temp + j] = data1[i / Row2 * Col1 + j / Col2] * data2[i % Row2 * Col2 + j % Col2]; \
    } \
}while(0)

    if(BZ_INT_TYPE == new_obj->type)
        DTYPE_HANDLE(bz_int);
    else if(BZ_DOUBLE_TYPE == new_obj->type)
        DTYPE_HANDLE(bz_dbl);
    else
    {
        bz_errno = BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;
        bz_tensor_del(&new_obj);
        return NULL;
    }

#undef DTYPE_HANDLE

    return new_obj;
}

bz_tensor_o* bz_tensor_mul(bz_tensor_o* obj1 , bz_tensor_o* obj2)
{
    if(NULL == obj1 || NULL == obj2)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }
    if(obj1->type != obj2->type)
    {
        bz_errno = BZ_MH_DIFF_DATA_TYPE_ERROR;
        return NULL;
    }
    if(2 != obj1->dim || 2 != obj2->dim || obj1->darr[1] != obj2->darr[0])
    {
        bz_errno = BZ_MH_TENSOR_SHAPE_ERROR;
        return NULL;
    }

    bz_uint new_darr[2];
    new_darr[0] = obj1->darr[0];
    new_darr[1] = obj2->darr[1];
    bz_tensor_o* new_obj = bz_tensor_create(new_darr , 2 , obj1->type);
    if(NULL == new_obj)
        return NULL;
    
    bz_uint count = obj1->darr[1];
    bz_uint temp;
    bz_uint temp2;

#define DTYPE_HANDLE(dtype) \
do{ \
    dtype* new_data = (dtype*)(new_obj->data); \
    dtype* data1 = (dtype*)(obj1->data); \
    dtype* data2 = (dtype*)(obj2->data); \
    dtype value; \
    for(bz_uint i = 0 ; i < new_darr[0] ; i++) \
    { \
        temp = i * count; \
        temp2 = i * new_darr[1]; \
        for(bz_uint j = 0 ; j < new_darr[1] ; j++) \
        { \
            value = 0; \
            for(bz_uint k = 0 ; k < count ; k++) \
                value = value + data1[temp + k] * data2[k * new_darr[1] + j]; \
            new_data[temp2 + j] = value; \
        } \
    } \
}while(0)

    if(BZ_INT_TYPE == obj1->type)
        DTYPE_HANDLE(bz_int);
    else if(BZ_DOUBLE_TYPE == obj1->type)
        DTYPE_HANDLE(bz_dbl);
    else
    {
        bz_errno = BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;
        bz_tensor_del(&new_obj);
        return NULL;
    }

#undef DTYPE_HANDLE

    return new_obj;
}

bz_int bz_tensor_reshape(bz_tensor_o* obj , bz_uint* darr , bz_uint dim)
{
    //参数检查
    if(NULL == obj || NULL == darr || 0 == dim)
        return BZ_FUNC_PARAM_ERROR;

    bz_uint new_total_count = 1;
    for(bz_uint i = 0 ; i < dim ; i++)
    {
        if(0 == darr[i])
            return BZ_MH_TENSOR_DARR_NON_POS_VALUE_ERROR;
        else
            new_total_count *= darr[i];
    }
    if(new_total_count != obj->total_count)
        return BZ_MH_TENSOR_DATA_COUNT_ERROR;

    //分配并计算新的维度数组
    bz_uint* new_darr = (bz_uint*)bz_mem_alloc(dim * sizeof(bz_uint));
    if(NULL == new_darr)
        return BZ_MEM_ALLOC_ERROR;
    bz_uint* new_varr = (bz_uint*)bz_mem_alloc(dim * sizeof(bz_uint));
    if(NULL == new_varr)
    {
        bz_mem_free(new_darr);
        return BZ_MEM_ALLOC_ERROR;
    }

    bz_mem_free(obj->darr);
    bz_mem_free(obj->varr);
    obj->darr = new_darr;
    obj->varr = new_varr;
    obj->dim = dim;

    //darr赋值
    for(bz_uint i = 0 ; i < dim ; i++)
        obj->darr[i] = darr[i];
    //计算varr并赋值，相比正向遍历计算O(n^2)，该方法只需要O(n)
    obj->varr[dim - 1] = 1;
    if(dim > 1)
    {
        bz_uint i = dim - 2;
        while(1)
        {
            obj->varr[i] = obj->varr[i + 1] * obj->darr[i + 1];
            if(0 == i)
                break;
            else
                i--;
        }    
    }

    return 0;
}

bz_tensor_o* bz_tensor_pad(bz_tensor_o* obj , bz_uint pad , void* init_v)
{
    if(NULL == obj || 0 == pad || NULL == init_v)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }

    bz_int init_v_int = 0;
    bz_dbl init_v_dbl = 0;

    if(BZ_INT_TYPE == obj->type)
        init_v_int = *( (bz_int*)init_v );
    else if(BZ_DOUBLE_TYPE == obj->type)
        init_v_dbl = *( (bz_dbl*)init_v );
    else
    {
        bz_errno = BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;
        return NULL;
    }

    //计算新建张量的维度数组
    bz_uint* new_darr;
    bz_uint new_dim;
    if(1 == obj->dim)
        new_darr = (bz_uint*)bz_mem_alloc(2 * sizeof(bz_uint));
    else
        new_darr = (bz_uint*)bz_mem_alloc(obj->dim * sizeof(bz_uint));

    if(NULL == new_darr)
    {
        bz_errno = BZ_MEM_ALLOC_ERROR;
        return NULL;
    }
    else if(1 == obj->dim)
    {
        new_darr[0] = 2 * pad + 1;
        new_darr[1] = 2 * pad + obj->darr[0];
        new_dim = 2;
    }
    else
    {
        new_darr[0] = 2 * pad + obj->darr[0];
        new_darr[1] = 2 * pad + obj->darr[1];
        for(bz_uint i = 2 ; i < obj->dim ; i++)
            new_darr[i] = obj->darr[i];
        new_dim = obj->dim;
    }

    //创建新建张量
    bz_tensor_o* new_obj = bz_tensor_create(new_darr , new_dim , obj->type);
    if(NULL == new_obj)
    {
        bz_mem_free(new_darr);
        return NULL;
    }

    //张量一维化后 前段或后段 要填充的数据数量
    bz_uint se_count = pad * new_obj->varr[0];
    //张量一维化后 间隔部分 要填充的数据数量
    bz_uint gap_count = pad * new_obj->varr[1];
    //每过间隔新建张量需要从原张量拷贝的数据数量
    bz_uint data_count;
    if(1 == obj->dim) //一维张量按行视角
        data_count = obj->darr[0];
    else
        data_count = obj->varr[0];
    //原张量一维化数据索引、数据总数量
    bz_uint data_index = 0;
    bz_uint data_total_count = obj->total_count;
    //新建张量一维化数据索引
    bz_uint new_data_index = 0;

#define DTYPE_HANDLE(dtype,pad_v) \
do{ \
    dtype* new_data = (dtype*)(new_obj->data); \
    dtype* data = (dtype*)(obj->data); \
    /*前段填充*/ \
    for(bz_uint i = 0 ; i < se_count ; i++ , new_data_index++) \
        new_data[new_data_index] = pad_v; \
    /*中间填充*/ \
    while(data_index < data_total_count) \
    { \
        /*间隔填充*/ \
        for(bz_uint i = 0 ; i < gap_count ; i++ , new_data_index++) \
            new_data[new_data_index] = pad_v; \
        /*数据填充*/ \
        for(bz_uint i = 0 ; i < data_count ; i++ , new_data_index++ , data_index++) \
            new_data[new_data_index] = data[data_index]; \
        /*间隔填充*/ \
        for(bz_uint i = 0 ; i < gap_count ; i++ , new_data_index++) \
            new_data[new_data_index] = pad_v; \
    } \
    /*后段填充*/ \
    for(bz_uint i = 0 ; i < se_count ; i++ , new_data_index++) \
        new_data[new_data_index] = pad_v; \
}while(0)

    if(BZ_INT_TYPE == new_obj->type)
        DTYPE_HANDLE(bz_int,init_v_int);
    else if(BZ_DOUBLE_TYPE == new_obj->type)
        DTYPE_HANDLE(bz_dbl,init_v_dbl);
    else
    {
        //上面有对张量未处理类型进行判断，该部分不会被执行，仅用于规范统一，以及新增类型时，
        //方便根据 BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR 进行搜索
        bz_errno = BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;
        bz_tensor_del(&new_obj);
    }

#undef DTYPE_HANDLE

    bz_mem_free(new_darr);
    return new_obj;
}

/*
bz_tensor_slice 函数涉及 bz_uint 与 bz_int 型相互运算，关于无符号与有符号整型运算特点示例如下：
    unsigned i = 5;
	int j = -6;
	int k = 6;
	unsigned r1 = i + j;
	unsigned r2 = i + (unsigned)j;
	int r3 = i + j;
	int r4 = i + (unsigned)j;
	unsigned r5 = i - k;
	unsigned r6 = i + (unsigned)(-k);
	int r7 = i - k;
	int r8 = i + (unsigned)(-k);
	printf("r1:%u\nr2:%u\nr3:%d\nr4:%d\nr5:%u\nr6:%u\nr7:%d\nr8:%d\n" , 
		    r1 , r2 , r3 , r4 , r5 , r6 , r7 , r8);
	printf("sizeof(unsigned) is %d\nsizeof(int) is %d\n", sizeof(unsigned) , sizeof(int));
	printf("r2 memcmp r3 [sizeof(unsigned)]: %d\n" , memcmp(&r2 , &r3 , sizeof(unsigned)));
	printf("r2 memcmp r3 [sizeof(int)]: %d\n" , memcmp(&r2 , &r3 , sizeof(int)));
*/
bz_tensor_o* bz_tensor_slice(bz_tensor_o* obj , bz_mh_irange* range , bz_uint count)
{
    //参数检查
    if(NULL == obj || NULL == range || count != obj->dim)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }
    for(bz_uint i = 0 ; i < count ; i++)
    {
        if(range[i].start < 0 || (bz_uint)(range[i].start) >= obj->darr[i] 
            || range[i].end < 0 || (bz_uint)(range[i].end) >= obj->darr[i])
        {
            bz_errno = BZ_MH_OUT_OF_BOUNDS_ERROR;
            return NULL;
        }
    }

    //切片范围修正
    bz_mh_irange_corr(range , count);
    //计算新张量的各个维度
    bz_uint* new_darr = (bz_uint*)bz_mem_alloc(obj->dim * sizeof(bz_uint));
    if(NULL == new_darr)
    {
        bz_errno = BZ_MEM_ALLOC_ERROR;
        return NULL;
    }
    for(bz_uint i = 0 ; i < obj->dim ; i++)
        new_darr[i] = (bz_uint)((range[i].end - range[i].start) / range[i].step + 1);
    //创建一个新的张量
    bz_tensor_o* new_obj = bz_tensor_create(new_darr , obj->dim , obj->type);
    if(NULL == new_obj)
        goto BZ_TENSOR_SLICE_FREE_NEW_DARR;
    //复用 new_darr 数组，作为坐标数组
    bz_uint* coo = new_darr;
    //坐标数组初始化，并计算原张量数据一维化后的初始索引位置
    bz_uint data_idx = 0;
    for(bz_uint i = 0 ; i < obj->dim ; i++)
    {
        coo[i] = (bz_uint)(range[i].start);
        data_idx += coo[i] * obj->varr[i];
    }
    bz_int temp;
    bz_int diff;

#define DTYPE_HANDLE(dtype) \
do{ \
    dtype* data = (dtype*)(obj->data); \
    dtype* new_data = (dtype*)(new_obj->data); \
    for(bz_uint i = 0 ; i < new_obj->total_count ; i++) \
    { \
        new_data[i] = data[data_idx]; \
        /*开始步进，寻找下一个待复制的数据索引*/ \
        diff = 0; \
        for(bz_int j = (bz_int)(obj->dim - 1) ; j >= 0 ; j--) \
        { \
            temp = (bz_int)(coo[j]) + range[j].step; \
            if((range[j].step > 0 && (range[j].start <= temp && temp <= range[j].end)) /*可步进*/ \
                || (range[j].end <= temp && temp <= range[j].start)) \
            { \
                diff += range[j].step * (bz_int)(obj->varr[j]); \
                coo[j] = (bz_uint)temp; \
                break; \
            } \
            /*不可步进，前往前一维度寻找步进*/ \
            diff += (range[j].start - (bz_int)(coo[j])) * (bz_int)(obj->varr[j]); \
            coo[j] = (bz_uint)(range[j].start); \
        } \
        data_idx += (bz_uint)diff; \
    } \
}while(0)

    if(BZ_INT_TYPE == obj->type)
        DTYPE_HANDLE(bz_int);
    else if(BZ_DOUBLE_TYPE == obj->type)
        DTYPE_HANDLE(bz_dbl);
    else
    {
        bz_errno = BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;
        goto BZ_TENSOR_SLICE_DEL_NEW_OBJ;
    }

#undef DTYPE_HANDLE

    bz_mem_free(new_darr);
    return new_obj;
    
BZ_TENSOR_SLICE_DEL_NEW_OBJ:
    bz_tensor_del(&new_obj);
BZ_TENSOR_SLICE_FREE_NEW_DARR:
    bz_mem_free(new_darr);
    return NULL;
}

/*旧版切片函数，实现弃用
bz_tensor_o* bz_tensor_slice(bz_tensor_o* obj , bz_mh_irange* range , bz_uint count)
{
    //参数检查
    if(NULL == obj || NULL == range || count != obj->dim)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }
    for(bz_uint i = 0 ; i < count ; i++)
    {
        if(range[i].start < 0 || (bz_uint)(range[i].start) >= obj->darr[i] 
            || range[i].end < 0 || (bz_uint)(range[i].end) >= obj->darr[i])
        {
            bz_errno = BZ_MH_OUT_OF_BOUNDS_ERROR;
            return NULL;
        }
    }

    //切片范围修正
    bz_mh_irange_corr(range , count);
    //计算新张量的各个维度
    bz_uint* new_darr = (bz_uint*)bz_mem_alloc(obj->dim * sizeof(bz_uint));
    if(NULL == new_darr)
    {
        bz_errno = BZ_MEM_ALLOC_ERROR;
        return NULL;
    }
    for(bz_uint i = 0 ; i < obj->dim ; i++)
        new_darr[i] = (bz_uint)((range[i].end - range[i].start) / range[i].step + 1);
    //创建一个新的张量
    bz_tensor_o* new_obj = bz_tensor_create(new_darr , obj->dim , obj->type);
    if(NULL == new_obj)
        goto BZ_TENSOR_SLICE_FREE_NEW_DARR;
    //创建坐标数组，并将 new_darr 作为旧坐标数组
    bz_uint* old_coo = new_darr;
    bz_uint* new_coo = (bz_uint*)bz_mem_alloc(obj->dim * sizeof(bz_uint));
    if(NULL == new_coo)
    {
        bz_errno = BZ_MEM_ALLOC_ERROR;
        goto BZ_TENSOR_SLICE_DEL_NEW_OBJ;
    }
    
    //坐标数组初始化
    for(bz_uint i = 0 ; i < obj->dim ; i++)
    {
        old_coo[i] = (bz_uint)(range[i].start);
        new_coo[i] = (bz_uint)(range[i].start);
    }
    //原张量的数据索引
    bz_uint index = 0;
    for(bz_uint i = 0 ; i < obj->dim ; i++)
        index += old_coo[i] * obj->varr[i];
    //新建张量的数据索引
    bz_uint new_index = 0;
    //存放临时使用索引
    bz_uint temp_index;
    //存放坐标正负差值
    bz_uint pos_diff;
    bz_uint neg_diff;
    
    if(BZ_INT_TYPE == obj->type)
    {
        bz_int* new_data = (bz_int*)(new_obj->data);
        bz_int* data = (bz_int*)(obj->data);

        for(; new_index < new_obj->total_count ; new_index++)
        {
            new_data[new_index] = data[index];

            //从后往前，尝试对各维进行步进
            for(bz_uint i = 1 ; i <= obj->dim ; i++)
            {
                temp_index = obj->dim - i;
                //若该维可步进，跳出循环否则继续
                if(range[temp_index].step < 0 && new_coo[temp_index] >= (bz_uint)(range[temp_index].end - range[temp_index].step))
                    new_coo[temp_index] -= (bz_uint)(-range[temp_index].step);
                else if(range[temp_index].step > 0 && new_coo[temp_index] + (bz_uint)(range[temp_index].step) <= (bz_uint)(range[temp_index].end))
                    new_coo[temp_index] += (bz_uint)(range[temp_index].step);
                else
                    continue;
                
                break;
            }
            for(bz_uint i = temp_index + 1 ; i < obj->dim ; i++)
                new_coo[i] = (bz_uint)(range[i].start);
            //坐标作差
            pos_diff = 0;
            neg_diff = 0;
            for(bz_uint i = temp_index ; i < obj->dim ; i++)
            {
                if(new_coo[i] >= old_coo[i])
                    pos_diff += (new_coo[i] - old_coo[i]) * obj->varr[i];
                else
                    neg_diff += (old_coo[i] - new_coo[i]) * obj->varr[i];
            }
            index = index + pos_diff - neg_diff;
            //旧坐标跟进
            for(bz_uint i = temp_index ; i < obj->dim ; i++)
                old_coo[i] = new_coo[i];
        }
    }
    else if(BZ_DOUBLE_TYPE == obj->type)
    {
        bz_dbl* new_data = (bz_dbl*)(new_obj->data);
        bz_dbl* data = (bz_dbl*)(obj->data);

        for(; new_index < new_obj->total_count ; new_index++)
        {
            new_data[new_index] = data[index];

            //从后往前，尝试对各维进行步进
            for(bz_uint i = 1 ; i <= obj->dim ; i++)
            {
                temp_index = obj->dim - i;
                //若该维可步进，跳出循环否则继续
                if(range[temp_index].step < 0 && new_coo[temp_index] >= (bz_uint)(range[temp_index].end - range[temp_index].step))
                    new_coo[temp_index] -= (bz_uint)(-range[temp_index].step);
                else if(range[temp_index].step > 0 && new_coo[temp_index] + (bz_uint)(range[temp_index].step) <= (bz_uint)(range[temp_index].end))
                    new_coo[temp_index] += (bz_uint)(range[temp_index].step);
                else
                    continue;
                
                break;
            }
            for(bz_uint i = temp_index + 1 ; i < obj->dim ; i++)
                new_coo[i] = (bz_uint)(range[i].start);
            //坐标作差
            pos_diff = 0;
            neg_diff = 0;
            for(bz_uint i = temp_index ; i < obj->dim ; i++)
            {
                if(new_coo[i] >= old_coo[i])
                    pos_diff += (new_coo[i] - old_coo[i]) * obj->varr[i];
                else
                    neg_diff += (old_coo[i] - new_coo[i]) * obj->varr[i];
            }
            index = index + pos_diff - neg_diff;
            //旧坐标跟进
            for(bz_uint i = temp_index ; i < obj->dim ; i++)
                old_coo[i] = new_coo[i];
        }
    }
    else
    {
        bz_errno = BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;
        goto BZ_TENSOR_SLICE_FREE_NEW_COO;
    }

    bz_mem_free(new_darr);
    bz_mem_free(new_coo);
    return new_obj;

BZ_TENSOR_SLICE_FREE_NEW_COO:
    bz_mem_free(new_coo);
BZ_TENSOR_SLICE_DEL_NEW_OBJ:
    bz_tensor_del(&new_obj);
BZ_TENSOR_SLICE_FREE_NEW_DARR:
    bz_mem_free(new_darr);
    return NULL;
}
*/

bz_tensor_o* bz_tensor_T(bz_tensor_o* obj)
{
    if(NULL == obj)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }
    if(2 != obj->dim)
    {
        bz_errno = BZ_MH_TENSOR_SHAPE_ERROR;
        return NULL;
    }

    bz_tensor_o* new_obj = bz_tensor_create(obj->darr , obj->dim , obj->type);
    if(NULL == new_obj)
        return NULL;
    else
    {
        bz_uint temp = new_obj->darr[1];
        new_obj->darr[1] = new_obj->darr[0];
        new_obj->darr[0] = temp;
    }

    bz_uint r_count = obj->darr[0]; //原张量行数
    bz_uint c_count = obj->darr[1]; //原张量列数

    bz_uint temp;

#define DTYPE_HANDLE(dtype) \
do{ \
    dtype* data = (dtype*)(obj->data); \
    dtype* new_data = (dtype*)(new_obj->data); \
    for(bz_uint i = 0 ; i < r_count ; i++) \
    { \
        temp = i * c_count; \
        for(bz_uint j = 0 ; j < c_count ; j++) \
            new_data[j * r_count + i] = data[temp + j]; \
    } \
}while(0)

    if(BZ_INT_TYPE == obj->type)
        DTYPE_HANDLE(bz_int);
    else if(BZ_DOUBLE_TYPE == obj->type)
        DTYPE_HANDLE(bz_dbl);
    else
    {
        bz_errno = BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;
        bz_tensor_del(&new_obj);
        return NULL;
    }

#undef DTYPE_HANDLE

    return new_obj;
}

bz_tensor_o* bz_tensor_dimdiv8(bz_tensor_o* obj , bz_uint* div_arr , bz_uint arr_count , void* div_data , bz_uint data_count)
{
    if(NULL == obj || NULL == div_arr || 0 == arr_count)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }
    if(NULL != div_data && data_count != obj->total_count)
    {
        bz_errno = BZ_MH_TENSOR_DATA_COUNT_ERROR;
        return NULL;
    }
    //检查张量维度划分是否合规
    bz_uint temp_arr_count = obj->dim + 1;
    bz_uint* temp_arr = (bz_uint*)bz_mem_calloc(temp_arr_count , sizeof(bz_uint)); //初始化为 0
    if(NULL == temp_arr)
    {
        bz_errno = BZ_MEM_ALLOC_ERROR;
        return NULL;
    }
    else
    {
        //首尾不为 0
        if(0 == div_arr[0] || 0 == div_arr[arr_count - 1])
        {
            bz_errno = BZ_MH_TENSOR_DIM_DIV_ERROR;
            goto BZ_TENSOR_DIMDIV_FREE_TEMP_ARR;
        }
        
        bz_u8 fore_is_0 = false;
        for(bz_uint i = 0 ; i < arr_count ; i++)
        {
            //分隔仅单独的 0
            if(0 == div_arr[i])
            {
                if(fore_is_0)
                {
                    bz_errno = BZ_MH_TENSOR_DIM_DIV_ERROR;
                    goto BZ_TENSOR_DIMDIV_FREE_TEMP_ARR;
                }
                else
                    fore_is_0 = true;
            }
            else
                fore_is_0 = false;
            //维度不越界
            if(div_arr[i] > obj->dim)
            {
                bz_errno = BZ_MH_TENSOR_DIM_DIV_ERROR;
                goto BZ_TENSOR_DIMDIV_FREE_TEMP_ARR;
            }
            else
                temp_arr[div_arr[i]] += 1;
        }
        //每类维度仅出现一次
        for(bz_uint i = 1 ; i < temp_arr_count ; i++)
        {
            if(1 != temp_arr[i])
            {
                bz_errno = BZ_MH_TENSOR_DIM_DIV_ERROR;
                goto BZ_TENSOR_DIMDIV_FREE_TEMP_ARR;
            }
        } 
    }
    //seg_start、seg_end 每段划分在 div_arr 起止位置
    bz_uint new_dim = temp_arr[0] + 1;
    bz_uint* seg_start = (bz_uint*)bz_mem_alloc(new_dim * sizeof(bz_uint));
    if(NULL == seg_start)
    {
        bz_errno = BZ_MEM_ALLOC_ERROR;
        goto BZ_TENSOR_DIMDIV_FREE_TEMP_ARR;
    }
    bz_uint* seg_end = (bz_uint*)bz_mem_alloc(new_dim * sizeof(bz_uint));
    if(NULL == seg_end)
    {
        bz_errno = BZ_MEM_ALLOC_ERROR;
        goto BZ_TENSOR_DIMDIV_FREE_SEG_START;
    }
    bz_uint seg_idx = 0;
    seg_start[0] = 0;
    seg_end[new_dim - 1] = arr_count - 1;
    for(bz_uint i = 0 ; i < arr_count ; i++)
    {
        if(0 == div_arr[i])
        {
            seg_end[seg_idx] = i - 1;
            seg_start[seg_idx + 1] = i + 1;
            seg_idx++;
        }
    }
    //划分后的新张量，若是逆操作，则忽略
    bz_uint* new_darr = NULL;
    bz_tensor_o* new_obj = NULL;
    if(NULL == div_data)
    {
        new_darr = (bz_uint*)bz_mem_alloc(new_dim * sizeof(bz_uint));
        if(NULL == new_darr)
        {
            bz_errno = BZ_MEM_ALLOC_ERROR;
            goto BZ_TENSOR_DIMDIV_FREE_SEG_END;
        }
        for(bz_uint i = 0 ; i < new_dim ; i++)
        {
            new_darr[i] = 1;
            for(bz_uint j = seg_start[i] ; j <= seg_end[i] ; j++)
                new_darr[i] *= obj->darr[div_arr[j] - 1];
        }
        new_obj = bz_tensor_create(new_darr , new_dim , obj->type);
        if(NULL == new_obj)
            goto BZ_TENSOR_DIMDIV_FREE_NEW_DARR;
    }

    bz_uint* coo = temp_arr; //复用 temp_arr 数组
    for(bz_uint i = 0 ; i < temp_arr_count ; i++)
        coo[i] = 0;
    
    bz_uint neg_diff;
    seg_idx = new_dim - 1;
    bz_uint div_arr_idx = seg_start[seg_idx];
    bz_uint data_idx = 0;
    bz_uint dim_idx = div_arr[div_arr_idx] - 1; //取出当前步进的维度索引，并转为从 0 开始计数的索引序号

#define DTYPE_HANDLE(dtype) \
do{ \
    dtype* data = (dtype*)(obj->data); \
    dtype* new_data = NULL; \
    if(NULL == div_data) \
        new_data = (dtype*)(new_obj->data); \
    else \
        new_data = (dtype*)(div_data); \
 \
    for(bz_uint i = 0 ; i < obj->total_count ; i++) \
    { \
        if(NULL == div_data) \
            new_data[i] = data[data_idx]; \
        else \
            data[data_idx] = new_data[i]; \
 \
        if(coo[dim_idx] + 1 < obj->darr[dim_idx]) /*若当前维度可步进*/ \
        { \
            data_idx += obj->varr[dim_idx]; \
            coo[dim_idx] += 1; \
            continue; \
        } \
        /*当前维度无法步进*/ \
        neg_diff = 0; \
        for(;;) /*当前分段内从前往后寻找可步进维度*/ \
        { \
            neg_diff += coo[dim_idx] * obj->varr[dim_idx]; \
            coo[dim_idx] = 0; \
            div_arr_idx++; \
            if(div_arr_idx > seg_end[seg_idx]) /*分段内维度都无法步进，前往前一个分段*/ \
            { \
                if(0 == seg_idx) /*所有分段维度全部步进完毕*/ \
                    break; \
                seg_idx--; \
                div_arr_idx = seg_start[seg_idx]; \
            } \
 \
            dim_idx = div_arr[div_arr_idx] - 1; \
            if(coo[dim_idx] + 1 < obj->darr[dim_idx]) /*找到可步进维度*/ \
            { \
                data_idx += obj->varr[dim_idx] - neg_diff; \
                coo[dim_idx] += 1; \
                /*回到初始*/  \
                seg_idx = new_dim - 1; \
                div_arr_idx = seg_start[seg_idx]; \
                dim_idx = div_arr[div_arr_idx] - 1; \
                break; \
            } \
        } \
    } \
}while(0)

    if(BZ_INT_TYPE == obj->type)
        DTYPE_HANDLE(bz_int);
    else if(BZ_DOUBLE_TYPE == obj->type)
        DTYPE_HANDLE(bz_dbl);
    else
    {
        bz_errno = BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;
        goto BZ_TENSOR_DIMDIV_DEL_NEW_OBJ;
    }

#undef DTYPE_HANDLE

    bz_mem_free(new_darr);
    bz_mem_free(seg_end);
    bz_mem_free(seg_start);
    bz_mem_free(temp_arr);

    if(NULL == div_data)
        return new_obj;
    else
        return obj;

BZ_TENSOR_DIMDIV_DEL_NEW_OBJ:
    bz_tensor_del(&new_obj);
BZ_TENSOR_DIMDIV_FREE_NEW_DARR:
    bz_mem_free(new_darr);
BZ_TENSOR_DIMDIV_FREE_SEG_END:
    bz_mem_free(seg_end);
BZ_TENSOR_DIMDIV_FREE_SEG_START:
    bz_mem_free(seg_start);
BZ_TENSOR_DIMDIV_FREE_TEMP_ARR:
    bz_mem_free(temp_arr);
    return NULL;
}

bz_tensor_o* bz_tensor_mode_n_product(bz_tensor_o* obj1 , bz_tensor_o* obj2 , bz_uint n)
{
    if(NULL == obj1 || NULL == obj2 || 0 == n || n > obj1->dim)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }
    if(obj1->dim < 2 || 2 != obj2->dim)
    {
        bz_errno = BZ_MH_TENSOR_SHAPE_ERROR;
        return NULL;
    }

    bz_uint arr_count = obj1->dim + 1;
    bz_uint* mode_arr = (bz_uint*)bz_mem_alloc(arr_count * sizeof(bz_uint));
    if(NULL == mode_arr)
    {
        bz_errno = BZ_MEM_ALLOC_ERROR;
        return NULL;
    }
    mode_arr[0] = n;
    for(bz_uint i = 1 ; i <= n ; i++)
        mode_arr[i] = i - 1;
    for(bz_uint i = n + 1 ; i < arr_count ; i++)
        mode_arr[i] = i;

    bz_tensor_o* mode_n = bz_tensor_dimdiv8(obj1 , mode_arr , arr_count , NULL , 0);
    if(NULL == mode_n)
        goto BZ_TENSOR_MODE_N_PRODUCT_FREE_MODE_ARR;

    bz_tensor_o* rmat = bz_tensor_mul(obj2 , mode_n);
    if(NULL == rmat)
        goto BZ_TENSOR_MODE_N_PRODUCT_DEL_MODE_N;

    bz_uint* rdarr = (bz_uint*)bz_mem_alloc(obj1->dim * sizeof(bz_uint));
    if(NULL == rdarr)
        goto BZ_TENSOR_MODE_N_PRODUCT_DEL_RMAT;
    for(bz_uint i = 0 ; i < obj1->dim ; i++)
        rdarr[i] = obj1->darr[i];
    rdarr[n - 1] = obj2->darr[0];

    bz_tensor_o* rtensor = bz_tensor_create(rdarr , obj1->dim , obj1->type);
    if(NULL == rtensor)
        goto BZ_TENSOR_MODE_N_PRODUCT_FREE_RDARR;

    if(NULL == bz_tensor_dimdiv8(rtensor , mode_arr , arr_count , rmat->data , rmat->total_count))
        goto BZ_TENSOR_MODE_N_PRODUCT_DEL_RTENSOR;

    bz_mem_free(rdarr);
    bz_tensor_del(&rmat);
    bz_tensor_del(&mode_n);
    bz_mem_free(mode_arr);
    return rtensor;

BZ_TENSOR_MODE_N_PRODUCT_DEL_RTENSOR:
    bz_tensor_del(&rtensor);
BZ_TENSOR_MODE_N_PRODUCT_FREE_RDARR:
    bz_mem_free(rdarr);
BZ_TENSOR_MODE_N_PRODUCT_DEL_RMAT:
    bz_tensor_del(&rmat);
BZ_TENSOR_MODE_N_PRODUCT_DEL_MODE_N:
    bz_tensor_del(&mode_n);
BZ_TENSOR_MODE_N_PRODUCT_FREE_MODE_ARR:
    bz_mem_free(mode_arr);
    return NULL;
}

bz_int bz_tensor_retype(bz_tensor_o* obj , bz_mh_dtype type)
{
    if(NULL == obj)
        return BZ_FUNC_PARAM_ERROR;
    if(type == obj->type)
        return 0;

#define DTYPE_HANDLE(dtype,stype,DTYPE) \
do{ \
    bz_uint count = obj->total_count; \
    stype* data = (stype*)(obj->data);\
    dtype* new_data = (dtype*)bz_mem_alloc(count * sizeof(dtype)); \
    if(NULL == new_data) \
        return BZ_MEM_ALLOC_ERROR; \
    for(bz_uint i = 0 ; i < count ; i++) \
        new_data[i] = (dtype)(data[i]); \
    bz_mem_free(obj->data); \
    obj->data = new_data; \
    obj->type = DTYPE; \
}while(0)

    if(BZ_INT_TYPE == obj->type)
    {
        switch(type)
        {
        case BZ_DOUBLE_TYPE:
            DTYPE_HANDLE(bz_dbl,bz_int,BZ_DOUBLE_TYPE);
            break;
        default:
            return BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;
        }
    }
    else if(BZ_DOUBLE_TYPE == obj->type)
    {
        switch(type)
        {
        case BZ_INT_TYPE:
            DTYPE_HANDLE(bz_int,bz_dbl,BZ_INT_TYPE);
            break;
        default:
            return BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;
        }
    }
    else
        return BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;

#undef DTYPE_HANDLE

    return 0;
}

bz_tensor_o* bz_tensor_copy(bz_tensor_o* obj)
{
    if(NULL == obj)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }

    bz_tensor_o* cp = bz_tensor_create(obj->darr , obj->dim , obj->type);
    if(NULL == cp)
        return NULL;

#define DTYPE_HANDLE(dtype) \
do{ \
    dtype* data = (dtype*)(obj->data); \
    dtype* cp_data = (dtype*)(cp->data); \
    bz_uint count = obj->total_count; \
    for(bz_uint i = 0 ; i < count ; i++) \
        cp_data[i] = data[i]; \
}while(0)

    if(BZ_INT_TYPE == obj->type)
        DTYPE_HANDLE(bz_int);
    else if (BZ_DOUBLE_TYPE == obj->type)
        DTYPE_HANDLE(bz_dbl);
    else
    {
        bz_errno = BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;
        bz_tensor_del(&cp);
        return NULL;
    }

#undef DTYPE_HANDLE

    return cp;
}

bz_tensor_o* bz_tensor_eig(bz_tensor_o* obj)
{
    if(NULL == obj)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }
    if(2 != obj->dim || obj->darr[0] != obj->darr[1] || 1 == obj->total_count)
    {
        bz_errno = BZ_MH_TENSOR_SHAPE_ERROR;
        return NULL;
    }

#define JACOBI_ALGORITHM

#ifdef JACOBI_ALGORITHM
    //迭代方阵
    bz_tensor_o* A = bz_tensor_copy(obj);
    if(NULL == A)
        return NULL;
    bz_int err = bz_tensor_retype(A , BZ_DOUBLE_TYPE);
    if(0 != err)
    {
        bz_errno = err;
        goto BZ_TENSOR_EIG_DEL_A;
    }

    //形状
    bz_uint N = A->darr[0];

    //Givens 变换矩阵 R，仅存储关键元素
    bz_dbl rii , rij , rji , rjj;

    //特征向量 Q 矩阵，返回矩阵
    bz_uint dim = 2;
    bz_uint darr[2];
    darr[0] = N + 1; //多预留一行，函数返回时，用于存放特征值
    darr[1] = N;
    bz_tensor_o* Q = bz_tensor_create(darr , dim , BZ_DOUBLE_TYPE);
    if(NULL == Q)
        goto BZ_TENSOR_EIG_DEL_A;
    
    //初始化
    bz_dbl* q = (bz_dbl*)(Q->data);
    for(bz_uint i = 0 ; i < Q->total_count ; i++)
        q[i] = 0;
    for(bz_uint i = 0 ; i < N ; i++)
        q[i * N + i] = 1;

    //迭代求解
    bz_dbl* a = (bz_dbl*)(A->data);
    bz_dbl* ai_;
    bz_dbl* aj_;
    bz_dbl* a_i;
    bz_dbl* a_j;
    bz_dbl* q_i;
    bz_dbl* q_j;
    bz_dbl d , t; //会被复用
    bz_uint h , i , j , max_i , max_j;
    bz_uint count = N * N;
    for(h = 0 ; h < count ; h++)
    {
        //找出绝对值最大非对角元素
        d = a[1] >= 0 ? a[1] : -a[1];
        max_i = 0;
        max_j = 1;
        for(i = 0 ; i < N ; i++)
        {
            ai_ = a + i * N;
            for(j = i + 1 ; j < N ; j++)
            {
                t = ai_[j] >= 0 ? ai_[j] : -ai_[j];
                if(t > d)
                {
                    max_i = i;
                    max_j = j;
                    d = t;
                }
            }
        }
        i = max_i;
        j = max_j;
        if(-BZ_FPP <= d && d <= BZ_FPP) //最大非对角元素已经接近 0，迭代完成
            break;
        //Jacobi 计算
        ai_ = a + i * N;
        aj_ = a + j * N;
        a_i = a + i;
        a_j = a + j;
        q_i = q + i;
        q_j = q + j;
        //计算矩阵 R
        d = (ai_[i] - aj_[j]) / (2 * ai_[j]);
        if(d >= 0)
            t = -d + bz_mh_sqrt(1 + d * d);
        else
            t = -d - bz_mh_sqrt(1 + d * d);
        rii = 1 / bz_mh_sqrt(1 + t * t);
        rij = t * rii;
        rjj = rii;
        rji = -rij;
        //计算 A 左乘 R
        for(bz_uint k = 0 ; k < N ; k++)
        {
            t = ai_[k];
            ai_[k] = rii * ai_[k] + rij * aj_[k];
            aj_[k] = rji * t + rjj * aj_[k];
        }
        //计算 A 右乘 转置R 与 Q 右乘 转置R
        for(bz_uint k = 0 ; k < N ; k++)
        {
            t = *a_i;
            *a_i = (*a_i) * rii + (*a_j) * rij;
            *a_j = t * rji + (*a_j) * rjj;
            a_i = a_i + N;
            a_j = a_j + N;

            t = *q_i;
            *q_i = (*q_i) * rii + (*q_j) * rij;
            *q_j = t * rji + (*q_j) * rjj;
            q_i = q_i + N;
            q_j = q_j + N;
        }
    }

    //将特征值存入 Q 最后一行
    q = q + N * N;
    for(bz_uint i = 0 ; i < N ; i++)
        q[i] = a[i * N + i];

    bz_tensor_del(&A);
    return Q;

BZ_TENSOR_EIG_DEL_A:
    bz_tensor_del(&A);
    return NULL;
#undef JACOBI_ALGORITHM
#endif

    return NULL;
}

void bz_tensor_svd_auxfunc_sort(bz_dbl* dbl_data , bz_uint* idx_arr , bz_uint count)
{
    bz_dbl dbl_temp;
	bz_uint idx_temp;
    for(bz_uint end = count - 1 ; end > 0 ; end--)
    {
        bz_uint nonleaf = (end - 1) / 2; //非叶子节点
        for(; ; nonleaf--)
        {
            //对该非叶子节点进行调整
            bz_uint index = nonleaf;
            bz_uint left = 2 * index + 1;
            bz_uint right = left + 1;
            bz_uint child_winner;
            while (left <= end)
            {
                //左右子节点大小比较
                if(right <= end) //如果存在右孩子
                {
                    if(dbl_data[left] <= dbl_data[right])
                        child_winner = left;
                    else
                        child_winner = right;
                }
                else //不存在右孩子
                    child_winner = left;
                //父节点与子节点大小比较
                if(dbl_data[index] <= dbl_data[child_winner])
                    break;
                else
                {
                    dbl_temp = dbl_data[child_winner];
                    dbl_data[child_winner] = dbl_data[index];
                    dbl_data[index] = dbl_temp;

					idx_temp = idx_arr[child_winner];
                    idx_arr[child_winner] = idx_arr[index];
                    idx_arr[index] = idx_temp;

                    index = child_winner;
                    left = 2 * index + 1;
                    right = left + 1;
                }
            }

            if(0 == nonleaf)
                break;
        }
        dbl_temp = dbl_data[end];
        dbl_data[end] = dbl_data[0];
        dbl_data[0] = dbl_temp;

		idx_temp = idx_arr[end];
        idx_arr[end] = idx_arr[0];
        idx_arr[0] = idx_temp;
    }
}

bz_tensor_o** bz_tensor_svd(bz_tensor_o* obj)
{
    if(NULL == obj)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }
    if(2 != obj->dim || 1 == obj->darr[0] || 1 == obj->darr[1])
    {
        bz_errno = BZ_MH_TENSOR_SHAPE_ERROR;
        return NULL;
    }

    bz_tensor_o** ret = (bz_tensor_o**)bz_mem_alloc(3 * sizeof(bz_tensor_o*));
    if(NULL == ret)
    {
        bz_errno = BZ_MEM_ALLOC_ERROR;
        return NULL;
    }

    bz_tensor_o* A = obj;
    bz_uint m = A->darr[0];
    bz_uint n = A->darr[1];

    bz_tensor_o* A_T = bz_tensor_T(A);
    if(NULL == A_T)
        goto BZ_TENSOR_SVD_FREE_RET;

    //计算 U 矩阵
    bz_tensor_o* AxA_T = bz_tensor_mul(A , A_T);
    if(NULL == AxA_T)
        goto BZ_TENSOR_SVD_DEL_A_T;
    bz_tensor_o* U_unsort = bz_tensor_eig(AxA_T);
    if(NULL == U_unsort)
        goto BZ_TENSOR_SVD_DEL_AxA_T;

    //对附带特征值排序，U_unsort 形状 (m + 1) * m
    bz_uint* idx_arr = (bz_uint*)bz_mem_alloc(m * sizeof(bz_uint));
    if(NULL == idx_arr)
    {
        bz_errno = BZ_MEM_ALLOC_ERROR;
        goto BZ_TENSOR_SVD_DEL_U_UNSORT;
    }
    for(bz_uint i = 0 ; i < m ; i++)
        idx_arr[i] = i;
    //对 U_unsort 所附带的特征值进行排序
    bz_dbl* src = (bz_dbl*)(U_unsort->data) + m * m;
    bz_tensor_svd_auxfunc_sort(src , idx_arr , m);

    //计算截断位置
    bz_uint r = 0;
    for(bz_uint i = 0 ; i < m ; i++)
    {
        if(-BZ_FPP <= src[i] && src[i] <= BZ_FPP)
            break;
        else
            r++;
    }
    r = (0 == r ? 1 : r); // r 为 0，函数可能接收了零矩阵，则至少预留一个位置

    //创建左奇异矩阵 U
    bz_uint darr[2];
    darr[0] = m;
    darr[1] = r;
    bz_tensor_o* U = bz_tensor_create(darr , 2 , U_unsort->type);
    if(NULL == U)
        goto BZ_TENSOR_SVD_FREE_IDX_ARR;
    
    //创建对角阵 diag
    darr[0] = r;
    bz_tensor_o* diag = bz_tensor_create(darr , 2 , U_unsort->type);
    if(NULL == diag)
        goto BZ_TENSOR_SVD_DEL_U;

    //将 U_unsort 中按 idx_arr 索引顺序排序的前 r 个特征向量复制到 U
    bz_dbl* des = (bz_dbl*)(U->data);
    src = (bz_dbl*)(U_unsort->data);
    bz_uint src_idx , des_idx;
    for(bz_uint i = 0 ; i < r ; i++)
    {
        des_idx = i;
        src_idx = idx_arr[i];
        for(bz_uint j = 0 ; j < m ; j++)
        {
            des[des_idx] = src[src_idx];
            des_idx += r;
            src_idx += m;
        }
    }

    //复制特征值到 diag
    des = (bz_dbl*)(diag->data);
    src = (bz_dbl*)(U_unsort->data) + m * m;
    for(bz_uint i = 0 ; i < r * r ; i++)
        des[i] = 0;
    for(bz_uint i = 0 ; i < r ; i++)
        des[i * r + i] = bz_mh_sqrt(src[i] < 0.0 ? -src[i] : src[i]); // svd 特征值理应非负，但为防止 eig 迭代未优，仍然求绝对值

    //计算 V
    bz_int err = bz_tensor_retype(A_T , U->type);
    if(0 != err)
    {
        bz_errno = err;
        goto BZ_TENSOR_SVD_DEL_DIAG;
    }
    bz_tensor_o* V = bz_tensor_mul(A_T , U);
    if(NULL == V)
        goto BZ_TENSOR_SVD_DEL_DIAG;
    src = (bz_dbl*)(diag->data);
    bz_dbl sv; //奇异值
    for(bz_uint j = 0 ; j < r ; j++)
    {
        sv = src[j * r + j];
        des = (bz_dbl*)(V->data) + j;
        for(bz_uint i = 0 ; i < n ; i++)
        {
            (*des) /= sv;
            des += r;
        }
    }

    bz_mem_free(idx_arr);
    bz_tensor_del(&U_unsort);
    bz_tensor_del(&AxA_T);
    bz_tensor_del(&A_T);

    ret[0] = U;
    ret[1] = diag;
    ret[2] = V;

    return ret;

BZ_TENSOR_SVD_DEL_DIAG:
    bz_tensor_del(&diag);
BZ_TENSOR_SVD_DEL_U:
    bz_tensor_del(&U);
BZ_TENSOR_SVD_FREE_IDX_ARR:
    bz_mem_free(idx_arr);
BZ_TENSOR_SVD_DEL_U_UNSORT:
    bz_tensor_del(&U_unsort);
BZ_TENSOR_SVD_DEL_AxA_T:
    bz_tensor_del(&AxA_T);
BZ_TENSOR_SVD_DEL_A_T:
    bz_tensor_del(&A_T);
BZ_TENSOR_SVD_FREE_RET:
    bz_mem_free(ret);
    return NULL;
}

bz_tensor_o** bz_tensor_svd_trunc(bz_tensor_o* obj , bz_dbl tr)
{
    if(NULL == obj)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }

    bz_tensor_o** ret = bz_tensor_svd(obj);
    if(NULL == ret)
        return NULL;

    bz_tensor_o* diag = ret[1];
    bz_uint N = diag->darr[0];
    bz_uint r = 0;
    bz_dbl* src = (bz_dbl*)(diag->data);
    for(bz_uint i = 0 ; i < N ; i++)
    {
        if(src[i * N + i] >= src[0] * tr)
            r++;
        else
            break;
    }
    if(0 == r)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        goto BZ_TENSOR_SVD_TRUNC_FREE_RET;
    }
    if(N == r)
        return ret;

    //截断 对角阵
    bz_uint darr[2];
    darr[0] = r;
    darr[1] = r;
    diag = bz_tensor_create(darr , 2 , BZ_DOUBLE_TYPE);
    if(NULL == diag)
        goto BZ_TENSOR_SVD_TRUNC_FREE_RET;
    bz_dbl* des = (bz_dbl*)(diag->data);
    for(bz_uint i = 0 ; i < diag->total_count ; i++)
        des[i] = 0;
    for(bz_uint i = 0 ; i < r ; i++)
        des[i * r + i] = src[i * N + i];

    //截断 U
    darr[0] = ret[0]->darr[0];
    bz_tensor_o* U = bz_tensor_create(darr , 2 , BZ_DOUBLE_TYPE);
    if(NULL == U)
        goto BZ_TENSOR_SVD_TRUNC_DEL_DIAG;
    src = (bz_dbl*)(ret[0]->data);
    des = (bz_dbl*)(U->data);
    bz_uint row = darr[0];
    for(bz_uint i = 0 ; i < row ; i++)
    {
        for(bz_uint j = 0 ; j < r ; j++)
            des[j] = src[j];
        src += N;
        des += r;
    }

    //截断 V
    darr[0] = ret[2]->darr[0];
    bz_tensor_o* V = bz_tensor_create(darr , 2 , BZ_DOUBLE_TYPE);
    if(NULL == V)
        goto BZ_TENSOR_SVD_TRUNC_DEL_U;
    src = (bz_dbl*)(ret[2]->data);
    des = (bz_dbl*)(V->data);
    row = darr[0];
    for(bz_uint i = 0 ; i < row ; i++)
    {
        for(bz_uint j = 0 ; j < r ; j++)
            des[j] = src[j];
        src += N;
        des += r;
    }

    bz_tensor_del(ret + 2);
    bz_tensor_del(ret + 1);
    bz_tensor_del(ret + 0);
    ret[2] = V;
    ret[1] = diag;
    ret[0] = U;
    return ret;

BZ_TENSOR_SVD_TRUNC_DEL_U:
    bz_tensor_del(&U);
BZ_TENSOR_SVD_TRUNC_DEL_DIAG:
    bz_tensor_del(&diag);
BZ_TENSOR_SVD_TRUNC_FREE_RET:
    bz_tensor_del(ret + 2);
    bz_tensor_del(ret + 1);
    bz_tensor_del(ret + 0);
    bz_mem_free(ret);
    return NULL;
}

bz_tensor_o** bz_tensor_mps(bz_tensor_o* obj , bz_dbl tr , bz_uint* count_p)
{
    if(NULL == obj || NULL == count_p)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }

    //计算分解次数
    bz_uint data_count = obj->total_count;
    *count_p = 0;
    for(;;)
    {
        if(!(data_count & 1U))
        {
            (*count_p)++;
            data_count = data_count >> 1;
        }
        else
            break;  
    }
    if(0 == (*count_p) || 1 != data_count)
    {
        bz_errno = BZ_MH_TENSOR_DATA_COUNT_ERROR;
        return NULL;
    }

    bz_tensor_o** ret = (bz_tensor_o**)bz_mem_alloc((*count_p) * sizeof(bz_tensor_o*));
    if(NULL == ret)
    {
        bz_errno = BZ_MEM_ALLOC_ERROR;
        return NULL;
    }
    for(bz_uint i = 0 ; i < (*count_p) ; i++)
        ret[i] = NULL;

    bz_tensor_o* tsr = bz_tensor_copy(obj);
    if(NULL == tsr)
        goto BZ_TENSOR_MPS_FREE_RET;
    bz_int r = bz_tensor_retype(tsr , BZ_DOUBLE_TYPE);
    if(0 != r)
    {
        bz_errno = r;
        goto BZ_TENSOR_MPS_DEL_TSR;
    }
    bz_uint darr[2];
    darr[0] = 1;
    darr[1] = tsr->total_count;
    r = bz_tensor_reshape(tsr , darr , 2);
    if(0 != r)
    {
        bz_errno = r;
        goto BZ_TENSOR_MPS_DEL_TSR;
    }

    //分解
    bz_tensor_o* tsr_split = NULL;
    bz_dbl* src = NULL;
    bz_dbl* des = NULL;
    bz_tensor_o** svd_ret = NULL;
    bz_tensor_o* V_T;
    for(bz_uint i = 0 ; ; i++)
    {
        darr[0] *= 2;
        darr[1] /= 2;
        tsr_split = bz_tensor_create(darr , 2 , BZ_DOUBLE_TYPE);
        if(NULL == tsr_split)
            goto BZ_TENSOR_MPS_DEL_TSR;
        
        src = (bz_dbl*)(tsr->data);
        des = (bz_dbl*)(tsr_split->data);
        for(bz_uint j = 0 ; j < darr[0] ; j++)
        {
            for(bz_uint k = 0 ; k < darr[1] ; k++)
                des[k] = src[k];

            if(j == darr[0] / 2 - 1)
                src = (bz_dbl*)(tsr->data) + darr[1];
            else
                src += 2 * darr[1];

            des += darr[1];
        }

        bz_tensor_del(&tsr);

        if(1 == darr[1])
        {
            ret[i] = tsr_split;
            break;
        }

        svd_ret = bz_tensor_svd_trunc(tsr_split , tr);
        bz_tensor_del(&tsr_split);
        if(NULL == svd_ret)
            goto BZ_TENSOR_MPS_FREE_RET;

        ret[i] = svd_ret[0];

        V_T = bz_tensor_T(svd_ret[2]);
        tsr = bz_tensor_mul(svd_ret[1] , V_T);
        bz_tensor_del(&V_T);
        bz_tensor_del(svd_ret + 1);
        bz_tensor_del(svd_ret + 2);
        bz_mem_free(svd_ret);
        if(NULL == tsr)
            goto BZ_TENSOR_MPS_FREE_RET;
        darr[0] = tsr->darr[0];
        darr[1] = tsr->darr[1];
    }
    
    return ret;

BZ_TENSOR_MPS_DEL_TSR:
    bz_tensor_del(&tsr);
BZ_TENSOR_MPS_FREE_RET:
    for(bz_uint i = 0 ; i < (*count_p) ; i++)
        bz_tensor_del(ret + i);
    bz_mem_free(ret);
    *count_p = 0;
    return NULL;
}

bz_dbl bz_tensor_norm_f(bz_tensor_o* obj)
{
    if(NULL == obj)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return -1.0;
    }

    bz_dbl norm = 0;

#define DTYPE_HANDLE(dtype) \
do{ \
    dtype* data = (dtype*)(obj->data); \
    for(bz_uint i = 0 ; i < obj->total_count ; i++) \
        norm += (bz_dbl)(data[i] * data[i]); \
}while(0)

    if(BZ_INT_TYPE == obj->type)
        DTYPE_HANDLE(bz_int);
    else if(BZ_DOUBLE_TYPE == obj->type)
        DTYPE_HANDLE(bz_dbl);
    else
    {
        bz_errno = BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;
        return -1.0;
    }

#undef DTYPE_HANDLE

    return bz_mh_sqrt(norm);
}

bz_tensor_o* bz_tensor_add(bz_tensor_o* obj1 , bz_tensor_o* obj2)
{
    if(NULL == obj1 || NULL == obj2)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }

    if(obj1->total_count != obj2->total_count)
    {
        bz_errno = BZ_MH_TENSOR_DATA_COUNT_ERROR;
        return NULL;
    }

    if(obj1->type != obj2->type)
    {
        bz_errno = BZ_MH_DIFF_DATA_TYPE_ERROR;
        return NULL;
    }

    bz_tensor_o* new_obj = bz_tensor_create(obj1->darr , obj1->dim , obj1->type);
    if(NULL == new_obj)
        return NULL;

#define DTYPE_HANDLE(dtype) \
do{ \
    dtype* data1 = (dtype*)(obj1->data); \
    dtype* data2 = (dtype*)(obj2->data); \
    dtype* new_data = (dtype*)(new_obj->data); \
    for(bz_uint i = 0 ; i < new_obj->total_count ; i++) \
        new_data[i] = data1[i] + data2[i]; \
}while(0)

    if(BZ_INT_TYPE == new_obj->type)
        DTYPE_HANDLE(bz_int);
    else if(BZ_DOUBLE_TYPE == new_obj->type)
        DTYPE_HANDLE(bz_dbl);
    else
    {
        bz_errno = BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;
        bz_tensor_del(&new_obj);
        return NULL;
    }

#undef DTYPE_HANDLE

    return new_obj;
}

bz_tensor_o* bz_tensor_dec(bz_tensor_o* obj1 , bz_tensor_o* obj2)
{
    if(NULL == obj1 || NULL == obj2)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }

    if(obj1->total_count != obj2->total_count)
    {
        bz_errno = BZ_MH_TENSOR_DATA_COUNT_ERROR;
        return NULL;
    }

    if(obj1->type != obj2->type)
    {
        bz_errno = BZ_MH_DIFF_DATA_TYPE_ERROR;
        return NULL;
    }

    bz_tensor_o* new_obj = bz_tensor_create(obj1->darr , obj1->dim , obj1->type);
    if(NULL == new_obj)
        return NULL;

#define DTYPE_HANDLE(dtype) \
do{ \
    dtype* data1 = (dtype*)(obj1->data); \
    dtype* data2 = (dtype*)(obj2->data); \
    dtype* new_data = (dtype*)(new_obj->data); \
    for(bz_uint i = 0 ; i < new_obj->total_count ; i++) \
        new_data[i] = data1[i] - data2[i]; \
}while(0)

    if(BZ_INT_TYPE == new_obj->type)
        DTYPE_HANDLE(bz_int);
    else if(BZ_DOUBLE_TYPE == new_obj->type)
        DTYPE_HANDLE(bz_dbl);
    else
    {
        bz_errno = BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;
        bz_tensor_del(&new_obj);
        return NULL;
    }

#undef DTYPE_HANDLE

    return new_obj;
}

bz_tensor_o** bz_tensor_tucker(bz_tensor_o* obj , bz_dbl tr)
{
    if(NULL == obj || tr < 0.0 || tr > 1.0)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }
    if(1 == obj->dim)
    {
        bz_errno = BZ_MH_TENSOR_SHAPE_ERROR;
        return NULL;
    }
    for(bz_uint i = 0 ; i < obj->dim ; i++)
    {
        if(1 == obj->darr[i])
        {
            bz_errno = BZ_MH_TENSOR_SHAPE_ERROR;
            return NULL;
        }
    }
    
    bz_uint ndim = obj->dim;
    bz_uint arr_count = ndim + 1;
    //创建 模n 变换数组
    bz_uint* mode_arr = (bz_uint*)bz_mem_alloc(arr_count * sizeof(bz_uint));
    if(NULL == mode_arr)
    {
        bz_errno = BZ_MEM_ALLOC_ERROR;
        return NULL;
    }
    mode_arr[0] = 1;
    mode_arr[1] = 0;
    for(bz_uint i = 2 ; i < arr_count ; i++)
        mode_arr[i] = i;
    //创建 因子（核心张量、矩阵因子） 数组，首元素放核心张量
    bz_tensor_o** factor = (bz_tensor_o**)bz_mem_alloc(arr_count * sizeof(bz_tensor_o*));
    if(NULL == factor)
    {
        bz_errno = BZ_MEM_ALLOC_ERROR;
        goto BZ_TENSOR_TUCKER_FREE_MODE_ARR;
    }
    for(bz_uint i = 0 ; i < arr_count ; i++)
        factor[i] = NULL;

    //求解因子矩阵
    bz_tensor_o* mode_n = NULL;
    bz_tensor_o** svd_ret = NULL;
    bz_uint temp;
    for(bz_uint i = 1 ; ; )
    {
        mode_n = bz_tensor_dimdiv8(obj , mode_arr , arr_count , NULL , 0);
        if(NULL == mode_n)
            goto BZ_TENSOR_TUCKER_FREE_FACTOR;

        svd_ret = bz_tensor_svd(mode_n);
        bz_tensor_del(&mode_n);
        if(NULL == svd_ret)
            goto BZ_TENSOR_TUCKER_FREE_FACTOR;
        factor[i] = svd_ret[0];
        bz_tensor_del(svd_ret + 1);
        bz_tensor_del(svd_ret + 2);
        bz_mem_free(svd_ret);
        
        if(i == ndim)
            break;
        //准备模 i + 1 展开
        i++;
        temp = mode_arr[0];
        mode_arr[0] = mode_arr[i];
        mode_arr[i] = temp;
    }

    //连续模 n 积的模 n 展开，求核心张量
    bz_tensor_o* U_T[2] = {NULL , NULL};
    bz_tensor_o* G = NULL;
    U_T[1] = bz_tensor_T(factor[ndim]);
    if(NULL == U_T[1])
        goto BZ_TENSOR_TUCKER_FREE_FACTOR;
    for(bz_uint i = ndim - 1 ; i > 1 ; i--)
    {
        U_T[0] = bz_tensor_T(factor[i]);
        if(NULL == U_T[0])
        {
            bz_tensor_del(U_T + 1);
            goto BZ_TENSOR_TUCKER_FREE_FACTOR;
        }

        G = bz_tensor_kronecker_product(U_T[1] , U_T[0]);
        bz_tensor_del(U_T + 0);
        bz_tensor_del(U_T + 1);
        if(NULL == G)
            goto BZ_TENSOR_TUCKER_FREE_FACTOR;

        U_T[1] = G;
        G = NULL;
    }
    G = bz_tensor_T(U_T[1]);
    bz_tensor_del(U_T + 1);
    if(NULL == G)
        goto BZ_TENSOR_TUCKER_FREE_FACTOR;
    U_T[1] = G;
    G = NULL;

    mode_arr[0] = 1;
    mode_arr[1] = 0;
    for(bz_uint i = 2 ; i < arr_count ; i++)
        mode_arr[i] = i;
    U_T[0] = bz_tensor_dimdiv8(obj , mode_arr , arr_count , NULL , 0);
    if(NULL == U_T[0])
    {
        bz_tensor_del(U_T + 1);
        goto BZ_TENSOR_TUCKER_FREE_FACTOR;
    }
    bz_int err = bz_tensor_retype(U_T[0] , U_T[1]->type);
    if(0 != err)
    {
        bz_errno = err;
        bz_tensor_del(U_T + 0);
        bz_tensor_del(U_T + 1);
        goto BZ_TENSOR_TUCKER_FREE_FACTOR;
    }

    G = bz_tensor_mul(U_T[0] , U_T[1]);
    bz_tensor_del(U_T + 0);
    bz_tensor_del(U_T + 1);
    if(NULL == G)
        goto BZ_TENSOR_TUCKER_FREE_FACTOR;
    U_T[1] = G;
    G = NULL;

    U_T[0] = bz_tensor_T(factor[1]);
    if(NULL == U_T[0])
    {
        bz_tensor_del(U_T + 1);
        goto BZ_TENSOR_TUCKER_FREE_FACTOR;
    }

    G = bz_tensor_mul(U_T[0] , U_T[1]);
    bz_tensor_del(U_T + 0);
    bz_tensor_del(U_T + 1);
    if(NULL == G)
        goto BZ_TENSOR_TUCKER_FREE_FACTOR;

    //计算 G 模 1 还原后的维度数组
    bz_uint* gdarr = (bz_uint*)bz_mem_alloc(ndim * sizeof(bz_uint));
    if(NULL == gdarr)
    {
        bz_errno = BZ_MEM_ALLOC_ERROR;
        goto BZ_TENSOR_TUCKER_DEL_G;
    }
    for(bz_uint i = 0 ; i < ndim ; i++)
        gdarr[i] = factor[i + 1]->darr[1];
    //对 G 进行模 1 还原，得到核心张量
    factor[0] = bz_tensor_create(gdarr , ndim , G->type);
    bz_mem_free(gdarr);
    gdarr = NULL;
    if(NULL == factor[0])
        goto BZ_TENSOR_TUCKER_DEL_G;
    if(NULL == bz_tensor_dimdiv8(factor[0] , mode_arr , arr_count , G->data , G->total_count))
        goto BZ_TENSOR_TUCKER_DEL_G;
    bz_tensor_del(&G);
    bz_mem_free(mode_arr);
    mode_arr = NULL;

    //计算核心张量的截断范围
    bz_mh_irange* g_tr_range = (bz_mh_irange*)bz_mem_alloc(ndim * sizeof(bz_mh_irange));
    if(NULL == g_tr_range)
    {
        bz_errno = BZ_MEM_ALLOC_ERROR;
        goto BZ_TENSOR_TUCKER_FREE_FACTOR;
    }
    for(bz_uint i = 0 ; i < ndim ; i++)
    {
        g_tr_range[i].start = 0;
        g_tr_range[i].step = 1;

        temp = factor[i + 1]->darr[1];
        temp = temp - (bz_uint)(temp * tr);
        if(temp < 1)
            temp = 1;

        g_tr_range[i].end = (bz_int)(temp - 1);
    }

    //截断核心张量
    bz_tensor_o* temp_tsr = bz_tensor_slice(factor[0] , g_tr_range , ndim);
    if(NULL == temp_tsr)
        goto BZ_TENSOR_TUCKER_FREE_G_TR_RANGE;
    bz_tensor_del(factor + 0);
    factor[0] = temp_tsr;

    //截断其余因子矩阵
    bz_mh_irange f_tr_range[2];
    f_tr_range[0].start = 0;
    f_tr_range[0].step = 1;
    f_tr_range[1].start = 0;
    f_tr_range[1].step = 1;
    for(bz_uint i = 1 ; i < arr_count ; i++)
    {
        f_tr_range[0].end = (bz_int)(factor[i]->darr[0] - 1);
        f_tr_range[1].end = g_tr_range[i - 1].end;
        temp_tsr = bz_tensor_slice(factor[i] , f_tr_range , 2);
        if(NULL == temp_tsr)
            goto BZ_TENSOR_TUCKER_FREE_G_TR_RANGE;
        bz_tensor_del(factor + i);
        factor[i] = temp_tsr;
    }

    bz_mem_free(g_tr_range);
    return factor;

BZ_TENSOR_TUCKER_FREE_G_TR_RANGE:
    bz_mem_free(g_tr_range);
BZ_TENSOR_TUCKER_DEL_G:
    bz_tensor_del(&G);
BZ_TENSOR_TUCKER_FREE_FACTOR:
    for(bz_uint i = 0 ; i < arr_count ; i++)
    {
        if(NULL != factor[i])
            bz_tensor_del(factor + i);
    }
    bz_mem_free(factor);
BZ_TENSOR_TUCKER_FREE_MODE_ARR:
    bz_mem_free(mode_arr);
    return NULL;
}
