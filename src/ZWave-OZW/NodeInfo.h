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
	class NodeInfo
	{
	public:
		uint32			m_homeId;
		uint8			m_nodeId;
		uint8			m_generic;
		uint8			m_specific;
		bool			m_polled;
		list<OpenZWave::ValueID>	m_values;
	};
}

#endif
