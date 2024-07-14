#include "bz.h"

bz_int bz_errno = 0;

void* bz_mem_alloc(bz_uint size)
{
#if BZ_MEM_ALLOC_MODE == 0
    return malloc(size);
#elif BZ_MEM_ALLOC_MODE == 1
    //此部分进行自定义修改
    return NULL;
#else
    //框架自身内存分配，待编写
    return NULL;
#endif
}

void* bz_mem_calloc(bz_uint nitems , bz_uint size)
{
#if BZ_MEM_ALLOC_MODE == 0
    return calloc(nitems , size);
#elif BZ_MEM_ALLOC_MODE == 1
    //此部分进行自定义修改
    return NULL;
#else
    //框架自身内存分配，待编写
    return NULL;
#endif
}

void* bz_mem_ralloc(void* ptr , bz_uint size)
{
#if BZ_MEM_ALLOC_MODE == 0
    return realloc(ptr ,size);
#elif BZ_MEM_ALLOC_MODE == 1
    //此部分进行自定义修改
    return NULL;
#else
    //框架自身内存分配，待编写
    return NULL;
#endif
}

void* bz_mem_cpy(void* str1 , const void* str2 , bz_uint n)
{
#if BZ_MEM_ALLOC_MODE == 0
    return memcpy(str1 , str2 , n);
#elif BZ_MEM_ALLOC_MODE == 1
    //此部分进行自定义修改
    return NULL;
#else
    //框架自身内存分配，待编写
    return NULL;
#endif
}

void* bz_mem_set(void* str , bz_int c , bz_uint n)
{
#if BZ_MEM_ALLOC_MODE == 0
    return memset(str , c , n);
#elif BZ_MEM_ALLOC_MODE == 1
    //此部分进行自定义修改
    return NULL;
#else
    //框架自身内存分配，待编写
    return NULL;
#endif
}

bz_int bz_mem_cmp(const void* str1 , const void* str2 , bz_uint n)
{
#if BZ_MEM_ALLOC_MODE == 0
    return memcmp(str1 , str2 , n);
#elif BZ_MEM_ALLOC_MODE == 1
    //此部分进行自定义修改
    return 0;
#else
    //框架自身内存分配，待编写
    return 0;
#endif
}

void bz_mem_free(void* ptr)
{
#if BZ_MEM_ALLOC_MODE == 0
    free(ptr);
#elif BZ_MEM_ALLOC_MODE == 1
    //此部分进行自定义修改
    return;
#else
    //框架自身内存分配，待编写
    return;
#endif
}

bz_dbl bz_mh_exp(bz_dbl x)
{
#if BZ_MH_MODE == 1
    return exp(x);
#else
    //此部分进行自定义修改
    return 0;
#endif
}

bz_dbl bz_mh_pow(bz_dbl x , bz_dbl y)
{
#if BZ_MH_MODE == 1
    return pow(x , y);
#else
    //此部分进行自定义修改
    return 0;
#endif
}

bz_dbl bz_mh_sqrt(bz_dbl x)
{
#if BZ_MH_MODE == 1
    return sqrt(x);
#else
    //此部分进行自定义修改
    return 0;
#endif
}

bz_dbl bz_mh_fabs(bz_dbl x)
{
#if BZ_MH_MODE == 1
    return fabs(x);
#else
    //此部分进行自定义修改
    return 0;
#endif
}

/*
  Mersenne Twister 算法
  (w , n , m , r) = (32 , 624 , 397 , 31)
  a = 0x9908B0DF
  (u , d) = (11 , 0xFFFFFFFF)
  (s , b) = (7 , 0x9D2C5680)
  (t , c) = (15 , 0xEFC60000)
  l = 18
  f = 1812433253
*/
static bz_int32 MT[624];
static bz_int32 index = 625;
static const bz_int32 lower_mask = 0x7FFFFFFF;
static const bz_int32 upper_mask = 0x80000000;
void bz_mh_srand(bz_int seed)
{
    index = 624;
    MT[0] = (bz_int32)seed;
    for(bz_uint i = 1 ; i < 624 ; i++)
        MT[i] = 1812433253 * (MT[i-1] ^ (MT[i-1] >> 30)) + i;
}
bz_uint bz_mh_rand()
{
    if(index > 624)
        return 0;
    if(624 == index) //旋转
    {
        bz_int32 x , xA;
        for(bz_uint i = 0 ; i < 624 ; i++) 
        {
            x = (MT[i] & upper_mask) + (MT[(i + 1) % 624] & lower_mask);
            xA = x >> 1;
            if(0 != x % 2)
                xA = xA ^ 0x9908B0DF;
            MT[i] = MT[(i + 397) % 624] ^ xA;
        }
        index = 0;
    }

    bz_int32 y = MT[index];
    y = y ^ (y >> 11);
    y = y ^ ((y << 7) & 0x9D2C5680);
    y = y ^ ((y << 15) & 0xEFC60000);
    y = y ^ (y >> 18);

    index++;

    //限制 随机数 范围
    if(y < 0)
        y = -y;
    y = y % BZ_RAND_MAX;
    return y;
}

void* bz_dev_open(const void* dev , const void* mode)
{
#if BZ_DEV_RW_MODE == 1
    return fopen(dev , mode);
#else
    //此部分进行自定义修改
    return NULL;
#endif
}

void* bz_dev_readonly_open(const void* dev)
{
#if BZ_DEV_RW_MODE == 1
    return fopen(dev , "rb");
#else
    //此部分进行自定义修改
    return NULL;
#endif
}

bz_int bz_dev_close(void* stream)
{
#if BZ_DEV_RW_MODE == 1
    return fclose(stream);
#else
    //此部分进行自定义修改
    return EOF;
#endif
}

bz_uint bz_dev_read(void* ptr , bz_uint size , bz_uint nmemb , void* stream)
{
#if BZ_DEV_RW_MODE == 1
    return fread(ptr , size , nmemb , stream);
#else
    //此部分进行自定义修改
    return 0;
#endif
}

bz_uint bz_dev_write(const void* ptr , bz_uint size , bz_uint nmemb , void* stream)
{
#if BZ_DEV_RW_MODE == 1
    return fwrite(ptr , size , nmemb , stream);
#else
    //此部分进行自定义修改
    return 0;
#endif
}

bz_int bz_dev_error(void* stream)
{
#if BZ_DEV_RW_MODE == 1
    return ferror(stream);
#else
    //此部分进行自定义修改
    return 0;
#endif
}

bz_int bz_dev_eof(void* stream)
{
#if BZ_DEV_RW_MODE == 1
    return feof(stream);
#else
    //此部分进行自定义修改
    return 0;
#endif
}

void bz_dev_clearerr(void* stream)
{
#if BZ_DEV_RW_MODE == 1
    clearerr(stream);
#else
    //此部分进行自定义修改
    return;
#endif
}