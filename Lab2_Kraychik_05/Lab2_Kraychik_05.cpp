/*
	������������ ������ �2 �� ����� ���������������� �� ����� ����������                                
	������� �5.                                                                                                                                                
	�������� ������� �������. ������ 344 
	
	�������, ���������� �� ����� �++, ��������������� ��� ������������ �������, ����������
	�� ����� ����������, ����� � ����� �������� ������� _test (��������, GF_MulX_test)
	�� ���� ��������, ����������� �������� � ���������� ��� ����� GF2_64, ������������ ������� ��������-����������
*/
#include "stdafx.h"
#include <iostream>
#include <time.h>

typedef __int64 GF2_64;  // ������� ���� GF2_64 ����� ��������� � __int64
using namespace std;

const int size = 8 * sizeof(GF2_64);  // ������ � ����� ���� ������ GF2_64
typedef enum { ECX, EDX } _register;

extern "C" // ������������� ������� �������� �� ����� ���������� (��. ���� Lab2Func_Kraychik_05.asm)
{
	GF2_64 GF_MulX(GF2_64 a);
	GF2_64 GF_PowX(unsigned int Power);
	GF2_64 GF_Multiply(GF2_64 a, GF2_64 b);
	GF2_64 GF_Reciprocal(GF2_64 a);
	int PolyMulX(GF2_64 *a, int deg);
	int PolyMulConst(GF2_64 *a, int deg, GF2_64 c);
	int PolyZero(GF2_64 *a, int deg);
	int PolyCpy(GF2_64 *dest, GF2_64 *src, unsigned char deg);
	int PolySum(GF2_64 *sum, GF2_64 *a, int deg_a, GF2_64 *b, int deg_b);

	/*
		���������� ��������� ����������� ��������� ����������
		Input:
			reg - RCX ��� RDX, �������� �������, �� �������� ����� ����������� ����������
			bit_number - ����� ���� � ������ ��������
		Output:
			��������� true, ���� ���������� ���������� ��������������
			����� false
	*/
	bool has_sse(_register reg, unsigned int bit_number);
}

GF2_64 add_test(GF2_64 a, GF2_64 b)  //���������� ��� �������� ����
{
	return (a ^ b);  // �������� � ���� �� ���� ��������� ��������� � ��������� xor
}

