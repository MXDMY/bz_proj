#include "bz_api.h"

__bz bz_api_init()
{
    __bz bz;

    bz.tensor.del = bz_tensor_del;
    bz.tensor.get = bz_tensor_get;
    bz.tensor.hadamard_x = bz_tensor_hadamard_product;
    bz.tensor.kronecker_x = bz_tensor_kronecker_product;
    bz.tensor.mul = bz_tensor_mul;
    bz.tensor.T = bz_tensor_T;
    bz.tensor.copy = bz_tensor_copy;
    bz.tensor.eig = bz_tensor_eig;
    bz.tensor.svd = bz_tensor_svd;
    bz.tensor.svd_trunc = bz_tensor_svd_trunc;
    bz.tensor.mode_n_x = bz_tensor_mode_n_product;
    bz.tensor.norm_f = bz_tensor_norm_f;
    bz.tensor.add = bz_tensor_add;
    bz.tensor.dec = bz_tensor_dec;
    bz.tensor.retype = bz_tensor_retype;
    bz.tensor.create = bz_api_tensor_create;
    bz.tensor.create_rand = bz_api_tensor_create_rand;
    bz.tensor.tucker = bz_tensor_tucker;

    bz.tensor.print = bz_api_tensor_print;
    bz.tensor.print_shape = bz_api_tensor_print_shape;


    bz.list.create = bz_api_list_create;
    bz.list.del = bz_list_del;
    bz.list.append = bz_list_append;
    bz.list.index = bz_list_index;
    
    bz.list.print = bz_api_list_print;


    bz.srand = bz_mh_srand;
    bz.rand = bz_mh_rand;

    return bz;
}