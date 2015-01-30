// FixupBMP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "stdafx.h"

#include <sys\stat.h>

#define BYTE	unsigned char
#define WORD	unsigned short
#define DWORD	unsigned long

#pragma pack(push)
#pragma pack(1)

typedef struct _tagBMPHeader
{
	WORD		dwSignature;
	DWORD		dwFileSize;
	WORD		wReserved1;
	WORD		wReserved2;
	DWORD		dwImageOffset;
	DWORD		dwBIHSize;
	DWORD		dwWidth;
	DWORD		dwHeight;
	WORD		wPlanes;
	WORD		wBPP;
	DWORD		dwCompressionType;
	DWORD		dwImageSize;
	DWORD		dwHResPPM;
	DWORD		dwVResPPM;
	DWORD		dwNumberOfColours;
	DWORD		dwNumberOfImportantColours;
} BMPHeader;

#pragma pack(pop)

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 4)
	{
		printf("command line: <BMP> <EXTRA> <OUT>\n");
		return 1;
	}

	FILE* pInFile = fopen(argv[1], "rb");		
	if (pInFile == NULL)
	{
		printf("unable to open file '%s'...\n", argv[1]);
		return 1;
	}

	FILE* pExtraDataFile = fopen(argv[2], "rb");
	if (pExtraDataFile == NULL)
	{
		printf("unable to open file '%s'...\n", argv[2]);
		return 1;
	}

	FILE* pOutFile = fopen(argv[3], "wb");
	if (pOutFile == NULL)
	{
		printf("unable to open file '%s'...\n", argv[3]);
		return 1;
	}

	// read the BMP header
	BMPHeader Header;
	fread(&Header, sizeof(Header), 1, pInFile);			

	// allocate memory for the image data
	BYTE* pbyData = new BYTE[Header.dwImageSize];

	// read the image data
	fread(pbyData, Header.dwImageSize, 1, pInFile);
	fclose(pInFile);

	// get the size of the file to add
	struct stat statbuf;
	fstat(_fileno(pExtraDataFile), &statbuf);

	// allocate and read the extra data
	BYTE* pbyExtraData = new BYTE[statbuf.st_size];
	fread(pbyExtraData, statbuf.st_size, 1, pExtraDataFile);
	fclose(pExtraDataFile);

	// modify the header
	Header.dwFileSize += statbuf.st_size;
	//Header.dwImageOffset += ADD_BYTES;

	// write the header, image and extra data to the output file
	fwrite(&Header, sizeof(Header), 1, pOutFile);	
	fwrite(pbyData, Header.dwImageSize, 1, pOutFile);
	fwrite(pbyExtraData, statbuf.st_size, 1, pOutFile);			
	
	fclose(pOutFile);

	delete[] pbyData;
	delete[] pbyExtraData;

	printf("FixupBMP: completed, added %u bytes\n", statbuf.st_size);

	return 0;
}

