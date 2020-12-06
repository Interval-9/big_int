/*
	This file contains the modular exponentiation function, one of the main functions, and other functions necessary to implement the modular exponentiation function.
*/
#include "bn_struct.h"

/*
	> Compute remainder mod N for Modular Exponentiaiton
	> X <- X mod N
	> Input: double pointer of X, pointer of N
	> Output: None
*/
void bi_reduction(bigint** X, bigint* N)
{
	bigint* Q = NULL;
	bigint* R = NULL;
	Multi_Long_DIV(&Q, &R, *X, N);
	bi_assign(X, R);
	bi_delete(&Q);
	bi_delete(&R);
}

/*
	> Left to Right Modular Exponentiaiton
	> C <- x^n mod N
	> Input: double pointer of C, pointer of X, pointer of n, pointer of N
	> Output: None
*/
void MOD_EXP_L2R(bigint** C, bigint* x, bigint* n, bigint* M)
{
	int i = 0;
	int wordLen = n->wordLen;
	int L = get_bit_length(n);
	int n_i = 0;

	bigint* Q = NULL;
	bigint* tempC = NULL;

	// C <- 1
	bi_set_one(&tempC);

	for (i = L - 1; i >= 0; i--) 
	{
		/*
			C <- C^2 mod M
			1) C <- C*C
			2) C <- C mod M
		*/
		//1)
		SQU(C, tempC);

		//2)C = C'*M + R (R becomes C) -> Long_Div(bigint** Q, bigint** R, bigint* A: C, bigint* B: M)
		Multi_Long_DIV(&Q, &tempC, *C, M);
		bi_assign(C, tempC);

		/*
			C <- C * (x^n_i)
			=> if n_i = 1: C <- 1)C*x 2)mod M
			=> if n_i = 0: do not multiple
		*/
		n_i = get_jth_bit_of_bi(n, i);
		
		if (n_i) //n_i == 1
		{
			//1)
			MUL(&tempC, *C, x);

			//2)C = C'*M + R (R becomes C) -> Long_Div(bigint** Q, bigint** R, bigint* A: C, bigint* B: M)
			Multi_Long_DIV(&Q, C, tempC, M);
		}
		bi_assign(&tempC, *C);
	}
	bi_assign(C, tempC);
	bi_delete(&Q);
	bi_delete(&tempC);
}

/*
	> Right to Left Modular Exponentiaiton
	> C <- x^n mod N
	> Input: double pointer of C, pointer of X, pointer of n, pointer of N
	> Output: None
*/
void MOD_EXP_R2L(bigint** C, bigint* x, bigint* n, bigint* M)
{
	bigint* T0 = NULL;
	bigint* T1 = NULL; bigint* tempT1 = NULL;
	bigint* tempC = NULL;
	bigint* Q = NULL;

	int L = get_bit_length(n);
	int i = 0;
	int n_i = 0;

	//t0, t1 <- 1, x
	bi_set_one(&T0);
	bi_assign(&T1, x);

	for (i = 0; i < L; i++) 
	{
		n_i = get_jth_bit_of_bi(n, i);

		/*
			t0 <- t0*(t1^n_i) mod M
			-> if n_i = 1: t0 = (t0*t1) mod M
			-> if n_i = 0: t0 = t0 mod M
		*/
		if (n_i) //n_i == 1
		{
			// t0 <- t0 * t1
			MUL(&tempC, T0, T1);
			
			// t0 <- t0 mod M :  Long_Div(bigint** Q, bigint** R: C, bigint* A: t0, bigint* B: M)
			Multi_Long_DIV(&Q, C, tempC, M);
		}
		
		else //if n_i = 0: t0 <- t0 mod M
			Multi_Long_DIV(&Q, C, T0, M);

		bi_assign(&T0, *C);

		/*
			t1 <- t1^2 mod M
			1) t1 <- t1^2
			2) t1 <- t1 mod M
		*/
		//1)
		SQU(&tempT1, T1);
		
		//2)Long_Div(bigint** Q, bigint** R:t1 , bigint* A: t1, bigint* B: M)
		Multi_Long_DIV(&Q, &T1, tempT1, M);
	}
	bi_refine(*C);
	bi_delete(&T0);
	bi_delete(&T1); bi_delete(&tempT1);
	bi_delete(&tempC);
	bi_delete(&Q);
}

/*
	> Montgomery Modular Exponentiaiton
	> C <- x^n mod N
	> Input: double pointer of C, pointer of X, pointer of n, pointer of N
	> Output: None
*/
void MOD_EXP_Mon(bigint** C, bigint* X, bigint* n, bigint* N)
{
	int i = 0, L = get_bit_length(n);
	bigint* T0 = NULL;
	bigint* T1 = NULL;
	bigint* TMP = NULL;

	bi_set_one(&T0); //t0 <- 1
	bi_assign(&T1, X); //t1 <- x

	for (i = L - 1; i >= 0; i--)
	{
		MUL(&TMP, T0, T1);
		bi_reduction(&TMP, N);

		if (get_jth_bit_of_bi(n, i))
		{
			bi_assign(&T0, TMP); //t0 <- t0 * t1
			SQU(&TMP, T1); //t1 <- t1 * t1
			bi_reduction(&TMP, N);
			bi_assign(&T1, TMP);
		}
		else
		{
			bi_assign(&T1, TMP); //t1 <- t0 * t1
			SQU(&TMP, T0); //t0 <- t0 * t0
			bi_reduction(&TMP, N);
			bi_assign(&T0, TMP);
		}
	}
	bi_assign(C, T0);
	bi_delete(&T0); bi_delete(&T1); bi_delete(&TMP);
}

void MOD_EXP(bigint** C, bigint* x, bigint* n, bigint* M)
{
#if MOD_EXP_Type == 0 //L2R
	MOD_EXP_L2R(C, x, n, M);

#elif MOD_EXP_Type == 1//R2L
	MOD_EXP_R2L(C, x, n, M);

#elif MOD_EXP_Type == 2 //Montgomery Ladder
	MOD_EXP_Mon(C, x, n, M);
#endif

	return;
}
