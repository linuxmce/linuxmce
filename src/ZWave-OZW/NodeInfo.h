/*
     Copyright (C) 2012 LinuxMCE

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef NodeInfo_h
#define NodeInfo_h

#include <openzwave/Defs.h>
#include <openzwave/value_classes/Value.h>
#include <vector>
#include <list>
using namespace std;

namespace DCE {

	class LMCEDevice;

	class NodeInfo
	{
	public:
		uint32			m_homeId;
		uint8			m_nodeId;
		uint8			m_generic;
		uint8			m_specific;
		bool			m_polled;
		uint32			m_iDeadCount;
		// List of devices that this node is mapped to
		vector<LMCEDevice*>              m_vectDevices;
		// list of values for this node
		list<OpenZWave::ValueID>	m_values;

		NodeInfo();
		~NodeInfo();
		void ClearDeviceMapping();

		void AddDevice(unsigned int deviceNo);
		void MapValueToDevice(unsigned int deviceNo, OpenZWave::ValueID value);
		
		unsigned long GetPKDeviceForValue(OpenZWave::ValueID value);
	};
}

#endif
