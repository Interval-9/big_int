/*
	This file contains the division function, one of the main functions, and other functions necessary to implement the division function.
*/
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
	> A1 should be less than B
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
	> Compute quotient, Q and remainder, R of A divided by B with (2^(w-1)) <= B < (2^w)
	> Input: address of word Q, double pointer of bigint structure R, pointer of bigint struture A, B
	> Output: None
*/
void DIVCC(word* Q, bigint** R, bigint* A, bigint* B)
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
	MUL_word_bigint(&Btmp, B, *Q);
	SUB(&Rtmp, A, Btmp); //Rtmp <- A - B * Q

	while (Rtmp->sign == Negative) //Rtmp < 0
	{
		(*Q) = (*Q) - 1; //Q <- Q - 1
		ADD(R, B, Rtmp); //R <- B + Rtmp
		bi_assign(&Rtmp, *R); //Rtmp <- R
	}
	bi_assign(R, Rtmp); //R <- Rtmp

	bi_delete(&Qtmp); bi_delete(&Btmp); bi_delete(&Rtmp); //Delete temporary variable
}

/*
	> Compute A', B' st 2^(w-1) <= B' < 2^w and quotient, Q' and remainder, R' of A' divided by B'
	> Input: address of word Q, double pointer of bigint structure R, pointer of bigint struture A, B
	> Output: None
*/
void DIVC(word* Q, bigint** R, bigint* A, bigint* B)
{
	int k = 0;
	bigint* Atmp = NULL; 
	bigint* Btmp = NULL;

	if (Compare(A, B) == -1) //A < B
	{
		(*Q) = 0; //Q = 0
		bi_assign(R, A); //R = A
	}
	else
	{
		k = Compute_k(*(B->a + B->wordLen - 1)); //Compute k such that (2 ^ (w - 1)) <= (2 ^ k * b) < (2 ^ w)

		//Atmp <- 2^k * A, Btmp <- 2^k * B
		bi_assign(&Atmp, A);
		bi_assign(&Btmp, B);
		Left_shift(&Atmp, k);
		Left_shift(&Btmp, k);

		DIVCC(Q, R, Atmp, Btmp); //(Q, Rtmp) <- DIVCC(Atmp, Btmp)
		Right_shift(R, k); //R <- 2^(-k) * Rtmp
	}
	bi_delete(&Atmp); bi_delete(&Btmp); //Delete temporary variable
}

/*
	> Compute quotient, Q and remainder, R of A divided by B using Multi-Precision Long Division
	> Input: double pointer of bigint structure Q and R, pointer of bigint structure A and B
	> Output: None
*/
void Multi_Long_DIV(bigint** Q, bigint** R, bigint* A, bigint* B)
{
	int i, n = A->wordLen, m = B->wordLen;
	int sign_A = 0, sign_B = 0; 
	int ret = 0; 
	bigint* tempR = NULL; 
	
	if ((Is_Zero(B)) | (B->sign == 1)) //B <= 0
		return;
	
	if (Is_One(B)) //B = 1
	{
		bi_assign(Q, A); //Q = A
		bi_set_zero(R); //R = 0
		return;
	}
	
	if (CompareABS(A, B) == -1) // A < B
	{
		bi_set_zero(Q);	//Q = 0
		bi_assign(R, A); //R = A
		return;
	}
	
	bi_new(Q, n); //Q <- 0
	bi_set_zero(R);	// R <- 0
	for (i = n - 1; i >= 0; i--) 
	{
		/*
			R <- R*W + A_i
			1) R*W
			2) R <- R + A_i
		*/
		//1)
		Left_shift(R, WORD_BITLEN);
		//2)
		(*R)->a[0] = A->a[i];

		//(Q_i,(temp)R) <- DIVC(R, B)
		DIVC((*Q)->a + i, &tempR, *R, B);
		bi_assign(R, tempR);
	}
	bi_refine(*Q);
	bi_refine(*R);

	bi_delete(&tempR);
	return;
}

/*
	> Compute quotient, Q and remainder, R of A divided by B using Binary Long Division
	> Input: double pointer of bigint structure Q and R, pointer of bigint structure A and B
	> Output: None
*/
void Binary_Long_DIV(bigint** Q, bigint** R, bigint* A, bigint* B)
{
	int j, n;
	word aj = 0;
	bigint* Rtmp = NULL;
	bigint* Qtmp = NULL;
	bigint* q = NULL;

	n = (A->wordLen) * WORD_BITLEN;

	//(Q, R) <- (0, 0)
	bi_set_zero(Q);
	bi_set_zero(R);

	for (j = n - 1;j >= 0;j--)
	{
		//R <- 2R + aj
		Left_shift(R, 1);
		aj = get_jth_bit_of_bi(A, j);
		(*R)->a[0] = ((*R)->a[0]) ^ aj;
		
		if ((Compare(*R, B) == 1) || (Compare(*R, B) == 0)) //R >= B
		{
			//R <- R - B
			SUB(&Rtmp, *R, B);
			bi_assign(R, Rtmp);
			
			bi_set_one(&q);
			Left_shift(&q, j); //q <- 2^j

			//Q <- Q + 2 ^ j
			ADD(&Qtmp, *Q, q);
			bi_assign(Q, Qtmp);
		}
	}
	bi_delete(&Rtmp); bi_delete(&Qtmp); bi_delete(&q);
}

/*
	> Compute quotient, Q and remainder, R of A divided by B using Naive Division
	> Input: double pointer of bigint structure Q and R, pointer of bigint structure A and B
	> Output: None
*/
void Naive_DIV(bigint** Q, bigint** R, bigint* A, bigint* B)
{
	bigint* Rtmp = NULL;

	if ((B->sign) | (Is_Zero(B))) //B <= 0
		return;

	if (Compare(B, A) == 1) //A < B
	{
		bi_set_zero(Q); //Q = 0
		bi_assign(R, A); //R = A
		return;
	}
	if (Is_One(B)) //B = 1
	{
		bi_assign(Q, A); //Q = A
		bi_set_zero(R); //R = 0
		return;
	}
	bi_set_zero(Q); //Q <- 0
	bi_assign(R, A); // R <- A

	
	while (!(Compare(*R, B) == -1)) //R >= B
	{
		Positive_ADD_1(Q); //Q <- Q + 1
		SUB(&Rtmp, *R, B); //R <- R - B
		bi_assign(R, Rtmp);
	}
	bi_delete(&Rtmp);
}

/*
	> Compute quotient, Q and remainder, R of A divided by B according to DIV_Type defined in "bn_struct.h"
	> Input: double pointer of bigint structure Q and R, pointer of bigint structure A and B
	> Output: None
*/
void DIV(bigint** Q, bigint** R, bigint* A, bigint* B)
{
#if DIV_Type == 0 //Naive Division
	Naive_DIV(Q, R, A, B);
#elif DIV_Type == 1 //Binary Long Division
	Binary_Long_DIV(Q, R, A, B);
#elif DIV_Type == 2 //Multi-Precision Long Division
	Multi_Long_DIV(Q, R, A, B);
#endif
	return;
}