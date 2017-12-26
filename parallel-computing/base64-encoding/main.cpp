
#include <string>
#include <cstdio>
#include <fstream>
#include <iostream>
#include "base64.h"

// Максимальный размер файла в байтах, 20 Mb
#define MAX_FILE_SIZE 1024*1024

int main()
{
	char fname[256];

	printf("Введите имя файла для преобразования его в base64: ");
	scanf("%s", fname);

	FILE* file;
	int n;
	unsigned char buffer[MAX_FILE_SIZE];

	file = fopen(fname, "rb");
	if( file )
	{
		n = fread(buffer, MAX_FILE_SIZE, 1, file);
	}
	else
	{
		printf("Ошибка: Не возможно открыть файл %s для чтения\n", fname);
	}

	std::string encoded_str = base64_encode( buffer, n );

	std::string encoded_fname = std::string(fname)+".txt";
	FILE* fout = fopen( encoded_fname.c_str(), "w" );
	if( fout )
	{
		std::cout << encoded_str << std::endl;
	}
	else
	{
		printf("Ошибка: Не возможно открыть файл %s для чтения\n", encoded_fname.c_str() );
	}

	return 0;
}
