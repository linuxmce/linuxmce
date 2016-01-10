/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "RubySerialIOConnectionWrapper.h"
#include "DCE/Logger.h"

namespace DCE {

RubySerialIOConnectionWrapper::RubySerialIOConnectionWrapper(IOConnection* pconn)
	: BufferedIOConnection(pconn)
{
}


RubySerialIOConnectionWrapper::~RubySerialIOConnectionWrapper()
{
}

bool 
RubySerialIOConnectionWrapper::Reconnect() {
	IOConnection* pconn = getConnection();
	pconn->Close();
	bool bResult = pconn->Open();
	if( !bResult )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"RubySerialIOConnectionWrapper::Reconnect Open() failed");

	return bResult;
}


};
