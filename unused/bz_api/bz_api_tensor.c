#include "bz_api.h"

static void print_start_bracket(bz_uint n)
{
#if BZ_IO_MODE == 1
    for(bz_uint i = 0 ; i < n ; i++)
        printf("[");
#endif
}

static void print_mid_bracket(bz_uint n , bz_uint enter)
{
#if BZ_IO_MODE == 1
    for(bz_uint i = 0 ; i < n ; i++)
        printf("]");
    printf(", ");
    if(enter)
        printf("\n ");
    for(bz_uint i = 0 ; i < n ; i++)
        printf("[");
#endif
}

static void print_end_bracket(bz_uint n)
{
#if BZ_IO_MODE == 1
    for(bz_uint i = 0 ; i < n ; i++)
        printf("]");
    printf("\n");
#endif
}

static const char* table = "0123456789";
static const bz_uint max_digit_w = 19; //可支持打印的整数位最大宽度

static void set_print_int_param(char* param , bz_int* data , bz_uint count)
{
    bz_uint digit_w = 1;
    bz_uint ten_x = 10; // 10 的 x 次方，x == digit_w
    bz_uint param_idx = 0;

    param[param_idx] = '%';
    param_idx++;

    if(NULL != data && 0 != count)
    {
        for(bz_uint i = 0 ; i < count ; i++)
        {
            while(digit_w <= max_digit_w)
            {
                if(0 == (bz_uint)(data[i] < 0 ? -data[i] : data[i]) / ten_x)
                {
                    if(data[i] < 0 && 0 != (bz_uint)(-data[i]) / (ten_x / 10))
                    {
                        ten_x *= 10;
                        digit_w++;
                    }
                    break;
                }
                ten_x *= 10;
                digit_w++;
            }
        }

        if(digit_w >= 10) //即 10 <= digit_w <= 19，高位必为 1
        {
            param[param_idx] = '1';
            param_idx++;
        }
        param[param_idx] = table[digit_w % 10];
        param_idx++;
    }

    //param[param_idx] = 'l';
    //param_idx++;
    //param[param_idx] = 'l';
    //param_idx++;
    param[param_idx] = 'd';
    param_idx++;
    param[param_idx] = '\0';
}

static void set_print_double_param(char* param , bz_dbl* data , bz_uint count)
{
    bz_uint dec_digit_w = 3; //小数位宽
    bz_uint digit_w = 1;     //整数位宽
    bz_uint ten_x = 10;      // 10 的 x 次方，x == digit_w
    bz_uint param_idx = 0;

    param[param_idx] = '%';
    param_idx++;

    if(NULL != data && 0 != count)
    {
        for(bz_uint i = 0 ; i < count ; i++)
        {
            while(digit_w <= max_digit_w)
            {
                if((data[i] < 0 ? -data[i] : data[i]) / ten_x < 1)
                {
                    if(data[i] < 0)
                    {
                        if((-data[i]) / (ten_x / 10) >= 1 || digit_w < 2/*浮点数有 -0. */)
                        {
                            ten_x *= 10;
                            digit_w++;
                        }
                    }
                    break;
                }
                ten_x *= 10;
                digit_w++;
            }
        }

        digit_w = digit_w + 1 + dec_digit_w; //使 digit_w 指代最终位宽，包括小数点和小数位
        if(digit_w >= 10) //通常 10 <= digit_w < 100
        {
            param[param_idx] = table[digit_w / 10];
            param_idx++;
        }
        param[param_idx] = table[digit_w % 10];
        param_idx++;
        param[param_idx] = '.';
        param_idx++;
        param[param_idx] = table[dec_digit_w];
        param_idx++;
    }
    
    param[param_idx] = 'l';
    param_idx++;
    param[param_idx] = 'f';
    param_idx++;
    param[param_idx] = '\0';
}

