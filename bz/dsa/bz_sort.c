#include "bz_dsa.h"

void bz_sort_heap(bz_int* int_data , bz_uint int_count , bz_dbl* dbl_data , bz_uint dbl_count , bz_int order)
{
    if(0 == order)
        return;

    if(NULL == int_data || 0 == int_count)
        goto BZ_SORT_HEAP_DBL;

    bz_int int_temp;
    for(bz_uint end = int_count - 1 ; end > 0 ; end--)
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
                    if(order * (int_data[left] - int_data[right]) >= 0)
                        child_winner = left;
                    else
                        child_winner = right;
                }
                else //不存在右孩子
                    child_winner = left;
                //父节点与子节点大小比较
                if(order * (int_data[index] - int_data[child_winner]) >= 0)
                    break;
                else
                {
                    int_temp = int_data[child_winner];
                    int_data[child_winner] = int_data[index];
                    int_data[index] = int_temp;
                    index = child_winner;
                    left = 2 * index + 1;
                    right = left + 1;
                }
            }

            if(0 == nonleaf)
                break;
        }
        int_temp = int_data[end];
        int_data[end] = int_data[0];
        int_data[0] = int_temp;
    }

BZ_SORT_HEAP_DBL:
    if(NULL == dbl_data || 0 == dbl_count)
        goto BZ_SORT_HEAP_END;

    bz_dbl dbl_temp;
    for(bz_uint end = dbl_count - 1 ; end > 0 ; end--)
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
                    if(order * (dbl_data[left] - dbl_data[right]) >= 0)
                        child_winner = left;
                    else
                        child_winner = right;
                }
                else //不存在右孩子
                    child_winner = left;
                //父节点与子节点大小比较
                if(order * (dbl_data[index] - dbl_data[child_winner]) >= 0)
                    break;
                else
                {
                    dbl_temp = dbl_data[child_winner];
                    dbl_data[child_winner] = dbl_data[index];
                    dbl_data[index] = dbl_temp;
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
    }

BZ_SORT_HEAP_END:
}
