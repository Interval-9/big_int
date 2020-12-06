# Interval-9 BigInt Library
-----------------------------------------------------------------
## **bn_struct.h**
###Macros and Constants
+ _WORD_BITLEN_
+ _MUL_Type_
+ _SQU_Type_
+ _DIV_Type_
+ _MOD_EXP_Type_
+ _FLAG_ (END condition for Karatsuba)


+ Negative(1) / Non_Negative(0)
+ BINARY(2) / DECI(10) / HEXA(16)
+ ERROR(-1)
+ TRUE(1) / FALSE(0)
+ _byte_ : unsigned char

###BigInt Structure
+ _typedef_ struct _bigint_
 - _int_ sign (Non-Negaive / Negative)
 - _int_ wordLen (should be more than 0)
 - _word*_ a (address for BigInt)

###Function Headers

## **Basic_Operation.c**
+ void array_init(word* a, int wordLen)
+ void bi_delete(bigint** x)
+ void bi_new(bigint** x, int wordLen)


+ int chton(char x)
+ void bi_set_by_array(bigint** x, int sign, word* arr, int wordLen)
+ void bi_set_by_string(bigint** x, int sign, char* str, char base)


+ void bi_show(bigint* x, char base)
+ void bi_refine(bigint* x)
+ void array_copy(word* a, word* b, int wordLen)
 - Copy array _**b**_ to array _**a**_
+ void bi_assign(bigint** y, bigint* x)
+ void array_rand(word* dst, int wordLen)
+ void bi_gen_rand(bigint** x, int sign, int wordLen)


+ int get_word_length(bigint* x)
+ int get_bit_length(bigint* x)
+ int get_jth_bit_of_bi(bigint* x, int j)
+ int get_bi_sign(bigint* x)
+ void flip_bi_sign(bigint* x)


+ void bi_set_one(bigint** x)
+ void bi_set_zero(bigint** x)


+ int Is_Zero(bigint* x)
+ int Is_One(bigint* x)
+ int Is_NegativeOne(bigint* x)


+ int CompareABS(bigint* A, bigint* B) 
+ int Compare(bigint* A, bigint* B)


+ void Left_shift(bigint** x, int r)
+ void Right_shift(bigint** x, int r)

## **main.c**
There is toy example of this library.

You can set up macro variables below to execute the example code.
Or you can modify some parameters to use this library.
## **Add_Sub.c**
+ int ADD_ABc(word* Cj, word Aj, word Bj, int cb)
+ void ADDC(bigint** C, bigint* A, bigint* B)
+ void ADD(bigint** C, bigint* A, bigint* B)


+ int SUB_AbB(word* Cj, word Aj, int bb, word Bj)
+ void SUBC(bigint** C, bigint* A, bigint* B)
+ void SUB(bigint** C, bigint* A, bigint* B)

## **Mul.c**
+ void MUL_AB(word* Cj, word Aj, word Bi)
+ void MULC_S(bigint** C, bigint* A, bigint* B)
+ void MUL_word_bigint(bigint** C, bigint* A, word B)
+ void Modified_MULC_S(bigint** C, bigint* A, bigint* B)
+ void MULC_K(bigint** C, bigint* A, bigint* B)
+ void MUL(bigint** C, bigint* A, bigint* B)

## **Squ.c**
+ void SQU_AB(bigint** C, word A)
+ void SQUC_S(bigint** C, bigint* A)
+ void SQUC_K(bigint** C, bigint* A)
+ void SQU(bigint** C, bigint* A)

## **Div.c**
+ int Compute_k(word b)
+ void Long_Div_2word(word* Q, word* A, word* B)
+ void DIVCC(word* Q, bigint** R, bigint* A, bigint* B)
+ void DIVC(word* Q, bigint** R, bigint* A, bigint* B)
+ void Naive_DIV(bigint** Q, bigint** R, bigint* A, bigint* B)
+ void Binary_Long_DIV(bigint** Q, bigint** R, bigint* A, bigint* B)
+ void Multi_Long_DIV(bigint** Q, bigint** R, bigint* A, bigint* B)
+ void DIV(bigint** Q, bigint** R, bigint* A, bigint* B)

## **Mod_Exp.c**
+ void bi_reduction(bigint** X, bigint* N)
+ void MOD_EXP_L2R(bigint** C, bigint* x, bigint* n, bigint* M)
+ void MOD_EXP_R2L(bigint** C, bigint* x, bigint* n, bigint* M)
+ void MOD_EXP_Mon(bigint** C, bigint* X, bigint* n, bigint* N)
+ void MOD_EXP(bigint** C, bigint* x, bigint* n, bigint* M)

## **test.c**
For Correctness Test
+ void Random_Test()
+ void Basic_operation_test()
+ void ADD_test(bigint* A, bigint* B, bigint* C)
+ void SUB_test(bigint* A, bigint* B, bigint* C)
+ void MUL_test(bigint* A, bigint* B, bigint* C)
+ void SQU_test(bigint* A, bigint* C)
+ void DIV_test(bigint* B, bigint* Q, bigint* R)
+ void MOD_EXP_test(bigint* x, bigint* n, bigint* M, bigint* C)