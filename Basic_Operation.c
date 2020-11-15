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

int bi_new(bigint** x, int wordLen)	//Create BigInt: -1(error), 0(success)
{
	if (*x != NULL)
		bi_delete(x);

	(*x) = (bigint*)malloc(sizeof(bigint));
	if (*x == NULL)
		return ERROR;

	(*x)->sign = Non_Negative;
	(*x)->wordLen = wordLen;
	(*x)->a = (word*)calloc(wordLen, sizeof(word));
	return 0;
}

int chton(char x)	//string to number
{
	int ascii = 0;
	if (x >= '0' && x <= '9')
		ascii = x - '0';
	else  //x�� a���� f�����϶�
		ascii = x - 87;
	return ascii;
}

void bi_set_by_array(bigint** x, int sign, word* arr, int wordLen)	//BigInt <- sign, word array
{
	/*
		====== WORD_BITLEN = 8 ======
		===== A = {0x12, 0x34, 0x56}					 =====
		===== a[0] = 0x12, a[1] = 0x34, a[2] = 0x56      =====
	*/
	int check = 0;
	check = bi_new(x, wordLen);
	if (check == ERROR)
		printf("Fail: bi_set_by_array(bi_new)\n");

	(*x)->sign = sign;

	int i = 0;
	for (i = 0; i < wordLen; i++)
		(*x)->a[i] = arr[i];

	check = bi_refine(*x);
	if (check == ERROR)
		printf("Fail: bi_set_by_array(bi_refine)\n");
}

void bi_set_by_string(bigint** x, int sign, char* str, char base)	//BigInt <- base string
{
	int wordLen = 0;			//���� �迭 ����
	int strLen = strlen(str);	//���ڿ� ����
	
	/*
		====== WORD_BITLEN = 8 ======

		===== 0b 0001 0010 0011 =====	(base = 2)
		===== 0x   1    2    3  =====	(base = 16)
		===== 0d       291      =====	(base = 10)

		a[0] = 0b 0010 0011	||	a[1] = 0b 0000 0001
			 = 0x   2    3	||		 = 0x   0    1
	*/

	if (base == 2) // 2���� ���ڿ��� ���
	{
		int i = 0;
		int j = 0;

		wordLen = (int)ceil((double)strLen / WORD_BITLEN);	//�ʿ��� word�� ������ ���
		int check = bi_new(x, wordLen);						//bigint�� ������ ����ü ����
		if (check == ERROR)
			printf("Fail: bi_set_by_string(bi_new)\n");
		(*x)->sign = sign;

		for (i = 0; i < wordLen; i++)	//0�� 1�� �ϳ��� �о word ������ ����
		{
			for (j = 0; j < WORD_BITLEN; j++) {
				(*x)->a[i] ^= (chton(str[strLen - (WORD_BITLEN * i) - j - 1]) << j);
				if (strLen - (WORD_BITLEN * i) - j - 1 == 0)
					break;
			}
		}
	}
	else if (base == 16)
	{
		int i = 0;
		int j = 0;
		int byteLen = 2*sizeof(word);	//byteLen: unsigned char(2), unsigned int(8), unsigned long long(16)

		wordLen = (int)ceil((double)strLen / byteLen);	//�ʿ��� word�� ������ ���
		int check = bi_new(x, wordLen);					//bigint�� ������ ����ü ����
		if (check == ERROR)
			printf("Fail: bi_set_by_string(bi_new)\n");
		(*x)->sign = sign;

		for (i = 0; i < wordLen; i++)	//���ڸ� �ϳ��� �о word ������ ����
		{
			for (j = 0; j < byteLen; j++) {
				(*x)->a[i] ^= (chton(str[strLen - (byteLen * i) - j - 1]) << (4 * j));
				if (strLen - (byteLen * i) - j - 1 == 0)
					break;
			}
		}
	}
	else if(base == 10)   //base = 10
	{
		
	}

}

void bi_show(bigint* x, char base)
{
	int i = 0;
	int j = 0;
	int bit = 0;

	int sign = x->sign;
	int wordLen = x->wordLen;

	if (sign == 1)
		printf("-");
	
	if (base == 2)
	{
		printf("0b ");
		for (i = wordLen - 1; i >= 0; i--) {
			for (j = 0; j < WORD_BITLEN; j++) {
				bit = ((x->a[i]) >> (WORD_BITLEN - j - 1)) & 0x1;
				printf("%d", bit);
			}printf(" ");
		}
	}
	else if (base == 16) //16������ ���
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
	else if (base == 10) //10������ ���
	{
		printf("0d ");
		unsigned int num = 0;
		for (i = 0; i < wordLen; i++)
			num ^= ((x->a[i]) << (8 * sizeof(word) * i));
		printf("%u", num);
	}
	//printf("\n");
}