bz_int bz_api_tensor_print(bz_tensor_o* obj)
{
#if BZ_IO_MODE == 1
    char param[10];

    if(0 != bz_api_tensor_print_shape(obj))
        return BZ_FUNC_PARAM_ERROR;
    else if(BZ_INT_TYPE == obj->type)
    {
        //获取打印参数
        set_print_int_param(param , (bz_int*)(obj->data) , obj->total_count);

        //分配计数器内存
        bz_uint dim = obj->dim;
        bz_uint* counter = (bz_uint*)bz_mem_alloc(dim * sizeof(bz_uint));
        if(NULL == counter)
            return BZ_MEM_ALLOC_ERROR;
        else
        {
            for(bz_uint i = 0 ; i < dim ; i++)
                counter[i] = 0;
        }

        bz_int* data = (bz_int*)(obj->data);
        bz_uint counter_idx = dim - 1; //计数器从最后一个维度开始计数
        bz_uint n; //从后往前，维度已达到终点的个数
        print_start_bracket(dim);
        for(bz_uint i = 0 ; i < obj->total_count ; i++)
        {
            printf(param , data[i]);
            if(counter[counter_idx] < obj->darr[counter_idx] - 1)
            {
                printf(", ");
                counter[counter_idx]++;
            }
            else
            {
                n = 0;
                for(bz_uint j = dim - 1 ; j >= 0 ; j--)
                {
                    if(counter[j] >= obj->darr[j] -1)
                        n++;
                    else
                        break;
                    if(0 == j)
                        break;
                }

                if(n != dim)
                {
                    for(bz_uint j = 0 ; j < n ; j++)
                        counter[dim - 1 - j] = 0;
                    counter[dim - 1 - n]++;
                    if(0 == dim - 1 - n)
                        print_mid_bracket(n , 1);
                    else
                        print_mid_bracket(n , 0);
                }
            }
        }
        print_end_bracket(dim);

        bz_mem_free(counter);
        return 0;
    }
    else if(BZ_DOUBLE_TYPE == obj->type)
    {
        //获取打印参数
        set_print_double_param(param , (bz_dbl*)(obj->data) , obj->total_count);

        //分配计数器内存
        bz_uint dim = obj->dim;
        bz_uint* counter = (bz_uint*)bz_mem_alloc(dim * sizeof(bz_uint));
        if(NULL == counter)
            return BZ_MEM_ALLOC_ERROR;
        else
        {
            for(bz_uint i = 0 ; i < dim ; i++)
                counter[i] = 0;
        }

        bz_dbl* data = (bz_dbl*)(obj->data);
        bz_uint counter_idx = dim - 1; //计数器从最后一个维度开始计数
        bz_uint n; //从后往前，维度已达到终点的个数
        print_start_bracket(dim);
        for(bz_uint i = 0 ; i < obj->total_count ; i++)
        {
            printf(param , data[i]);
            if(counter[counter_idx] < obj->darr[counter_idx] - 1)
            {
                printf(", ");
                counter[counter_idx]++;
            }
            else
            {
                n = 0;
                for(bz_uint j = dim - 1 ; j >= 0 ; j--)
                {
                    if(counter[j] >= obj->darr[j] -1)
                        n++;
                    else
                        break;
                    if(0 == j)
                        break;
                }

                if(n != dim)
                {
                    for(bz_uint j = 0 ; j < n ; j++)
                        counter[dim - 1 - j] = 0;
                    counter[dim - 1 - n]++;
                    if(0 == dim - 1 - n)
                        print_mid_bracket(n , 1);
                    else
                        print_mid_bracket(n , 0);
                }
            }
        }
        print_end_bracket(dim);

        bz_mem_free(counter);
        return 0;
    }
    else
        return BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;
#else
    return 0;
#endif
}

bz_int bz_api_tensor_print_shape(bz_tensor_o* obj)
{
#if BZ_IO_MODE == 1
    if(NULL == obj)
        return BZ_FUNC_PARAM_ERROR;

    printf("#0x%p : [" , obj);
    for(bz_uint i = 0 ; i < obj->dim - 1 ; i++)
        printf("%u, " , obj->darr[i]);
    printf("%u]\n" , obj->darr[obj->dim - 1]);

    return 0;
#else
    return 0;
#endif
}

bz_tensor_o* bz_api_tensor_create(char* str , bz_mh_dtype type)
{
    if(NULL == str)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }

    bz_list_o* list = bz_api_list_create(str);
    if(NULL == list)
        return NULL;

    if(0 == list->count)
    {
        bz_errno = BZ_APT_PARAM_STR_ERROR;
        goto BZ_API_TENSOR_CREATE_DEL_LIST;
    }
    bz_list_u* list_u;
    bz_uint i;
    for(i = 0 , list_u = list->head ; i < list->count ; i++ , list_u = list_u->next)
    {
        if(BZ_DS_INT_TYPE != list_u->ds_u.type)
        {
            bz_errno = BZ_APT_PARAM_STR_ERROR;
            goto BZ_API_TENSOR_CREATE_DEL_LIST;
        }
        if(list_u->ds_u.data.i <= 0)
        {
            bz_errno = BZ_MH_TENSOR_DARR_NON_POS_VALUE_ERROR;
            goto BZ_API_TENSOR_CREATE_DEL_LIST;
        }
    }

    bz_uint dim = list->count;
    bz_uint* darr = (bz_uint*)bz_mem_alloc(dim * sizeof(bz_uint));
    if(NULL == darr)
    {
        bz_errno = BZ_MEM_ALLOC_ERROR;
        goto BZ_API_TENSOR_CREATE_DEL_LIST;
    }
    for(i = 0 , list_u = list->head ; i < dim ; i++ , list_u = list_u->next)
        darr[i] = (bz_uint)(list_u->ds_u.data.i);

    bz_tensor_o* tsr = bz_tensor_create(darr , dim , type);
    if(NULL == tsr)
        goto BZ_API_TENSOR_CREATE_FREE_DARR;

    bz_mem_free(darr);
    bz_list_del(&list);

    return tsr;

BZ_API_TENSOR_CREATE_FREE_DARR:
    bz_mem_free(darr);
BZ_API_TENSOR_CREATE_DEL_LIST:
    bz_list_del(&list);
    return NULL;
}

bz_tensor_o* bz_api_tensor_create_rand(char* str , bz_dbl min , bz_dbl max , bz_mh_dtype type)
{
    if(min > max)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }
    bz_dbl range = max - min;

    bz_tensor_o* tsr = bz_api_tensor_create(str , type);
    if(NULL == tsr)
        return NULL;

#define DTYPE_HANDLE(dtype) \
do{ \
    dtype* data = (dtype*)(tsr->data); \
    for(bz_uint i = 0 ; i < tsr->total_count ; i++) \
        data[i] = (dtype)(bz_mh_rand() / (bz_dbl)BZ_RAND_MAX * range + min); \
}while(0)

    if(BZ_INT_TYPE == type)
        DTYPE_HANDLE(bz_int);
    else if(BZ_DOUBLE_TYPE == type)
        DTYPE_HANDLE(bz_dbl);
    else
    {
        bz_errno = BZ_MH_DATA_TYPE_NO_SUPPORT_ERROR;
        bz_tensor_del(&tsr);
        return NULL;
    }

#undef DTYPE_HANDLE

    return tsr;
}