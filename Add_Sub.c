#include "bn_struct.h"

/*
	> Word addition
	> Cj <- Aj + Bj + cb(carry) (0 <= Cj, Aj, Bj < 2^WORD_BITLEN, cb: 0 or 1)
	> Input: address of word Cj, a word Aj, a word Bj, carry of the results of the previous operation, cb
	> Ouput: carry of current result (0 or 1)
*/
int ADD_ABc(word* Cj, word Aj, word Bj, int cb)
{
	int c = 0;

	(*Cj) = (word)(Aj + Bj);	//Cj = (Aj + Bj) mod word
	if (*Cj < Aj)				//carry
		c = 1;

	(*Cj) = (word)((*Cj) + cb); //Cj + carry
	if (*Cj < cb)				//carry
		c = 1;

	return c;
}

/*
	> Bigint addition 
	> C <- A + B (wordLen(A) >= wordLen(B))
	> Input: double pointer of bigint C, pointer of bigint A, pointer of bigint B
	> Output: None
*/
void ADDC(bigint** C, bigint* A, bigint* B)
{
	int max_wordLen = 0, old_B_wordLen = 0;
	int c = 0, j = 0; //c(carry) <- 0

	word* Cp = NULL;
	word* Bp = NULL;

	//Compute wordLen(C) & Create bigint C
	max_wordLen = (A->wordLen);
	bi_new(C, max_wordLen + 1);

	Cp = (*C)->a;

	//realloc B
	old_B_wordLen = B->wordLen;
	Bp = (word*)realloc(B->a, sizeof(word) * max_wordLen);
	if (Bp == NULL)
		return;
	else
		B->a = Bp;

	//initialize B_m ~ B_(n-1) to 0
	array_init(((B->a) + old_B_wordLen), (max_wordLen - old_B_wordLen));

	//C, c <- (A[j] + B[j] + c)
	for (j = 0; j < max_wordLen; j++)
		c = ADD_ABc(Cp + j, *(A->a + j), *(B->a + j), c);

	//if carry remains when compute LSB words
	if (c == 1)
		Cp[max_wordLen] = 1;

	(*C)->sign = A->sign;
	B->wordLen = max_wordLen;
	bi_refine(B);	
	bi_refine(*C);
}

/*
	> 1 addition to bigint X (used by Naive division)
	> X <- X + 1
	> Input: double pointer of bigint X
	> Output: None
*/
void Positive_ADD_1(bigint** X)
{
	int j = 0, c1 = 1, c2 = 0;
	int X_wordLen = (*X)->wordLen;

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
	
	//if carry remains when compute LSB words
	if (c2 == 1)
	{
		Xa = (word*)realloc((*X)->a, (long long)X_wordLen + 1);
		if (Xa == NULL)
			return;

		(*X)->a = Xa;
		*((*X)->a + X_wordLen) = c2;
		(*X)->wordLen = X_wordLen + 1;
	}
}

/*
	> Bigint addition
	> C <- A + B
	> Input: double pointer of bigint C, pointer of bigint A, pointer of bigint B
	> Output: None
*/
void ADD(bigint** C, bigint* A, bigint* B)
{
	if (Is_Zero(A) == 1) //A = 0, C <- B
	{
		bi_assign(C, B);
		return;
	}
	if (Is_Zero(B) == 1) //B = 0, C <- A
	{
		bi_assign(C, A);
		return;
	}
	if ((A->sign == 0) & (B->sign == 1)) //A > 0, B < 0
	{
		if (CompareABS(A, B) == -1) //|A| < |B|, A + B = |A| - |B| < 0
		{
			SUBC(C, B, A);
			(*C)->sign = Negative;
			return;
		}
		else //|A| > |B|, A + B = |A| - |B| > 0
		{ 
			SUBC(C, A, B);
			return;
		}
	}
	if ((A->sign == 1) & (B->sign == 0)) //A < 0, B > 0
	{
		// |A| > |B|, A + B = |A| - |B| < 0
		if (CompareABS(A, B) == 1)
		{
			SUBC(C, A, B);
			(*C)->sign = Negative;
			return;
		}
		else { // |A| < |B|, A + B = |A| - |B| > 0
			SUBC(C, B, A);
			return;
		}
	}
	if((A->wordLen) >= (B->wordLen)) //wordLen(A) >= wordLen(B)
	{
		ADDC(C, A, B);
		return;
	}
	else //wordLen(A) < wordLen(B)
	{
		ADDC(C, B, A);
		return;
	}
}

