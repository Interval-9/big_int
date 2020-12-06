/*
	This file declares all macro variables and functions for Big integer operations.
	The variables that can be changed by the user and the functions to be used by the user are marked separately.
*/
#ifndef _BN_STURCT_H_
#define _BN_STURCT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <time.h>

//#define ZERORIZE

//**************** User-alterable variables ******************
/*
	> WORD_BITLEN	8 / 32 / 64
	> MUL_Type		0 (Modified Schoolbook Mul) / 1 (Karatsuba Mul)
	> SQU_Type		0 (Schoolbook Squ) / 1 (Karatsuba Squ)
	> DIV_Type		0 (Naive) / 1 (Binary Div) / 2 (Multi Long Div)
	> MOD_EXP_Type	0 (L2R) / 1 (R2L) / 2 (Montgomery Ladder)
	> FLAG			3 (END condition for Karatsuba : ? recommend)
*/
#define	WORD_BITLEN	    32
#define MUL_Type		1
#define	SQU_Type		1
#define DIV_Type        2
#define MOD_EXP_Type	2
#define	FLAG			128
//*************************************************************


// ************* Variables for implementation  ****************
#define	Negative		1
#define	Non_Negative	0
#define	BINARY			2
#define	DECI			10
#define	HEXA			16
#define ERROR			-1
#define	TRUE			1
#define	FALSE			0

#if WORD_BITLEN == 8
typedef	unsigned char	word;
#define MASK 0xff
#define HARF_MASK 0xf
#endif
#if	WORD_BITLEN == 32
typedef	unsigned int	word;
#define MASK 0xffffffff
#define HARF_MASK 0xffff
#endif
#if WORD_BITLEN == 64
#define MASK 0xffffffffffffffff
#define HARF_MASK 0xffffffff
typedef	unsigned long long	word;
#endif
//*************************************************************


//***************** BigInt Structure *************************
/*
	> int sign		0 (Non-Negative) / 1 (Negative)
	> int wordLen	should be 'more than 0'
	> word* a		address for big integer
*/
typedef struct {
	int	sign;
	int	wordLen;
	word* a;
} bigint;
//*************************************************************


//********** Main functions that the user can use ************
void ADD(bigint** C, bigint* A, bigint* B);
void SUB(bigint** C, bigint* A, bigint* B);
void MUL(bigint** C, bigint* A, bigint* B);
void SQU(bigint** C, bigint* A);
void DIV(bigint** Q, bigint** R, bigint* A, bigint* B);
void MOD_EXP(bigint** C, bigint* x, bigint* n, bigint* M);
//*************************************************************


//********** Basic operations and basic functions *************
void array_init(word* a, int wordLen);
void bi_delete(bigint** x);
void bi_new(bigint** x, int wordLen);

int chton(char x);
void bi_set_by_array(bigint** x, int sign, word* arr, int wordLen);
void bi_set_by_string(bigint** x, int sign, char* str, char base);
void bi_show(bigint* x, char base);

void bi_refine(bigint* x);
void array_copy(word* a, word* b, int wordLen);

void bi_assign(bigint** y, bigint* x);

void array_rand(word* dst, int wordLen);
void bi_gen_rand(bigint** x, int sign, int wordLen);

int get_word_length(bigint* x);
int get_bit_length(bigint* x);
int get_jth_bit_of_bi(bigint* x, int j);

int get_bi_sign(bigint* x);
void flip_bi_sign(bigint* x);

void bi_set_one(bigint** x);
void bi_set_zero(bigint** x);

int Is_Zero(bigint* x);
int Is_One(bigint* x);
int Is_NegativeOne(bigint* x);

int CompareABS(bigint* A, bigint* B);
int Compare(bigint* A, bigint* B);

void Left_shift(bigint** x, int r);
void Right_shift(bigint** x, int r);
void Reduction(bigint** x, int r);

int ADD_ABc(word* Cj, word Aj, word Bj, int cb);
void ADDC(bigint** C, bigint* A, bigint* B);
void Positive_ADD_1(bigint** X);

int SUB_AbB(word* Cj, word Aj, int bb, word Bj);
void SUBC(bigint** C, bigint* A, bigint* B);

void MUL_AB(word* Cj, word Aj, word Bi);
void MULC_S(bigint** C, bigint* A, bigint* B);
void MUL_word_bigint(bigint** C, bigint* A, word B);
void Modified_MULC_S(bigint** C, bigint* A, bigint* B);
void MULC_K(bigint** C, bigint* A, bigint* B);

void SQU_AB(bigint** C, word A);
void SQUC_S(bigint** C, bigint* A);
void SQUC_K(bigint** C, bigint* A);

int Compute_k(word b);
void Long_Div_2word(word* Q, word* A, word* B);
void DIVCC(word* Q, bigint** R, bigint* A, bigint* B);
void DIVC(word* Q, bigint** R, bigint* A, bigint* B);
void Naive_DIV(bigint** Q, bigint** R, bigint* A, bigint* B);
void Binary_Long_DIV(bigint** Q, bigint** R, bigint* A, bigint* B);
void Multi_Long_DIV(bigint** Q, bigint** R, bigint* A, bigint* B);

void bi_reduction(bigint** X, bigint* N);
void MOD_EXP_L2R(bigint** C, bigint* x, bigint* n, bigint* M);
void MOD_EXP_R2L(bigint** C, bigint* x, bigint* n, bigint* M);
void MOD_EXP_Mon(bigint** C, bigint* X, bigint* n, bigint* N);
//*************************************************************
#endif // !_BN_STURCT_H_