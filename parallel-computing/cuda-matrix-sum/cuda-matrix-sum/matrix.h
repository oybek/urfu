#pragma once

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>

#define MEMORY_ALLOCATION_ERROR false

// В анонимный namespace т. к. нам не зачем, эти функции вне данного файла
namespace
{
	//
	__global__ void addKernel(int *c, const int *a, const int *b)
	{
		int i = threadIdx.x;
		c[i] = a[i] + b[i];
	}

	// Helper function for using CUDA to add vectors in parallel.
	cudaError_t addWithCuda(int *c, const int *a, const int *b, unsigned int size)
	{
		int *dev_a = 0;
		int *dev_b = 0;
		int *dev_c = 0;
		cudaError_t cudaStatus;

		// Choose which GPU to run on, change this on a multi-GPU system.
		cudaStatus = cudaSetDevice(0);
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
			goto Error;
		}

		// Allocate GPU buffers for three vectors (two input, one output)    .
		cudaStatus = cudaMalloc((void**)&dev_c, size * sizeof(int));
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "cudaMalloc failed!");
			goto Error;
		}

		cudaStatus = cudaMalloc((void**)&dev_a, size * sizeof(int));
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "cudaMalloc failed!");
			goto Error;
		}

		cudaStatus = cudaMalloc((void**)&dev_b, size * sizeof(int));
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "cudaMalloc failed!");
			goto Error;
		}

		// Copy input vectors from host memory to GPU buffers.
		cudaStatus = cudaMemcpy(dev_a, a, size * sizeof(int), cudaMemcpyHostToDevice);
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "cudaMemcpy failed!");
			goto Error;
		}

		cudaStatus = cudaMemcpy(dev_b, b, size * sizeof(int), cudaMemcpyHostToDevice);
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "cudaMemcpy failed!");
			goto Error;
		}

		// Launch a kernel on the GPU with one thread for each element.
		addKernel << <1, size >> > (dev_c, dev_a, dev_b);

		// Check for any errors launching the kernel
		cudaStatus = cudaGetLastError();
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
			goto Error;
		}

		// cudaDeviceSynchronize waits for the kernel to finish, and returns
		// any errors encountered during the launch.
		cudaStatus = cudaDeviceSynchronize();
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
			goto Error;
		}

		// Copy output vector from GPU buffer to host memory.
		cudaStatus = cudaMemcpy(c, dev_c, size * sizeof(int), cudaMemcpyDeviceToHost);
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "cudaMemcpy failed!");
			goto Error;
		}

	Error:
		cudaFree(dev_c);
		cudaFree(dev_a);
		cudaFree(dev_b);

		return cudaStatus;
	}
}

template< class T >
class matrix
{
	private:
		// Сама матрица
		int buf_len;
		T * buf = nullptr;

		// Характеристики матрицы
		int row_n;
		int col_n;

	public:
		// Конструктор по умолчанию - создает пустую матрицу
		matrix()
			: buf_len(0)
			, buf(nullptr)
			, row_n(0)
			, col_n(0)
		{}

		// Создание матрицы заданием ее размеров - инициализирует элементы нулями
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
			buf_len = m.buf_len;
			buf = new T[buf_len];
			row_n = m.row_n;
			col_n = m.col_n;
			memcpy( buf, m.buf, sizeof(T)*buf_len );
		}

		// Накидал конструктор перемещения для эффективной реализации оператора суммирования
		matrix( matrix&& m )
		{
			std::swap(buf, m.buf);
			std::swap(buf_len, m.buf_len);
			std::swap(row_n, m.row_n);
			std::swap(col_n, m.col_n);
		}

		~matrix()
		{
			delete[] buf;
		}

		// Перегрузка оператора индексирования - возвращает указатель на участок буфера
		// что позволяет работать с объектом как с матрицей
		T* operator[] ( int row )
		{
			return buf + row * col_n;
		}

		T at(int i) const
		{
			return buf[i];
		}

		//
		void resize(int rown, int coln)
		{
			matrix tmp(rown, coln);

			memcpy( tmp.buf, buf, sizeof(T)*std::min(buf_len, tmp.buf_len) );

			std::swap(buf, tmp.buf);
			std::swap(buf_len, tmp.buf_len);
			std::swap(row_n, tmp.row_n);
			std::swap(col_n, tmp.col_n);
		}

		//
		matrix operator+ (const matrix& m) const
		{
			matrix sum = *this;

			// Add vectors in parallel.
			cudaError_t cudaStatus = addWithCuda(sum.buf, buf, m.buf, buf_len);
			if (cudaStatus != cudaSuccess) {
				fprintf(stderr, "addWithCuda failed!");
				exit(1);
			}

			// cudaDeviceReset must be called before exiting in order for profiling and
			// tracing tools such as Nsight and Visual Profiler to show complete traces.
			cudaStatus = cudaDeviceReset();
			if (cudaStatus != cudaSuccess) {
				fprintf(stderr, "cudaDeviceReset failed!");
				exit(1);
			}

			return sum;
		}

		//
		void info() const
		{
			std::cout << row_n << ' ' << col_n << std::endl;
		}
};
