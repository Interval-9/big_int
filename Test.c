/*
	This file containis all test functions for accuracy tests.
*/
#include "bn_struct.h"
#include "test.h"

//Choose the operation you want to test(either basic operation or main functions)
//When testing for basic operation, no other test can be performed at the same time
#define BASIC_TEST 0 //Test basic_operation
#define ADD_TEST 0 //Test addition
#define SUB_TEST 0 //Test subtraction
#define MUL_TEST 0 //Test multiplication
#define SQU_TEST 0 //Test squaring
#define DIV_TEST 1 //Test division
#define MOD_EXP_TEST 0 //Test modular exponentiation

void Random_Test()
{
	int i = 0, j = 0;
	int t = 0;

#if BASIC_TEST
	printf("\n************* Basic_Operation_test *************\n");
	Basic_operation_test();
#endif

#if !BASIC_TEST
	srand((unsigned int)time(NULL));

	for (t = 0; t < TestNum; t++) 
	{
		bigint* A = NULL;
		bigint* B = NULL;
		bigint* C = NULL;
		bigint* D = NULL;

		int sign[2] = { Non_Negative, Negative };

		int arr1Num = (rand() % MaxArrNum) + 1;	
		int arr2Num = (rand() % MaxArrNum) + 1;
		int arr3Num = (rand() % MaxArrNum) + 1;

#if ADD_TEST
		printf("\n************* ADD_test *************\n");
		for (i = 0; i < 2; i++) 
		{
			for (j = 0; j < 2; j++) 
			{
				bi_gen_rand(&A, sign[i], arr1Num);
				bi_gen_rand(&B, sign[j], arr2Num);

				ADD_test(A, B, C);
				ADD_test(B, A, C);
				ADD_test(A, A, C);
			}
		}
#endif
#if SUB_TEST
		printf("\n************* SUB_test *************\n");
		for (i = 0; i < 2; i++) 
		{
			for (j = 0; j < 2; j++) 
			{
				bi_gen_rand(&A, sign[i], arr1Num);
				bi_gen_rand(&B, sign[j], arr2Num);

				SUB_test(A, B, C);
				SUB_test(B, A, C);
				SUB_test(A, A, C);
			}
		}
#endif
#if MUL_TEST
		printf("\n************* MUL_test *************\n");
		for (i = 0; i < 2; i++) 
		{
			for (j = 0; j < 2; j++) 
			{
				bi_gen_rand(&A, sign[i], arr1Num);
				bi_gen_rand(&B, sign[j], arr2Num);
				
				MUL_test(A, B, C);
				MUL_test(B, A, C);
			}
		}
#endif
#if SQU_TEST
		printf("\n************* SQU_test *************\n");
		for (i = 0; i < 2; i++) 
		{
			bi_gen_rand(&A, sign[i], arr1Num);

			SQU_test(A, C);
		}
#endif
#if DIV_TEST
		for (i = 0; i < 1; i++)
		{
			bi_gen_rand(&A, sign[i], arr1Num);
			bi_gen_rand(&B, sign[i], arr2Num);
			bi_gen_rand(&C, Non_Negative, arr2Num);
			while (1) 
			{
				if (Compare(B, C) == 1)
					break;
				bi_gen_rand(&C, Non_Negative, arr2Num);
			}
			DIV_test(B, A, C); //DIV_test(B,Q,R)			
		}
#endif
#if MOD_EXP_TEST
		for (i = 0; i < 1; i++)
		{
			bi_gen_rand(&A, sign[i], arr1Num);
			bi_gen_rand(&B, Non_Negative, arr2Num);
			bi_gen_rand(&D, Non_Negative, arr3Num);

			MOD_EXP_test(A, B, D, C);
		}
#endif // MOD_EXP_TEST

#if DIV_TEST
		bi_delete(&C);
#endif
		bi_delete(&A);
		bi_delete(&B);
		bi_delete(&D);
	}
#endif
}