GF2_64 GF_MulX_test(GF2_64 a) // ��������� ������� ���� �� ��������� x
{
	/*
		���� a < 0, �� ����� ��������� �� � ��� ���������� ������������� �� x^64 + x^4 + x^3 + x + 1
		��� ���� ��-�� ���������� ������������, ������� ��� ����� ��������, �� ���� ������������� ����� ����� �� x^4 + x^3 + x + 1
		���� a >= 0, ����������� ����� �������� ����� ����� �� 1 ������
	*/
	GF2_64 b = (a < 0) ? 27 : 0; // 27 = x^4 + x^3 + x + 1
	return (a << 1) ^ b;
}
GF2_64 GF_PowX_test(unsigned int Power) // ���������� x � ������� Power
{
	GF2_64 result = 1;
	for (int i = 0; i < Power; i++)
	{
		result = GF_MulX_test(result);
	}
	return result;
}
GF2_64 GF_Multiply_test(GF2_64 a, GF2_64 b)  // ������������ ���� ��������� ����
{
	// �������� ����� ������������ �������� a �� ������ ��� �������� b
	GF2_64 result = 0;
	unsigned __int64 right_bit = 0;
	GF2_64 mul = 0;  // ����� ��������� ������������ a �� x^i
	for (int i = 0; i < size; i++)
	{
		right_bit = (b >> i) % 2;  // i-�� ��� �������� b
		if (right_bit)
		{
			mul = a;
			for (int j = 0; j < i; j++)
			{
				mul = GF_MulX_test(mul);
			}
		}
		else { mul = 0; }
		result = add_test(result, mul);  // ���������� ������� ����� �� mul
	}
	return (GF2_64)result;
}
GF2_64 GF_Reciprocal_test(GF2_64 a)  // ���������� ��������� �������� � �������� �
{
	/*
		a^(-1) = a^(2^64-2) = a^(2^0) * a^(2^1) * a^(2^2) * ... * a^(2^63)
	*/
	GF2_64 tmp = GF_Multiply_test(a, a); // a^(2^0)
	GF2_64 res = tmp;
	for (int i = 1; i <= 62; i++)
	{
		tmp = GF_Multiply_test(tmp, tmp);  // a^(2^i)
		res = GF_Multiply_test(res, tmp);
	}
	return res;
}
int PolyMulX_test(GF2_64 *a, int deg)  // ������������ ���������� � ������� deg �� x (��� ����� GF2_64)
{
	if (deg == -1) { return -1; }
	a[deg+1] = 0;
	return deg + 1;
}
int PolyMulConst_test(GF2_64 *a, int deg, GF2_64 c)  // ��������� ���������� � ������� deg �� ������� c ���� GF2_64
{
	if (c == 0) { return -1; }
	for (int i = 0; i <= deg; i++)
	{
		a[i] = GF_Multiply_test(a[i], c);
	}
	return deg;
}
int PolyZero_test(GF2_64 *a, int deg)  // ��������� ���������� ��� ����� GF2_64 �� 0; ��� ���� ���������� ��������� ����������
{
	for (int i = 0; i <= deg; i++)
	{
		a[i] = 0;
	}
	return -1;
}
/*
	�������� ����������� ��� ����� GF2_64
	a - ��������� �� ��������� ������� deg_a
	b - ��������� �� ��������� ������� deg_b
	sum - ��������� �� ���������-���������
*/
int PolySum_test(GF2_64 *sum, GF2_64 *a, int deg_a, GF2_64 *b, int deg_b)
{
	int deg_sum = ::max(deg_a, deg_b);
	int i;
	int j;
	int k = deg_sum;
	for (i = deg_a, j = deg_b; i >= 0 && j >= 0; i--, j--)
	{
		sum[k] = add_test(a[i], b[j]);
		k--;
	}
	// ���� deg_a != deg_b, ���������� ����������� ���������� ����������� � sum
	for (int itr = i; itr >= 0; itr--)
	{
		sum[k] = a[itr];
		k--;
	}
	for (int itr = j; itr >= 0; itr--)
	{
		sum[k] = b[itr];
		k--;
	}
	return deg_sum;
}
int PolyCpy_test(GF2_64 *dest, GF2_64 *src, unsigned char deg)  // ����������� ���������� src ������� deg � dest
{
	for (int i = 0; i <= deg; i++)
	{
		dest[i] = src[i];
	}
	return deg;
}

#define TEST_SIZE 10000  // ���������� ������ (�������� ����� ��� ������������ �������)
#define ARR_SIZE 100 // ������������ ������ ������� ��� ������������ ���������

bool arrs_eq(GF2_64 *a, int deg_a, GF2_64 *b, int deg_b)  // ��������� ����� �� ����������
{
	if (deg_a != deg_b) { return false; }
	for (int i = 0; i <= deg_a; i++)
	{
		if (a[i] != b[i]) { return false; }
	}
	return true;
}
bool is_zero(GF2_64 *a, int deg_a)  // ���������, ����� �� ��������� 0 (����������� �� ��� ����������� ������)
{
	for (int i = 0; i <= deg_a; i++)
	{
		if (a[i] != 0) { return false; }
	}
	return true;
}
GF2_64 random()  // ���������� ��������� ������� ���� GF2_64
{
	GF2_64 res = 0;
	for (int i = 0; i < size - 1; i++)
	{
		res += (::rand() % 2);
		res = res << 1;
	}
	res += (::rand() % 2);
	return res;
}
/*
	��������� ��������� �� ��������� SSE ����������
	���������� true, ���� �������������� SSE, SSE2, SSE3, SSSE3, SSE4.1, SSE4.2
	����� ��������� false
*/
bool test_sse(void)
{
	if ((has_sse(ECX, 20)) && (has_sse(ECX, 19)) && (has_sse(ECX, 9)) && (has_sse(ECX, 0)) && (has_sse(ECX, 26)) && (has_sse(ECX, 25)))
	{
		return true;
	}
	return false;
}

