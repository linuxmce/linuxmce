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
		cec_logical_address iLogical_Address;
		uint64_t iVendorId;
		uint16_t iPhysical_Address;
		bool     bActive;
		cec_version iCecVersion;
		cec_power_status power;
		cec_osd_name osdName;
		string strAddr;
		cec_menu_language lang;

		uint64_t iPK_Device;

		CECDevice(void) {
			iPK_Device = 0;
			iLogical_Address = (cec_logical_address) 0;
			iVendorId = 0;
			iPhysical_Address = 0;
			bActive = false;
			iCecVersion = (cec_version) 0;
			power = (cec_power_status) 0;
			strAddr = "";
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
