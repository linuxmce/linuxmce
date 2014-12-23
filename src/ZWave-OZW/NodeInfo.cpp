/*
     Copyright (C) 2012 LinuxMCE

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

#include "NodeInfo.h"
#include "LMCEDevice.h"

using namespace DCE;

NodeInfo::NodeInfo() {
	// Add main device at position 0
	m_vectDevices.push_back(new LMCEDevice(this));
	m_generic = 0;
	m_specific = 0;
	m_iDeadCount = 0;
}

NodeInfo::~NodeInfo() {
	for (vector<LMCEDevice*>::iterator it = m_vectDevices.begin(); it != m_vectDevices.end(); ++it)
	{
		if ((*it) != NULL)
		{
			delete (*it);
			(*it) = NULL;
		}
	}
	m_vectDevices.clear();
}

void NodeInfo::ClearDeviceMapping()
{
	if (m_vectDevices.size() > 1)
	{
		for (vector<LMCEDevice*>::iterator it = m_vectDevices.begin()+1; it != m_vectDevices.end(); ++it)
		{
			if ((*it) != NULL)
			{
				delete (*it);
				(*it) = NULL;
			}
		}
		m_vectDevices.erase(m_vectDevices.begin()+1, m_vectDevices.end());
	}
	m_vectDevices[0]->m_vectValues.clear();
	m_vectDevices[0]->m_iMainValue = -1;
	m_vectDevices[0]->m_dwFK_DeviceTemplate = 0;
}

void NodeInfo::AddDevice(unsigned int deviceNo) {
	if (deviceNo > m_vectDevices.size()-1) {
		LMCEDevice* pDevice = new LMCEDevice(this);
		m_vectDevices.push_back(pDevice);
	}
}
void NodeInfo::MapValueToDevice(unsigned int deviceNo, OpenZWave::ValueID value) {
	if (deviceNo < m_vectDevices.size()) {
		m_vectDevices[deviceNo]->assignValue(value);
	}
}

unsigned long NodeInfo::GetPKDeviceForValue(OpenZWave::ValueID value)
{
	for (vector<LMCEDevice*>::iterator it = m_vectDevices.begin(); it != m_vectDevices.end(); ++it)
	{
		LMCEDevice *pDevice = (*it);
		for (vector<OpenZWave::ValueID>::iterator it = pDevice->m_vectValues.begin(); it != pDevice->m_vectValues.end(); ++it)
		{
			OpenZWave::ValueID nodevalue = *it;
			if (value == nodevalue)
				return pDevice->m_dwPK_Device;
		}
	}
	return 0;
}

