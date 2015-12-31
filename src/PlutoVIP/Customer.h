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
#ifndef Customer_h
#define Customer_h

#define APPBAR_NEW		1
#define APPBAR_ACTIVE	2
#define APPBAR_RECENT	3

#include "ImageBaseDefs.h"

class Customer
{
public:
	class PhoneDevice *m_pPhoneDevice;
	class BDCommandProcessor *m_pBDCommandProcessor;
	
	// Response Variables from the server
	unsigned long m_iRecordVersion,m_iPlutoId;
	class CPicture *m_pPicture;
	string m_sEmail,m_sNickname,m_sFirstName,m_sLastName;
	string m_sAddress,m_sCity,m_sState,m_sZip,m_sCountry;
	string m_sGender,m_sBirthdate;
	string m_sComments,m_sEstablishmentCustomerId;
	MYSTL_CREATE_LIST(m_listPhoneNumbers,VIPPhoneNumber);
	MYSTL_CREATE_LIST(m_listAttributes,MiscVIPAttribute);

	
	// The customer's information from the local database
	time_t m_tLastVisit;
	long	m_iLoyaltyVisits, m_iTotalVisit, m_iAvailableVisit;
	double	m_iLoyaltyPurchases, m_iTotalPurchase, m_iAvailablePurchase;
	bool m_bDetected;

	char m_cAppBar;  // 0=not detected anymore, 1=new app bar, 2=active, 3=recent
	
	class PhoneDetection_VIP *m_pPhoneDetection_VIP;

	Customer(class PhoneDetection_VIP *pDetection)
	{
		m_pPhoneDetection_VIP = pDetection;
		m_pPhoneDevice = NULL;
		m_pBDCommandProcessor = NULL;
		m_bDetected=true;
		m_pPicture=NULL;
		m_cAppBar=IMAGE_TYPE_NEW;
		m_iAvailableVisit=0;
		m_iAvailablePurchase=0;
		m_iTotalVisit=0;
		m_iTotalPurchase=0;
		m_iRecordVersion=0;
	}

	class BDCommandProcessor *GetCommandProcessor();
};

#endif
