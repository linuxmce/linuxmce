/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef PhoneDetection_Bluetooth_Linux_h
#define PhoneDetection_Bluetooth_Linux_h

#include "PhoneDetectionEngine.h"
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>


#ifndef EVT_INQUIRY_RESULT_WITH_RSSI
#define EVT_INQUIRY_RESULT_WITH_RSSI	0x22
typedef struct {
	bdaddr_t	bdaddr;
	uint8_t		pscan_rep_mode;
	uint8_t		pscan_period_mode;
	uint8_t		dev_class[3];
	uint16_t	clock_offset;
	uint8_t		rssi;
} __attribute__ ((packed)) inquiry_info_with_rssi;
#define INQUIRY_INFO_WITH_RSSI_SIZE 14
#else
#undef  EVT_INQUIRY_RESULT_WITH_RSSI
#define EVT_INQUIRY_RESULT_WITH_RSSI	0x22
#endif

class PhoneDetection_Bluetooth_Linux : public PhoneDetectionEngine
{
	struct hci_dev_info m_DevInfo;
	string m_sDongleMacAddress;

public:
	PhoneDetection_Bluetooth_Linux();
    virtual ~PhoneDetection_Bluetooth_Linux() { StopScanning(); }

    virtual int GetLinkQuality(const char *addr);	   

	virtual string MacAddress() { return "";}

protected:
	// This function needs to be overridden 
	virtual bool ScanningLoop();
};
#endif

