#include "bz_list.h"

bz_list_o* bz_list_create(bz_uint count)
{
    bz_list_o* list = (bz_list_o*)bz_mem_alloc(sizeof(bz_list_o));
    if(NULL == list)
    {
        bz_errno = BZ_MEM_ALLOC_ERROR;
        return NULL;
    }
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;

    bz_list_u* list_u = NULL;
    for(bz_uint i = 0 ; i < count ; i++)
    {
        list_u = (bz_list_u*)bz_mem_alloc(sizeof(bz_list_u));
        if(NULL == list_u)
        {
            bz_errno = BZ_MEM_ALLOC_ERROR;
            bz_list_del(&list);
            return NULL;
        }

        list_u->ds_u.type = BZ_DS_NONE_TYPE;

        if(0 == list->count)
        {
            list_u->pre = NULL;
            list_u->next = NULL;
            list->head = list_u;
            list->tail = list_u;
        }
        else
        {
            list_u->pre = list->tail;
            list_u->next = NULL;
            list->tail->next = list_u;
            list->tail = list_u;
        }

        list->count++;
    }

    return list;
}

void bz_list_del(bz_list_o** plist)
{
    if(NULL == plist)
        return;

    bz_list_o* list = *plist;
    bz_list_o* temp = NULL;
    if(NULL != list)
    {
        bz_list_u* list_u = list->head;
        bz_list_u* free_u = NULL;
        for(bz_uint i = 0 ; i < list->count ; i++)
        {
            if(BZ_DS_LIST_TYPE == list_u->ds_u.type)
            {
                temp = (bz_list_o*)(list_u->ds_u.data.v);
                bz_list_del(&temp);
            }

            free_u = list_u;
            list_u = list_u->next;
            bz_mem_free(free_u);
        }
        bz_mem_free(list);
        *plist = NULL;
    }
}

bz_int bz_list_append(bz_list_o* list , bz_ds_u ds_u)
{
    if(NULL == list)
        return BZ_FUNC_PARAM_ERROR;

    bz_list_u* list_u = (bz_list_u*)bz_mem_alloc(sizeof(bz_list_u));
    if(NULL == list_u)
        return BZ_MEM_ALLOC_ERROR;

    list_u->ds_u = ds_u;

    if(0 == list->count)
    {
        list_u->pre = NULL;
        list_u->next = NULL;
        list->head = list_u;
        list->tail = list_u;
    }
    else
    {
        list_u->pre = list->tail;
        list_u->next = NULL;
        list->tail->next = list_u;
        list->tail = list_u;
    }

    list->count++;
    return 0;
}

bz_ds_u bz_list_index(bz_list_o* list , bz_uint index)
{
    bz_ds_u ds_u;
    ds_u.type = BZ_DS_NONE_TYPE;
    if(NULL == list)
    {
        bz_errno = BZ_FUNC_PARAM_ERROR;
        return ds_u;
    }
    if(index >= list->count)
    {
        bz_errno = BZ_DSA_INDEX_ERROR;
        return ds_u;
    }

    bz_list_u* list_u;
    bz_uint i;
    for(i = 0 , list_u = list->head ; i < index ; i++ , list_u = list_u->next);

    bz_errno = 0;
    return list_u->ds_u;
}