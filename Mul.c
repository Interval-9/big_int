#include "bn_struct.h"
/*
	> Single-Precision Multiplication
	> Multiplying two words (Aj * Bi = Cj)
	> Success: return 0, Failure: return -1
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
	A0 = Aj & MASK;											

	//B1, B0 <- Bi >> w/2, Bi mod 2^(w/2)
	B1 = Bi >> Half_bitLen;
	B0 = Bi & MASK;

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
	> SchoolBook Multiplication
	> Multiplying two positive integers
	> Success: return 0, Failure: return -1
*/
int MULC_S(bigint** C, bigint* A, bigint* B)
{
	int n = 0, m = 0;
	int max_wordLen = 0;
	int i = 0, j = 0;

	//word* temp = NULL;

	bigint* T = NULL;
	bigint* tempC = NULL;

	//n: wordlen of A, m: wordlen of B
	n = A->wordLen;
	m = B->wordLen;

	//C <- 0
	max_wordLen = n + m;		//Compute maximum wordlen of C
	if (-1 == bi_new(&tempC, max_wordLen)) //fail in dynamic allocation
		return ERROR;
	//array_init(tempC->a, max_wordLen);

	for (j = 0; j < n; j++) {
		//A와 B의 각 워드를 곱할 임시 bignum 구조체 T 생성
		for (i = 0; i < m; i++) {
			if (bi_new(&T, sizeof(word) * 2) == -1)
				return ERROR;
			/*
				1)	T <- A_j * B_i
				2)	T <- T << w(i+j)
			*/
			//1) 0 <= T < W^2
			MUL_AB(T->a, A->a[j], B->a[i]);
			//2)
			Left_shift(&T, WORD_BITLEN * (i + j));

			//C <- ADDC(C, T)
			if (ADDC(C, tempC, T) == -1)
				return ERROR;
				
			array_copy(tempC->a, (*C)->a, (*C)->wordLen);
		}
	}
	bi_refine(*C);

	//free(temp);
	bi_delete(&T);
	bi_delete(&tempC);
	return 0;
}

int Modified_MULC_S(bigint** C, bigint* A, bigint* B) //Modified version
{
	int j = 0, k = 0, n = 0, m = 0, t = 0;
	bigint* T1 = NULL; bigint* T2 = NULL; bigint* T3 = NULL;
	word* pT1 = NULL; word* pT2 = NULL;

	n = A->wordLen;
	m = B->wordLen;
	t = ((B->wordLen) >> 1);

	//define new bigint structure
	if (-1 == bi_new(C, n + m))
		return ERROR;

	if (m & 1) //if m is odd
	{
		for (j = 0; j < n; j++)
		{
			if (-1 == bi_new(&T1, n + m))
				return ERROR;
			if (-1 == bi_new(&T2, n + m))
				return ERROR;
			pT1 = T1->a; pT2 = T2->a; //starting address of array

			for (k = 0; k < t; k++)
			{
				MUL_AB(pT1 + 2 * k + j, A->a[j], B->a[2 * k]);          //T1 <- ...|| AjB4 || AjB2 || AjB0
				MUL_AB(pT2 + 2 * k + j + 1, A->a[j], B->a[2 * k + 1]);  //T2 <- ...|| AjB5 || AjB3 || AjB1
			}
			MUL_AB((pT1 + 2 * t + j), A->a[j], B->a[2 * t]);

			if (-1 == ADD(&T3, T1, T2))	//T3 <- ...|| (AjB5 + AjB4) || (AjB3 + AjB2) || (AjB1 + AjB0)
				return ERROR;
			if (-1 == ADD(&T2, (*C), T3))
				return ERROR;
			array_copy((*C)->a, T2->a, T2->wordLen);
		}
	}
	else //if m is even
	{
		for (j = 0; j < n; j++)
		{
			if (-1 == bi_new(&T1, n + m))
				return ERROR;
			if (bi_new(&T2, n + m))
				return ERROR;
			pT1 = T1->a; pT2 = T2->a; //starting address of array

			for (k = 0; k < t; k++)
			{
				MUL_AB(pT1 + 2 * k + j, A->a[j], B->a[2 * k]);         //T1 <- ...|| AjB4 || AjB2 || AjB0
				MUL_AB(pT2 + 2 * k + j + 1, A->a[j], B->a[2 * k + 1]); //T2 <- ...|| AjB5 || AjB3 || AjB1
			}
			if (-1 == ADD(&T3, T1, T2)) //C <- ...|| (AjB5 + AjB4) || (AjB3 + AjB2) || (AjB1 + AjB0)
				return ERROR;
			if (-1 == ADD(&T2, (*C), T3)) //T2 <- C + T3
				return ERROR;
			array_copy((*C)->a, T2->a, T2->wordLen);
		}
	}
	bi_refine(*C);

	bi_delete(&T1);
	bi_delete(&T2);
	bi_delete(&T3);

	return 0;
}

