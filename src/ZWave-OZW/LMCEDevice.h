/*
     Copyright (C) 2012 LinuxMCE

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef LMCEDevice_h
#define LMCEDevice_h

#include "Defs.h"
#include "NodeInfo.h"
#include <openzwave/Manager.h>

namespace DCE {

	class LMCEDevice
	{
	public:
		NodeInfo* m_pNodeInfo;
		unsigned long m_dwPK_Device;
		unsigned long m_dwPK_Parent_Device;
		unsigned long m_dwFK_DeviceTemplate;
		int m_iMainValue;
		// pointers to the values in the NodeInfo mapped to this device
		vector<OpenZWave::ValueID>	m_vectValues;

		LMCEDevice(NodeInfo* pNodeInfo) {
			m_pNodeInfo = pNodeInfo;
			m_dwPK_Device = 0;	
			m_dwPK_Parent_Device = 0;
			m_dwFK_DeviceTemplate = 0;
			m_iMainValue = -1;
		}
		~LMCEDevice() {
		}
		void assignValue(OpenZWave::ValueID valueID) {
/*			if (m_iMainValue == -1 || (bMainValue &&m_vectValues[m_iMainValue].GetCommandClassId() == 32))
			m_iMainValue = m_vectValues.size();*/
			m_vectValues.push_back( valueID );
		}
		void SetMainValue(int i)
		{
			m_iMainValue = i;
		}
		
		OpenZWave::ValueID GetMainValue()
		{
			return m_vectValues[m_iMainValue];
		}
		bool HasMainValue() {
			return m_iMainValue >= 0;
		}

		bool IsCompatible(OpenZWave::ValueID newValue, string label) {
			for (vector<OpenZWave::ValueID>::iterator it = m_vectValues.begin(); it != m_vectValues.end(); ++it)
			{
				OpenZWave::ValueID ourValue = *it;
				string ourLabel = OpenZWave::Manager::Get()->GetValueLabel(ourValue);
				if (IsSpecialLabel(label) && IsSpecialLabel(ourLabel) &&
				    !(label == "Temperature" && m_pNodeInfo->m_generic == GENERIC_TYPE_THERMOSTAT) &&
				    !(label == "Level" && ourLabel == "Switch") &&
				    !(label == "Switch" && ourLabel == "Level"))
					return false;
			}
			return true;
/*
			if ((label == "Temperature" || label  == "Luminance" || label == "Relative Humidity" || label == "Switch" || label == "Level" || label == "Sensor")
					    && (mainLabel == "Temperature" || mainLabel  == "Luminance" || mainLabel == "Relative Humidity" || mainLabel == "Switch" || mainLabel == "Level" || mainLabel == "Sensor")
					    && !(label == "Temperature" && node->m_generic == DCE::GENERIC_TYPE_THERMOSTAT)
					    && !(label == "Level" && mainLabel == "Switch")
						&& mainLabel != "Basic" ) {
						}*/
		}

		bool IsSpecialLabel(string label) {
			return label == "Temperature" || label  == "Luminance" || label == "Relative Humidity" || label == "Switch" || label == "Level" || label == "Sensor";
		}

	};

}
#endif
