#ifndef _BN_STURCT_H_
#define _BN_STURCT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

//#define ZERORIZE

#define	WORD_BITLEN		8	//8, 32, 64
#define MUL_Type		2	//0: Schoolbook Mul, 1: Modfied Schoolbook Mul,	2: Karatsuba
#define	FLAG			3

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
#define MASK 0xf
#elif	WORD_BITLEN == 32
typedef	unsigned int	word;
#define MASK 0xffff
#else	//WORD_BITLEN == 64
#define MASK 0xffffffff
typedef	unsigned long long	word;
#endif	//WORD_BITLEN == 8

typedef struct {
	int	sign;		//Negative(1) or Non-Negative(0)
	int	wordLen;	//wordLen >= 0
	word* a;		// address for big integer
} bigint;

#define byte unsigned char

void array_init(word* a, int wordLen);
void bi_delete(bigint** x);
int bi_new(bigint** x, int wordLen);

int chton(char x);
void bi_set_by_array(bigint** x, int sign, word* arr, int wordLen);
void bi_set_by_string(bigint** x, int sign, char* str, char base);
void bi_show(bigint* x, char base);

int bi_refine(bigint* x);
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
int ADDC(bigint** C, bigint* A, bigint* B);
int ADD(bigint** C, bigint* A, bigint* B);

int SUB_AbB(word* Cj, word Aj, int bb, word Bj);
int SUBC(bigint** C, bigint* A, bigint* B);
int SUB(bigint** C, bigint* A, bigint* B);

void MUL_AB(word* Cj, word Aj, word Bi);
int MULC_S(bigint** C, bigint* A, bigint* B);
int Modified_MULC_S(bigint** C, bigint* A, bigint* B);
int MULC_K(bigint** C, bigint* A, bigint* B);
int MUL(bigint** C, bigint* A, bigint* B);
#endif // !_BN_STURCT_H_