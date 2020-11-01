#include "bn_struct.h"

//워드 단위 덧셈 연산을 수행하는 함수
//캐리 비트(0 또는 1)를 반환
int ADD_ABc(word* Cj, word Aj, word Bj, int cb)
{
	int c = 0; //반환할 캐리 비트 변수 선언

	(*Cj) = (word)(Aj + Bj); //Cj = (Aj + Bj) mod word

	if (*Cj < Aj) //캐리 발생
		c = 1;
	
	(*Cj) = (word)((*Cj) + cb); //그 전 캐리와 덧셈
	if (*Cj < cb) //캐리 발생
		c = 1;

	return c;
}

//부호가 같은 두 정수의 덧셈 연산 함수 (wordLen(A) >= wordLen(B)인 경우)
//동적 할당에 실패할 경우: -1, 성공할 경우: 0을 반환
int ADDC(bigint** C, bigint* A, bigint* B) 
{
	int max_wordLen = (A->wordLen); //C의 워드 길이를 계산
	bi_new(C, max_wordLen + 1); //결과값을 저장할 big_int 구조체 생성

	int c = 0, j = 0; //캐리를 0으로 초기화
	word* Cp = (*C)->a; word* Bp = B->a; //주소 접근을 위한 포인터 선언

	Bp = (word*)realloc(Bp, sizeof(word) * max_wordLen); //B에 메모리 재할당
	if (Bp == NULL) //동적할당 실패시 함수 종료
		return -1;
	
	array_init(((B->a) + (B->wordLen)), (max_wordLen - (B->wordLen)));// Bm ~ Bn-1까지 0으로 초기화
	 
	for (j = 0;j < max_wordLen;j++) //워드 단위 덧셈 연산
	{
		c = ADD_ABc(Cp + j, (A->a)[j], (B->a)[j], c); //C, c <- (A[j] + B[j] + c)
		//printf("Aj = %02x, Bj = %02x\n", (A->a)[j], (B->a)[j]); 
	}

	if (c == 1) //최상위 워드 연산 결과, 캐리가 남아있을 경우
		Cp[max_wordLen] = 1;
	else //캐리가 남아있지 않을 경우 0인 부분을 없애줌
		bi_refine(*C);

	(*C)->sign = A->sign; //C의 부호를 정함
	return 0;
}

//두 정수 A, B의 합을 계산하는 함수
//성공 여부를 반환함 (성공: 0, 실패: -1)
int ADD(bigint** C, bigint* A, bigint* B)
{
	int ret = 0; 

	if (Is_Zero(A) == 1) //A가 0일 경우, C <- B
	{
		bi_assign(C, B);
		return 0;
	}

	else if (Is_Zero(B) == 1) //B가 0일 경우, C <- A
	{
		bi_assign(C, A);
		return 0;
	}

	else if ((A->sign == 0) & (B->sign == 1)) // A > 0, B < 0
	{
		if (CompareABS(A, B) == -1) // |A| < |B|, A + B = |A| - |B| < 0
		{
			ret = SUBC(C, B, A);
			(*C)->sign = Negative; 
			return ret;
		}
		else // |A| > |B|, A + B = |A| - |B| > 0
			return SUBC(C, A, B);
	}

	else if ((A->sign == 1) & (B->sign == 0)) // A < 0, B > 0
	{
		if (CompareABS(A, B) == 1) // |A| > |B|, A + B = |A| - |B| < 0
		{
			ret = SUBC(C, A, B);
			(*C)->sign = Negative;
			return ret;
		}
		else // |A| < |B|, A + B = |A| - |B| > 0
			return SUBC(C, B, A);
	}

	else if (A->wordLen >= B->wordLen) //A의 워드길이가 더 긴 경우
		return ADDC(C, A, B);

	else //B의 워드길이가 더 긴 경우
		return ADDC(C, B, A);
}

//워드 단위 뺄셈 연산을 수행하는 함수
//캐리 비트(0 또는 1)을 반환
int SUB_AbB(word* Cj, word Aj, int bb, word Bj)
{
	int b = 0; //반환할 캐리 비트 선언

	(*Cj) = (word)(Aj - bb);

	if (Aj < bb)
		b = 1;
	
	if ((*Cj) < Bj)
		b = 1;

	(*Cj) = (word)((*Cj) - Bj);
	
	return b;
}

//두 양의 정수의 뺄셈 연산을 수행하는 함수 (A >= B인 경우)
//동적 할당에 실패할 경우: -1, 성공할 경우: 0을 반환
int SUBC(bigint** C, bigint* A, bigint* B)
{
	int max_wordLen = (A->wordLen); //C의 워드 길이를 계산
	bi_new(C, max_wordLen); //결과값을 저장할 big_int 구조체 생성

	int b = 0, j = 0, tmp = 0; //캐리를 0으로 초기화
	word* Cp = (*C)->a; word* Bp = B->a;

	Bp = (word*)realloc(Bp, sizeof(word) * max_wordLen); //B에 메모리 재할당
	if (Bp == NULL) //동적할당 실패시 함수 종료
		return -1;

	array_init(((B->a) + (B->wordLen)), (max_wordLen - (B->wordLen)));// Bm ~ Bn-1까지 0으로 초기화

	for (j = 0;j < max_wordLen;j++) //워드 단위 덧셈 연산
	{
		b = SUB_AbB(Cp + j, (A->a)[j], b, (B->a)[j]); //C, b <- (A - B - b)
		//printf("carry = %d\n", b);
		//printf("Aj = %llx, Bj = %llx, Cj = %llx\n", (A->a)[j], (B->a)[j], *(Cp + j));
	}
	bi_refine(*C);
	return 0;
}

