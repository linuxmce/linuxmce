/*
     Copyright (C) 2012 LinuxMCE

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef ZWConfigData_h
#define ZWConfigData_h

#include <string>

namespace DCE
{

	class ZWConfigData
	{
	public:
		string m_sPort;
		int m_itimeout;
		ZWConfigData(string port, int timeout) {
			m_sPort = port;
			m_itimeout = timeout;
		}
	};
}
#endif
