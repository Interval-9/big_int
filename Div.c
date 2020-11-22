#include "bn_struct.h"

/*
	> Compute k such that 2^(w-1) <= 2^k * b < 2^w 
	> Input: a word b
	> Output: k
*/
int Compute_k(word b)
{
	int i;
	for (i = WORD_BITLEN - 1;i >= 0;i--)
	{
		if ((word)(b >> i) & 1)
			return (WORD_BITLEN - 1 - i);
	}
	return (WORD_BITLEN - 1);
}

/*
	> Compute the quotient of A divided by B (size of A(=A1*W + A0): 2 words, size of B: 1 word)
	> Input: address of word A0, address of word B, address of word Q
	> Output: None
*/
void Long_Div_2word(word* Q, word* A, word* B)
{
	int j; 
	word R; //remainder of A divided by B
	
	(*Q) = 0; R = *(A + 1); //(Q, R) <- (0, A1)
	for (j = WORD_BITLEN - 1;j >= 0;j--)
	{
		if (R >= ((word)1 << (WORD_BITLEN - 1))) //R >= 2^(w-1)
		{
			(*Q) = (*Q) + ((word)1 << j); //Q <- Q +  2^j
			R = 2 * R + (((*A) >> j) & 1) - (*B); //R <- 2*R + aj - B
		}
		else //R < 2^(w-1)
		{
			R = 2 * R + (((*A) >> j) & 1); //R <- 2*R + aj 
			if (R >= (*B)) //R >= B
			{
				(*Q) = (*Q) + ((word)1 << j); //Q <- Q +  2^j
				R = R - (*B); //R <- R - B
			}
		}
	}
}

/*
	> Compute quotient, Q and remainder, R of A divided by B with 2^(w-1) <= B < 2^w
	> Input: address of word Q, double pointer of bigint structure R, pointer of bigint struture A, B
	> Output: 0(Success) or -1(Failure)
*/
int DIVCC(word* Q, bigint** R, bigint* A, bigint* B)
{
	int n = A->wordLen, m = B->wordLen; 
	word* Aa = NULL; word* Ba = NULL; 
	bigint* Qtmp = NULL; bigint* Btmp = NULL; bigint* Rtmp = NULL;

	Aa = A->a; Ba = B->a;

	if (n == m)
	{
		(*Q) = (word)((*(Aa + n - 1)) / (*(Ba + m - 1))); //Q <- floor(Am-1 / Bm-1)
	}
	else if (n == m + 1)
	{
		if (*(Aa + m) == *(Ba + m - 1))
			(*Q) = MASK; //Q <- W - 1
		else
			Long_Div_2word(Q, Aa + m - 1, Ba + m - 1); //Q <- floor((Am * W + Am-1) / Bm-1) 
	}

	//R <- A - BQ
	bi_set_by_array(&Qtmp, Non_Negative, Q, 1);
	if (-1 == MUL(&Btmp, B, Qtmp)) //Btmp <- B * Qtmp
		return ERROR;
	if (-1 == SUB(&Rtmp, A, Btmp)) //Rtmp <- A - B * Qtmp
		return ERROR;

	while (Rtmp->sign == Negative) //Rtmp < 0
	{
		(*Q) = (*Q) - 1; //Q <- Q - 1
		if (-1 == ADD(R, B, Rtmp)) //R <- B + Rtmp
			return ERROR;
		bi_assign(&Rtmp, *R); //Rtmp <- R
	}
	bi_assign(R, Rtmp); //R <- Rtmp

	bi_delete(&Qtmp); bi_delete(&Btmp); bi_delete(&Rtmp); //Delete temporary variable

	return 0;
}

/*
	> Compute A', B' st 2^(w-1) <= B' < 2^w and quotient, Q' and remainder, R' of A' divided by B'
	> Input: address of word Q, double pointer of bigint structure R, pointer of bigint struture A, B
	> Output: 0(Success) or -1(Failure)
*/
int DIVC(word* Q, bigint** R, bigint* A, bigint* B)
{
	int k = 0;
	bigint* Atmp = NULL; bigint* Btmp = NULL;

	if (Compare(A, B) == -1) //A < B
	{
		(*Q) = 0; //Q = 0
		bi_assign(R, A); //R = A
	}
	else
	{
		k = Compute_k(*(B->a + B->wordLen - 1)); //Compute k such that 2 ^ (w - 1) <= 2 ^ k * b < 2 ^ w

		//Atmp <- 2^k * A, Btmp <- 2^k * B
		bi_assign(&Atmp, A);
		bi_assign(&Btmp, B);
		Left_shift(&Atmp, k);
		Left_shift(&Btmp, k);

		if (-1 == DIVCC(Q, R, Atmp, Btmp)) //(Q, Rtmp) <- DIVCC(Atmp, Btmp)
			return ERROR;
		Right_shift(R, k); //R <- 2^(-k) * Rtmp
	}
	bi_delete(&Atmp); bi_delete(&Btmp); //Delete temporary variable
	return 0;
}

/*
	> Compute quotient, Q and remainder, R of A divided by B using Multi-Precision Long Division
	> Input: double pointer of bigint structure Q and R, pointer of bigint structure A and B
	> Output: 0(Success) or -1(Failure)
*/
int Long_Div(bigint** Q, bigint** R, bigint* A, bigint* B)
{
	int i, n = A->wordLen, m = B->wordLen; 
	int sign_A = 0, sign_B = 0; 
	int ret = 0; 
	bigint* Rtmp = NULL; 

	if (Is_Zero(B)) //B = 0
		return ERROR;

	else if (Is_One(B)) //B = 1
	{
		bi_assign(Q, A); //Q = A
		bi_set_zero(R); //R = 0
		return 0;
	}

	else if (CompareABS(A, B) == -1) //|A| < |B|
	{
		bi_set_zero(Q); //Q = 0
		bi_assign(R, A); //R = A
		return 0;
	}
	
	sign_A = A->sign; sign_B = B->sign;
	A->sign = Non_Negative; B->sign = Non_Negative; //A <- |A|, B <- |B|

	if (-1 == bi_new(Q, n - 1)) //Q <- 0
		return ERROR;
	if (-1 == bi_new(&Rtmp, 1)) //Rtmp <- 0
		return ERROR;

	for (i = n - 1;i >= 0;i--) //n = word length of A
	{
		//R <- RW + Ai
		Left_shift(&Rtmp, WORD_BITLEN); //Rtmp <- Rtmp * W
		bi_assign(R, Rtmp); //R <- Rtmp
		*((*R)->a) = *(A->a + i); //R <- Rtmp * W + Ai (0 <= Ai < W)

		//(Qi, R) <- DIVC(R, B)
		if (-1 == DIVC((*Q)->a + i, &Rtmp, *R, B)) 
			return ERROR;
	}

	if (sign_A == Negative) //A < 0 
	{
		ret = Positive_ADD_1(Q); //Q <- Q'+ 1
		if (ret == 0)
		{
			(*Q)->sign = Negative; //Q <- -(Q' + 1)
			if (-1 == SUB(R, B, Rtmp)) //R <- B - R'
				return ERROR;
		}
	}
	else
		bi_assign(R, Rtmp); //R <- Rtmp

	if (sign_B == Negative) //B < 0
		(*Q)->sign = ((*Q)->sign) ^ 1; //Q <- -Q'
		
	A->sign = sign_A; B->sign = sign_B; 
	bi_refine(*Q);
	bi_delete(&Rtmp); //Delete temporary variable
	return 0;
}