//두 정수 A, B의 뺄셈 연산을 수행하는 함수
//성공 여부를 반환함 (성공: 0, 실패: -1)
int SUB(bigint** C, bigint* A, bigint* B)
{
	int ret = 0;

	if (Is_Zero(A) == 1) //A가 0인 경우, C <- (-B)
	{
		bi_assign(C, B);
		(*C)->sign = (B->sign) * (-1);
		return 0;
	}
	else if (Is_Zero(B) == 1) //B가 0인 경우, C <- A
	{
		bi_assign(C, A);
		return 0;
	}
	else if (!Compare(A, B))
	{
		bi_set_zero(C);
		return 0;
	}
	else if ((A->sign == 0) & (B->sign == 0)) //둘 다 양수
	{
		if (Compare(A, B) == 1) //A > B > 0
			return SUBC(C, A, B);

		else if (Compare(A, B) == -1) //B > A > 0
		{
			ret = SUBC(C, B, A);
			(*C)->sign = Negative;
			return ret;
		}
	}
	else if ((A->sign == 1) & (B->sign == 1)) //둘 다 음수
	{
		if (Compare(A, B) == 1) //0 > A > B
			return SUBC(C, B, A);

		else if (Compare(A, B) == -1) //0 > B > A
		{
			ret = SUBC(C, A, B);
			(*C)->sign = Negative;
		}
	}
	else if ((A->sign == 0) & (B->sign == 1)) //A > 0 , B < 0
	{
		if (A->wordLen > B->wordLen) // A - B = A - (-|B|) = A + |B| > 0 
			ret = ADDC(C, A, B);
		else
			ret = ADDC(C, B, A);
		(*C)->sign = Non_Negative;
		return ret;
	}

	else if ((A->sign == 1) & (B->sign == 0)) //A < 0 , B > 0
	{
		if (A->wordLen > B->wordLen) // A - B = (-|A|) - B = -(|A| + B) < 0 
			ret = ADDC(C, A, B);
		else
			ret = ADDC(C, B, A);
		(*C)->sign = Negative;
		return ret;
	}
	return 0;
}

void ADD_test() //덧셈 연산 테스트 함수
{
	bigint* A = NULL;
	bigint* B = NULL;
	bigint* C = NULL;

	int ret = 0;

#if WORD_BITLEN == 8
	word arr_b[4] = { 0x78,0x56,0x34,0x12 };
	word arr_s[5] = { 0xa9,0xb8,0xc7,0xd6,0xe5 };
#elif WORD_BITLEN == 32
	word arr_b[4] = { 0x7878,0x5656,0x3434,0x1212 };
	word arr_s[4] = { 0xefef,0xcdcd,0xabab,0x0909 };
#elif WORD_BITLEN == 64
	word arr_b[4] = { 0x787878787878,0x565600005656,0x343400003434,0x121200001212 };
	word arr_s[4] = { 0xefef0000efef,0xcdcd0000cdcd,0xabab0000abab,0x090900000909 };
#endif
	printf("****Test big integer addition (wordlen = %d)****\n\n", WORD_BITLEN);
	bi_set_by_array(&A, Non_Negative, arr_b, 4);
	bi_set_by_array(&B, Non_Negative, arr_s, 5);

	//test A>0 and B>0
	printf("A = "); bi_show(A, 16);
	printf("B = "); bi_show(B, 16);

	ret = ADD(&C, A, B);
	if (ret != -1)
	{
		printf("A + B = ");
		bi_show(C, 16);
	}

	ret = ADD(&C, B, A);
	if (ret != -1)
	{
		printf("B + A = ");
		bi_show(C, 16);
	}

	ret = ADD(&C, A, A);
	if (ret != -1)
	{
		printf("A + A = ");
		bi_show(C, 16);
	}printf("\n");

	//test A<0 and B<0
	A->sign = Negative; B->sign = Negative;
	printf("A = "); bi_show(A, 16);
	printf("B = "); bi_show(B, 16);

	ret = ADD(&C, A, B);
	if (ret != -1)
	{
		printf("A + B = ");
		bi_show(C, 16);
	}

	ret = ADD(&C, B, A);
	if (ret != -1)
	{
		printf("B + A = ");
		bi_show(C, 16);
	}printf("\n");

	//test A>0 and B<0
	A->sign = Non_Negative; B->sign = Negative;
	printf("A = "); bi_show(A, 16);
	printf("B = "); bi_show(B, 16);

	ret = ADD(&C, A, B);
	if (ret != -1)
	{
		printf("A + B = ");
		bi_show(C, 16);
	}

	ret = ADD(&C, B, A);
	if (ret != -1)
	{
		printf("B + A = ");
		bi_show(C, 16);
	}printf("\n");

	//test A<0 and B>0
	A->sign = Negative; B->sign = Non_Negative;
	printf("A = "); bi_show(A, 16);
	printf("B = "); bi_show(B, 16);

	ret = ADD(&C, A, B);
	if (ret != -1)
	{
		printf("A + B = ");
		bi_show(C, 16);
	}

	ret = ADD(&C, B, A);
	if (ret != -1)
	{
		printf("B + A = ");
		bi_show(C, 16);
	}printf("\n");

	//test A = 0 or B = 0
	A->sign = Non_Negative; bi_set_zero(&B);
	printf("A = "); bi_show(A, 16);
	printf("B = "); bi_show(B, 16);
	ret = ADD(&C, A, B);
	if (ret != -1)
	{
		printf("A + B = ");
		bi_show(C, 16);
	}printf("\n");

	A->sign = Negative; bi_set_zero(&B);
	printf("A = "); bi_show(B, 16);
	printf("B = "); bi_show(A, 16);
	ret = ADD(&C, B, A);
	if (ret != -1)
	{
		printf("A + B = ");
		bi_show(C, 16);
	}printf("**********************************************\n\n");
}

