#include "bn_struct.h"
/*
	> Func: Single_Precision Squaring
	> return: Success 0, Fail -1
*/
int SQU_AB(bigint** C, word A)
{
	//printf("\n###### Squ_AB ######\n");
	int Half_bitLen = WORD_BITLEN >> 1;

	word A1 = 0; word A0 = 0;
	word C1 = 0; word C0 = 0;

	bigint* tempC = NULL;
	bigint* T = NULL;

	//A1, A0 <- |A| >> w/2, |A| mod 2^(w/2)
	A1 = A >> Half_bitLen;
	A0 = A & HARF_MASK;
	//printf("A1, A0: %0x, %0x\n", A1, A0);

	//C1, C0 <- A1^2, A0^2
	C1 = A1 * A1;
	C0 = A0 * A0;
	//printf("C1, C0: %0x, %0x\n", C1, C0);

	//C <- (C1<<w) + C0
	if (-1 == bi_new(&tempC, 2))
		return ERROR;
	tempC->a[0] = C0;
	tempC->a[1] = C1;
	//printf("tempC: "); bi_show(tempC, 16); printf("\n");

	//T <- A0*A1
	bi_new(&T, 1);
	T->a[0] = A0 * A1;
	//printf("T: "); bi_show(T, 16); printf("\n");

	//T <- T << (w/2 + 1)
	Left_shift(&T, Half_bitLen + 1);
	//printf("T: "); bi_show(T, 16); printf("\n");

	//C <- C+T
	if (-1 == ADD(C, tempC, T))
		return ERROR;

	bi_refine(*C);
	//printf("C: "); bi_show(*C, 16); printf("\n");

	bi_delete(&tempC);
	bi_delete(&T);
	return 0;
}

/*
	> Func: A^2 using SchoolBook
	>> A->sign = 0 (Positive)
	> return: Success 0, Fail -1
*/
int SQUC_S(bigint** C, bigint* A)
{
	int i = 0, j = 0;
	int t = A->wordLen;
	//int t = (get_bit_length(A) / WORD_BITLEN) + 1;
	//printf("t: %d\n", t);

	bigint* C1 = NULL; bigint* C2 = NULL; bigint* tempC1 = NULL; bigint* tempC2 = NULL;
	bigint* T1 = NULL; bigint* T2 = NULL;

	//C1, C2 <- 0
	bi_set_zero(&tempC1); bi_set_zero(&tempC2);

	for (j = 0; j < t; j++) {
		//T1 <- Aj ^ 2
		if (-1 == SQU_AB(&T1, A->a[j]))
			return ERROR;
		//printf("T1: "); bi_show(T1, 16); printf("\n");

		//printf("\n###### Squ_S ######\n");
		//T1 <- T1 << 2jw
		Left_shift(&T1, 2 * j * WORD_BITLEN);
		//printf("T1 << 2lw: "); bi_show(T1, 16); printf("\n");

		//C1 <- T1 + C1
		if (-1 == ADD(&C1, T1, tempC1))
			return ERROR;
		bi_assign(&tempC1, C1);
		//printf("C1: "); bi_show(C1, 16); printf("\n");
		//printf("tempC1: "); bi_show(tempC1, 16); printf("\n");
		
		for (i = j + 1; i < t; i++) {
			//T2 <- Aj*Ai
			if (-1 == bi_new(&T2, 2))
				return ERROR;
			MUL_AB(T2->a, A->a[j], A->a[i]);
			//printf("T2: "); bi_show(T2, 16); printf("\n");

			//T2 <- T2 << (i+j)w
			Left_shift(&T2, (i + j) * WORD_BITLEN);
			//printf("T2: "); bi_show(T2, 16); printf("\n");

			//C2 <- C2 + T2
			if (-1 == ADD(&C2, tempC2, T2))
				return ERROR;
			bi_assign(&tempC2, C2);
			//printf("C2: "); bi_show(C2, 16); printf("\n");
			//printf("tempC2: "); bi_show(tempC2, 16); printf("\n");
		}
	}

	if (C2 == NULL)
		bi_set_zero(&C2);

	//C2 <- C2 << 1
	Left_shift(&C2, 1);
	//printf("C2 << 1: "); bi_show(C2, 16); printf("\n");
	//C1 + C2
	if (-1 == ADD(C, C1, C2))
		return ERROR;

	//printf("C: "); bi_show(*C, 16); printf("\n");
	bi_refine(*C);

	bi_delete(&C1); bi_delete(&C2); bi_delete(&tempC1); bi_delete(&tempC2);
	bi_delete(&T1); bi_delete(&T2);
	return 0;
}

