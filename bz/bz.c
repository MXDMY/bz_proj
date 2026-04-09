#include "bz.h"

const bz_uint bz_dtype_size_table[BZ_DTYPE_END] = {sizeof(bz_int),
                                                   sizeof(bz_flt),
                                                   sizeof(bz_int8),
                                                   sizeof(bz_int16),
                                                   sizeof(bz_int32)};

bz_int bz_errno = 0;

#ifndef bz_mem_alloc
void* bz_mem_alloc(bz_uint size)
{
    (void)size;
    return NULL;
}
#endif

#ifndef bz_mem_calloc
void* bz_mem_calloc(bz_uint nmemb, bz_uint size)
{
    (void)nmemb;
    (void)size;
    return NULL;
}
#endif

#ifndef bz_mem_ralloc
void* bz_mem_ralloc(void* ptr, bz_uint size)
{
    (void)ptr;
    (void)size;
    return NULL;
}
#endif

#ifndef bz_mem_cpy
void* bz_mem_cpy(void* dest, const void* src, bz_uint n)
{
    (void)dest;
    (void)src;
    (void)n;
    return NULL;
}
#endif

#ifndef bz_mem_set
void* bz_mem_set(void* s, bz_int c, bz_uint n)
{
    (void)s;
    (void)c;
    (void)n;
    return NULL;
}
#endif

#ifndef bz_mem_cmp
bz_int bz_mem_cmp(const void* s1, const void* s2, bz_uint n)
{
    (void)s1;
    (void)s2;
    (void)n;
    return -1;
}
#endif

#ifndef bz_mem_free
void bz_mem_free(void* ptr)
{
    (void)ptr;
    return;
}
#endif

#ifndef bz_mh_exp
bz_flt bz_mh_exp(bz_flt x)
{
    (void)x;
    return 0.0;
}
#endif

#ifndef bz_mh_pow
bz_flt bz_mh_pow(bz_flt x, bz_flt y)
{
    (void)x;
    (void)y;
    return 0.0;
}
#endif

#ifndef bz_mh_sqrt
bz_flt bz_mh_sqrt(bz_flt x)
{
    (void)x;
    return 0.0;
}
#endif

#ifndef bz_mh_fabs
bz_flt bz_mh_fabs(bz_flt x)
{
    (void)x;
    return 0.0;
}
#endif

#ifndef bz_mh_srand
/*
 * Mersenne Twister 算法
 * (w , n , m , r) = (32 , 624 , 397 , 31)
 * a = 0x9908B0DF
 * (u , d) = (11 , 0xFFFFFFFF)
 * (s , b) = (7 , 0x9D2C5680)
 * (t , c) = (15 , 0xEFC60000)
 * l = 18
 * f = 1812433253
*/
static bz_int32 MT[624];
static bz_int32 index = 625;
static const bz_int32 lower_mask = 0x7FFFFFFF;
static const bz_int32 upper_mask = (bz_int32)0x80000000;

void bz_mh_srand(bz_uint seed)
{
    index = 624;
    MT[0] = (bz_int32)seed;
    for (bz_int i = 1; i < 624; i++)
        MT[i] = 1812433253 * (MT[i - 1] ^ (MT[i - 1] >> 30)) + i;
}

bz_int bz_mh_rand(void)
{
    if (index > 624)
        return 0;
    if (624 == index) // 旋转
    {
        bz_int32 x, xA;
        for (bz_int i = 0; i < 624; i++) 
        {
            x = (MT[i] & upper_mask) + (MT[(i + 1) % 624] & lower_mask);
            xA = x >> 1;
            if (x & 1 /* 0 != x % 2 */)
                xA = xA ^ (bz_int32)0x9908B0DF;
            MT[i] = MT[(i + 397) % 624] ^ xA;
        }
        index = 0;
    }

    bz_int32 y = MT[index];
    y = y ^ (y >> 11);
    y = y ^ ((y << 7) & (bz_int32)0x9D2C5680);
    y = y ^ ((y << 15) & (bz_int32)0xEFC60000);
    y = y ^ (y >> 18);

    index++;

    // 限制随机数范围
    if (y < 0)
        y = -y;
    y = y % BZ_RAND_MAX;
    return y;
}
#endif

#ifndef bz_fs_open
void* bz_fs_open(const char* path, const char* mode)
{
    (void)path;
    (void)mode;
    return NULL;
}
#endif

#ifndef bz_fs_close
bz_int bz_fs_close(void* stream)
{
    (void)stream;
    return -1;
}
#endif

#ifndef bz_fs_read
bz_uint bz_fs_read(void* ptr, bz_uint size, bz_uint nmemb, void* stream)
{
    (void)ptr;
    (void)size;
    (void)nmemb;
    (void)stream;
    return 0;
}
#endif

#ifndef bz_fs_write
bz_uint bz_fs_write(const void* ptr, bz_uint size, bz_uint nmemb, void* stream)
{
    (void)ptr;
    (void)size;
    (void)nmemb;
    (void)stream;
    return 0;
}
#endif

#ifndef bz_fs_error
bz_int bz_fs_error(void* stream)
{
    (void)stream;
    return -1;
}
#endif

#ifndef bz_fs_eof
bz_int bz_fs_eof(void* stream)
{
    (void)stream;
    return -1;
}
#endif

#ifndef bz_fs_clearerr
void bz_fs_clearerr(void* stream)
{
    (void)stream;
    return;
}
#endif

