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
// C++ Interface: proxyeventhandler
//
// Description:
//
//
// Author: igor <igor@dexx>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MYTHTVPROXYEVENTHANDLER_H
#define MYTHTVPROXYEVENTHANDLER_H

namespace MYTHTV
{
	/**
	@author igor
	*/
	class ProxyEventHandler
	{
	public:
		virtual void ChannelChanged(const char *host, int channelid) = 0;
// 		{
// 		};

		virtual void FrontendConnected(const char *host) = 0;
// 		{
// 		};

		virtual void FrontendDisconnected(const char *host) = 0;
/*		{
		};*/
	};
};

#endif