/*
	> Func: A^2 using Karatsuba
	> return: Success 0, Fail -1
*/
int SQUC_K(bigint** C, bigint* A)
{
	//printf("\n##### SQUC_K start #####\n");
	int L = 0;
	int wordLen_A = A->wordLen;
	//printf("wordLen_A: %d\n", wordLen_A);

	bigint* A1 = NULL; bigint* A0 = NULL;
	bigint* T1 = NULL; bigint* T0 = NULL;
	bigint* R = NULL; bigint* tempR = NULL;
	bigint* S = NULL;

	if (FLAG >= wordLen_A)
		return SQUC_S(C, A);

	L = (wordLen_A + 1) >> 1;
	//printf("L: %d\n", L);

	/*
		1)	A1, A0 <- |A| >> lw, |A| mod 2^lw
		2)	T1, T0 <- SQUC_K(A1), SQUA_K(A0)
	*/
	//1)
	bi_assign(&A1, A); //A1->sign = 0;
	bi_assign(&A0, A); //A0->sign = 0;
	Right_shift(&A1, L * WORD_BITLEN);
	Reduction(&A0, L * WORD_BITLEN);

	//printf("A1: "); bi_show(A1, 16); printf("\n");
	//printf("A0: "); bi_show(A0, 16); printf("\n");

	//2)
	//printf("== A1 * A1 ==\n");
	if (-1 == SQUC_K(&T1, A1))
		return ERROR;
	//printf("T1: "); bi_show(T1, 16); printf("\n");

	//printf("== A0 * A0 ==\n");
	if (-1 == SQUC_S(&T0, A0))
		return ERROR;
	//printf("T0: "); bi_show(T0, 16); printf("\n");
	/*
		R <- T1||T0
		1) T1 << 2lw
		2) T1 + T0
	*/
	//1)
	Left_shift(&T1, 2 * L * WORD_BITLEN);
	//printf("T1 << 2lw: "); bi_show(T1, 16); printf("\n");
	//2)
	if (-1 == ADD(&tempR, T1, T0))
		return ERROR;
	//printf("tempR: "); bi_show(tempR, 16); printf("\n");
	//S <- A1*A0
	if (-1 == MULC_K(&S, A1, A0))
		return ERROR;
	//printf("S: "); bi_show(S, 16); printf("\n");
	//S <- S <<lw+1
	Left_shift(&S, L * WORD_BITLEN + 1);
	//printf("S: "); bi_show(S, 16); printf("\n");
	//R <- ADDC(R,S)
	if (-1 == ADDC(&R, tempR, S))
		return ERROR;
	//printf("R: "); bi_show(R, 16); printf("\n");

	bi_assign(C, R);
	//printf("C: "); bi_show(*C, 16); printf("\n");
	bi_refine(*C);

	bi_delete(&A1); bi_delete(&A0);
	bi_delete(&T1); bi_delete(&T0);
	bi_delete(&R); bi_delete(&tempR);
	bi_delete(&S);

	return 0;
}

/*
	> Func: Á¤¼ö AÀÇ Á¦°ö
	> return: Success 0, Fail -1
*/
int SQU(bigint** C, bigint* A)
{
	//Case: A = 0 or A = 1 or A = -1
	if (Is_Zero(A) | Is_One(A) | Is_NegativeOne(A)) {
		bi_assign(C, A);
		(*C)->sign = 0;
		return 0;
	}

	//Otherwise
#if SQU_Type == 0 //SchoolBook Squaring
	if (-1 == SQUC_S(C, A))
		return ERROR;
#elif SQU_Type == 1 //Karatsuba Squaring
	if (-1 == SQUC_K(C, A))
		return ERROR;
#endif

	return 0;
}