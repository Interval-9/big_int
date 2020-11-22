#include "bn_struct.h"

//워드 단위 덧셈 연산을 수행하는 함수
//캐리 비트(0 또는 1)를 반환
int ADD_ABc(word* Cj, word Aj, word Bj, int cb)
{
	int c = 0;					//반환할 캐리 비트 변수

	(*Cj) = (word)(Aj + Bj);	//Cj = (Aj + Bj) mod word
	if (*Cj < Aj)				//캐리 발생
		c = 1;

	(*Cj) = (word)((*Cj) + cb); //그 전 캐리와 덧셈
	if (*Cj < cb)				//캐리 발생
		c = 1;

	return c;
}

//부호가 같은 두 정수의 덧셈 연산 함수 (wordLen(A) >= wordLen(B)인 경우)
//동적 할당에 실패할 경우: -1, 성공할 경우: 0을 반환
int ADDC(bigint** C, bigint* A, bigint* B)
{
	int max_wordLen = 0;
	int c = 0, j = 0;				//캐리(c)는 0으로 초기화

	word* Cp = NULL;				//주소 접근을 위한 포인터 선언
	word* Bp = NULL;

	max_wordLen = (A->wordLen);		//C의 워드 길이를 계산
	bi_new(C, max_wordLen + 1);		//결과값을 저장할 big_int 구조체 생성

	Cp = (*C)->a;

	//B에 메모리 재할당
	Bp = (word*)realloc(B->a, sizeof(word) * max_wordLen);
	if (Bp == NULL) //동적할당 실패시 함수 종료
		return -1;
	else
		B->a = Bp;

	array_init(((B->a) + (B->wordLen)), (max_wordLen - (B->wordLen)));	// Bm ~ Bn-1까지 0으로 초기화

	//워드 단위 덧셈 연산: C, c <- (A[j] + B[j] + c)
	for (j = 0; j < max_wordLen; j++)
	{
		c = ADD_ABc(Cp + j, *(A->a + j), *(B->a + j), c);
		//printf("Aj = %02x, Bj = %02x\n", (A->a)[j], (B->a)[j]); 
	}

	if (c == 1) //최상위 워드 연산 결과, 캐리가 남아있을 경우
		Cp[max_wordLen] = 1;

	bi_refine(B);	bi_refine(*C);

	(*C)->sign = A->sign; //C의 부호를 정함

	return 0;
}

int Positive_ADD_1(bigint** X)
{
	int X_wordLen = (*X)->wordLen, j = 0, c1 = 1, c2 = 0;
	word tmp = 0;
	word* Xa = NULL;
	Xa = (*X)->a;

	for (j = 0; j < X_wordLen; j++)
	{
		tmp = (word)(*(Xa + j) + c1);
		if (tmp < *(Xa + j))
			c2 = 1;
		
		c1 = c2;
		*(Xa + j) = tmp;
	}

	if (c2 == 1) //최상위 워드 연산 결과, 캐리가 남아있을 경우
	{
		Xa = (word*)realloc((*X)->a, X_wordLen + 1);
		if (Xa == NULL)
			return ERROR;
		*((*X)->a + X_wordLen) = c2;
		(*X)->wordLen = X_wordLen + 1;
	}
	return 0;
}

