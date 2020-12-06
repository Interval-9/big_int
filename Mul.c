/*
	This file contains the multiplication function, one of the main functions, and other functions necessary to implement the multiplication function.
*/
#include "bn_struct.h"
/*
	> Single-Precision Multiplication
	> Multiplying two words (Aj * Bi = Cj)
	> Input: address of word Cj, a word Aj, a word Bj
	> Output: None
*/
void MUL_AB(word* Cj, word Aj, word Bi)
{
	word A1 = 0, A0 = 0;
	word B1 = 0, B0 = 0;
	word T1 = 0, T0 = 0, T = 0;
	word C1 = 0, C0 = 0;

	int Half_bitLen = WORD_BITLEN >> 1; //Half_bitLen = WORD_BITLEN / 2

	//A1, A0 <- Aj >> w/2, Aj mod 2^(w/2)
	A1 = Aj >> Half_bitLen;
	A0 = Aj & HARF_MASK;

	//B1, B0 <- Bi >> w/2, Bi mod 2^(w/2)
	B1 = Bi >> Half_bitLen;
	B0 = Bi & HARF_MASK;

	/*
		T1W + T0 = A1*B0 + A0*B1
		1)	T1, T0 <- A1*B0, A0*B1
		2)	T0 <- T1 + T0 (mod 2^(w/2))
		3)	T1 <- T0 < T1
	*/
	//1)
	T1 = A1 * B0; T0 = A0 * B1;
	//2)
	T0 = (word)(T1 + T0);
	//3) 
	T1 = (T0 < T1);

	/*
		1)	C1, C0 <- A1*B1,A0*B0
		2)	T <- C0
		3)	C0 <- C0 + (T0 << w/2) (mod 2^(w/2))
		4)	C1 <- C1 + (T1 << w/2) + (T0 >> w/2) + (C0 < T)
	*/
	//1)
	C1 = A1 * B1; C0 = A0 * B0;
	//2)
	T = C0;
	//3)
	C0 = (word)(C0 + (T0 << Half_bitLen));
	//4)
	C1 = C1 + (word)(T1 << Half_bitLen) + (word)(T0 >> Half_bitLen) + (C0 < T);
	//C <- (C1 << w) + C0
	Cj[0] = C0;	Cj[1] = C1;
}

/*
	> Multiplication: C <- A * B (A: bigint, B: word)
	> Multiplying a big integer by a word
	> Input: doulbe pointer of C, pointer of A, pointer of B
	> Output: None
*/
void MUL_word_bigint(bigint** C, bigint* A, word B)
{
	int j = 0, n = 0, t = 0;
	bigint* T1 = NULL; bigint* T2 = NULL;
	word* pT1 = NULL; word* pT2 = NULL;

	n = A->wordLen;
	bi_new(C, n + 1);
	bi_new(&T1, n + 1);
	bi_new(&T2, n + 1);
	pT1 = T1->a; pT2 = T2->a; //starting address of array

	t = n >> 1; //(A->wordLen)/2

	for (j = 0;j < t;j++)
	{
		MUL_AB(pT1 + (unsigned long long)2 * j, A->a[2 * j], B);
		MUL_AB(pT2 + (unsigned long long)2 * j + 1, A->a[2 * j + 1], B);
	}
	if (n & 1)
	{
		MUL_AB(pT1 + n - 1, A->a[n - 1], B);
	}
	ADD(C, T1, T2);
	bi_delete(&T1);
	bi_delete(&T2);
}

/*
   > SchoolBook Multiplication: C <- A * B
   > Multiplying two positive integers
   > Input: doulbe pointer of C, pointer of A, pointer of B
   > Output: None
*/
void MULC_S(bigint** C, bigint* A, bigint* B)
{
	int j = 0, k = 0, n = 0, m = 0;
	word TMP[2] = { 0, 0 };
	bigint* T1 = NULL; bigint* T2 = NULL;
	n = A->wordLen;
	m = B->wordLen;

	bi_new(C, n + m);
	for (j = 0; j < n; j++)
	{
		for (k = 0; k < m; k++)
		{
			MUL_AB(TMP, A->a[j], B->a[k]);
			bi_set_by_array(&T1, Non_Negative, TMP, 2);
			Left_shift(&T1, WORD_BITLEN * (j + k));
			bi_assign(&T2, *C);
			ADD(C, T2, T1);
		}
	}
	bi_refine(*C);
	bi_delete(&T1);
	bi_delete(&T2);
}

