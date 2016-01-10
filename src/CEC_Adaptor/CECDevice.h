/*
     Copyright (C) 2014 LinuxMCE

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef CECDevice_h
#define CECDevice_h

#include <cec.h>

using namespace CEC;

namespace DCE {

	class CECDevice
	{
	public:
		uint16_t iPhysicalAddress;
		cec_logical_addresses LogicalAddresses;
		uint64_t iVendorId;
		bool     bActive;
		cec_version iCecVersion;
		cec_power_status power;
		cec_osd_name osdName;
		string sAddr;
		cec_menu_language lang;

		int     iPK_Device;

	// FIXME: use CONSTs to fill these with proper data.
		CECDevice(void) {
			LogicalAddresses.Clear();
			iPhysicalAddress = 0;;
			iPK_Device = 0;
			iVendorId = 0;
			bActive = false;
			iCecVersion = (cec_version) 0;
			power = (cec_power_status) 0;
			sAddr = "";
			//lang = CECDEVICE_UNKNOWN;
/*
//iVendorId = parser->GetDeviceVendorId((cec_logical_address)iPtr);
//iPhysicalAddress = parser->GetDevicePhysicalAddress((cec_logical_address)iPtr);
//bActive = parser->IsActiveSource((cec_logical_address)iPtr);
//iCecVersion = parser->GetDeviceCecVersion((cec_logical_address)iPtr);
//power = parser->GetDevicePowerStatus((cec_logical_address)iPtr);
//osdName = parser->GetDeviceOSDName((cec_logical_address)iPtr);
//strAddr.Format("%x.%x.%x.%x", (iPhysicalAddress >> 12) & 0xF, (iPhysicalAddress >> 8) & 0xF, (iPhysicalAddress >> 4) & 0xF, iPhysicalAddress & 0xF);
//lang.device = CECDEVICE_UNKNOWN;
//parser->GetDeviceMenuLanguage((cec_logical_address)iPtr, &lang);
*/
		}
		~CECDevice() {
		}

	};

}
#endif
