/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef __HALD_HELPER_H__
#define __HALD_HELPER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <map>
#include <string>
#include <vector>

#include <dbus/dbus.h>
#include <hal/libhal.h>
#include <glib-2.0/glib.h>
#include <dbus/dbus-glib-lowlevel.h>

using namespace std;

class HalDHelper
{
	private:
	
		static DBusError halError;
		static void getPortIdentification(string portFromBus, string& portID);
		static void InternalFindDevice(LibHalContext * ctx, const map<unsigned long, vector<string> >& mapDevices);

	public:
	
		static void* FindDevice(const map<unsigned long, vector<string> >& mapDevices);
};

#endif /*__HALD_HELPER_H__*/
