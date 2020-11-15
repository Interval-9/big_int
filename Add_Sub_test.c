#include "bn_struct.h"

void ADD_test(bigint* A, bigint*B, bigint* C)
{
	
	int ret = 0;

	printf("A = "); bi_show(A, 16); printf("\n");
	printf("B = "); bi_show(B, 16); printf("\n");

	ret = ADD(&C, A, B);
	if (ret != -1)
	{
		printf("print(A + B == ");
		bi_show(C, 16);
	}printf(")\n");

	ret = ADD(&C, B, A);
	if (ret != -1)
	{
		printf("print(B + A == ");
		bi_show(C, 16);
	}printf(")\n");

	ret = ADD(&C, A, A);
	if (ret != -1)
	{
		printf("print(A + A == ");
		bi_show(C, 16);
	}printf(")\n");

	printf("\n");
}

void SUB_test(bigint* A, bigint* B, bigint* C)
{
	
	int ret = 0;

	printf("A = "); bi_show(A, 16); printf("\n");
	printf("B = "); bi_show(B, 16); printf("\n");

	ret = SUB(&C, A, B);
	if (ret != -1)
	{
		printf("print(A - B == ");
		bi_show(C, 16);
	}printf(")\n");

	ret = SUB(&C, B, A);
	if (ret != -1)
	{
		printf("print(B - A == ");
		bi_show(C, 16);
	}printf(")\n");

	ret = SUB(&C, A, A);
	if (ret != -1)
	{
		printf("print(A - A == ");
		bi_show(C, 16);
	}printf(")\n"); 
	
	printf("\n");
}