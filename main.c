#include <stdio.h>

#include "./bz/tensor/bz_tensor.h"

void bz_tensor_print_info(bz_tensor* t)
{
	if(t)
	{
		printf("shape   : [");
		for(bz_uint i = 0 ; i < t->ndim - 1 ; i++)
			printf("%u , " , t->shape[i]);
		printf("%u]\n" , t->shape[t->ndim - 1]);

		printf("offsets : [");
		for(bz_uint i = 0 ; i < t->ndim - 1 ; i++)
			printf("%u , " , t->offsets[i]);
		printf("%u]\n" , t->offsets[t->ndim - 1]);

		printf("ndim    : %u\n" , t->ndim);
		printf("count   : %u\n" , t->count);
		printf("dtype   : %d\n" , t->dtype);
		printf("itemsize: %u\n" , t->itemsize);
	}
}

int main(int argc , char* argv[])
{
	(void)argc;
	(void)argv;

#define NDIM 4
	bz_uint shape[NDIM] = {5 , 11 , 2 , 7};

	getchar();

	bz_tensor* t = bz_tensor_create(shape , NDIM , BZ_INT_TYPE , NULL);
	bz_tensor_print_info(t);

	getchar();

	bz_tensor_delete(&t);
	if(NULL == t)
		printf("free ! \n");

	getchar();

	shape[1] = 15;
	t = bz_tensor_create(shape , NDIM , BZ_INT_TYPE , NULL);
	bz_tensor_print_info(t);

	getchar();

	bz_tensor_delete(&t);
	if(NULL == t)
		printf("free ! \n");

	getchar();

	return 0;
}
