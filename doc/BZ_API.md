# 代码结构

源码主要位于 **bz** 与 **bz_api** 目录，**baize.h** 仅用于头文件包含，其中 **bz** 为核心源码，提供了原始的 API，**bz_api** 源码则主要对原始的 API 进行封装，以提高更加方便的 **用户级 API**，若不需要 **用户级 API**，仅复制 **bz** 目录到目的工程即可。**bz** 与 **bz_api** 内代码采用自上而下结构，如下所示（目录加粗，注释斜体）：

---

**bz**
|-- bz_conf.h 💡*设置框架的基础行为*
|-- bz.\* 💡*框架的基础，包括全局错误号、内存分配、初等数学函数等等*
|-- **dsa** 💡***数据结构与算法模块***
    |-- bz_dsa.h 💡*定义或声明该部分的基础*
    |-- bz_sort.c 💡*排序算法实现*
    \`-- bz_list.\* 💡*列表相关声明和定义*
|-- **fmt** 💡***格式（图像、音频等）处理模块***
    |-- bz_fmt.h 💡*定义或声明该部分的基础*
    \`-- bz_img.\* 💡*图像处理相关声明和定义*
|-- **mh** 💡***数学运算模块***
    |-- bz_mh.\* 💡*定义或声明该部分的基础*
    \`-- bz_tensor.\* 💡*张量运算相关声明和定义*
\`-- **nn** 💡***神经网络模块***
    |-- bz_nn.\* 💡*定义或声明该部分的基础*
    \`-- bz_fc.\* 💡*全连接层相关声明和定义*

---

**bz_api**
|-- bz_api.\* 💡*提供 用户级 API 的基础*
\`-- bz_\*.c 💡*各模块 API 接口再封装*

---

1. 在 **bz** 中，**bz_conf.h** 与 **bz.\*** 提供了整个框架的基础，其中 **bz_conf.h** 定义了整个框架的行为，其余子目录则为各个功能模块的实现。在每个模块中，都有一个或一对文件提供该模块的基础，例如在该模块中所使用的局部错误号、结构体数据等等，这一类文件命名遵循模板 **bz_模块目录名.\***。
2. **bz_api** 中的 **用户级 API** 是通过函数指针形式来调用的，这一类函数指针被统一地封装到特定的结构体里，简称为 **函数集**。

# API

对于 **原始 API** 与 **用户 API** 的调用区别，以张量操作为例，下面将创建一个 3x3x3 张量，tucker 分解之后再通过连续 模n积 还原，最后释放所有内存:

```c
/* 原始 API 代码示例 */
bz_uint darr[3] = {3 , 3 , 3};
bz_tensor_o* tsr = bz_tensor_create(darr , 3 , BZ_DOUBLE_TYPE);
bz_tensor_o** r = bz_tensor_tucker(tsr , 0.0);

bz_tensor_o* m[3];
m[0] = bz_tensor_mode_n_product(r[0] , r[1] , 1);
m[1] = bz_tensor_mode_n_product(m[0] , r[2] , 2);
m[2] = bz_tensor_mode_n_product(m[1] , r[3] , 3);

bz_tensor_del(&tsr);
for(bz_uint i = 0 ; i < 4 ; i++)
    bz_tensor_del(r + i);
bz_mem_free(r);
for(bz_uint i = 0 ; i < 3 ; i++)
    bz_tensor_del(m + i);
```

```c
/* 用户 API 代码示例 */
__bz bz = bz_api_init();
__bz_tensor t = bz.tensor;

bz_tensor_o* tsr = t.create("[3 , 3 , 3]" , BZ_DOUBLE_TYPE);
bz_tensor_o** r = t.tucker(tsr , 0.0);

bz_tensor_o* m[3];
m[0] = t.mode_n_x(r[0] , r[1] , 1);
m[1] = t.mode_n_x(m[0] , r[2] , 2);
m[2] = t.mode_n_x(m[1] , r[3] , 3);

t.del(&tsr);
for(bz_uint i = 0 ; i < 4 ; i++)
    t.del(r + i);
bz.free(r);
for(bz_uint i = 0 ; i < 3 ; i++)
    t.del(m + i);
```

**用户 API** 的实现主要通过对部分函数再次封装以及函数指针，简化函数参数以及函数名长度，且函数指针名与原函数名一一对应：

```c
/* 张量函数集 */
typedef struct __bz_api_tensor_func_s
{
    ...
    void (*del)(bz_tensor_o**); //对应 bz_tensor_del
    ...
    bz_tensor_o* (*create)(char* , bz_mh_dtype); //对应 bz_tensor_create
    ...
}__bz_tensor;

/* 总函数集 */
typedef struct __bz_api_func_s
{
    ...
    __bz_tensor tensor;
    ...
    void (*free)(void*); //对应 bz_mem_free
    ...
}__bz;
```

### 目录

[数据结构与算法](./DSA_API.md "DSA_API.md")
[格式处理](./FMT_API.md "FMT_API.md")
[数学运算](./MH_API.md "MH_API.md")
[神经网络](./NN_API.md "NN_API.md")
