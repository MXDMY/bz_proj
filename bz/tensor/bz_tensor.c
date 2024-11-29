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

    bz_uint itemsize = bz_dtype_size_table[dtype]; // 根据数据类型，从映射表中获得每个元素的字节大小

    void* data = bz_mem_alloc(count * itemsize);
    if(NULL == data)
        goto BZ_TENSOR_CREATE_MEM_ALLOC_ERROR;

    bz_uint* shape_dup = (bz_uint*)bz_mem_alloc(ndim * sizeof(bz_uint));
    if(NULL == shape_dup)
        goto BZ_TENSOR_CREATE_FREE_DATA;
    for(bz_uint i = 0 ; i < ndim ; i++)
        shape_dup[i] = shape[i];

    bz_uint* offsets = (bz_uint*)bz_mem_alloc(ndim * sizeof(bz_uint));
    if(NULL == offsets)
        goto BZ_TENSOR_CREATE_FREE_SHAPE_DUP;
    // 反向遍历计算 offsets，复杂度 O(n)
    offsets[ndim - 1] = 1;
    if(ndim > 1)
    {
        bz_uint i = ndim - 2;
        while(true)
        {
            offsets[i] = offsets[i + 1] * shape[i + 1];
            if(0 == i)
                break;
            i--;
        }
    }
    
    bz_tensor* t = (bz_tensor*)bz_mem_alloc(sizeof(bz_tensor));
    if(NULL == t)
        goto BZ_TENSOR_CREATE_FREE_OFFSETS;
    t->data = data;
    t->shape = shape_dup;
    t->offsets = offsets;
    t->ndim = ndim;
    t->count = count;
    t->dtype = dtype;
    t->itemsize = itemsize;
    return t;

BZ_TENSOR_CREATE_FREE_OFFSETS:
    bz_mem_free(offsets);
BZ_TENSOR_CREATE_FREE_SHAPE_DUP:
    bz_mem_free(shape_dup);
BZ_TENSOR_CREATE_FREE_DATA:
    bz_mem_free(data);
BZ_TENSOR_CREATE_MEM_ALLOC_ERROR:
    *errcode_p = BZ_MEM_ALLOC_ERROR;
    return NULL;
}

void bz_tensor_delete(bz_tensor** dblptr)
{
    if(NULL == dblptr || NULL == *dblptr)
        return;
    
    bz_tensor* t = *dblptr;
    bz_mem_free(t->data);
    bz_mem_free(t->shape);
    bz_mem_free(t->offsets);
    bz_mem_free(t);
    *dblptr = NULL;
}