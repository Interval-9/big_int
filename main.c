/*
	This file is an example code for the user.
	You can set up macro vaiables below to execute the example code.
*/

#include "bn_struct.h"
//#include <vld.h> //For checking Memory Leakage in Visual Studio (only)

#define ADD_EX 1 //Toy example for Addition
#define SUB_EX 1 //Toy example for Subtraction
#define MUL_EX 1 //Toy example for Multiplication
#define SQU_EX 1 //Toy example for Square
#define DIV_EX 1 //Toy example for Division
#define MOD_EXP_EX 1 //Toy example for Modular Exponentiation

void main()
{
#if (ADD_EX | SUB_EX | MUL_EX | SQU_EX)
	printf("\n*********************************************************");
	printf("\n   Toy example for Addition/Subtraction/Multiplication   \n\n");

	//1. Declare two operands by string
	//If you want to declare it by array, you should use bi_set_by_arr() as a transform function
	char* operand1 = "1234567890";
	char* operand2 = "9876543210";

	//2. Transform the string into bigint structure
	bigint* A = NULL; //bigint structure for operand1
	bigint* B = NULL; //bigint structure for operand2
	bigint* C = NULL; //bigint structure for result of Addtion/Subtraction/Multiplication
	bi_set_by_string(&A, Non_Negative, operand1, 16);
	bi_set_by_string(&B, Non_Negative, operand2, 16);

#if ADD_EX
	//3. (ADD)Call the addition function
	//C <- A + B (operand1 + operand2)
	ADD(&C, A, B);

	//4. (ADD)Print out the result of operation
	//You can choose base between 2, 10, or 16
	printf("A = "); bi_show(A, 16); printf("\n");
	printf("B = "); bi_show(B, 16); printf("\n");
	printf("C(= A + B) = "); bi_show(C, 16); printf("\n");
#endif

#if SUB_EX
	//3. (SUB)Call the subtraction function
	//C <- A - B (operand1 - operand2)
	SUB(&C, A, B);

	//4. (SUB)Print out the result of operation
	//You can choose base between 2, 10, or 16
	printf("A = "); bi_show(A, 16); printf("\n");
	printf("B = "); bi_show(B, 16); printf("\n");
	printf("C(= A - B) = "); bi_show(C, 16); printf("\n");
#endif

#if MUL_EX
	//3. (MUL)Call the multiplication function
	//C <- A * B (operand1 * operand2)
	//We offer two multiplication modes. You can control the mode by changing MUL_Type in "bn_struct.h"
	MUL(&C, A, B);

	//4. (MUL)Print out the result of operation
	//You can choose base between 2, 10, or 16
	printf("A = "); bi_show(A, 16); printf("\n");
	printf("B = "); bi_show(B, 16); printf("\n");
	printf("C(= A * B) = "); bi_show(C, 16); printf("\n");
#endif

#if SQU_EX
	//3. (SQU)Call the square function
	//C <- A * A (operand1 * operand1)
	//We offer two sqauring modes. You can control the mode by changing SQU_Type in "bn_struct.h"
	SQU(&C, A);

	//4. (SQU)Print out the result of operation
	//You can choose base between 2, 10, or 16
	printf("A = "); bi_show(A, 16); printf("\n");
	printf("C(= A * A) = "); bi_show(C, 16); printf("\n");
#endif

	//5. Delete the structure that has been used
	bi_delete(&A);
	bi_delete(&B);
	bi_delete(&C);

	printf("\n*********************************************************\n");
#endif

#if DIV_EX
	printf("\n*********************************************************");
	printf("\n               Toy example for Division                  \n\n");

	//1. Declare dividend and divisor as string 
	//If you want to divide A by B, A is dividend and B is divisor
	//If you want to declare it by array, you should use bi_set_by_arr() as a transform function
	char* dividend = "12345678901234567890";
	char* divisor = "9876543210";

	//2. Transform the string into bigint structure
	bigint* AA = NULL; //bigint structure for dividend
	bigint* BB = NULL; //bigint structure for divisor
	bigint* Q = NULL; //bigint structure for quotient 
	bigint* R = NULL; //bigint structure for remainder
	bi_set_by_string(&AA, Non_Negative, dividend, 16);
	bi_set_by_string(&BB, Non_Negative, divisor, 16);

	//3. Call the division function
	//Q = floor(A / B) and R = A - B * Q
	//We offer three division modes. You can control the mode by changing DIV_Type in "bn_struct.h"
	DIV(&Q, &R, AA, BB);

	//4. Print out the result of operation
	//You can choose base between 2, 10, or 16
	printf("A = "); bi_show(AA, 16); printf("\n");
	printf("B = "); bi_show(BB, 16); printf("\n");
	printf("Q = "); bi_show(Q, 16); printf("\n");
	printf("R = "); bi_show(R, 16); printf("\n");

	//5. Delete the structure that has been used
	bi_delete(&AA);
	bi_delete(&BB);
	bi_delete(&Q);
	bi_delete(&R);

	printf("\n*********************************************************\n");
#endif

#if MOD_EXP_EX
	printf("\n*********************************************************");
	printf("\n         Toy example for Modular Exponentiation          \n\n");

	//1. Declare base, exponent and modulo as string 
	//If you want to operate x^n mod N, x is base, n is exponent and N is modulo.
	//If you want to declare it by array, you should use bi_set_by_arr() as a transform function
	char* base = "1234567890";
	char* exponent = "98765";
	char* modulo = "9876543210";

	//2. Transform the string into bigint structure
	bigint* x = NULL; //bigint structure for base
	bigint* n = NULL; //bigint structure for exponent
	bigint* N = NULL; //bigint structure for modulo
	bigint* Ret = NULL; //bigint structure for result of modular exponentiation
	bi_set_by_string(&x, Non_Negative, base, 16);
	bi_set_by_string(&n, Non_Negative, exponent, 16);
	bi_set_by_string(&N, Non_Negative, modulo, 16);

	//3. Call the modular exponentiation function
	//Ret <- x^n mod N
	//We offer three modular exponentiation modes. You can control the mode by changing MOD_EXP_Type in "bn_struct.h"
	MOD_EXP(&Ret, x, n, N);

	//4. Print out the result of operation
	//You can choose base between 2, 10, or 16
	printf("x = "); bi_show(x, 16); printf("\n");
	printf("n = "); bi_show(n, 16); printf("\n");
	printf("N = "); bi_show(N, 16); printf("\n");
	printf("x^n mod N = "); bi_show(Ret, 16); printf("\n");

	//5. Delete the structure that has been used
	bi_delete(&x);
	bi_delete(&n);
	bi_delete(&N);
	bi_delete(&Ret);

	printf("\n*********************************************************\n");
#endif
}
