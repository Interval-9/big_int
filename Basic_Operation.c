/*
	This file contains the basic operation functions required to implement the main functions.
*/

#include "bn_struct.h"

void array_init(word* a, int wordLen)	//initialize array a to 0 (not change sign)
{
	int i = 0;

	for (i = 0; i < wordLen; i++)
		a[i] = 0;
}

void bi_delete(bigint** x)	//Delete & Zerorize BigInt
{
	if (*x == NULL)
		return;

#ifdef ZERORIZE
	array_init((*x)->a, (*x)->wordLen);
#endif // ZERORIZE

	free((*x)->a);
	free(*x);
	*x = NULL;
}

void bi_new(bigint** x, int wordLen)	//Create BigInt
{
	if (*x != NULL)
		bi_delete(x);

	(*x) = (bigint*)malloc(sizeof(bigint));
	if (*x == NULL)
		return;

	(*x)->sign = Non_Negative;
	(*x)->wordLen = wordLen;
	(*x)->a = (word*)calloc(wordLen, sizeof(word));
}

int chton(char x)	//string to number
{
	int ascii = 0;
	
	//x: 0 ~ 9
	if ((x >= '0') & (x <= '9'))
		ascii = x - '0';

	//x: a ~ f
	else if ((x >= 'a') & (x <= 'f'))
		ascii = x - 87;

	//x: A ~ F
	else if ((x >= 'A') & (x <= 'F'))
		ascii = x - 55;

	//x: otherwise
	else
		return ERROR;

	return ascii;
}

void bi_set_by_array(bigint** x, int sign, word* arr, int wordLen)	//BigInt <- sign, word array
{
	/*
		====== WORD_BITLEN = 8 ======
		===== A = {0x12, 0x34, 0x56}					 =====
		===== A[0] = 0x12, A[1] = 0x34, A[2] = 0x56      =====
	*/
	int i = 0;

	bi_new(x, wordLen);

	(*x)->sign = sign;

	for (i = 0; i < wordLen; i++)
		(*x)->a[i] = arr[i];

	bi_refine(*x);
}

void bi_set_by_string(bigint** x, int sign, char* str, char base)	//BigInt <- base string
{
	if (str == NULL)
		return;

	if ((sign != Negative) & (sign != Non_Negative)) {
		printf("Wrong Sign: %d\n",sign);
		return;
	}

	/*
		====== WORD_BITLEN = 8 ======

		===== 0b 0001 0010 0011 =====	(base = 2)
		===== 0x   1    2    3  =====	(base = 16)
		===== 0d       291      =====	(base = 10)

		a[0] = 0b 0010 0011	||	a[1] = 0b 0000 0001
			 = 0x   2    3	||		 = 0x   0    1
	*/
	int i = 0;
	int wordLen = 0;
	int strLen = (int)strlen(str);

	word BB = 0;

	bigint* A = NULL;
	bigint* B = NULL;
	bigint* tmp = NULL;

	//X <- 0
	bi_set_zero(x);

	//A->a[0] = base
	bi_set_by_array(&A, Non_Negative, &base, 1);

	while (str[i] != '\0')	//until string is END
	{
		//x(tmp) <- x*A(base)
		MUL(&tmp, *x, A);

		BB = chton(str[i]);
		if (BB != ERROR) {
			bi_set_by_array(&B, Non_Negative, &BB, 1);	//B->a[0]=BB
			//x <- tmp(x*A) + chton(str[i])
			ADD(x, tmp, B);
		}
		i++;
	}

	(*x)->sign = sign;
	bi_delete(&A);
	bi_delete(&B);
	bi_delete(&tmp);
}

