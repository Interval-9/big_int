#include "bn_struct.h"

void SQU_test(bigint* A, bigint* C)
{
	int ret = 0;

	printf("A = "); bi_show(A, 16); printf("\n");

	ret = SQU(&C, A);
	if (ret != -1)
	{
		printf("print(A * A == ");
		bi_show(C, 16);
	}printf(")\n");

	printf("\n");
}