/*
	> Modified SchoolBook Multiplication: C <- A * B
	> Multiplying two positive integers
	> Input: doulbe pointer of C, pointer of A, pointer of B
	> Output: None
*/
void Modified_MULC_S(bigint** C, bigint* A, bigint* B)
{
	int j = 0, k = 0, n = 0, m = 0, t = 0;
	bigint* T1 = NULL; bigint* T2 = NULL; bigint* T3 = NULL;
	word* pT1 = NULL; word* pT2 = NULL;

	n = A->wordLen;
	m = B->wordLen;
	t = ((B->wordLen) >> 1);

	//define new bigint structure
	bi_new(C, n + m);

	if (m & 1) //if m is odd
	{
		for (j = 0; j < n; j++)
		{
			bi_new(&T1, n + m);
			bi_new(&T2, n + m);
			pT1 = T1->a; pT2 = T2->a; //starting address of array

			for (k = 0; k < t; k++)
			{
				MUL_AB(pT1 + (unsigned long long)2 * k + j, A->a[j], B->a[2 * k]);         //T1 <- ...|| AjB4 || AjB2 || AjB0
				MUL_AB(pT2 + (unsigned long long)2 * k + j + 1, A->a[j], B->a[2 * k + 1]);  //T2 <- ...|| AjB5 || AjB3 || AjB1
			}
			MUL_AB((pT1 + (unsigned long long)2 * t + j), A->a[j], B->a[2 * t]);

			ADD(&T3, T1, T2);	//T3 <- ...|| (AjB5 + AjB4) || (AjB3 + AjB2) || (AjB1 + AjB0)
			ADD(&T2, (*C), T3);
			array_copy((*C)->a, T2->a, T2->wordLen);
		}
	}
	else //if m is even
	{
		for (j = 0; j < n; j++)
		{
			bi_new(&T1, n + m);
			bi_new(&T2, n + m);
			pT1 = T1->a; pT2 = T2->a; //starting address of array

			for (k = 0; k < t; k++)
			{
				MUL_AB(pT1 + (unsigned long long)2 * k + j, A->a[j], B->a[2 * k]);         //T1 <- ...|| AjB4 || AjB2 || AjB0
				MUL_AB(pT2 + (unsigned long long)2 * k + j + 1, A->a[j], B->a[2 * k + 1]); //T2 <- ...|| AjB5 || AjB3 || AjB1
			}
			ADD(&T3, T1, T2); //C <- ...|| (AjB5 + AjB4) || (AjB3 + AjB2) || (AjB1 + AjB0)
			ADD(&T2, (*C), T3); //T2 <- C + T3
			array_copy((*C)->a, T2->a, T2->wordLen);
		}
	}
	bi_refine(*C);
	bi_delete(&T1);
	bi_delete(&T2);
	bi_delete(&T3);
}