int bi_refine(bigint* x)	//Remove Last Zero Words
{
	if (x == NULL)
		return ERROR;

	word* p = NULL;
	p = (x->a);
	int new_wordLen = x->wordLen;

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
			return ERROR;

		x->a = p;
	}

	if ((x->wordLen == 1) && (*(x->a) == 0x0))
		x->sign = Non_Negative;
	return 0;
}

void array_copy(word* a, word* b, int wordLen)	//copy array b to array a (not change sign)
{
	//a�� b�� wordLen�� ���ƾ� ��
	int i = 0;
	for (i = 0; i < wordLen; i++)
		a[i] = b[i];
}

void bi_assign(bigint** y, bigint* x)	//assign bigint x to new bigint structure y
{
	if (*y != NULL)
		bi_delete(y);

	int check = bi_new(y, x->wordLen);
	if (check == ERROR)
		printf("Fail: bi_assign(bi_new)");

	(*y)->sign = x->sign;
	array_copy((*y)->a, x->a, x->wordLen);
}

void array_rand(word* dst, int wordLen)
{	
	//rand() �̿�
	byte* p = (byte*)dst;
	int cnt = wordLen * sizeof(word);
	while (cnt > 0) {
		*p = rand() & 0xff;
		p++;
		cnt--;
	}
}

void bi_gen_rand(bigint** x, int sign, int wordLen)
{
	int check = bi_new(x, wordLen);
	if (check == ERROR)
		printf("Fail: bi_gen_rand(bi_new)");
	(*x)->sign = sign;
	array_rand((*x)->a, wordLen);
}

/*
	WORD_BITLEN = 8

	== 0b 0001 0010 0011 ==	(base = 2)
	== 0x   1    2    3  ==	(base = 16)

	a[0] = 0b 0010 0011	||	a[1] = 0b 0000 0001
		 = 0x   2    3	||		 = 0x   0    1

	===== get_word_length ======
	wordLen <- 4
	============================

	===== get_bit_length =====
	bitLen <- 4*WORD_BITLEN
	==========================

	===== get_jth_bit_of_bi =====
	j starts at 1
	0b 0001 0010 0011 = 0b 0000 0001 0010 0011
	j = 4 -> bit  = 0
	j = 11 -> bit = 1
	=============================
*/
int get_word_length(bigint* x)	//after refine x, get word length of x
{
	int wordLen = 0;

	int check = bi_refine(x);
	if (check == ERROR)
		printf("Fail: get_word_length(bi_refine)");

	wordLen = x->wordLen;
	
	return wordLen;
}

int get_bit_length(bigint* x)	//get bit length by get word length
{
	int i;
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
	
	//j��° ��Ʈ�� ��� �迭(arr_idx)�� �ش��ϴ���
	int arr_idx = get_word_length(x) - 1 - ((int)j / WORD_BITLEN);
	//�� �迭���� �� ��°(bit_idx)�� �ش��ϴ���
	int bit_idx = j % WORD_BITLEN;

	jth_bit = ((x->a[arr_idx]) >> (WORD_BITLEN - bit_idx)) & 1;
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
	int check = bi_new(x, 1);
	if (check == ERROR)
		printf("Fail: bi_set_one(bi_new)");

	(*x)->sign = Non_Negative;
	(*x)->a[0] = 0x1;
}
void bi_set_zero(bigint** x)	//create big num 0
{
	int check = bi_new(x, 1);
	if (check == ERROR)
		printf("Fail: bi_set_zero(bi_new)");

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

int CompareABS(bigint* A, bigint* B) //���� ũ�� ��: 1(A>B) or -1(A<B) or 0(A=B)
{
	int n = 0, m = 0, j = 0;
	n = get_word_length(A), m = get_word_length(B);

	if (n > m)		//A�� ���� ���̰� �� �� ���
		return 1;

	else if (n < m)	//B�� ���� ���̰� �� �� ���
		return -1;

	for (j = n - 1;j >= 0;j--)		//A�� ���� ���� = B�� ���� ���� �� ���
	{
		if (A->a[j] > B->a[j])		//A > B
			return 1;
		else if (A->a[j] < B->a[j])	//A < B
			return -1;
	}
	return 0;
}

int Compare(bigint* A, bigint* B) //���� ũ�� ��: 1(A>B) or -1(A<B) or 0(A=B)
{
	int ret = 0;

	if ((A->sign == 0) && (B->sign == 1))		//A: non_negative, B: negative
		return 1;
	else if ((A->sign == 1) && (B->sign == 0))	//A: negative, B: non_negative
		return -1;

	ret = CompareABS(A, B);	//AB >= 0 �� ���: ���� ��

	if (A->sign == 0)		// A, B: non_negative
		return ret;
	else                    // A, B: negative
		return (-1) * ret;
}

void Left_shift(bigint** x, int r) //A << r   
{
	word* p = NULL;
	p = (*x)->a;

	int k = 0, rr = 0, new_wordlen = 0, i;

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

			for (i = 0; i < (*x)->wordLen; i++) //(A_n+k-1, ..., A_k) <- (A_n-1, ..., A_0)
				(*x)->a[(*x)->wordLen + k - 1 - i] = (*x)->a[(*x)->wordLen - 1 - i];

			array_init((*x)->a, k); //(A_k-1, ..., A_0) <- (0, 0, ..., 0)
		}
		else
			printf("ERROR: Left_shift(realloc1)");
	}
	else if (rr > 0 && rr < WORD_BITLEN)//Case 2: r = wk + rr
	{
		new_wordlen = ((*x)->wordLen + (k + 1));

		word* temp = NULL;
		temp = (word*)realloc(p, sizeof(word) * new_wordlen);
		if (temp != NULL) {
			p = temp;
			(*x)->a = p;

			(*x)->a[(*x)->wordLen + k] = (((*x)->a[(*x)->wordLen - 1]) >> (WORD_BITLEN - rr)); //A_n+k <- (A_n-1 >> (w - rr))

			for (i = 1; i < (*x)->wordLen; i++) //A_n+k-j <- (A_n-j << rr) | (A_n-j-1 >> (w - rr))
				(*x)->a[(*x)->wordLen + k - i] = (((*x)->a[(*x)->wordLen - i] << rr) | ((*x)->a[(*x)->wordLen - 1 - i] >> (WORD_BITLEN - rr)));

			(*x)->a[k] = ((*x)->a[0] << rr); //A_k <- (A_0 << r)

			array_init((*x)->a, k); //(A_k-1, ..., A_0) <- (0, 0, ..., 0)
		}
		else
			printf("ERROR: Left_shift(realloc2)");
	}
	(*x)->wordLen = new_wordlen;

	int check = bi_refine(*x);
	if (check == ERROR)
		printf("Fail: Left_shift(bi_refine)");
}

