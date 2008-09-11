/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "IconDecoder.h"
//--------------------------------------------------------------------------------------------------
using namespace std;
#include "../PlutoUtils/uuencode.h"
#include <cstring>
//--------------------------------------------------------------------------------------------------
static int pr2six[256] = 
{
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, 
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, 
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, 
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};
//--------------------------------------------------------------------------------------------------
char *DecodeIcon(string sRawImage, size_t& unIconSize)
{
	char *pIconData = NULL;
	unIconSize = 0;

	string str = sRawImage.c_str();
	size_t Size = sRawImage.length();

	if(Size == 0)
		return pIconData;

	char *pDataEncoded = new char[Size];
	char *d, *s;
	d = pDataEncoded;
	s = (char *)str.c_str();

	while(*s)
	{
		if(pr2six[(int)*s]!=-1)
			*d++ = *s;
		s++;
	}

	*d = '\0';
	Size = strlen(pDataEncoded);
	pIconData = new char[Size];

	if(Size)
		unIconSize = Ns_HtuuDecode((unsigned char *) pDataEncoded, (unsigned char *) pIconData, (int)Size);

	delete [] pDataEncoded;
	return pIconData;
}
//--------------------------------------------------------------------------------------------------