void bi_show(bigint* x, char base)
{
	int i = 0;
	int j = 0;
	int bit = 0;

	int sign = x->sign;
	int bitLen = get_bit_length(x);
	int wordLen = x->wordLen;

	if (sign == 1)
		printf("-");
	
	if (base == 2)
	{
		printf("0b");
		for (i = bitLen-1; i >= 0; i--) {
			printf("%d", get_jth_bit_of_bi(x, i));
		}
	}
	else if (base == 16)
	{
		printf("0x");
#if (WORD_BITLEN == 8)
		for (i = wordLen - 1; i >= 0; i--) {
			printf("%02x", x->a[i]);
		}
#elif (WORD_BITLEN == 32)
		for (i = wordLen - 1; i >= 0; i--) {
			printf("%08x", x->a[i]);
		}
#elif (WORD_BITLEN == 64)
		for (i = wordLen - 1; i >= 0; i--) {
			printf("%016llx", x->a[i]);
		}
#endif
	}
	else if (base == 10)
	{
		printf("0d");

		bigint* A = NULL;
		bigint* B = NULL;
		bigint* Q = NULL;
		bigint* R = NULL;

		word* temp = NULL;
		word* numArr = NULL;
		numArr = (word*)malloc(sizeof(word) * 1);

		int arrLen = 0;

		//A <- x
		bi_assign(&A, x);

		//B <- 10
		bi_new(&B, 1);
		B->a[0] = 10;

		//Q(quotient), R(remainder): output
		while (1) {
			temp = (word*)realloc(numArr, sizeof(word) * ((size_t)arrLen + 1));
			if (temp == NULL)
				return;
			numArr = temp;

			Multi_Long_DIV(&Q, &R, A, B);
			bi_assign(&A, Q);

			numArr[arrLen] = R->a[0];
			arrLen++;

			if (Is_Zero(A))
				break;
		}

		for (i = arrLen - 1; i >= 0; i--)
			printf("%d", numArr[i]);

		bi_delete(&A);
		bi_delete(&B);
		bi_delete(&Q);
		bi_delete(&R);
		free(temp);
	}
}

void bi_refine(bigint* x)	//Remove Last Zero Words
{
	word* p = NULL;
	
	int new_wordLen = 0;

	if (x == NULL)
		return;

	p = (x->a);
	new_wordLen = x->wordLen;

	while (new_wordLen > 1)	//at least one word needed
	{
		if (x->a[new_wordLen - 1] != 0)
			break;
		new_wordLen--;
	}
	if (x->wordLen != new_wordLen)
	{
		x->wordLen = new_wordLen;
		p = (word*)realloc(p, sizeof(word) * new_wordLen);
		if (p == NULL)
			return;

		x->a = p;
	}

	if ((x->wordLen == 1) && (*(x->a) == 0x0))
		x->sign = Non_Negative;
}

/*
	> copy array b to array a
	> not change sign
	> wordLen(a) = wordLen(b)
*/
void array_copy(word* a, word* b, int wordLen)
{
	int i = 0;
	for (i = 0; i < wordLen; i++)
		a[i] = b[i];
}

void bi_assign(bigint** y, bigint* x)	//assign bigint x to new bigint structure y
{
	if (*y != NULL)
		bi_delete(y);

	bi_new(y, x->wordLen);

	(*y)->sign = x->sign;
	array_copy((*y)->a, x->a, x->wordLen);
}

void array_rand(word* dst, int wordLen)
{	
	unsigned char* p = NULL;

	int cnt = wordLen * sizeof(word);

	p = (unsigned char*)dst;
	while (cnt > 0) {
		*p = rand() & 0xff;
		p++;
		cnt--;
	}
}

void bi_gen_rand(bigint** x, int sign, int wordLen)
{
	bi_new(x, wordLen);

	(*x)->sign = sign;
	array_rand((*x)->a, wordLen);
}

/*
	WORD_BITLEN = 8

	===== get_word_length ======
	wordLen = bigint(A)->wordLen
	============================

	===== get_bit_length =====
	bitLen = len( last(1) ~ 0th-bit )
	==========================

	===== get_jth_bit_of_bi =====
	0b 0010 1011 0010
	-> 9th: 1, 8th: 0,	7th: 1, 6th: 0, 5th: 1, 4th: 1,	3th:0, 2nd: 0, 1st: 1, 0th: 0
	=============================
*/
int get_word_length(bigint* x)	//after refine x, get word length of x
{
	int wordLen = x->wordLen;
	
	return wordLen;
}

int get_bit_length(bigint* x)	//get bit length by get word length
{
	int i = 0;
	int bitLen = (get_word_length(x) - 1) * WORD_BITLEN;
	word last_word = x->a[(x->wordLen) - 1];

	for (i = WORD_BITLEN - 1;i > 0;i--)
	{
		if (((last_word >> i) & 0x01) != 0)
			break;
	}

	bitLen += (i + 1);
	return bitLen;
}

int get_jth_bit_of_bi(bigint* x, int j)
{
	int jth_bit = 0;
	
	//arr_idx where jth-bit located in bigint array
	int arr_idx = j / WORD_BITLEN;
	//bit_idx where jth_bit located in arr_idx
	int bit_idx = j % WORD_BITLEN;
	
	jth_bit = ((x->a[arr_idx]) >> bit_idx) & 0x1;
	return jth_bit;
}

