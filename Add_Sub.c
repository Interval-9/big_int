#include "bn_struct.h"

//���� ���� ���� ������ �����ϴ� �Լ�
//ĳ�� ��Ʈ(0 �Ǵ� 1)�� ��ȯ
int ADD_ABc(word* Cj, word Aj, word Bj, int cb)
{
	int c = 0;					//��ȯ�� ĳ�� ��Ʈ ����

	(*Cj) = (word)(Aj + Bj);	//Cj = (Aj + Bj) mod word
	if (*Cj < Aj)				//ĳ�� �߻�
		c = 1;

	(*Cj) = (word)((*Cj) + cb); //�� �� ĳ���� ����
	if (*Cj < cb)				//ĳ�� �߻�
		c = 1;

	return c;
}

//��ȣ�� ���� �� ������ ���� ���� �Լ� (wordLen(A) >= wordLen(B)�� ���)
//���� �Ҵ翡 ������ ���: -1, ������ ���: 0�� ��ȯ
int ADDC(bigint** C, bigint* A, bigint* B)
{
	int max_wordLen = 0;
	int c = 0, j = 0;				//ĳ��(c)�� 0���� �ʱ�ȭ

	word* Cp = NULL;				//�ּ� ������ ���� ������ ����
	word* Bp = NULL;

	max_wordLen = (A->wordLen);		//C�� ���� ���̸� ���
	bi_new(C, max_wordLen + 1);		//������� ������ big_int ����ü ����

	Cp = (*C)->a;

	//B�� �޸� ���Ҵ�
	Bp = (word*)realloc(B->a, sizeof(word) * max_wordLen);
	if (Bp == NULL) //�����Ҵ� ���н� �Լ� ����
		return -1;
	else
		B->a = Bp;

	array_init(((B->a) + (B->wordLen)), (max_wordLen - (B->wordLen)));	// Bm ~ Bn-1���� 0���� �ʱ�ȭ

	//���� ���� ���� ����: C, c <- (A[j] + B[j] + c)
	for (j = 0; j < max_wordLen; j++)
	{
		c = ADD_ABc(Cp + j, *(A->a + j), *(B->a + j), c);
		//printf("Aj = %02x, Bj = %02x\n", (A->a)[j], (B->a)[j]); 
	}

	if (c == 1) //�ֻ��� ���� ���� ���, ĳ���� �������� ���
		Cp[max_wordLen] = 1;

	bi_refine(B);	bi_refine(*C);

	(*C)->sign = A->sign; //C�� ��ȣ�� ����

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

	if (c2 == 1) //�ֻ��� ���� ���� ���, ĳ���� �������� ���
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
	> �� ���� A, B�� ���� ����ϴ� �Լ�
	> ���� ���θ� ��ȯ�� (����: 0, ����: -1)
	> �� �� A�� B�� ���Ƶ� ��. (A = B �� ��, A <- 2A ���� �Ǵ����� Ȯ�� �ʿ�)
*/
int ADD(bigint** C, bigint* A, bigint* B)
{
	int ret = 0;

	//A�� 0�� ���, C <- B
	if (Is_Zero(A) == 1)
	{
		bi_assign(C, B);
		return 0;
	}

	//B�� 0�� ���, C <- A
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

	//A�� ������̰� �� �� ���
	else if (A->wordLen >= B->wordLen)
		return ADDC(C, A, B);

	//B�� ������̰� �� �� ���
	else
		return ADDC(C, B, A);
}

//���� ���� ���� ������ �����ϴ� �Լ�
//ĳ�� ��Ʈ(0 �Ǵ� 1)�� ��ȯ
int SUB_AbB(word* Cj, word Aj, int bb, word Bj)
{
	int b = 0; //��ȯ�� ĳ�� ��Ʈ ����

	(*Cj) = (word)(Aj - bb);
	if (Aj < bb)
		b = 1;

	if ((*Cj) < Bj)
		b = 1;

	(*Cj) = (word)((*Cj) - Bj);

	return b;
}

//�� ���� ������ ���� ������ �����ϴ� �Լ� (A >= B�� ���)
//���� �Ҵ翡 ������ ���: -1, ������ ���: 0�� ��ȯ
int SUBC(bigint** C, bigint* A, bigint* B)
{
	int max_wordLen = 0;
	int b = 0, j = 0;				//ĳ��(b)�� 0���� �ʱ�ȭ

	word* Cp = NULL;				//�ּ� ������ ���� ������ ����
	word* Bp = NULL;

	max_wordLen = (A->wordLen);		//C�� ���� ���̸� ���
	bi_new(C, max_wordLen);			//������� ������ big_int ����ü ����

	Cp = (*C)->a;

	//B�� �޸� ���Ҵ�
	Bp = (word*)realloc(B->a, sizeof(word) * max_wordLen); 
	if (Bp == NULL) //�����Ҵ� ���н� �Լ� ����
		return -1;
	else
		B->a = Bp;

	array_init(((B->a) + (B->wordLen)), (max_wordLen - (B->wordLen)));	// Bm ~ Bn-1���� 0���� �ʱ�ȭ

	 //���� ���� ���� ����: C, b <- (A - B - b)
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
	> �� ���� A, B�� ���� ������ �����ϴ� �Լ�
	> ���� ���θ� ��ȯ�� (����: 0, ����: -1)
	> �� �� A�� B�� ���Ƶ� ��. (A = B �� ��, A <- 0 ���� �Ǵ����� Ȯ�� �ʿ�)
*/
int SUB(bigint** C, bigint* A, bigint* B)
{
	int ret = 0;

	//A�� 0�� ���, C <- (-B)
	if (Is_Zero(A) == 1)
	{
		bi_assign(C, B);
		flip_bi_sign(*C);
		return 0;
	}

	//B�� 0�� ���, C <- A
	else if (Is_Zero(B) == 1)
	{
		bi_assign(C, A);
		return 0;
	}

	//A = B�� ���, C <- 0
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