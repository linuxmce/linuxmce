/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file bauinitmessage.h
Header for BauInitMessage class.
*/
//
// C++ Interface: bauinitmessage
//
// Description:
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EIBBUSBAUINITMESSAGE_H
#define EIBBUSBAUINITMESSAGE_H

#include "message.h"

/** @namespace EIBBUS
For EIB (European Installation Bus) Lighting control.
*/
namespace EIBBUS {

/** @class BauInitMessage
For ???
*/
class BauInitMessage : public Message {
public:
	virtual int Send(BusConnector *pbusconn);
};

};

#endif
