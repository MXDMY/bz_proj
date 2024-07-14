#include "bz_mh.h"

bz_int bz_mh_irange_corr(bz_mh_irange* range , bz_uint count)
{
    if(0 == count || NULL == range)
        return BZ_FUNC_PARAM_ERROR;
    
    for(bz_uint i = 0 ; i < count ; i++)
    {
        if(range[i].start > range[i].end)
            range[i].step = range[i].step > 0 ? -(range[i].step) : (0 == range[i].step ? -1 : range[i].step);
        else if(range[i].start < range[i].end)
            range[i].step = range[i].step < 0 ? -(range[i].step) : (0 == range[i].step ? 1 : range[i].step);
        else
            range[i].step = 1;
    }
    return 0;
}