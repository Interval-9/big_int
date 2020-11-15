#include "bn_struct.h"

void basic_operation_test()
{
	bigint* A = NULL;
	bigint* B = NULL;
	bigint* C = NULL;
	bigint* D = NULL;

	int sign_1 = Non_Negative;
	int sign_2 = Negative;

	word arr[4] = { 0x00,0x00,0x1f,0x00 };
	char* str_1 = "12f";
	char* str_2 = "000100101111";

	int wordLen = sizeof(arr) / sizeof(arr[0]);
	bi_set_by_array(&A, sign_1, arr, wordLen);
	bi_set_by_array(&B, sign_2, arr, wordLen);

	bi_set_by_string(&C, sign_1, str_1, 16);
	bi_set_by_string(&D, sign_2, str_2, 2);

	printf("\n****array to bigint test****\n");
	bi_show(A, 16);   //0x 1f 00 00
	bi_show(B, 2);   //- 0b 00011111 00000000 00000000

	printf("\n****string to bigint test****\n");
	bi_show(C, 2);   //0b 00000001 00101111
	bi_show(D, 16);   //- 0x 01 2f
	bi_show(D, 10);   //- 0d 303

	printf("\n****array init test****\n");
	array_init(D->a, D->wordLen);
	bi_show(D, 16);   //- 0x 00 00

	printf("\n****array copy test****\n");
	array_copy(D->a, C->a, C->wordLen);
	bi_show(D, 16);   //- 0x 01 2f

	printf("\n****bigint copy test****\n");
	bi_assign(&D, A);
	bi_show(D, 16);   //0x 1f 00 00

	printf("\n****random array test****\n");
	array_rand(D->a, D->wordLen);
	bi_show(D, 16);   //0x be 23 29

	printf("\n****random bigint test****\n");
	bi_gen_rand(&D, sign_2, D->wordLen);
	bi_show(D, 16);   //- 0x 6c e1 84

	printf("\n****get word/bit length, jth bit test****\n");
	wordLen = get_word_length(A);
	int bitLen = get_bit_length(A);
	int j_bit = get_jth_bit_of_bi(A, 4);
	printf("%d -- %d -- %d\n", wordLen, bitLen, j_bit);   //3 -- 21 -- 1

	printf("\n****flip bigint sign test****\n");
	int sign = get_bi_sign(D);
	flip_bi_sign(D);
	printf("original sign: %d\n", sign);   //original sign:1 (Negative)
	bi_show(D, 16);   //0x 6c e1 84 (Non-Negative·Î ¹Ù²ñ)

	printf("\n****set one test****\n");
	bi_set_one(&D);
	bi_show(D, 16);   //0x 01

	printf("\n****set zero test****\n");
	bi_set_zero(&D);
	bi_show(D, 16);   //0x 00

	printf("\n****Is_zero, Is_one test****\n");
	int ret_1 = Is_Zero(D);
	int ret_2 = Is_One(D);
	printf("%d -- %d\n", ret_1, ret_2);   //1(True) -- 0(False)

	printf("\n****compare test****\n");
	ret_1 = CompareABS(A, B);
	ret_2 = Compare(A, C);
	printf("%d -- %d\n", ret_1, ret_2);   //0(A==B) -- 1(A>C)

	int r = 9;
	printf("\n****Left_shift test****\n");
	Left_shift(&A, r);
	bi_show(A, 16);   //0x 3e 00 00 00

	printf("\n****Right_shift test****\n");
	bi_show(A, 16);	//0x 3e 00 00 00
	Right_shift(&A, r);
	bi_show(A, 16);	//0x 1f 00 00

	printf("\n****Reduction test****\n");
	bi_show(A, 16);	//0x 1f 00 00
	Reduction(&A, 10);
	bi_show(A, 16);	//0x 00

}