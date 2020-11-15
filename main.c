#include "bn_struct.h"
#include "test_setting.h"
#include "Basic_Oper_test.h"
#include "Add_Sub_test.h"
#include "Mul_test.h"

#include <time.h>

int main()
{
	/*
		테스트할 bignum을 랜덤으로 생성 
		(테스트하고 싶은 횟수는 "test_setting.h" -> TestNum)
		(배열의 최대 길이는 "test_setting.h" -> MaxArrNum)

		//주석은 배열길이가 고정되었을 때의 테스트코드
	*/

	int i = 0, j = 0;
	int t = 0;

	clock_t tm_start, tm_end;

	srand((unsigned int)time(NULL));

	for (t = 0; t < TestNum; t++) {
		bigint* A = NULL;
		bigint* B = NULL;
		bigint* C = NULL;

		int sign[2] = { Non_Negative, Negative };

		int arr1Num = 400;//(rand() % MaxArrNum) + 1;	//rand(): 최대 15비트
		int arr2Num = 400;//(rand() % MaxArrNum) + 1;

		//int arr1Num = 2;
		//int arr2Num = 3;
		//
		//word arr1[2] = { 0xbe, 0x84 };
		//word arr2[3] = { 0xe1,0x6c,0xd6 };

		//word arr1[2] = { 0xbebe, 0x8484 };
		//word arr2[3] = { 0xe1e1,0x6c6c,0xd6d6 };

		word* arr1 = NULL;
		word* arr2 = NULL;

		arr1 = (word*)calloc(arr1Num, sizeof(word));
		if (arr1 == NULL)
			return -1;
		arr2 = (word*)calloc(arr2Num, sizeof(word));
		if (arr2 == NULL)
			return -1;

#if WORD_BITLEN == 8
		for (i = 0; i < arr1Num; i++)
			arr1[i] = rand() % 0x100;
		for (i = 0; i < arr2Num; i++)
			arr2[i] = rand() % 0x100;

#elif WORD_BITLEN == 32
		for (i = 0; i < arr1Num; i++)
			arr1[i] = (rand() << 17) | (rand() << 2) | rand() % 0x4;
		for (i = 0; i < arr2Num; i++)
			arr2[i] = (rand() << 17) | (rand() << 2) | rand() % 0x4;

#elif WORD_BITLEN == 64
		for (i = 0; i < arr1Num; i++)
			arr1[i] = (rand() << 49) | (rand() << 34) | (rand() << 19) | (rand() << 4) | (rand() % 0x100);
		for (i = 0; i < arr2Num; i++)
			arr2[i] = (rand() << 49) | (rand() << 34) | (rand() << 19) | (rand() << 4) | (rand() % 0x100);
#endif

		/////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////

		//printf("\n############ Basic_Oper_test ############\n");
		//basic_operation_test(); printf("\n");

		//printf("\n############ ADD_test ############\n");
		//for (i = 0; i < 2; i++) {
		//	for (j = 0; j < 2; j++) {
		//		bi_set_by_array(&A, sign[i], arr1, arr1Num);
		//		bi_set_by_array(&B, sign[j], arr2, arr2Num);

		//		ADD_test(A, B, C);
		//	}
		//}

		//printf("\n############ SUB_test ############\n");
		//for (i = 0; i < 2; i++) {
		//	for (j = 0; j < 2; j++) {
		//		bi_set_by_array(&A, sign[i], arr1, arr1Num);
		//		bi_set_by_array(&B, sign[j], arr2, arr2Num);

		//		SUB_test(A, B, C);
		//	}
		//}

		printf("\n############ MUL_test ############\n");
		for (i = 0; i < 2; i++) {
			for (j = 0; j < 2; j++) {
				bi_set_by_array(&A, sign[i], arr1, arr1Num);
				bi_set_by_array(&B, sign[j], arr2, arr2Num);

				if ((i == 0) & (j == 0))
					tm_start = clock();
				MUL_test(A, B, C);
				if ((i == 0) & (j == 0)) {
					tm_end = clock();
					printf("##Time during Multiple Operation(%d, %d wordLen): %.3lf초##\n", arr1Num, arr2Num, (double)(tm_end - tm_start) / 1000);
				}
			}
		}

		free(A);
		free(B);
		free(C);
		free(arr1);
		free(arr2);
	}
	return 0;
}