void Right_shift(bigint** x, int r) //A >> r
{
	int k = 0, rr = 0, i;

	if (r >= (*x)->wordLen * WORD_BITLEN) //r >= wn 
	{
		array_init((*x)->a, (*x)->wordLen); //0���� �ʱ�ȭ
	}
	else
	{
		if (r >= WORD_BITLEN) //r = k * w + rr
		{
			k = (int)(r / WORD_BITLEN); //��
			rr = r - k * WORD_BITLEN; //������
		}
		else
			rr = r;
	
		if (rr == 0) //r = wk
		{
			array_copy((*x)->a, (*x)->a + k, (*x)->wordLen - k); //(A_n-k-1,..., A_0) <- (A_n-1,..., A_k+1, A_k)
			array_init((*x)->a + (*x)->wordLen - k, k); //(A_n-1,..., A_n-k) <- (0, 0,..., 0)
		}
		else if (rr > 0) //r = wk + rr
		{
			for (i = 0;i < (*x)->wordLen - k - 1;i++) //A_j <- (A_k+j+1 << w - rr) | (A_k+j >> rr)
				(*x)->a[i] = ((*x)->a[k + 1 + i] << (WORD_BITLEN - rr)) | ((*x)->a[k + i] >> rr);

			(*x)->a[(*x)->wordLen - k - 1] = ((*x)->a[(*x)->wordLen - 1] >> rr); //A_n-k-1 <-(A_n-1 >> rr)

			array_init((*x)->a + (*x)->wordLen - k, k); //(A_n-1,..., A_n-k) <- (0, 0,..., 0)
		}
	}
	int check = bi_refine(*x);
	if (check == ERROR)
		printf("Fail: Right_shift(bi_refine)");
}

void Reduction(bigint** x, int r) //A mod 2^r
{
	int k = 0, rr = 0, i;

	if (r < (*x)->wordLen * WORD_BITLEN) //r < wn
	{
		if (r >= WORD_BITLEN) //r = k * w + rr
		{
			k = (int)(r / WORD_BITLEN); //��
			rr = r - k * WORD_BITLEN; //������
		}
		else
			rr = r;

		if (rr == 0) //r = wk
		{
			for (i = k;i < (*x)->wordLen;i++) //(A_n-1,..., A_k) <- (0, 0,..., 0)
				(*x)->a[i] = 0;
		}
		else //r = wk + rr
		{
			(*x)->a[k] = (*x)->a[k] & ((1 << rr) - 1);
			for (i = k + 1;i < (*x)->wordLen;i++) //(A_n-1,..., A_k) <- (0, 0,..., 0)
				(*x)->a[i] = 0;
		}
	}
	int check = bi_refine(*x);
	if (check == ERROR)
		printf("Fail: Reduction(bi_refine)");
}