/*
 PhoneDetection_Bluetooth_Windows
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include "VIPShared/VIPIncludes.h"
#include "PlutoUtils/MyStl.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "PhoneDetection_Bluetooth_Windows.h"
#include "BD/PhoneDevice.h"
#include <math.h>

#define INQUIRY_TIMEDOUT 30 //seconds

using namespace DCE;

bool PhoneDetection_Bluetooth_Windows::ScanningLoop()
{
	if( m_bAbortScanLoop )
		return false;

	m_bWaitingForInquiry=true;
	if( !StartInquiry() )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to start inquiry");
		Sleep(1000); // Sleep a second before we try again
		m_bWaitingForInquiry=false;
		return true;  // This loop will get called again
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Inquiry started");	
	PLUTO_SAFETY_LOCK(im,m_InquiryMutex);

	timespec abstime;
	abstime.tv_sec = (long) (time(NULL) + INQUIRY_TIMEDOUT);
	abstime.tv_nsec = 0;
	if(ETIMEDOUT == im.TimedCondWait(abstime))
	{
		StopInquiry();
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Inquiry stopped - timed out %d seconds", INQUIRY_TIMEDOUT);
	}

	return true;
} 

void PhoneDetection_Bluetooth_Windows::OnDeviceResponded(BD_ADDR bda,
										DEV_CLASS dev_class,
										BD_NAME bd_name,
										BOOL b_connected)
{
	char name[100];
	strcpy(name,(const char *)bd_name); 

	u_int64_t iMacAddress=0;
	for(int i=0;i<6;++i)
	{
		u_int64_t power = (u_int64_t) pow(256., 5-i);
		int Digit = bda[i];
		iMacAddress += (power * Digit);
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Device %s responded.", bd_name);

	PhoneDevice *pDNew = new PhoneDevice(name,iMacAddress,255);
	AddDeviceToDetectionList(pDNew);
}


void PhoneDetection_Bluetooth_Windows::OnInquiryComplete(BOOL /*success*/, short	/*num_responses*/)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Inquiry complete");
	pthread_cond_broadcast(&m_InquiryCond);
}

void PhoneDetection_Bluetooth_Windows::RequestStopScanning() 
{  
	PhoneDetectionEngine::RequestStopScanning(); 

	pthread_cond_broadcast(&m_InquiryCond); 
	StopInquiry(); 
};

void PhoneDetection_Bluetooth_Windows::SuspendScanning()
{
    PhoneDetectionEngine::SuspendScanning();
	pthread_cond_broadcast(&m_InquiryCond);
}

PhoneDetection_Bluetooth_Windows::~PhoneDetection_Bluetooth_Windows()
{
	//StopScanning() is called in the destructor base, BUT the derived
	//class is destroyed first, and calling RequestStopScanning in
	//virtual method StopScanning will call RequestStopScanning from
	//based class, and we'll never call RequestStopScanning from 
	//the derived class.

	RequestStopScanning();
}