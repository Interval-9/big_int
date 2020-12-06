/*
	This file contains the Square function, one of the main functions, and other functions necessary to implement the square function.
*/
#include "bn_struct.h"
/*
	> Single_Precision Squaring
	> Input: doulbe pointer of C, pointer of A
	> Output: None
*/
void SQU_AB(bigint** C, word A)
{
	int Half_bitLen = WORD_BITLEN >> 1;

	word A1 = 0; word A0 = 0;
	word C1 = 0; word C0 = 0;

	bigint* tempC = NULL;
	bigint* T = NULL;

	//A1, A0 <- |A| >> w/2, |A| mod 2^(w/2)
	A1 = A >> Half_bitLen;
	A0 = A & HARF_MASK;

	//C1, C0 <- A1^2, A0^2
	C1 = A1 * A1;
	C0 = A0 * A0;

	//C <- (C1<<w) + C0
	bi_new(&tempC, 2);
	tempC->a[0] = C0;
	tempC->a[1] = C1;

	//T <- A0*A1
	bi_new(&T, 1);
	T->a[0] = A0 * A1;

	//T <- T << (w/2 + 1)
	Left_shift(&T, Half_bitLen + 1);

	//C <- C+T
	ADD(C, tempC, T);

	bi_refine(*C);

	bi_delete(&tempC);
	bi_delete(&T);
}

/*
	> A^2 using SchoolBook Multiplication
	> Input: doulbe pointer of C, pointer of A
	> Output: None
*/
void SQUC_S(bigint** C, bigint* A)
{
	int i = 0, j = 0;
	int t = A->wordLen;

	bigint* C1 = NULL; bigint* C2 = NULL; bigint* tempC1 = NULL; bigint* tempC2 = NULL;
	bigint* T1 = NULL; bigint* T2 = NULL;

	//C1, C2 <- 0
	bi_set_zero(&tempC1); bi_set_zero(&tempC2);

	for (j = 0; j < t; j++) {
		//T1 <- Aj ^ 2
		SQU_AB(&T1, A->a[j]);

		//T1 <- T1 << 2jw
		Left_shift(&T1, 2 * j * WORD_BITLEN);

		//C1 <- T1 + C1
		ADD(&C1, T1, tempC1);
		bi_assign(&tempC1, C1);
		
		for (i = j + 1; i < t; i++) {
			//T2 <- Aj*Ai
			bi_new(&T2, 2);
			MUL_AB(T2->a, A->a[j], A->a[i]);

			//T2 <- T2 << (i+j)w
			Left_shift(&T2, (i + j) * WORD_BITLEN);

			//C2 <- C2 + T2
			ADD(&C2, tempC2, T2);
			bi_assign(&tempC2, C2);
		}
	}

	if (C2 == NULL)
		bi_set_zero(&C2);

	//C2 <- C2 << 1
	Left_shift(&C2, 1);

	//C1 + C2
	ADD(C, C1, C2);

	bi_refine(*C);

	bi_delete(&C1); bi_delete(&C2); bi_delete(&tempC1); bi_delete(&tempC2);
	bi_delete(&T1); bi_delete(&T2);
}

/*
	> A^2 using Karatsuba Multiplication 
	> Input: doulbe pointer of C, pointer of A
	> Output: None
*/
void SQUC_K(bigint** C, bigint* A)
{
	int L = 0;
	int wordLen_A = A->wordLen;

	bigint* A1 = NULL; bigint* A0 = NULL;
	bigint* T1 = NULL; bigint* T0 = NULL;
	bigint* R = NULL; bigint* tempR = NULL;
	bigint* S = NULL;

	//Case: A = 0 or A = 1 or A = -1
	if (Is_Zero(A) | Is_One(A) | Is_NegativeOne(A)) 
	{
		bi_assign(C, A);
		(*C)->sign = 0;
	}

	if (FLAG >= wordLen_A) 
	{
		SQUC_S(C, A);
		return;
	}

	L = (wordLen_A + 1) >> 1;

	/*
		1)	A1, A0 <- |A| >> lw, |A| mod 2^lw
		2)	T1, T0 <- SQUC_K(A1), SQUA_K(A0)
	*/
	//1)
	bi_assign(&A1, A); //A1->sign = 0;
	bi_assign(&A0, A); //A0->sign = 0;
	Right_shift(&A1, L * WORD_BITLEN);
	Reduction(&A0, L * WORD_BITLEN);

	//2)
	SQUC_K(&T1, A1);
	SQUC_K(&T0, A0);							

	/*
		R <- T1||T0
		1) T1 << 2lw
		2) T1 + T0
	*/
	//1)
	Left_shift(&T1, 2 * L * WORD_BITLEN);
	//2)
	ADD(&tempR, T1, T0);
	//S <- A1*A0
	MULC_K(&S, A1, A0);
	//S <- S <<lw+1
	Left_shift(&S, L * WORD_BITLEN + 1);
	//R <- ADDC(R,S)
	ADDC(&R, tempR, S);

	bi_assign(C, R);
	bi_refine(*C);

	bi_delete(&A1); bi_delete(&A0);
	bi_delete(&T1); bi_delete(&T0);
	bi_delete(&R); bi_delete(&tempR);
	bi_delete(&S);
}

/*
	> Square a integer A
	> Input: doulbe pointer of C, pointer of A
	> Output: None
*/
void SQU(bigint** C, bigint* A)
{
	//Case: A = 0 or A = 1 or A = -1
	if (Is_Zero(A) | Is_One(A) | Is_NegativeOne(A)) {
		bi_assign(C, A);
		(*C)->sign = 0;
	}

	//Otherwise
#if SQU_Type == 0 //SchoolBook Squaring
	SQUC_S(C, A);

#elif SQU_Type == 1 //Karatsuba Squaring
	SQUC_K(C, A);
#endif

	return;
}