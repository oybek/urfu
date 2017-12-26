#pragma once

#include <algorithm>
#include <cassert>

#define MEMORY_ALLOCATION_ERROR false

template< class T >
class matrix
{
	private:
		// ���� �������
		int buf_len;
		T * buf;

		// �������������� �������
		int row_n;
		int col_n;

	public:
		// ����������� �� ��������� - ������� ������ �������
		matrix()
			: buf_len(0)
			, buf(nullptr)
			, row_n(0)
			, col_n(0)
		{}

		// �������� ������� �������� �� �������� - �������������� �������� ������
		matrix(int row_n, int col_n)
			: row_n(row_n)
			, col_n(col_n)
		{
			buf_len = row_n * col_n;
			buf = new T[ buf_len ];
			if (buf == nullptr)
			{
				assert( MEMORY_ALLOCATION_ERROR );
			}

			std::fill(buf, buf + buf_len, 0);
		}

		//
		matrix( const matrix& m )
		{
			delete[] buf;
			matrix(m.row_n, m.col_n);

			std::copy( m.buf, m.buf + m.buf_len, buf );
		}

		// ������� ����������� ����������� ��� ����������� ���������� ��������� ������������
		matrix( matrix&& m )
		{
			std::swap(buf, m.buf);
			std::swap(buf_len, m.buf_len);
			std::swap(row_n, m.row_n);
			std::swap(col_n, m.col_n);
		}

		// ���������� ��������� �������������� - ���������� ��������� �� ������� ������
		// ��� ��������� �������� � �������� ��� � ��������
		T* operator[] ( int row )
		{
			return buf + row * col_n;
		}

		//
		matrix operator+ (const matrix& m) const
		{
			matrix sum = *this;
			return sum;
		}
};
