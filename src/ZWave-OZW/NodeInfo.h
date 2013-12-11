/*
     Copyright (C) 2012 LinuxMCE

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef NodeInfo_h
#define NodeInfo_h

namespace DCE {
	class LMCEDevice
	{
	public:
		unsigned long m_dwPK_Device;
		unsigned long m_dwPK_Parent_Device;
		unsigned long m_dwFK_DeviceTemplate;
		int m_iMainValue;
		// pointers to the values in the NodeInfo mapped to this device
		vector<OpenZWave::ValueID>	m_vectValues;

		LMCEDevice() {
			m_dwPK_Device = 0;	
			m_dwPK_Parent_Device = 0;
			m_dwFK_DeviceTemplate = 0;
			m_iMainValue = -1;
		}
		void assignValue(OpenZWave::ValueID valueID, bool bMainValue) {
			if (m_iMainValue == -1 || (bMainValue &&m_vectValues[m_iMainValue].GetCommandClassId() == 32))
				m_iMainValue = m_vectValues.size();
			m_vectValues.push_back( valueID );
		}
		OpenZWave::ValueID GetMainValue()
		{
			return m_vectValues[m_iMainValue];
		}
		bool hasMainValue() {
			return m_iMainValue >= 0;
		}

	};

	class NodeInfo
	{
	public:
		uint32			m_homeId;
		uint8			m_nodeId;
		uint8			m_generic;
		uint8			m_specific;
		bool			m_polled;
		// List of devices that this node is mapped to
		vector<LMCEDevice*>              m_vectDevices;
		// list of values for this node
		list<OpenZWave::ValueID>	m_values;

		NodeInfo() {
			// Add main device at position 0
			m_vectDevices.push_back(new LMCEDevice());
			m_generic = 0;
			m_specific = 0;
		}

		~NodeInfo() {
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

		void ClearDeviceMapping()
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

		void AddDevice(unsigned int deviceNo, unsigned long FK_DeviceTemplate, unsigned long PK_Parent_Device) {
			if (deviceNo > m_vectDevices.size()-1) {
				LMCEDevice* pDevice = new LMCEDevice();
				pDevice->m_dwFK_DeviceTemplate = FK_DeviceTemplate;
				pDevice->m_dwPK_Parent_Device = PK_Parent_Device;
				m_vectDevices.push_back(pDevice);
			}
		}
		void MapValueToDevice(unsigned int deviceNo, OpenZWave::ValueID value, bool bMainValue) {
			if (deviceNo < m_vectDevices.size()) {
				m_vectDevices[deviceNo]->assignValue(value, bMainValue);
			}
		}
		
		unsigned long GetPKDeviceForValue(OpenZWave::ValueID value)
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
	};
}

#endif
