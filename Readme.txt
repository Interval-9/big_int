********************* interval-9 Add_Sub.c *********************

int ADD_ABc(word* Cj, word Aj, word Bj, int cb);
- 워드 단위의 덧셈 연산을 수행하는 함수
- 입력으로 덧셈 대상 두 워드, 결과를 저장할 주소, 이전 캐리를 받음
- 출력으로 덧셈 결과 캐리비트(0 또는 1)를 반환함 

int ADDC(bigint** C, bigint* A, bigint* B);
- 부호가 같은 두 정수의 덧셈 연산을 수행하는 함수
- A의 워드길이 > B의 워드길이
- 두 정수 A, B와 결과를 저장할 주소를 입력으로 받음
- 동적 할당에 실패할 경우 -1을 반환, 성공할 경우 0을 반환함

int ADD(bigint** C, bigint* A, bigint* B);
- 두 정수의 덧셈 연산을 수행하는 함수
- 연산의 성공 여부를 반환함 (성공시 0, 실패시 -1)

int SUB_AbB(word* Cj, word Aj, int bb, word Bj);
- 워드 단위의 뺄셈 연산을 수행하는 함수
- 입력으로 뺄셈 대상 두 워드, 결과를 저장할 주소, 이전 캐리를 받음
- 출력으로 뺄셈 결과 캐리비트(0 또는 1)를 반환함

int SUBC(bigint** C, bigint* A, bigint* B);
- 두 양의 정수의 뺄셈 연산을 수행하는 함수
- A >= B인 조건에서 연산을 수행함
- 두 양의 정수 A, B와 결과를 저장할 주소를 입력으로 받음
- 동적 할당에 실패할 경우 -1, 성공할 경우 0을 반환함
 
int SUB(bigint** C, bigint* A, bigint* B);
- 두 정수의 뺄셈 연산을 수행하는 함수
- 연산의 성공 여부를 반환함 (성공시 0, 실패시 -1)

void ADD_test();
- 덧셈 연산의 정확성을 체크하기 위한 함수
- 워드길이가 8, 32, 64비트일 경우를 각각 나누어서 예제를 구성함
- A>0 and B>0 / A<0 and B<0 / A>0 and B<0 / A<0 and B>0 / A = 0 or B = 0 총 5가지 경우에 대해서 덧셈 연산의 정확성을 확인함

void SUB_test();
- 뺄셈 연산의 정확성을 체크하기 위한 함수
- 워드길이가 8, 32, 64비트일 경우를 각각 나누어서 예제를 구성함
- A>0 and B>0 / A<0 and B<0 / A>0 and B<0 / A<0 and B>0 / A = 0 or B = 0 총 5가지 경우에 대해서 뺄셈 연산의 정확성을 확인함


********************** interval-9 Mul.c **********************

void MUL_AB(word* Cj, word Aj, word Bi);
- 워드 단위의 곱셈 연산을 수행하는 함수
- 연산의 성공 여부를 반환함 (성공시 0, 실패시 -1)

int MULC_S(bigint** C, bigint* A, bigint* B)
- 두 양의 정수의 곱셈을 수행하는 함수
- Schoolbook 곱셈 연산을 수행
- 연산의 성공 여부를 반환함 (성공시 0, 실패시 -1)

int Modified_MULC_S(bigint** C, bigint* A, bigint* B)
- 두 양의 정수의 곱셈을 수행하는 함수
- Schoolbook 곱셈 연산을 수행
- 연산의 성공 여부를 반환함 (성공시 0, 실패시 -1)

int MULC_K(bigint** C, bigint* A, bigint* B)
- 두 양의 정수의 곱셈을 수행하는 함수
- Karatsuba 곱셈 연산을 수행
- 연산의 성공 여부를 반환함 (성공시 0, 실패시 -1)

int MUL(bigint** C, bigint* A, bigint* B)
- 두 정수의 곱셈을 수행하는 함수
- A, B가 0, 1, -1인 경우와 아닌 경우를 나누어서 수행
- MUL_Type에 따라서 MULC_S, Modified_MULC_S, MULC_K을 수행

**MULC_S와 Modified_MULC_S 비교
- (반복문 수행 횟수) MULC_S: nm번 / Modified_MULC_S: nm/2번
- (메모리) MULC_S: bigint 구조체 2개 / Modified_MULC_S: bigint 구조체 3개


********************** interval-9 Div.c **********************

int Compute_k(word b)
- 2^(w-1) <= 2^k * b < 2^w 을 만족하는 k을 계산하는 함수
- 하나의 워드 b을 입력으로 받아서 k을 반환

void Long_Div_2word(word* Q, word* A, word* B)
- 2 워드 A(A1*W + A0)와 1 워드 B을 입력으로 받아서 A / B의 몫을 계산하는 함수
- Binary long division 방법으로 연산을 수행
- 계산한 몫을 저장할 주소, A0의 주소, B의 주소를 입력으로 받음

int DIVCC(word* Q, bigint** R, bigint* A, bigint* B)
- 2^(w-1) <= B < 2^w을 만족할 경우 A/B의 몫과 나머지를 계산하는 함수
- 계산한 몫(1 word), 나머지(bigint 구조체)를 저장할 주소, bigint 구조체 A, B을 입력으로 받음
- 성공시 0, 실패시 -1을 반환

int DIVC(word* Q, bigint** R, bigint* A, bigint* B)
- 2^(w-1) <= B < 2^w을 만족하도록 A', B'을 계산한 후 A'/B'의 몫 Q'과 나머지 R'을 계산하는 함수
- 계산한 몫(1 word), 나머지(bigint 구조체)를 저장할 주소, bigint 구조체 A, B을 입력으로 받음
- 성공시 0, 실패시 -1을 반환

int Long_Div(bigint** Q, bigint** R, bigint* A, bigint* B)
- Word 단위 나눗셈 연산을 이용해서 A/B의 몫과 나머지를 계산하는 함수
- 계산한 몫과 나머지를 저장할 bigint 구조체의 주소, bigint 구조체 A, B을 입력으로 받음
- 성공시 0, 실패시 -1을 반환