/*
	> Word subtraction
	> Cj <- Aj - borrow(bb) + Bj (0 <= Cj, Aj, Bj < 2^WORD_BITLEN)
	> Input: address of word Cj, a word Aj, a word Bj, borrow of the results of the previous operation, bb
	> Ouput: borrow of current result (0 or 1)
*/
int SUB_AbB(word* Cj, word Aj, int bb, word Bj)
{
	int b = 0;

	(*Cj) = (word)(Aj - bb);
	if (Aj < bb)
		b = 1;

	if ((*Cj) < Bj)
		b = 1;

	(*Cj) = (word)((*Cj) - Bj);

	return b;
}

/*
	> Bigint subtraction
	> C <- A - B (A > B > 0)
	> Input: double pointer of bigint C, pointer of bigint A, pointer of bigint B
	> Output: None
*/
void SUBC(bigint** C, bigint* A, bigint* B)
{
	int max_wordLen = 0;
	int b = 0, j = 0; //b(borrow) <- 0

	word* Cp = NULL;
	word* Bp = NULL;

	//Compute wordLen(C) & Create bigint C
	max_wordLen = (A->wordLen);
	bi_new(C, max_wordLen);

	Cp = (*C)->a;
	Bp = (word*)realloc(B->a, sizeof(word) * max_wordLen); 
	if (Bp == NULL)
		return;
	else
		B->a = Bp;
		
	//initialize Bm ~ Bn-1 to 0
	array_init(((B->a) + (B->wordLen)), (max_wordLen - (B->wordLen)));	

	 //C, b <- (A - B - b)
	for (j = 0; j < max_wordLen; j++)
		b = SUB_AbB(Cp + j, *(A->a + j), b, *(B->a + j));

	B->wordLen = max_wordLen;	
	bi_refine(B);
	bi_refine(*C);
}

/*
	> Bigint subtraction
	> C <- A - B
	> Input: double pointer of bigint C, pointer of bigint A, pointer of bigint B
	> Output: None
*/
void SUB(bigint** C, bigint* A, bigint* B)
{
	if (Is_Zero(A) == 1) //A = 0, C <- (-B)
	{
		bi_assign(C, B);
		flip_bi_sign(*C);
		return;
	}

	if (Is_Zero(B) == 1) //B = 0, C <- A
	{
		bi_assign(C, A);
		return;
	}

	else if (!Compare(A, B)) //A = B, C <- 0
	{
		bi_set_zero(C);
		return;
	}

	else if ((A->sign == 0) & (B->sign == 0)) //A > 0, B > 0
	{
		if (Compare(A, B) == 1) // A > B > 0
		{
			SUBC(C, A, B);
			return;
		}
		
		else if (Compare(A, B) == -1) // B > A > 0
		{
			SUBC(C, B, A);
			(*C)->sign = Negative;
			return;
		}
	}

	else if ((A->sign == 1) & (B->sign == 1)) //A < 0, B < 0
	{
		
		if (Compare(A, B) == 1) //0 > A > B
		{
			SUBC(C, B, A);
			return;
		}

		else if (Compare(A, B) == -1) //0 > B > A
		{
			SUBC(C, A, B);
			(*C)->sign = Negative;
			return;
		}
	}

	else if ((A->sign == 0) & (B->sign == 1)) //A > 0 , B < 0
	{
		if (A->wordLen > B->wordLen) //A - B = A - (-|B|) = A + |B| > 0 
			ADDC(C, A, B);
		else
			ADDC(C, B, A);
		(*C)->sign = Non_Negative;
		return;
	}

	else if ((A->sign == 1) & (B->sign == 0)) //A < 0 , B > 0
	{
		if (A->wordLen > B->wordLen) //A - B = (-|A|) - B = -(|A| + B) < 0 
			ADDC(C, A, B);
		else
			ADDC(C, B, A);
		(*C)->sign = Negative;
		return;
	}
}