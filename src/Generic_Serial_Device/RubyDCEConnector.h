/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

/** @file RubyDCEConnector.h
For ?
*/
#ifndef DCERUBYDCECONNECTOR_H
#define DCERUBYDCECONNECTOR_H

#include "RubyCommandWrapper.h"

/** @namespace DCE
The Data Commands and Events (DCE) namespace.
*/
namespace DCE {

/** @class RubyDCEConnector
For ???
*/
class RubyDCEConnector {
public:
	/*methods for comunicating with DCE */
	virtual void SendCommand(RubyCommandWrapper* pcmd) = 0;
	virtual void SendReceiveCommand(RubyCommandWrapper* pcmd) = 0;
	virtual string SendCommandReceiveString(RubyCommandWrapper* pcmd) = 0;
};

};

#endif
