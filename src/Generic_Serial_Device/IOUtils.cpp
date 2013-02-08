/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "IOUtils.h"
#include <stdio.h>

namespace DCE {

std::string 
IOUtils::FormatHexBuffer(const char* buff, unsigned int size) {
	std::string logstr;
	if(size <= 0) {
		logstr = "EMPTY BUFFER";
	} else {
		char hxbuff[5];
		for(unsigned int i = 0; i < size; i++) {
			unsigned char c = buff[i];
			sprintf(hxbuff, "0x%0hhx", (int) c);
			logstr += ((i > 0) ? " " : "");
			logstr += hxbuff;
		}
	}
	return logstr;
}

std::string IOUtils::FormatHexAsciiBuffer(const char* buff, unsigned int size, const char *color)
{
	std::string Result = FormatHexBuffer(buff,size);
	if( Result.size() )
	{
		Result += " (\x1b[" + std::string(color) + ";1m";

		for(int i=0;i<size;++i)
		{
			char c = buff[i];
			if( c>=' ' && c<='~' )
				Result += c;
			else if( c=='\r' )
				Result += "\\r";
			else if( c=='\n' )
				Result += "\\n";
			else
				Result += '#';
		}
		Result += "\x1b[0m)";
	}
	return Result;
}

};
