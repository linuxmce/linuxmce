/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/
#include "FileCodeSupplier.h"

#include <fstream>

#define READ_BUFFER_SIZE	512

using namespace std;

namespace EMBRUBY {

FileCodeSupplier::FileCodeSupplier()
{
}


FileCodeSupplier::~FileCodeSupplier()
{
}

void 
FileCodeSupplier::loadFile(const char* path) throw(RubyException) {
	ifstream f(path, fstream::in);
	if(!f.is_open()) {
		throw RubyException(string("Cannot open file: ") + path);
	}
	
	char rbuff[READ_BUFFER_SIZE];
	int nread = 0;
	while((nread = f.readsome(rbuff, READ_BUFFER_SIZE - 1)) > 0) {
		rbuff[nread] = 0;
		code_ += rbuff;
	}
}

};
