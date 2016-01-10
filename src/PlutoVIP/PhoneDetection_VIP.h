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
#ifndef PhoneDetection_VIP_h
#define PhoneDetection_VIP_h

#include "VIPShared/PhoneDetection_Bluetooth_Windows.h"
#include "VIPShared/PhoneDetection_Simulate.h"
#include "BD/PhoneDevice.h"
#include "PlutoUtils/CommonIncludes.h"
#include "VR_IdentifyPhone.h"

#include "AppBar.h"
#include "Customer.h"

extern map<unsigned long, Customer*> m_mapCustomers;

class PhoneDetection_VIP : 
#ifdef BT_SOCKET	
	public PhoneDetection_Simulate
#else
	public PhoneDetection_Bluetooth_Windows
#endif
{
public:
	void AddCustomerImageToImageList(unsigned long Id, CString FileName);
	CString SaveCustomerImage(VR_IdentifyPhone* pIP);
	class pluto_pthread_mutex_t *m_pThreadMutex;
	pthread_mutexattr_t m_MutexAttr;
	
	//map<unsigned long, Customer*> m_mapCustomers;
	CAppBar* m_pNewAB;
	CAppBar* m_pActiveAB;
	CAppBar* m_pRecentAB;

	PhoneDetection_VIP();
	virtual ~PhoneDetection_VIP() { m_mapCustomers.clear(); }

	void AddCustomer(Customer *pCustomer);

	// The derived class must decide how to handle when new devices are detected or lost, or 
	// when the signal strength changes by more than 10 in either direction
	virtual void NewDeviceDetected(PhoneDevice *pDevice);
	virtual void LostDevice(PhoneDevice *pDevice);
	virtual void SignalStrengthChanged(PhoneDevice *pDevice);

	void SetAppBars(CAppBar* pNewAB,
					CAppBar* pActiveAB,
					CAppBar* pRecentAB);
	
	void SetupAppBars();
};

#endif