// �������� ��������, ����� ����� � ���������
int _tmain(int argc, _TCHAR* argv[])
{
	if (test_sse())
	{
		cout << "Has SSE" << endl;
	}
	else
	{
		cout << "No SSE" << endl;
		cout << "Program terminated" << endl;
		system("pause");
		return 0;
	}
	GF2_64 a_test, b_test, c_test;
	srand((unsigned)time(NULL));
	GF2_64 a[5] = { 1, 2, 3, 4, 5 };
	GF2_64 b[5] = { 5, 4, 3, 2, 1 };
	GF2_64 sum[5] = { -1, -1, -1, -1, -1 };
	int z = PolySum(sum, a, 3, b, 2);
	cout << "Testing field functions:" << endl;
	/*
		����� ���� ������������ �������, ���������� �� ����� ����������
		� ������ ������, ��������� ��������������� ���������
	*/
	for (int i = 0; i < TEST_SIZE; i++)
	{
		a_test = random();
		b_test = random();
		unsigned int Power = ((unsigned int)a_test) % 100;  
		if (GF_MulX(a_test) != GF_MulX_test(a_test))
		{
			cout << "Error in GF_MulX" << endl;
		}
		if (GF_PowX_test(Power) != GF_PowX(Power))
		{
			cout << "Error in GF_PowX" << endl;
		}
		if (GF_Multiply_test(a_test, b_test) != (GF_Multiply(a_test, b_test)))
		{
			cout << "Error in GF_Multiply" << endl;
		}
		if (GF_Reciprocal_test(a_test) != GF_Reciprocal(a_test))
		{
			cout << "Error in GF_Reciprocal" << endl;
		}
		if (i % (TEST_SIZE / 10) == 0)
		{
			cout << i << endl;
		}
	}
	cout << "Testing polinoms:" << endl;
	int deg_a, deg_b;
	int deg, deg_test;
	for (int i = 0; i < TEST_SIZE; i++)
	{
		deg_a = (::rand() % ARR_SIZE) - 1;
		deg_b = deg_a;
		GF2_64 *a = new GF2_64[deg_a+2];
		GF2_64 *b = new GF2_64[deg_a+2];
		for (int j = 0; j <= deg_a; j++)
		{
			a[j] = random();
			b[j] = a[j];
		}
		deg = PolyMulX(a, deg_a);
		deg_test = PolyMulX_test(b, deg_b);
		if (!arrs_eq(a, deg, b, deg_test))
		{
			cout << "Error in PolyMulX" << endl;
		}
		for (int j = 0; j <= deg_a; j++)
			b[j] = a[j];
		GF2_64 cnst = random();
		deg = PolyMulConst(a, deg_a, cnst);
		deg_test = PolyMulConst_test(b, deg_a, cnst);
		if (!arrs_eq(a, deg, b, deg_test))
		{
			cout << "Error in PolyMulConst" << endl;
		}
		deg = PolyZero(a, deg_a);
		if ((deg != -1) || (!is_zero(a, deg_a)))
		{
			cout << "Error in PolyZero" << endl;
		}
		unsigned char pos_deg = (unsigned char)deg_a;
		delete[] a;
		delete[] b;
		a = new GF2_64[pos_deg+1];
		b = new GF2_64[pos_deg+1];
		for (int i = 0; i <= pos_deg; i++)
		{
			a[i] = random();
		}
		if (pos_deg == 255)
		{
			int p = 0;
		}
		deg = PolyCpy(b, a, pos_deg);
		if (!arrs_eq(a, pos_deg, b, deg))
		{
			cout << "Error in PolyCpy" << endl;
		}
		delete[] a;
		delete[] b;
		deg_a = (::rand() % ARR_SIZE) - 1;
		deg_b = (::rand() % ARR_SIZE) - 1;
		a = new GF2_64[deg_a+1];
		b = new GF2_64[deg_b+1];
		for (int j = 0; j <= deg_a; j++)
		{
			a[j] = random();
		}
		for (int j = 0; j <= deg_b; j++)
		{
			b[j] = random();
		}
		int deg_sum = ::max(deg_a, deg_b);
		GF2_64 *sum = new GF2_64[deg_sum+1];
		GF2_64 *sum_test = new GF2_64[deg_sum+1];
		deg_sum = PolySum(sum, a, deg_a, b, deg_b);
		int deg_sum_test = PolySum_test(sum_test, a, deg_a, b, deg_b);
		if (!arrs_eq(sum, deg_sum, sum_test, deg_sum_test))
		{
			cout << "Error in PolySum" << endl;
		}
		if (i % (TEST_SIZE / 10) == 0)
		{
			cout << i << endl;
		}
		delete[] a;
		delete[] b;
	}
	system("pause");
	return 0;
}

