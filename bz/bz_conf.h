/***********************************************************************
 作者：中国-非 985 非 211 普本-非全日制-**
 
                     ##                 ##      ############        
                    ##                   ##       ##    ##
                   ##                     ##        ####
            ################                       ##  ##
            ##            ##            ##       ##      ##
            ##            ##             ##          ##
            ##            ##              ##      ########
            ################                         ##
            ##            ##              ##   ##############
            ##            ##             ##          ##
            ##            ##            ##           ##
            ################           ##            ##

 函数库名称：白泽
 命名来源：白泽，为华夏古代神话故事中的一种神兽，能人言，聪慧，并通达知晓万物之情理。

          东晋《抱朴子‧极言》：穷神奸则记白泽之辞。
          南朝梁《宋书·志·卷二十九·符瑞下》：泽兽，黄帝时巡狩至于东滨，泽兽出，能言，
                            达知万物之精，以戒于民，为时除害。贤君明德幽远则来。

***********************************************************************/
/***********************************************************************
 配置
***********************************************************************/
#ifndef BZ_CONF_H
#define BZ_CONF_H

#define BZ_USE_C_STD_LIB 1
#if BZ_USE_C_STD_LIB

// 注释掉下面任何一种定义将会导致对应函数采用 bz.h/c 中的实现

#define bz_mem_alloc  malloc
#define bz_mem_calloc calloc
#define bz_mem_ralloc realloc
#define bz_mem_cpy    memcpy
#define bz_mem_set    memset
#define bz_mem_cmp    memcmp
#define bz_mem_free   free

#define bz_mh_exp   expl
#define bz_mh_pow   powl
#define bz_mh_sqrt  sqrtl
#define bz_mh_fabs  fabsl

#define bz_mh_srand srand
#ifdef bz_mh_srand
#define bz_mh_rand  rand
#define BZ_RAND_MAX RAND_MAX
#endif

#define bz_fs_open       fopen
#define bz_fs_close      fclose
#define bz_fs_read       fread
#define bz_fs_write      fwrite
#define bz_fs_error      ferror
#define bz_fs_eof        feof
#define bz_fs_clearerr   clearerr

#endif

// 定长整型
typedef signed char    bz_int8;
typedef unsigned char  bz_uint8;
typedef signed short   bz_int16;
typedef unsigned short bz_uint16;
typedef signed int     bz_int32;
typedef unsigned int   bz_uint32;
// 单、双、长双精度浮点
typedef float          bz_flo;
typedef double         bz_dbl;
typedef long double    bz_ldbl;
// 不固定
typedef bz_dbl         bz_flt;
typedef bz_int32       bz_int;
typedef bz_uint32      bz_uint;

// 浮点数精度
#define BZ_FPP 1e-9

#ifndef BZ_RAND_MAX
#define BZ_RAND_MAX 65536
#endif

#endif
