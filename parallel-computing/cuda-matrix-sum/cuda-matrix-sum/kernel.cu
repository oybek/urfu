
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <cstdio>
#include <iostream>
#include <fstream>

#include "matrix.h"

int main()
{
	// —читаем значени€ дл€ матрицы из файла
	matrix<int> m;
	int rown, coln;

#define LOCAL_PATH "E:\\urfu\\parallel-computing\\cuda-matrix-sum\\Debug\\"

	std::ifstream fin( LOCAL_PATH "m.txt" );
	std::ofstream fout( LOCAL_PATH "m_double.txt" );
	fin >> rown >> coln;

	m.resize(rown, coln);
	for (int i = 0; i < rown; ++i)
	{
		for (int j = 0; j < coln; ++j)
			fin >> m[i][j];
	}

	matrix<int> m_sum = m + m;

	m_sum.info();

	for (int i = 0; i < rown; ++i)
	{
		for (int j = 0; j < coln; ++j)
		{
			fout << m_sum[i][j] << ' ';
		}
		fout << std::endl;
	}

    return 0;
}
