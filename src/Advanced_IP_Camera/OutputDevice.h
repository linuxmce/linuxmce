/*
     Copyright (C) 2012 LinuxMCE

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef Output_Device_h
#define Output_Device_h

//	DCE Implemenation for #2208 Advanced IP Camera

namespace DCE {

	class OutputDevice {
	public:
		OutputDevice(int PK_Device, int FK_DeviceTemplate)
		{
			m_dwPK_Device = PK_Device;
			m_dwFK_DeviceTemplate = FK_DeviceTemplate;
		}
		
		int m_dwPK_Device;
		int m_dwFK_DeviceTemplate;
		int status;
		string m_sControlMethod;
		string m_sOn;
		string m_sOff;
		
		
		string ToString() 
		{
			return StringUtils::Format("m_dwPK_Device = %d, m_dwFK_DeviceTemplate = %d, status = %s, m_sControlMethod = %s, m_sOn = %s, m_sOff = %s", m_dwPK_Device, m_dwFK_DeviceTemplate, status, m_sControlMethod.c_str(), m_sOn.c_str(), m_sOff.c_str());
		}
	};
}	
#endif
