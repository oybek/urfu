
#include <iostream>
#include <cassert>
#include <cstdint>
#include <ctime>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>
#include <cstdio>
#include <cmath>
#include <omp.h>

// библиотека для работы с большими числами
#include "bigint.h"

typedef int64_t i64;
typedef int32_t i32;

// Реализация метода вычисления чисел Фибоначчи методом Кнута (Q-матрицы)
template <class T>
class QM
{
public:
	T m[2][2];

	QM()
	{}

	QM(T axx, T axy, T ayx, T ayy)
	{
		m[0][0] = axx;
		m[0][1] = axy;
		m[1][0] = ayx;
		m[1][1] = ayy;
	}

	const QM operator* (const QM& a) const
	{
		QM mult( 0, 0, 0, 0 );
		// Распараллеливаем умножение матриц
		omp_set_num_threads(4);
#pragma omp parallel for
		for( int i = 0; i < 2; ++i )
		{
			for( int k = 0; k < 2; ++k )
				mult.m[i][1] += m[i][k] * a.m[k][1];
		}

		mult.m[1][0] = mult.m[0][1];
		mult.m[0][0] = mult.m[0][1] + mult.m[1][1];
		return mult;
	}
};

//
typedef QM< bigint > m22;

// TODO: должно быть внутри класса или как шаблон
#define SIZE 20

// TODO: должно быть внутри класса
m22 Q[SIZE]; // Q^(2^i)

// Метод Кнута
class Fibonacci final
{
public:
	static bigint get(i64 n)
	{
		Q[0] = { 1, 1, 1, 0 };

		for (i32 i = 1; i < SIZE; ++i)
			Q[i] = Q[i - 1] * Q[i - 1];

		n--;
		m22 ans = { 1, 0, 0, 1 };

		for (i32 i = 0; i < SIZE; ++i)
		{
			if (n & (1ll << i))
				ans = ans * Q[i] ;
		}

		return ans.m[0][0];
	}
};

#undef SIZE

int main(void)
{
	i64 n;
	std::cin >> n;
	std::cout << Fibonacci::get(n) << std::endl;
	
	return 0;
}