/*
	> 두 정수 A, B의 합을 계산하는 함수
	> 성공 여부를 반환함 (성공: 0, 실패: -1)
	> 이 때 A와 B는 같아도 됨. (A = B 일 때, A <- 2A 여도 되는지는 확인 필요)
*/
int ADD(bigint** C, bigint* A, bigint* B)
{
	int ret = 0;

	//A가 0일 경우, C <- B
	if (Is_Zero(A) == 1)
	{
		bi_assign(C, B);
		return 0;
	}

	//B가 0일 경우, C <- A
	else if (Is_Zero(B) == 1)
	{
		bi_assign(C, A);
		return 0;
	}

	//A > 0, B < 0
	else if ((A->sign == 0) & (B->sign == 1))
	{
		// |A| < |B|, A + B = |A| - |B| < 0
		if (CompareABS(A, B) == -1)
		{
			ret = SUBC(C, B, A);
			(*C)->sign = Negative;
			return ret;
		}
		else // |A| > |B|, A + B = |A| - |B| > 0
			return SUBC(C, A, B);
	}

	//A < 0, B > 0
	else if ((A->sign == 1) & (B->sign == 0))
	{
		// |A| > |B|, A + B = |A| - |B| < 0
		if (CompareABS(A, B) == 1)
		{
			ret = SUBC(C, A, B);
			(*C)->sign = Negative;
			return ret;
		}
		else // |A| < |B|, A + B = |A| - |B| > 0
			return SUBC(C, B, A);
	}

	//A의 워드길이가 더 긴 경우
	else if (A->wordLen >= B->wordLen)
		return ADDC(C, A, B);

	//B의 워드길이가 더 긴 경우
	else
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
	int max_wordLen = 0;
	int b = 0, j = 0;				//캐리(b)를 0으로 초기화

	word* Cp = NULL;				//주소 접근을 위한 포인터 선언
	word* Bp = NULL;

	max_wordLen = (A->wordLen);		//C의 워드 길이를 계산
	bi_new(C, max_wordLen);			//결과값을 저장할 big_int 구조체 생성

	Cp = (*C)->a;

	//B에 메모리 재할당
	Bp = (word*)realloc(B->a, sizeof(word) * max_wordLen); 
	if (Bp == NULL) //동적할당 실패시 함수 종료
		return -1;
	else
		B->a = Bp;

	array_init(((B->a) + (B->wordLen)), (max_wordLen - (B->wordLen)));	// Bm ~ Bn-1까지 0으로 초기화

	 //워드 단위 덧셈 연산: C, b <- (A - B - b)
	for (j = 0; j < max_wordLen; j++)
	{
		b = SUB_AbB(Cp + j, *(A->a + j), b, *(B->a + j));
		//printf("carry = %d\n", b);
		//printf("Aj = %llx, Bj = %llx, Cj = %llx\n", (A->a)[j], (B->a)[j], *(Cp + j));
	}
	bi_refine(B);
	bi_refine(*C);

	return 0;
}

/*
	> 두 정수 A, B의 뺄셈 연산을 수행하는 함수
	> 성공 여부를 반환함 (성공: 0, 실패: -1)
	> 이 때 A와 B는 같아도 됨. (A = B 일 때, A <- 0 여도 되는지는 확인 필요)
*/
int SUB(bigint** C, bigint* A, bigint* B)
{
	int ret = 0;

	//A가 0인 경우, C <- (-B)
	if (Is_Zero(A) == 1)
	{
		bi_assign(C, B);
		flip_bi_sign(*C);
		return 0;
	}

	//B가 0인 경우, C <- A
	else if (Is_Zero(B) == 1)
	{
		bi_assign(C, A);
		return 0;
	}

	//A = B인 경우, C <- 0
	else if (!Compare(A, B))
	{
		bi_set_zero(C);
		return 0;
	}

	//A > 0, B > 0
	else if ((A->sign == 0) & (B->sign == 0))
	{
		// A > B > 0
		if (Compare(A, B) == 1)
			return SUBC(C, A, B);
		
		// B > A > 0
		else if (Compare(A, B) == -1)
		{
			ret = SUBC(C, B, A);
			(*C)->sign = Negative;
			return ret;
		}
	}

	//A < 0, B < 0
	else if ((A->sign == 1) & (B->sign == 1))
	{
		// 0 > A > B
		if (Compare(A, B) == 1)
			return SUBC(C, B, A);

		// 0 > B > A
		else if (Compare(A, B) == -1)
		{
			ret = SUBC(C, A, B);
			(*C)->sign = Negative;
		}
	}

	//A > 0 , B < 0
	else if ((A->sign == 0) & (B->sign == 1))
	{
		// A - B = A - (-|B|) = A + |B| > 0 
		if (A->wordLen > B->wordLen)
			ret = ADDC(C, A, B);
		else
			ret = ADDC(C, B, A);
		(*C)->sign = Non_Negative;
		return ret;
	}

	//A < 0 , B > 0
	else if ((A->sign == 1) & (B->sign == 0))
	{
		// A - B = (-|A|) - B = -(|A| + B) < 0 
		if (A->wordLen > B->wordLen)
			ret = ADDC(C, A, B);
		else
			ret = ADDC(C, B, A);
		(*C)->sign = Negative;
		return ret;
	}
	return 0;
}