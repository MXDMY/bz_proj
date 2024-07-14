#include "bz_fc.h"

bz_fc_m* bz_fc_model_create(void)
{
    bz_fc_m* m = (bz_fc_m*)bz_mem_alloc(sizeof(bz_fc_m));
    if(NULL == m)
        return NULL;

    m->input = NULL;
    m->input_count = 0;

    m->in_layer = NULL;
    m->out_layer = NULL;
    m->layer_count = 0;
    m->layer_n_arr = NULL;
    m->act_func_list = NULL;

    m->w = NULL;
    m->w_n_arr = NULL;

    m->b = NULL;

    m->pool_layer = NULL;
    
    return m;
}

bz_int bz_fc_model_add_input(bz_fc_m* m , bz_dbl* input , bz_uint input_count)
{
    if(NULL == m || NULL == input || 0 == input_count)
        return BZ_FUNC_PARAM_ERROR;

    m->input = input;
    m->input_count = input_count;

    return 0;
}

bz_int bz_fc_model_add_layer(bz_fc_m* m , bz_uint count , bz_int (*act_func)(bz_dbl* , bz_uint))
{
    if(NULL == m || 0 == count || NULL == act_func)
        return BZ_FUNC_PARAM_ERROR;

    bz_uint new_layer_count = m->layer_count + 1;

    bz_uint* new_layer_n_arr = (bz_uint*)bz_mem_alloc(new_layer_count * sizeof(bz_uint));
    if(NULL == new_layer_n_arr)
        return BZ_MEM_ALLOC_ERROR;

    bz_uint size = sizeof(bz_int (*)(bz_dbl* , bz_uint));
    bz_int (**new_act_func_list)(bz_dbl* , bz_uint);
    new_act_func_list = (bz_int (**)(bz_dbl* , bz_uint))bz_mem_alloc(new_layer_count * size);
    if(NULL == new_act_func_list)
        goto BZ_FC_MODEL_ADD_LAYER_FREE_NEW_LAYER_N_ARR;

    for(bz_uint i = 0 ; i < m->layer_count ; i++)
    {
        new_layer_n_arr[i] = m->layer_n_arr[i];
        new_act_func_list[i] = m->act_func_list[i];
    }
    new_layer_n_arr[new_layer_count - 1] = count;
    new_act_func_list[new_layer_count - 1] = act_func;

    bz_mem_free(m->layer_n_arr);
    bz_mem_free(m->act_func_list);

    m->layer_n_arr = new_layer_n_arr;
    m->act_func_list = new_act_func_list;
    m->layer_count = new_layer_count;
    
    return 0;

BZ_FC_MODEL_ADD_LAYER_FREE_NEW_LAYER_N_ARR:
    bz_mem_free(new_layer_n_arr);
    return BZ_MEM_ALLOC_ERROR;
}

