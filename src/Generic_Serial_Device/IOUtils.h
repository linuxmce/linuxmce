/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file IOUtils.h
Base classe for IOUtils.
*/
#ifndef DCEIOUTILS_H
#define DCEIOUTILS_H

#include <string>

// #pragma warning( disable : 4996 )

/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

/** @class IOUtils
Utilities?
*/
class IOUtils{
public:
	static std::string FormatHexBuffer(const char* buff, unsigned int size);
	static std::string FormatHexAsciiBuffer(const char* buff, unsigned int size,const char *color="0");
};

};

#endif
