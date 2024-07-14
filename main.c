#include "baize.h"

#include "stdio.h"

int main(int argc , char* argv[])
{
	__bz bz = bz_api_init();
	__bz_tensor t = bz.tensor;

	bz.srand(105);
	t.print(t.create_rand("[3 , 3 , 3]" , -10 , 10 , BZ_INT_TYPE));

#if 0
	FILE* fp = fopen("C:\\Users\\16281\\PycharmProjects\\AI\\img.rgb" , "rb");
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp , 0 , SEEK_SET);

	bz_dbl* data = (bz_dbl*)bz_mem_alloc(size * sizeof(bz_dbl));
	bz_u8 byte;
	for(bz_uint i = 0 ; i < size ; i++)
	{
		fread(&byte , 1 , 1 , fp);
		data[i] = byte / 255.0;
	}
	
	fclose(fp);

	bz_tensor_o* tsr = t.create("[457 , 341 , 3]" , BZ_DOUBLE_TYPE);
	t.get(tsr , data);
	bz_tensor_o** r = t.tucker(tsr , 0.7);
	printf("%d" , r[0]->total_count + r[1]->total_count + r[2]->total_count , r[3]->total_count);

	bz_dbl* new_data = t.mode_n_x(t.mode_n_x(t.mode_n_x(r[0] , r[1] , 1) , r[2] , 2) , r[3] , 3)->data;

	fp = fopen("C:\\Users\\16281\\PycharmProjects\\AI\\img.rgb" , "wb");

	for(bz_uint i = 0 ; i < size ; i++)
	{
		byte = new_data[i] * 255;
		fwrite(&byte , 1 , 1 , fp);
	}

	fclose(fp);
#endif
}