bz_int bz_fc_model_build(bz_fc_m* m)
{
    if(NULL == m)
        return BZ_FUNC_PARAM_ERROR;

    bz_uint layer_count = m->layer_count;

    bz_dbl** new_in_layer = (bz_dbl**)bz_mem_alloc(layer_count * sizeof(bz_dbl*));
    if(NULL == new_in_layer)
        return BZ_MEM_ALLOC_ERROR;
    bz_dbl** new_out_layer = (bz_dbl**)bz_mem_alloc(layer_count * sizeof(bz_dbl*));
    if(NULL == new_out_layer)
        goto BZ_FC_MODEL_BUILD_FREE_NEW_IN_LAYER;
    bz_dbl** new_w = (bz_dbl**)bz_mem_alloc(layer_count * sizeof(bz_dbl*));
    if(NULL == new_w)
        goto BZ_FC_MODEL_BUILD_FREE_NEW_OUT_LAYER;
    bz_uint* new_w_n_arr = (bz_uint*)bz_mem_alloc(layer_count * sizeof(bz_uint));
    if(NULL == new_w_n_arr)
        goto BZ_FC_MODEL_BUILD_FREE_NEW_W;
    bz_dbl* new_b = (bz_dbl*)bz_mem_alloc(layer_count * sizeof(bz_dbl));
    if(NULL == new_b)
        goto BZ_FC_MODEL_BUILD_FREE_NEW_W_N_ARR;

    for(bz_uint i = 0 ; i < layer_count ; i++)
    {
        new_in_layer[i] = NULL;
        new_out_layer[i] = NULL;
        new_w[i] = NULL;
    }

    new_w_n_arr[0] = m->input_count * m->layer_n_arr[0];
    for(bz_uint i = 1 ; i < layer_count ; i++)
        new_w_n_arr[i] = m->layer_n_arr[i - 1] * m->layer_n_arr[i];

    for(bz_uint i = 0 ; i < layer_count ; i++)
    {
        new_in_layer[i] = (bz_dbl*)bz_mem_alloc(m->layer_n_arr[i] * sizeof(bz_dbl));
        new_out_layer[i] = (bz_dbl*)bz_mem_alloc(m->layer_n_arr[i] * sizeof(bz_dbl));
        new_w[i] = (bz_dbl*)bz_mem_alloc(new_w_n_arr[i] * sizeof(bz_dbl));

        if(NULL == new_in_layer[i] || NULL == new_out_layer[i] || NULL == new_w[i])
            goto BZ_FC_MODEL_BUILD_FREE_NEW_B_AND_OTHER;
    }

    m->in_layer = new_in_layer;
    m->out_layer = new_out_layer;
    m->w = new_w;
    m->w_n_arr = new_w_n_arr;
    m->b = new_b;

    return 0;

BZ_FC_MODEL_BUILD_FREE_NEW_B_AND_OTHER:
    for(bz_uint i = 0 ; i < layer_count ; i++)
    {
        bz_mem_free(new_in_layer[i]);
        bz_mem_free(new_out_layer[i]);
        bz_mem_free(new_w[i]);
    }
    bz_mem_free(new_b);
BZ_FC_MODEL_BUILD_FREE_NEW_W_N_ARR:
    bz_mem_free(new_w_n_arr);
BZ_FC_MODEL_BUILD_FREE_NEW_W:
    bz_mem_free(new_w);
BZ_FC_MODEL_BUILD_FREE_NEW_OUT_LAYER:
    bz_mem_free(new_out_layer);
BZ_FC_MODEL_BUILD_FREE_NEW_IN_LAYER:
    bz_mem_free(new_in_layer);
    return BZ_MEM_ALLOC_ERROR;
}

bz_int bz_fc_model_set_w(bz_fc_m* m , bz_dbl* v , bz_uint count , bz_uint idx)
{
    if(NULL == m || NULL == v)
        return BZ_FUNC_PARAM_ERROR;
    if(idx >= m->layer_count || count != m->w_n_arr[idx])
        return BZ_FUNC_PARAM_ERROR;
    
    for(bz_uint i = 0 ; i < count ; i++)
        m->w[idx][i] = v[i];

    return 0;
}

bz_int bz_fc_model_set_b(bz_fc_m* m , bz_dbl* v , bz_uint count)
{
    if(NULL == m || NULL == v || count != m->layer_count)
        return BZ_FUNC_PARAM_ERROR;

    for(bz_uint i = 0 ; i < count ; i++)
        m->b[i] = v[i];

    return 0;
}

void bz_fc_model_forward_calcu(bz_fc_m* m)
{
    if(NULL == m)
        return;

    bz_uint layer_count = m->layer_count;
    bz_dbl* input = NULL;
    bz_dbl* in = NULL;
    bz_dbl* out = NULL;
    bz_dbl* w1 = NULL;
    bz_dbl b1 = 0;
    bz_uint input_count = 0;
    bz_uint io_count = 0;
    bz_int (*act_func)(bz_dbl* , bz_uint);

    input = m->input;
    input_count = m->input_count;
    for(bz_uint i = 0 ; i < layer_count ; i++)
    {
        in = m->in_layer[i];
        out = m->out_layer[i];
        w1 = m->w[i];
        b1 = m->b[i];
        io_count = m->layer_n_arr[i];
        act_func = m->act_func_list[i];

        for(bz_uint j = 0 ; j < io_count ; j++)
        {
            in[j] = b1;
            for(bz_uint k = 0 ; k < input_count ; k++)
                in[j] += input[k] * w1[j * input_count + k];
            out[j] = in[j];
        }

        act_func(out , io_count);

        input = out;
        input_count = io_count;
    }
}

