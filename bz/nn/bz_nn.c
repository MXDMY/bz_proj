#include "bz_nn.h"

bz_dbl bz_nn_relu(bz_dbl input)
{
    if(input > 0)
        return input;
    else
        return 0;
}

bz_int bz_nn_relu_bat(bz_dbl* input , bz_uint input_count)
{
    if(NULL == input || 0 == input_count)
        return BZ_FUNC_PARAM_ERROR;

    for(bz_uint i = 0 ; i < input_count ; i++)
    {
        if(input[i] < 0)
            input[i] = 0;   
    }

    return 0;
}

bz_dbl bz_nn_relu_d(bz_dbl input)
{
    //Relu函数本身在原点处不可导，这里进行特殊处理
    if(input <= 0)
        return 0;
    else
        return 1;
}

bz_int bz_nn_relu_d_bat(bz_dbl* input , bz_uint input_count)
{
    if(NULL == input || 0 == input_count)
        return BZ_FUNC_PARAM_ERROR;

    for(bz_uint i = 0 ; i < input_count ; i++)
    {
        if(input[i] <= 0)
            input[i] = 0;
        else
            input[i] = 1;
    }

    return 0;
}

bz_dbl bz_nn_sigmoid(bz_dbl input)
{
    return 1 / (1 + bz_mh_exp(-input));
}

bz_int bz_nn_sigmoid_bat(bz_dbl* input , bz_uint input_count)
{
    if(NULL == input || 0 == input_count)
        return BZ_FUNC_PARAM_ERROR;

    for(bz_uint i = 0 ; i < input_count ; i++)
        input[i] = 1 / (1 + bz_mh_exp(-input[i]));

    return 0;
}

bz_dbl bz_nn_sigmoid_d(bz_dbl input)
{
    //bz_dbl s = bz_nn_sigmoid(input);
    bz_dbl s = 1 / (1 + bz_mh_exp(-input));
    return (1 - s) * s;
}

bz_int bz_nn_sigmoid_d_bat(bz_dbl* input , bz_uint input_count)
{
    if(NULL == input || 0 == input_count)
        return BZ_FUNC_PARAM_ERROR;

    bz_dbl s;
    for(bz_uint i = 0 ; i < input_count ; i++)
    {
        s = 1 / (1 + bz_mh_exp(-input[i]));
        input[i] = (1 - s) * s;
    }

    return 0;
}

void* bz_nn_map_deri_func(void* prim_func)
{
    if(bz_nn_relu == prim_func)
        return bz_nn_relu_d;
    else if(bz_nn_relu_bat == prim_func)
        return bz_nn_relu_d_bat;
    else if(bz_nn_sigmoid == prim_func)
        return bz_nn_sigmoid_d;
    else if(bz_nn_sigmoid_bat == prim_func)
        return bz_nn_sigmoid_d_bat;
    else
        return NULL;
}