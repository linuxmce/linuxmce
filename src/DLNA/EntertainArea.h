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
	class LMCERenderer;

	class EntertainArea {
	public:
		EntertainArea(int iPK_EntArea, string description) {
			m_iPK_EntertainArea = iPK_EntArea;
			m_sDescription = description;
			m_pRenderer = NULL;
		}
		int m_iPK_EntertainArea;
		string m_sDescription;
		set<long> m_setDevices;
		LMCERenderer* m_pRenderer;
	};
}
#endif