bz_int bz_fc_model_back_calcu(bz_fc_m* m , bz_dbl* err , bz_uint err_count , bz_dbl rate)
{
    if(NULL == m || NULL == err || err_count != m->layer_n_arr[m->layer_count - 1] || rate <= 0)
        return BZ_FUNC_PARAM_ERROR;

    bz_uint layer_count = m->layer_count;
    bz_dbl** pool_layer = m->pool_layer;
    //若初次使用反馈层，则分配其空间
    if(NULL == pool_layer)
    {
        pool_layer = (bz_dbl**)bz_mem_alloc(layer_count * sizeof(bz_dbl*));
        if(NULL == pool_layer)
            return BZ_MEM_ALLOC_ERROR;
        else
        {
            for(bz_uint i = 0 ; i < layer_count ; i++)
                pool_layer[i] = NULL;
        }

        for(bz_uint i = 0 ; i < layer_count ; i++)
        {
            pool_layer[i] = (bz_dbl*)bz_mem_alloc(m->layer_n_arr[i] * sizeof(bz_dbl));
            if(NULL == pool_layer[i])
                goto BZ_FC_MODEL_BACK_CALCU_FREE_POOL_LAYER;
        }

        m->pool_layer = pool_layer;
    }

    bz_uint io_count = 0;
    bz_uint io_count_next = 0;
    bz_dbl* pool = NULL;
    bz_dbl* pool_next = NULL;
    bz_dbl* in = NULL;
    bz_int (*act_func_d)(bz_dbl* , bz_uint);
    bz_dbl* w1 = NULL;
    //首先更新最后一层反馈层
    io_count = m->layer_n_arr[layer_count - 1];
    pool = pool_layer[layer_count - 1];
    in = m->in_layer[layer_count - 1];
    act_func_d = bz_nn_map_deri_func(m->act_func_list[layer_count - 1]);
    if(NULL == act_func_d)
        return BZ_NN_MAP_DERI_FUNC_ERROR;
    for(bz_uint i = 0 ; i < io_count ; i++)
        pool[i] = in[i];
    act_func_d(pool , io_count);
    for(bz_uint i = 0 ; i < io_count ; i++)
        pool[i] = pool[i] * err[i];
    //从后往前更新其它反馈层
    for(bz_uint i = layer_count - 2 ; ; i--)
    {
        pool = pool_layer[i];
        pool_next = pool_layer[i + 1];
        io_count = m->layer_n_arr[i];
        io_count_next = m->layer_n_arr[i + 1];
        w1 = m->w[i + 1];
        in = m->in_layer[i];
        act_func_d = bz_nn_map_deri_func(m->act_func_list[i]);
        if(NULL == act_func_d)
            return BZ_NN_MAP_DERI_FUNC_ERROR;

        //复用本层次级对应的输入层
        act_func_d(in , io_count);

        for(bz_uint j = 0 ; j < io_count ; j++)
        {
            pool[j] = 0;
            for(bz_uint k = 0 ; k < io_count_next ; k++)
                pool[j] = pool[j] + pool_next[k] * w1[k * io_count + j];
            pool[j] = pool[j] * in[j];
        }

        if(0 == i)
            break;
    }

    bz_dbl* out = NULL;
    bz_dbl w1_count = 0;
    //从后往前更新权重，不包括第一层
    for(bz_uint i = layer_count - 1 ; i > 0 ; i--)
    {
        pool = pool_layer[i];
        w1 = m->w[i];
        w1_count = m->w_n_arr[i];
        out = m->out_layer[i - 1];
        io_count = m->layer_n_arr[i - 1];
        
        for(bz_uint j = 0 ; j < w1_count ; j++)
            w1[j] = w1[j] - rate * out[j % io_count] * pool[j / io_count];
    }
    //更新第一层权重
    pool = pool_layer[0];
    w1 = m->w[0];
    w1_count = m->w_n_arr[0];
    out = m->input;
    io_count = m->input_count;
        
    for(bz_uint j = 0 ; j < w1_count ; j++)
        w1[j] = w1[j] - rate * out[j % io_count] * pool[j / io_count];

    return 0;

BZ_FC_MODEL_BACK_CALCU_FREE_POOL_LAYER:
    for(bz_uint i = 0 ; i < layer_count ; i++)
    {
        if(NULL != pool_layer[i])
            bz_mem_free(pool_layer[i]);
        else
            break;
    }
    bz_mem_free(pool_layer);
    return BZ_MEM_ALLOC_ERROR;
}