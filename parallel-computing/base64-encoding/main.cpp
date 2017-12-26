
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include "base64.h"

int main( int argc, char** argv )
{
	if( argc != 4 || strcmp( argv[1], "to64" ) && strcmp( argv[1], "from64" ) )
	{
		fprintf( stderr, "Usage: %s <to64|from64> <fname> <converted file>\n", argv[0] );
		exit(1);
	}

	FILE* pFile;
	FILE* oFile;
	long lSize;
	unsigned char* buffer;
	size_t result;

	if( strcmp( argv[1], "to64" ) == 0 )
	{
		pFile = fopen( argv[2], "rb" );
		if( pFile == NULL ) { fputs("File error", stderr); exit (1); }

		// ���������� ������ �����
		fseek (pFile , 0 , SEEK_END);
		lSize = ftell (pFile);
		rewind (pFile);

		// �������� ������ ��� �������� �����
		buffer = (unsigned char*) malloc (sizeof(unsigned char)*lSize);
		if( buffer == NULL ) { fputs("Memory error",stderr); exit (2); }

		// �������� ���� � ������
		result = fread (buffer,1,lSize,pFile);
		if (result != lSize) { fputs("Reading error",stderr); exit (3); }

		// ���� ������ � ������ - ��������������� � base64
		std::string encoded_str = base64_encode( buffer, lSize );

		oFile = fopen( argv[3], "w" );
		if( oFile == NULL ) { fputs("File error", stderr); exit (1); }

		fprintf( oFile, "%s", encoded_str.c_str() );

		// terminate
		fclose( oFile );
		fclose( pFile );
		free( buffer );
	}
	else if( strcmp( argv[1], "from64" ) == 0 )
	{
		pFile = fopen( argv[2], "r" );
		if( pFile == NULL ) { fputs("File error", stderr); exit (1); }

		// ����������� 1Mb ������
		std::string str; str.reserve(1024*1024);
		char c;
		while( (c = fgetc(pFile)) != EOF )
		{
			str.push_back(c);
		}

		std::string decoded_str = base64_decode( str );
		oFile = fopen( argv[3], "wb" );
		if( oFile == NULL ) { fputs("File error", stderr); exit (1); }

		const char* buffer = decoded_str.c_str();
		fwrite( buffer, sizeof(unsigned char), decoded_str.size(), oFile );

		// terminate
		fclose( pFile );
		fclose( oFile );
	}

	return 0;
}
