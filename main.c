#include "bn_struct.h"
#include "test_setting.h"
#include "Basic_Oper_test.h"
#include "Add_Sub_test.h"
#include "Mul_test.h"
#include "Squ_test.h"
#include "Div_test.h"

#include <time.h>

#define ADD_TEST 0 //test addition
#define SUB_TEST 0 //test subtraction
#define MUL_TEST 0 //test multiplication
#define SQU_TEST 0 //test squaring
#define DIV_TEST 1 //test division

int main()
{
	/*
		테스트할 bignum을 랜덤으로 생성
		(테스트하고 싶은 횟수는 "test_setting.h" -> TestNum)
		(배열의 최대 길이는 "test_setting.h" -> MaxArrNum)
	*/

	int i = 0, j = 0;
	int t = 0;

	clock_t tm_start, tm_end;

	srand((unsigned int)time(NULL));
	bigint* A = NULL;
	bigint* B = NULL;
	bigint* C = NULL;

	for (t = 0; t < TestNum; t++) {
		bigint* A = NULL;
		bigint* B = NULL;
		bigint* C = NULL;

		int sign[2] = { Non_Negative, Negative };

		int arr1Num = (rand() % MaxArrNum) + 1;	//rand(): 최대 15비트
		int arr2Num = (rand() % MaxArrNum) + 1;
		int arr3Num = arr2Num - 1;

		//printf("\n############ Basic_Oper_test ############\n");
		//basic_operation_test(); printf("\n");

#if ADD_TEST
		printf("\n############ ADD_test ############\n");
		for (i = 0; i < 2; i++) {
			for (j = 0; j < 2; j++) {
				bi_gen_rand(&A, sign[i], arr1Num);
				bi_gen_rand(&B, sign[j], arr2Num);

				ADD_test(A, B, C);
				ADD_test(B, A, C);
				ADD_test(A, A, C);
			}
		}
#endif
#if SUB_TEST
		printf("\n############ SUB_test ############\n");
		for (i = 0; i < 2; i++) {
			for (j = 0; j < 2; j++) {
				bi_gen_rand(&A, sign[i], arr1Num);
				bi_gen_rand(&B, sign[j], arr2Num);

				SUB_test(A, B, C);
				SUB_test(B, A, C);
				SUB_test(A, A, C);
			}
		}
#endif
#if MUL_TEST
		printf("\n############ MUL_test ############\n");
		for (i = 0; i < 2; i++) {
			for (j = 0; j < 2; j++) {
				bi_gen_rand(&A, sign[i], arr1Num);
				bi_gen_rand(&B, sign[j], arr2Num);

				if ((i == 0) & (j == 0))
					tm_start = clock();
				MUL_test(A, B, C);
				if ((i == 0) & (j == 0)) {
					tm_end = clock();
					printf("##Time during Multiple Operation(%d, %d wordLen): %.3lf초##\n", arr1Num, arr2Num, (double)(tm_end - tm_start) / 1000);
				}
				MUL_test(B, A, C);
			}
		}
#endif
#if SQU_TEST
		printf("\n############ SQU_test ############\n");
		for (i = 0; i < 2; i++) {
			bi_gen_rand(&A, sign[i], arr1Num);

			if (i == 0)
				tm_start = clock();
			SQU_test(A, C);
			if (i == 0) {
				tm_end = clock();
				printf("##Time during Multiple Operation(%d wordLen): %.3lf초##\n", arr1Num, (double)(tm_end - tm_start) / 1000);
			}
		}
#endif
#if DIV_TEST
		for (i = 0; i < 2; i++)
		{
			for (j = 0; j < 2; j++)
			{
				bi_gen_rand(&A, sign[i], arr1Num);
				bi_gen_rand(&B, sign[j], arr2Num);
				bi_gen_rand(&C, Non_Negative, arr3Num);

				if ((i == 0) & (j == 0))
					tm_start = clock();
				DIV_test(B, A, C);
				if ((i == 0) & (j == 0)) {
					tm_end = clock();
					printf("##Time during Multiple Operation(%d, %d wordLen): %.3lf초##\n", arr1Num, arr2Num, (double)(tm_end - tm_start) / 1000);
				}
			}
		}
#endif
	}
	bi_delete(&A);
	bi_delete(&B);
	bi_delete(&C);
	return 0;
}