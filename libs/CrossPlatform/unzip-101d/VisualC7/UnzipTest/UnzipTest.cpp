// UnzipTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "unzip_wrapper.h"

int _tmain(int argc, _TCHAR* argv[])
{
	const char *pSourceZipFile = "c:\\temp\\bubu.zip";
	const char *pDestionationFolder = "c:\\temp";
	UnzipFile(pSourceZipFile, pDestionationFolder);

	return 0;
}