int get_bi_sign(bigint* x)	//get sign from bigint structure
{
	int bi_sign = x->sign;
	return bi_sign;
}

void flip_bi_sign(bigint* x)	//sign = 0(1) -> flip_sign = 1(0)
{
	int sign = x->sign;

	if (sign == 1)
		x->sign = 0;
	else
		x->sign = 1;
}

void bi_set_one(bigint** x)	//create big int 1
{
	bi_new(x, 1);

	(*x)->sign = Non_Negative;
	(*x)->a[0] = 0x1;
}
void bi_set_zero(bigint** x)	//create big num 0
{
	bi_new(x, 1);

	(*x)->sign = Non_Negative;
	(*x)->a[0] = 0x0;
}

int Is_Zero(bigint* x)	//x == 0 -> 1(TRUE),	x != 0 -> 0(FALSE)
{
	if ((x->sign == 1) | (x->a[0] != 0))
		return FALSE;

	int i = 0;
	int wordLen = x->wordLen;
	for (i = wordLen - 1; i >= 0; i--)
		if (x->a[i] != 0)
			return FALSE;

	return TRUE;
}
int Is_One(bigint* x)	//x == 1 -> 1(TRUE),	x != 1 -> 0(FALSE)
{
	if ((x->sign == 1) | (x->a[0] != 1))
		return FALSE;

	int i = 0;
	int wordLen = x->wordLen;
	for (i = wordLen - 1; i >= 0; i--)
		if (x->a[i] != 1)
			return FALSE;

	return TRUE;
}

int Is_NegativeOne(bigint* x)	//x == -1 -> 1(TRUE),	x != -1 -> 0(FALSE)
{
	if ((x->sign == 0) || (x->a[0] != 1))
		return FALSE;

	int i = 0;
	int wordLen = x->wordLen;
	for (i = wordLen - 1; i >= 0; i--)
		if (x->a[i] != 1)
			return FALSE;

	return TRUE;
}

/*
	> Compare abs(A), abs(B)
	> return: 1(|A|>|B|) or -1(|A|<|B|) or 0(|A|=|B|)
*/
int CompareABS(bigint* A, bigint* B) 
{
	int n = 0, m = 0, j = 0;
	n = get_word_length(A), m = get_word_length(B);

	if (n > m)		//wordLen(|A|) > wordLen(|B|)
		return 1;

	if (n < m)	//wordLen(|A|) > wordLen(|B|)
		return -1;

	for (j = n - 1;j >= 0;j--)		//wordLen(|A|) = wordLen(|B|)
	{
		if (A->a[j] > B->a[j])		//|A| > |B|
			return 1;
		if (A->a[j] < B->a[j])	//|A| < |B|
			return -1;
	}
	return 0;
}

/*
	> Compare A, B
	> return: 1(A>B) or -1(A<B) or 0(A=B)
*/
int Compare(bigint* A, bigint* B)
{
	int ret = 0;

	if ((A->sign == 0) && (B->sign == 1))		//A: non_negative, B: negative
		return 1;
	else if ((A->sign == 1) && (B->sign == 0))	//A: negative, B: non_negative
		return -1;

	//if AB >= 0: compare abs(A), abs(B)
	ret = CompareABS(A, B);
	if (A->sign == 0)	// A, B: non_negative
		return ret;
	else                // A, B: negative
		return (-1) * ret;
}

