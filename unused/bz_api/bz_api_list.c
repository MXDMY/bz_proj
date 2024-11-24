#include "bz_api.h"

static void list_print(bz_list_o* list)
{
#if BZ_IO_MODE == 1
    printf("[");
    bz_list_u* list_u = list->head;
    bz_ds_u ds_u;
    for(bz_uint i = 0 ; i < list->count ; i++)
    {
        ds_u = list_u->ds_u;
        switch(ds_u.type)
        {
        case BZ_DS_INT_TYPE:
            printf("%d" , ds_u.data.i);
            break;
        case BZ_DS_UINT_TYPE:
            printf("%u" , ds_u.data.u);
            break;
        case BZ_DS_U8_TYPE:
            printf("%u" , ds_u.data.b);
            break;
        case BZ_DS_DOUBLE_TYPE:
            printf("%.3lf" , ds_u.data.d);
            break;
        case BZ_DS_LIST_TYPE:
            list_print((bz_list_o*)(ds_u.data.v));
            break;
        
        default:
            printf("None");
            break;
        }
        if(i != list->count - 1)
            printf(", ");
        list_u = list_u->next;
    }
    printf("]");
#endif
}

bz_int bz_api_list_print(bz_list_o* list)
{
#if BZ_IO_MODE == 1
    if(NULL == list)
        return BZ_FUNC_PARAM_ERROR;

    printf("#0x%p : %u\n" , list , list->count);
    list_print(list);
    printf("\n");

    return 0;
#else
    return 0;
#endif
}

static bz_ds_u get_unit_from_stack(char* stack , bz_uint st_top , bz_u8 st_dot)
{
    bz_ds_u unit;
    unit.type = BZ_DS_NONE_TYPE;

    if(st_dot) //浮点数
    {
        if('.' == stack[st_top] || ('-' == stack[1] && '.' == stack[2]) || '.' == stack[1])
            return unit;

        unit.data.d = 0;
        unit.type = BZ_DS_DOUBLE_TYPE;
        
        bz_uint dot_idx = 1;
        for(; dot_idx <= st_top ; dot_idx++)
        {   
            if('.' == stack[dot_idx])
                break;
        }

        //计算小数部分
        bz_uint i;
        bz_dbl ten_x;
        for(i = dot_idx + 1 , ten_x = 0.1 ; i <= st_top ; i++ , ten_x /= 10)
            unit.data.d += (stack[i] - 48) * ten_x;
        //计算整数部分
        for(i = dot_idx - 1 , ten_x = 1 ; i >= 1 && '-' != stack[i] ; i-- , ten_x *= 10)
            unit.data.d += (stack[i] - 48) * ten_x;

        if('-' == stack[1])
            unit.data.d *= -1;

        return unit;
    }
    else //整数
    {
        if('-' == stack[1] && 1 == st_top)
            return unit;

        unit.data.i = 0;
        unit.type = BZ_DS_INT_TYPE;

        bz_uint i , ten_x;
        for(i = st_top , ten_x = 1 ; i >= 1 && '-' != stack[i] ; i-- , ten_x *= 10)
            unit.data.i += (stack[i] - 48) * ten_x;

        if('-' == stack[1])
            unit.data.i *= -1;

        return unit;
    }
}

static bz_uint mov_len; //递归函数移动字符长度
bz_list_o* bz_api_list_create(char* str)
{
    if(NULL == str)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return NULL;
    }

    //字符栈，目前还未实现 栈 数据结构，因时间要求，先简化处理，用定长数组替代
    char stack[100]; //首位不用于放字符，用于空间检查
    const bz_uint st_end = 99;
    bz_uint st_top = 0;
    bz_u8 st_flag; //对栈操作是否成功
    bz_u8 st_dot = false;  //栈中是否存在小数点

#define ST_PUSH(ch) \
do{ \
    if(st_top < st_end) \
    { \
        st_top++; \
        stack[st_top] = ch; \
        st_flag = true; \
    } \
    else \
        st_flag = false; \
}while(0)