void SUB_test() //뺄셈 연산 테스트 함수
{
	bigint* A = NULL;
	bigint* B = NULL;
	bigint* C = NULL;
	
	int ret = 0;

#if WORD_BITLEN == 8
	word arr_b[4] = { 0x78,0x56,0x34,0x12 };
	word arr_s[5] = { 0xef,0xcd,0xab,0x09, 0x11 };
#elif WORD_BITLEN == 32
	word arr_b[4] = { 0x7878,0x5656,0x3434,0x1212 };
	word arr_s[4] = { 0xefef,0xcdcd,0xabab,0x0909 };
#elif WORD_BITLEN == 64
	word arr_b[4] = { 0x787878787878,0x565600005656,0x343400003434,0x121200001212 };
	word arr_s[4] = { 0xefef0000efef,0xcdcd0000cdcd,0xabab0000abab,0x090900000909 };
#endif
	printf("****Test big integer subtraction (wordlen = %d)****\n\n", WORD_BITLEN);
	bi_set_by_array(&A, Non_Negative, arr_b, 4);
	bi_set_by_array(&B, Non_Negative, arr_s, 5);

	//test A>0 and B>0
	printf("A = "); bi_show(A, 16);
	printf("B = "); bi_show(B, 16);
	
	ret = SUB(&C, A, B);
	if (ret != -1)
	{
		printf("A - B = ");
		bi_show(C, 16);
	}

	ret = SUB(&C, B, A);
	if (ret != -1)
	{
		printf("B - A = ");
		bi_show(C, 16);
	}
	ret = SUB(&C, A, A);
	if (ret != -1)
	{
		printf("A - A = ");
		bi_show(C, 16);
	}printf("\n");

	//test A<0 and B<0
	A->sign = Negative; B->sign = Negative;
	printf("A = "); bi_show(A, 16);
	printf("B = "); bi_show(B, 16);

	ret = SUB(&C, A, B);
	if (ret != -1)
	{
		printf("A - B = ");
		bi_show(C, 16);
	}
	ret = SUB(&C, B, A);
	if (ret != -1)
	{
		printf("B - A = ");
		bi_show(C, 16);
	}printf("\n");

	//test A>0 and B<0
	A->sign = Non_Negative; B->sign = Negative;
	printf("A = "); bi_show(A, 16);
	printf("B = "); bi_show(B, 16);

	ret = SUB(&C, A, B);
	if (ret != -1)
	{
		printf("A - B = ");
		bi_show(C, 16);
	}
	ret = SUB(&C, B, A);
	if (ret != -1)
	{
		printf("B - A = ");
		bi_show(C, 16);
	}printf("\n");

	//test A<0 and B>0
	A->sign = Negative; B->sign = Non_Negative;
	printf("A = "); bi_show(A, 16);
	printf("B = "); bi_show(B, 16);

	ret = SUB(&C, A, B);
	if (ret != -1)
	{
		printf("A - B = ");
		bi_show(C, 16);
	}
	ret = SUB(&C, B, A);
	if (ret != -1)
	{
		printf("B - A = ");
		bi_show(C, 16);
	}printf("\n");

	//test A = 0 or B = 0
	A->sign = Negative; bi_set_zero(&B);
	printf("A = "); bi_show(A, 16);
	printf("B = "); bi_show(B, 16);
	ret = SUB(&C, A, B);
	if (ret != -1)
	{
		printf("A - B = ");
		bi_show(C, 16);
	}printf("\n");

	A->sign = Negative; bi_set_zero(&B);
	printf("A = "); bi_show(B, 16);
	printf("B = "); bi_show(A, 16);
	ret = SUB(&C, B, A);
	if (ret != -1)
	{
		printf("A - B = ");
		bi_show(C, 16);
	}printf("***********************************************\n\n");
}