/*
	> Karatsuba Multiplication: C <- A * B
	> Multiplying two positive integers
	> Input: doulbe pointer of C, pointer of A, pointer of B
	> Output: None
*/
void MULC_K(bigint** C, bigint* A, bigint* B)
{
	int n = 0, m = 0;
	int L = 0;
	int i = 0, j = 0;
	int max_wordLen = 0;
	int new_sign = 0;

	bigint* A1 = NULL; bigint* A0 = NULL;
	bigint* B1 = NULL; bigint* B0 = NULL;
	bigint* T1 = NULL; bigint* T0 = NULL; bigint* tempT1 = NULL;
	bigint* R = NULL; bigint* tempR = NULL;
	bigint* S1 = NULL; bigint* S0 = NULL; bigint* S = NULL; bigint* tempS = NULL;

	//n: wordlen of A, m: wordlen of B
	n = get_bit_length(A)/WORD_BITLEN + 1;
	m = get_bit_length(B) / WORD_BITLEN + 1;

	//if A = 0 or B = 0, then C <- 0
	if ((Is_Zero(A) == 1) | (Is_Zero(B) == 1)) 
	{
		bi_set_zero(C);
		return;
	}

	//if A = 1, then C <- B
	else if (Is_One(A) == 1) 
	{
		bi_assign(C, B);
		return;
	}

	//if B = 1, then C <- A
	else if (Is_One(B) == 1) 
	{
		bi_assign(C, A);
		return;
	}

	if (FLAG >= min(n, m)) 
	{
		Modified_MULC_S(C, A, B);
		return;
	}

	L = (max(n, m) + 1) >> 1;

	/*
		1)	A1, A0 <- A >> lw, A mod(2^lw)
		2)	B1, B0 <- B >> lw, B mod(2^lw)
	*/
	//1)
	bi_assign(&A1, A);
	bi_assign(&A0, A);
	Right_shift(&A1, L * WORD_BITLEN);
	Reduction(&A0, L * WORD_BITLEN);
	//2)
	bi_assign(&B1, B);
	bi_assign(&B0, B);
	Right_shift(&B1, L * WORD_BITLEN);
	Reduction(&B0, L * WORD_BITLEN);

	/*
		1)	T1, T0 <- MULC_K(A1, B1), MULC_K(A0, B0)
		2)	R <- T1 || T0
	*/
	//1)
	MULC_K(&T1, A1, B1); 
	MULC_K(&T0, A0, B0);
	//2) R <- (T1 << 2lw) + T0
	bi_assign(&tempT1, T1);
	Left_shift(&tempT1, 2 * L * WORD_BITLEN);
	ADD(&tempR, tempT1, T0);

	/*
		1)	S1, S0 <- A0 - A1, B1 - B0
		2)	S <- (-1)^(S1->sign) ^ (S0->sign) * MULC_K(|S1|, |S0|)
	*/
	//1)
	SUB(&S1, A0, A1);
	SUB(&S0, B1, B0);
	//2)
	new_sign = (S1->sign) ^ (S0->sign);
	S1->sign = 0; S0->sign = 0;		//|S1|, |S0|

	MULC_K(&tempS, S1, S0);
	tempS->sign = new_sign;

	/*
		R + S
		1)	S <= ADD(S, T1)
		2)	S <- ADD(S, T0)
		3)	S <- S << lw
		4)	R <- ADD(R, S)
	*/
	//1)
	ADD(&S, tempS, T1);
	//2)
	ADD(&tempS, S, T0);
	//3)
	Left_shift(&tempS, L * WORD_BITLEN);
	//4)
	ADD(C, tempR, tempS);
	bi_refine(*C);

	bi_delete(&A0); bi_delete(&A1); bi_delete(&B0); bi_delete(&B1); 
	bi_delete(&T0); bi_delete(&T1); bi_delete(&tempT1); bi_delete(&R); bi_delete(&tempR);
	bi_delete(&S0); bi_delete(&S1); bi_delete(&tempS); bi_delete(&S);
}

/*
	> Multiplying two integers: C <- A * B
	> Schoolbook(MUL_Type == 0) Karatsuba(MUL_Type == 1) (defined in "bn_struct.h")
	> Input: doulbe pointer of C, pointer of A, pointer of B
	> Output: None
*/
void MUL(bigint** C, bigint* A, bigint* B)
{
	int sign_A = 0, sign_B = 0;

	//if A = 0 or B = 0, then C <- 0
	if ((Is_Zero(A) == 1) | (Is_Zero(B) == 1)) {
		bi_set_zero(C);
		return;
	}

	//if A = 1, then C <- B
	else if (Is_One(A) == 1) {
		bi_assign(C, B);
		return;
	}

	//if B = 1, then C <- A
	else if (Is_One(B) == 1) {
		bi_assign(C, A);
		return;
	}

	//if A = -1, then C <- -B
	else if (Is_NegativeOne(A))
	{
		bi_assign(C, B);
		(*C)->sign = (A->sign) ^ (B->sign);
		return;
	}

	//if B = -1, then C <- -A
	else if (Is_NegativeOne(B))
	{
		bi_assign(C, A);
		(*C)->sign = (A->sign) ^ (B->sign);
		return;
	}

	sign_A = (A->sign);
	sign_B = (B->sign);
	A->sign = 0; B->sign = 0;

#if MUL_Type == 0 //Modified Schoolbook Mul
	Modified_MULC_S(C, A, B);

#elif MUL_Type == 1 //Karatsuba Mul
	MULC_K(C, A, B);
#endif
	(*C)->sign = sign_A ^ sign_B;
	A->sign = sign_A; B->sign = sign_B;

	return;
}