#define ST_POP(ch) \
do{ \
    if(0 == st_top) \
        st_flag = false; \
    else \
    { \
        ch = stack[st_top]; \
        st_top--; \
        st_flag = true; \
    } \
}while(0)

#define ST_CLEAR \
do{ \
    st_top = 0; \
    st_dot = false; \
}while(0)

#define LIST_APPEND_FROM_ST \
do{ \
    if(NULL == list) \
        goto BZ_API_LIST_CREATE_RAISE_PARAM_STR_ERROR; \
    bz_ds_u ds_u = get_unit_from_stack(stack , st_top , st_dot); \
 \
    st_top = 0; \
    st_dot = false; \
 \
    if(BZ_DS_NONE_TYPE == ds_u.type) \
        goto BZ_API_LIST_CREATE_RAISE_PARAM_STR_ERROR; \
    bz_errno = bz_list_append(list , ds_u); \
    if(0 != bz_errno) \
        goto BZ_API_LIST_CREATE_DEL_LIST; \
}while(0)

    bz_list_o* list = NULL;
    mov_len = 0;
    bz_u8 var_len_app = false; //列表刚添加变长数据类型
    for(bz_uint i = 0 ; '\0' != str[i] ; i++)
    {
        if('0' <= str[i] && str[i] <= '9')
        {
            ST_PUSH(str[i]);
            if(!st_flag)
                goto BZ_API_LIST_CREATE_RAISE_MEM_ALLOC_ERROR;
            continue;
        }

        switch(str[i])
        {
        case ' ':
            /* 忽略 */
            break;
        case '[':
            if(NULL == list) //主列表
            {
                list = (bz_list_o*)bz_list_create(0);
                if(NULL == list)
                    goto BZ_API_LIST_CREATE_DEL_LIST;
            }
            else //子列表
            {
                bz_ds_u ds_u;
                ds_u.type = BZ_DS_LIST_TYPE;
                ds_u.data.v = bz_api_list_create(str + i);
                i += mov_len;
                if(NULL == ds_u.data.v)
                    goto BZ_API_LIST_CREATE_DEL_LIST;
                else
                {
                    bz_errno = bz_list_append(list , ds_u);
                    if(0 != bz_errno)
                        goto BZ_API_LIST_CREATE_DEL_LIST;
                }

                var_len_app = true;
            }
            break;
        case ']':
            if(0 != st_top)
                LIST_APPEND_FROM_ST;
            //break;
            mov_len = i;
            return list;
        case ',':
            if(0 != st_top)
                LIST_APPEND_FROM_ST;
            else
            {
                if(!var_len_app)
                    goto BZ_API_LIST_CREATE_RAISE_PARAM_STR_ERROR;
            }
            var_len_app = false;
            break;
        case '.':
            if(st_dot)
                goto BZ_API_LIST_CREATE_RAISE_PARAM_STR_ERROR;
            ST_PUSH(str[i]);
            st_dot = true;
            if(!st_flag)
                goto BZ_API_LIST_CREATE_RAISE_MEM_ALLOC_ERROR;
            break;
        case '-':
            if(0 != st_top)
                goto BZ_API_LIST_CREATE_RAISE_PARAM_STR_ERROR;
            ST_PUSH(str[i]);
            if(!st_flag)
                goto BZ_API_LIST_CREATE_RAISE_MEM_ALLOC_ERROR;
            break;
        
        default:
            goto BZ_API_LIST_CREATE_RAISE_PARAM_STR_ERROR;
            break;
        }
    }

    goto BZ_API_LIST_CREATE_RAISE_PARAM_STR_ERROR;
BZ_API_LIST_CREATE_RAISE_MEM_ALLOC_ERROR:
    bz_errno = BZ_MEM_ALLOC_ERROR;
    goto BZ_API_LIST_CREATE_DEL_LIST;
BZ_API_LIST_CREATE_RAISE_PARAM_STR_ERROR:
    bz_errno = BZ_APT_PARAM_STR_ERROR;
BZ_API_LIST_CREATE_DEL_LIST:
    bz_list_del(&list);
    return NULL;
}