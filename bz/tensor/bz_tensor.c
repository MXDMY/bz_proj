#include "bz_tensor.h"

bz_tensor* bz_tensor_create(bz_uint* shape, bz_uint ndim, bz_dtype dtype, bz_int* errno_p)
{
    bz_int err = 0;
    bz_tensor* t = NULL;

    if (NULL == shape || 0 == ndim)
    {
        err = BZ_FUNC_PARAM_ERROR;
        goto END;
    }
    for (bz_uint i = 0; i < ndim; i++)
    {
        if (0 == shape[i])
        {
            err = BZ_FUNC_PARAM_ERROR;
            goto END;
        }
    }

    t = bz_mem_alloc(sizeof(bz_tensor));
    if (NULL == t)
    {
        err = BZ_MEM_ALLOC_ERROR;
        goto END;
    }
    t->data = NULL;
    t->dim.ext.shape = NULL;
    t->dim.ext.strides = NULL;
    t->ndim = ndim;
    // t->count = 0;
    t->dtype = dtype;
    t->itemsize = bz_dtype_size_table[dtype];
    t->flags = BZ_TENSOR_FLAG_C_ORDER | BZ_TENSOR_FLAG_OWN_DATA;

    if (ndim > BZ_TENSOR_INLINE_DIM)
    {
        t->flags |= BZ_TENSOR_FLAG_EXT_DIM;
        t->dim.ext.shape = bz_mem_alloc(ndim * sizeof(bz_uint) * 2);
        if (NULL == t->dim.ext.shape)
        {
            err = BZ_MEM_ALLOC_ERROR;
            goto FREE_EXT_DIM;
        }
        t->dim.ext.strides = t->dim.ext.shape + ndim;
    }

    bz_uint* t_shape = bz_tensor_shape(t);
    bz_uint* t_strides = bz_tensor_strides(t);
    bz_uint last_index = ndim - 1;
    t->count = shape[last_index];
    t_shape[last_index] = shape[last_index];
    t_strides[last_index] = 1;
    for (bz_int i = (bz_int)last_index - 1; i >= 0; i--)
    {
        t_strides[i] = t->count;
        t->count *= shape[i];
        t_shape[i] = shape[i];
    }

    t->data = bz_mem_alloc(t->count * t->itemsize);
    if (NULL == t->data)
    {
        err = BZ_MEM_ALLOC_ERROR;
        goto FREE_EXT_DIM;
    }

    goto END;

FREE_EXT_DIM:
    if (t->dim.ext.shape)
    {
        bz_mem_free(t->dim.ext.shape);
        t->dim.ext.shape = NULL;
        t->dim.ext.strides = NULL;
    }
    bz_mem_free(t);
    t = NULL;
END:
    if (errno_p)
        *errno_p = err;
    return t;
}

void bz_tensor_delete(bz_tensor** t_2p)
{
    if (t_2p && *t_2p)
    {
        bz_tensor* t_p = *t_2p;
        if (t_p->flags & BZ_TENSOR_FLAG_OWN_DATA)
            bz_mem_free(t_p->data);
        if (t_p->flags & BZ_TENSOR_FLAG_EXT_DIM)
            bz_mem_free(t_p->dim.ext.shape);
        bz_mem_free(t_p);
        *t_2p = NULL;
    }
}