//Test for Basic Operation function
void Basic_operation_test()
{
	bigint* A = NULL;
	bigint* B = NULL;
	bigint* C = NULL;
	bigint* D = NULL;

	int sign_1 = Non_Negative;
	int sign_2 = Negative;

	word arr[5] = { 0xfc, 0xcb, 0x74, 0x2a, 0x2b };
	char* str_1 = "12f";
	char* str_2 = "111011111111000100000110110111111111";

	int wordLen = sizeof(arr) / sizeof(arr[0]);

	printf("\n*************array to bigint test*************\n");
	bi_set_by_array(&A, sign_1, arr, wordLen);
	bi_set_by_array(&B, sign_2, arr, wordLen);

	printf("A: "); bi_show(A, 16); printf("\n");
	printf("B: "); bi_show(B, 2); printf("\n");

	printf("\n*************string to bigint test*************\n");
	bi_set_by_string(&C, sign_1, str_1, 16);
	bi_set_by_string(&D, sign_2, str_2, 2);

	printf("C: "); bi_show(C, 2); printf("\n");

	printf("D_2: "); bi_show(D, 2); printf("\n");
	printf("D_10: "); bi_show(D, 10); printf("\n");
	printf("D_16: "); bi_show(D, 16); printf("\n");

	printf("\n*************array init test*************\n");
	array_init(D->a, D->wordLen);
	bi_show(D, 16);

	printf("\n*************array copy test*************\n");
	array_copy(D->a, C->a, C->wordLen);
	bi_show(D, 16);

	printf("\n*************bigint copy test*************\n");
	bi_assign(&D, A);
	bi_show(D, 16);

	printf("\n*************random array test*************\n");
	array_rand(D->a, D->wordLen);
	bi_show(D, 16);

	printf("\n*************random bigint test*************\n");
	bi_gen_rand(&D, sign_2, D->wordLen);
	bi_show(D, 16);

	printf("\n*************get word/bit length, jth bit test*************\n");
	wordLen = get_word_length(A);
	int bitLen = get_bit_length(A);
	printf("%d -- %d\n", wordLen, bitLen);

	int j_bit = 0;
	printf("A: "); bi_show(A, 16); printf("\n");
	for (int i = bitLen - 1; i >= 0; i--) {
		j_bit = get_jth_bit_of_bi(A, i);
		printf("%d ", j_bit);
	}printf("\n");

	printf("\n*************flip bigint sign test*************\n");
	int sign = get_bi_sign(D);
	flip_bi_sign(D);
	printf("original sign: %d\n", sign);
	bi_show(D, 16);

	printf("\n*************set one test*************\n");
	bi_set_one(&D);
	bi_show(D, 16);

	printf("\n*************set zero test*************\n");
	bi_set_zero(&D);
	bi_show(D, 16);

	printf("\n*************Is_zero, Is_one test*************\n");
	int ret_1 = Is_Zero(D);
	int ret_2 = Is_One(D);
	printf("%d -- %d\n", ret_1, ret_2);

	printf("\n*************compare test*************\n");
	ret_1 = CompareABS(A, B);
	ret_2 = Compare(A, C);
	printf("%d -- %d\n", ret_1, ret_2);

	int r = 9;
	printf("\n*************Left_shift test*************\n");
	Left_shift(&A, r);
	bi_show(A, 16);

	printf("\n*************Right_shift test*************\n");
	bi_show(A, 16);
	Right_shift(&A, r);
	printf("\n"); bi_show(A, 16);

	printf("\n*************Reduction test*************\n");
	bi_show(A, 16);
	Reduction(&A, 5);
	printf("\n"); bi_show(A, 16);
	printf("\n");

	bi_delete(&A); bi_delete(&B); bi_delete(&C); bi_delete(&D);
}

//Compute C = A + B (A, B: random bigint)
void ADD_test(bigint* A, bigint* B, bigint* C)
{
	printf("A = "); bi_show(A, 16); printf("\n");
	printf("B = "); bi_show(B, 16); printf("\n");

	ADD(&C, A, B);
	printf("print(A + B == ");
	bi_show(C, 16);
	printf(")\n");

	bi_delete(&C);
	printf("\n");
}

//Compute C = A - B (A, B: random bigint)
void SUB_test(bigint* A, bigint* B, bigint* C)
{
	printf("A = "); bi_show(A, 16); printf("\n");
	printf("B = "); bi_show(B, 16); printf("\n");

	SUB(&C, A, B);
	printf("print(A - B == ");
	bi_show(C, 16);
	printf(")\n");

	bi_delete(&C);
	printf("\n");
}

//Compute C = A * B (A, B: random bigint)
void MUL_test(bigint* A, bigint* B, bigint* C)
{
	printf("A = "); bi_show(A, 16); printf("\n");
	printf("B = "); bi_show(B, 16); printf("\n");

	MUL(&C, A, B);
	
	printf("print(A * B == ");
	bi_show(C, 16);
	printf(")\n");

	bi_delete(&C);
	printf("\n");
}

//Compute C = A * A (A: random bigint)
void SQU_test(bigint* A, bigint* C)
{
	printf("A = "); bi_show(A, 16); printf("\n");

	SQU(&C, A);
	
	printf("print(A * A == ");
	bi_show(C, 16);
	printf(")\n");

	bi_delete(&C);
	printf("\n");
}

/*
	> Input: B, Q, R
	> compute A = B*Q + R
	> Long_Div(A, B) -> quotient: Qtmp, remainder: Rtmp
*/
void DIV_test(bigint* B, bigint* Q, bigint* R)
{
	bigint* BQ = NULL;
	bigint* A = NULL;
	bigint* Qtmp = NULL;
	bigint* Rtmp = NULL;

	printf("B = "); bi_show(B, 16); printf("\n");
	printf("Q = "); bi_show(Q, 16); printf("\n");
	printf("R = "); bi_show(R, 16); printf("\n");

	MUL(&BQ, B, Q);
	ADD(&A, BQ, R);
	printf("A = "); bi_show(A, 16); printf("\n");

	DIV(&Qtmp, &Rtmp, A, B);
	
	printf("Qtmp = "); bi_show(Qtmp, 16); printf("\n");
	printf("Rtmp = "); bi_show(Rtmp, 16); printf("\n");

	printf("print(Qtmp == Q)\n");
	printf("print(Rtmp == R)\n");

	bi_delete(&BQ);
	bi_delete(&A);
	bi_delete(&Qtmp);
	bi_delete(&Rtmp);
}

//Compute C = x^n mod N (x, n, N: random bigint)
void MOD_EXP_test(bigint* x, bigint* n, bigint* M, bigint* C)
{
	printf("x= "); bi_show(x, 16); printf("\n");
	printf("n= "); bi_show(n, 16); printf("\n");
	printf("M= "); bi_show(M, 16); printf("\n");

	MOD_EXP(&C, x, n, M);
	
	printf("print((x^n)%%M == ");
	bi_show(C, 16);
	printf(")\n");

	bi_delete(&C);
}