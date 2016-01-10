/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//
// C++ Implementation: monitorrequestmessage
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "monitorrequestmessage.h"
#include "busconnector.h"

namespace EIBBUS {

static const unsigned char s_monitorrequest[6] = 
/*		{0xA9, 0x90, 0x18, 0x34, 0x56, 0x78, 0x0A};*/
			  {0x90, 0x18, 0x34, 0x56, 0x78, 0x0A};

int 
MonitorRequestMessage::Send(BusConnector *pbusconn) {
	SendPeiBuffer(pbusconn, 0x68, 0xA9, s_monitorrequest, sizeof(s_monitorrequest));
	return 0;
}

int 
MonitorRequestMessage::Recv(BusConnector *pbusconn) {
	return -1;
}

};
