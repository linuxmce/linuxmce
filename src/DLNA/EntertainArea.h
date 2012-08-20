/*
     Copyright (C) 2012 LinuxMCE
     www.linuxmce.org

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef EntertainArea_h
#define EntertainArea_h

namespace DCE {
/*	class LMCEDevice {
	public:
		LMCEDevice(long PK_Device, long PK_MediaType
		long m_dwPK_Device;
	        long m_dwPK_MediaType;
	};
*/	class EntertainArea {
	public:
		EntertainArea(int iPK_EntArea, string description) {
			m_iPK_EntertainArea = iPK_EntArea;
			m_sDescription = description;
		}
		int m_iPK_EntertainArea;
		string m_sDescription;
		vector<long> m_vectDevices;
	};
}
#endif