void Left_shift(bigint** x, int r) //A << r   
{
	word* p = NULL;
	p = (*x)->a;

	int i = 0, k = 0, rr = 0, new_wordlen = 0, old_x_wordlen = 0;
	old_x_wordlen = (*x)->wordLen;

	/*
	   ===========================
	   r = k * WORD_BITLEN(w) + rr
	   ===========================
	*/
	if (r >= WORD_BITLEN) {
		k = (int)(r / WORD_BITLEN);
		rr = r - k * WORD_BITLEN;
	}
	else
		rr = r;

	//Case 1: r = wk
	if (rr == 0) {
		new_wordlen = (*x)->wordLen + k;

		word* temp = NULL;
		temp = (word*)realloc(p, sizeof(word) * new_wordlen);
		if (temp != NULL) {
			p = temp;
			(*x)->a = p;

			//(A_n+k-1, ..., A_k) <- (A_n-1, ..., A_0)
			for (i = 0; i < old_x_wordlen; i++)
				(*x)->a[old_x_wordlen + k - 1 - i] = (*x)->a[old_x_wordlen - 1 - i];

			//(A_k-1, ..., A_0) <- (0, 0, ..., 0)
			array_init((*x)->a, k);
		}
		else
			return;
	}

	//Case 2: r = wk + rr
	else if ((rr > 0) && (rr < WORD_BITLEN))
	{
		//new_wordlen = ((*x)->wordLen + (k + 1));
		new_wordlen = (old_x_wordlen + (k + 1));

		word* temp = NULL;
		temp = (word*)realloc(p, sizeof(word) * new_wordlen);	//size(array): new_wordlen = n(old_x_wordlen)+ k + 1
		if (temp != NULL) {
			p = temp;
			(*x)->a = p;

			//A_n+k <- (A_(n-1) >> (w - rr))
			(*x)->a[old_x_wordlen + k] = (((*x)->a[old_x_wordlen - 1]) >> (WORD_BITLEN - rr));

			//A_n+k-j <- (A_n-j << rr) | (A_n-j-1 >> (w - rr))
			for (i = 1; i < old_x_wordlen; i++)
				(*x)->a[old_x_wordlen + k - i] = (((*x)->a[old_x_wordlen - i] << rr) | ((*x)->a[old_x_wordlen - 1 - i] >> (WORD_BITLEN - rr)));

			//A_k <- (A_0 << r)
			(*x)->a[k] = ((*x)->a[0] << rr);

			//(A_k-1, ..., A_0) <- (0, 0, ..., 0)
			array_init((*x)->a, k);
		}
		else
			return;
	}
	(*x)->wordLen = new_wordlen;

	bi_refine(*x);
}

void Right_shift(bigint** x, int r) //A >> r
{
	int i = 0, k = 0, rr = 0;
	int wordLen = (*x)->wordLen;

	//r >= wn 
	if (r >= wordLen * WORD_BITLEN)
	{
		//initialize x to 0
		array_init((*x)->a, wordLen);
	}
	else
	{
		//r = k * w + rr
		if (r >= WORD_BITLEN)
		{
			k = (int)(r / WORD_BITLEN);
			rr = r - k * WORD_BITLEN;
		}
		else
			rr = r;
	
		//r = wk
		if (rr == 0)
		{
			array_copy((*x)->a, (*x)->a + k, wordLen - k);	//(A_n-k-1,..., A_0) <- (A_n-1,..., A_k+1, A_k)
			array_init((*x)->a + wordLen - k, k);		//(A_n-1,..., A_n-k) <- (0, 0,..., 0)
		}
		//r = wk + rr
		else if (rr > 0)
		{
			//A_j <- (A_k+j+1 << w - rr) | (A_k+j >> rr)
			for (i = 0;i < wordLen - k - 1;i++)
				(*x)->a[i] = ((*x)->a[k + 1 + i] << (WORD_BITLEN - rr)) | ((*x)->a[k + i] >> rr);

			//A_n-k-1 <-(A_n-1 >> rr)
			(*x)->a[wordLen - k - 1] = ((*x)->a[wordLen - 1] >> rr);

			//(A_n-1,..., A_n-k) <- (0, 0,..., 0)
			array_init((*x)->a + wordLen - k, k);
		}
	}

	bi_refine(*x);
}

/*
	> Input: x, r
	> Output: x mod 2^r
*/
void Reduction(bigint** x, int r)
{
	int i = 0, k = 0, rr = 0;
	int wordLen = (*x)->wordLen;

	//r < wn
	if (r < wordLen * WORD_BITLEN)
	{
		//r = k * w + rr
		if (r >= WORD_BITLEN)
		{
			k = (int)(r / WORD_BITLEN);
			rr = r - k * WORD_BITLEN;
		}
		else
			rr = r;
		
		//r = wk
		if (rr == 0)
		{
			//(A_n-1,..., A_k) <- (0, 0,..., 0)
			for (i = k; i < wordLen; i++)
				(*x)->a[i] = 0;
		}
		//r = wk + rr
		else
		{
			(*x)->a[k] = (*x)->a[k] & ((1 << rr) - 1);
			//(A_n-1,..., A_k) <- (0, 0,..., 0)
			for (i = k + 1;i < wordLen;i++)
				(*x)->a[i] = 0;
		}
	}

	bi_refine(*x);
}