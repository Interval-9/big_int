#include "bn_struct.h"

/*
	>
	> Input:
	> Output:
*/
void DIV_test(bigint* B, bigint* Q, bigint* R)
{
	int ret1 = 0, ret2 = 0, ret3 = 0;
	bigint* A = NULL; bigint* QB = NULL; bigint* Qtmp = NULL; bigint* Rtmp = NULL;

	//printf("Q = "); bi_show(Q, 16); printf("\n");
	printf("B = "); bi_show(B, 16); printf("\n");
	//printf("R = "); bi_show(R, 16); printf("\n");

	ret1 = MUL(&QB, Q, B);
	ret2 = ADD(&A, QB, R);
	ret3 = Long_Div(&Qtmp, &Rtmp, A, B);
	
	if (ret3 != -1)
	{
		printf("Q = ");
		bi_show(Qtmp, 16); printf("\n");
		printf("R = ");
		bi_show(Rtmp, 16); printf("\n");
	}
	if ((ret1 != -1) & (ret2 != -1))
	{
		printf("print((Q * B + R) == ");
		bi_show(A, 16);
	}printf(")\n");
	printf("\n");
	bi_delete(&Rtmp); bi_delete(&Qtmp);
	bi_delete(&A); bi_delete(&QB);
}