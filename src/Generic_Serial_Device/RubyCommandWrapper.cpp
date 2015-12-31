/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "RubyCommandWrapper.h"

namespace DCE {

RubyCommandWrapper::RubyCommandWrapper() 
	: devidfrom_(0), devidto_(0), priority_(0), type_(0), id_(0), category_(0), template_(0)
{}

RubyCommandWrapper::RubyCommandWrapper(long devidfrom, long devidto, long priority, long type, long id )
	: devidfrom_(devidfrom), devidto_(devidto), priority_(priority), type_(type), id_(id)
{}

RubyCommandWrapper::~RubyCommandWrapper() 
{}

};