/*
	> Karatsuba Multiplication
	> Multiplying two positive integers
	> Success: return 0, Failure: return -1
*/
int MULC_K(bigint** C, bigint* A, bigint* B)
{
	int n = 0, m = 0;
	int L = 0;
	int i = 0, j = 0;
	int max_wordLen = 0;

	bigint* A1 = NULL; bigint* A0 = NULL;
	bigint* B1 = NULL; bigint* B0 = NULL;
	bigint* T1 = NULL; bigint* T0 = NULL; bigint* tempT1 = NULL;
	bigint* R = NULL;
	bigint* S1 = NULL; bigint* S0 = NULL; bigint* S = NULL; bigint* tempS = NULL;

	//n: wordlen of A, m: wordlen of B					
	//n = get_bit_length(A) / WORD_BITLEN;
	//m = get_bit_length(B) / WORD_BITLEN;
	n = A->wordLen;
	m = B->wordLen;

	if (FLAG >= min(n, m))
	{
		if (-1 == MULC_S(C, A, B))
			return ERROR;
		else
			return 0;
	}

	L = (max(n, m) + 1) >> 1;

	/*
		1)	A1, A0 <- A >> lw, A mod(2^lw)
		2)	B1, B0 <- B >> lw, B mod(2^lw)
	*/
	//define new A1, A0, B1, B0 bigint structure
	if (-1 == bi_new(&A1, A->wordLen))
		return ERROR;
	if (-1 == bi_new(&A0, A->wordLen))
		return ERROR;
	if (-1 == bi_new(&B1, B->wordLen))
		return ERROR;
	if (-1 == bi_new(&B0, B->wordLen))
		return ERROR;
	//1)
	array_copy(A1->a, A->a, A->wordLen);
	array_copy(A0->a, A->a, A->wordLen);
	Right_shift(&A1, L * WORD_BITLEN);
	Reduction(&A0, L * WORD_BITLEN);
	//2)
	array_copy(B1->a, B->a, B->wordLen);
	array_copy(B0->a, B->a, B->wordLen);
	Right_shift(&B1, L * WORD_BITLEN);
	Reduction(&B0, L * WORD_BITLEN);

	/*
		1)	T1, T0 <- MULC_K(A1, B1), MULC_K(A0, B0)
		2)	R <- T1 || T0
	*/
	//1)
	if (-1 == MULC_K(&T1, A1, B1))
		return ERROR;
	if (-1 == MULC_K(&T0, A0, B0))
		return ERROR;
	//2) R <- (T1 << 2lw) + T0
	/*if (-1 == bi_new(&tempT1, T1->wordLen))
		return ERROR;
	array_copy(tempT1->a, T1->a, T1->wordLen);*/
	bi_assign(&tempT1, T1);
	Left_shift(&tempT1, 2 * L * WORD_BITLEN);
	if (-1 == ADD(&R, tempT1, T0))
		return ERROR;

	/*
		1)	S1, S0 <- A0 - A1, B1 - B0
		2)	S <- (-1)^(S1->sign) ^ (S0->sign) * MULC_K(|S1|, |S0|)
	*/
	//1)
	if (-1 == SUB(&S1, A0, A1))
		return ERROR;
	if (-1 == SUB(&S0, B1, B0))
		return ERROR;
	//2)
	if (-1 == MULC_K(&S, S1, S0))
		return ERROR;
	S->sign = (S1->sign) ^ (S0->sign);

	/*
		R + S
		1)	S <= ADD(S, T1)
		2)	S <- ADD(S, T0)
		3)	S <- S << lw
		4)	R <- ADD(R, S)
	*/
	//1)
	if (-1 == ADD(&tempS, S, T1))
		return ERROR;
	//2)
	if (-1 == ADD(&S, tempS, T0))
		return ERROR;
	//3)
	Left_shift(&S, L * WORD_BITLEN);
	//4)
	if (-1 == ADD(C, R, S))
		return ERROR;

	bi_refine(*C);

	bi_delete(&A0); bi_delete(&A1); bi_delete(B0); bi_delete(B1); 
	bi_delete(&T0); bi_delete(&T1); bi_delete(&tempT1); bi_delete(&R);
	bi_delete(&S0); bi_delete(&S1); bi_delete(&tempS); bi_delete(&S);
	return 0;
}

/*
	> Multiplying two integers
	> Schoolbook(MUL_Type == 0) Karatsuba(MUL_Type == 1) (defined in "bn_struct.h")
	> Success: return 0, Failure: return -1
*/
int MUL(bigint** C, bigint* A, bigint* B)
{
	//if A = 0 or B = 0, then C <- 0
	if ((Is_Zero(A) == 1) | (Is_Zero(B) == 1)) 
	{
		bi_set_zero(C);
		return 0;
	}

	//if A = 1, then C <- B
	else if(Is_One(A) == 1) 
	{
		bi_assign(C, B);
		return 0;
	}
	//if B = 1, then C <- A
	else if (Is_One(B) == 1) 
	{
		bi_assign(C, A);
		return 0;
	}
	//if A = -1, then C <- -B
	else if (Is_NegativeOne(A))
	{
		bi_assign(C, B);
		(*C)->sign = (A->sign) ^ (B->sign);
		return 0;
	}
	//if B = -1, then C <- -A
	else if (Is_NegativeOne(B))
	{
		bi_assign(C, A);
		(*C)->sign = (A->sign) ^ (B->sign);
		return 0;
	}

#if MUL_Type == 0 //Schoolbook Mul
	if (-1 == MULC_S(C, A, B))
		return ERROR;
	(*C)->sign = (A->sign) ^ (B->sign);

#elif MUL_Type == 1//Modified Schoolbook Mul
	if (-1 == Modified_MULC_S(C, A, B))
		return ERROR;
	(*C)->sign = (A->sign) ^ (B->sign);

#elif MUL_Type == 2 //Karatsuba Mul
	if (-1 == MULC_K(C, A, B))
		return ERROR;
	(*C)->sign = (A->sign) ^ (B->sign);
#endif
	return 0;
}