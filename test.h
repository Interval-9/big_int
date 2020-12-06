/*
	This file declares all test functions for accuracy tests.
*/
#ifndef _TEST_H_
#define _TEST_H_

#include "bn_struct.h"

#define TestNum		1000
#define	MaxArrNum	(int)1024/WORD_BITLEN

void Random_Test();

void Basic_operation_test();
void ADD_test(bigint* A, bigint* B, bigint* C);
void SUB_test(bigint* A, bigint* B, bigint* C);
void MUL_test(bigint* A, bigint* B, bigint* C);
void SQU_test(bigint* A, bigint* C);
void DIV_test(bigint* B, bigint* Q, bigint* R);
void MOD_EXP_test(bigint* x, bigint* n, bigint* M, bigint* C);

#endif // !_TEST_H_