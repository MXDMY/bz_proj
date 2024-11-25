#include "bz_tensor.h"

bz_tensor* bz_tensor_create(bz_uint* shape , bz_uint ndim , bz_dtype dtype , bz_int* errcode_p)
{
    if(NULL == shape || 0 == ndim || NULL == errcode_p)
    {
        *errcode_p = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }
    bz_uint count = 1;
    for(bz_uint i = 0 ; i < ndim ; i++)
    {
        if(0 == shape[i])
        {
            *errcode_p = BZ_FUNC_PARAM_ERROR;
            return NULL;
        }
        else
            count *= shape[i];
    }

    // 根据数据类型，从映射表中获得每个元素的字节大小
    bz_uint itemsize = bz_dtype_size_table[dtype];
#if 0
    // 根据不同的数据类型分配不同的数据内存
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
#endif
    return NULL;
}