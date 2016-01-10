/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef AskXine_Socket_h
#define AskXine_Socket_h

class AskXine_Socket : public ClientSocket
{
public:
	// more simple c-tor: we don't need deviceID here
	AskXine_Socket(string sIPAddress, string sName ):ClientSocket(0, sIPAddress, sName){};
	// overriding on connect event handler, because we use custom protocol (actually just reading)
	virtual bool OnConnect( int PK_DeviceTemplate,string sExtraInfo="" ){return true;};
};

#endif
