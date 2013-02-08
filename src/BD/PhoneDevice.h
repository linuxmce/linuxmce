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
#ifndef PhoneDevice_h
#define PhoneDevice_h

#if !defined(WIN32) && !defined(NOBLUETOOTH) && !defined(__APPLE_CC__)
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#endif

#include <math.h>

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"


class PhoneDevice
{

public:
	string m_sID,m_sMacAddress,m_UserVersion;
	unsigned long m_PKID_Users;
	u_int64_t m_iMacAddress;
	bool m_bIsConnected;
	time_t m_tFirstDetected,m_tLastDetected;
	int m_iLinkQuality;

#if !defined(WIN32) && !defined(NOBLUETOOTH) && !defined(__APPLE_CC__)
	bdaddr_t m_bdaddrDongle;
#endif
	
	// These are just for using the Socket communication & the tester
	string m_sIPAddress;
	int m_iPort;

	PhoneDevice(string ID,string MacAddress,int LinkQuality)
	{
		m_PKID_Users=0;
		m_iLinkQuality=LinkQuality;
		m_sID=ID;
		m_sMacAddress=MacAddress;
		m_bIsConnected=false;
		string::size_type pos=0;
		m_iMacAddress=0;
		for(int i=5;i>=0;--i)
		{
			string sHex = StringUtils::Tokenize(m_sMacAddress,":",pos);
			int hex = 0;
			sscanf(sHex.c_str(),"%x",&hex);
			u_int64_t power = (u_int64_t) pow( (long double) 256, i);
			m_iMacAddress += (power * hex);
		}
		m_tFirstDetected=m_tLastDetected=time(NULL);
		m_sMacAddress=StringUtils::ToUpper(m_sMacAddress);
		CheckForPlutoID();
	}
	PhoneDevice(string ID,u_int64_t MacAddress,int LinkQuality)
	{
		m_PKID_Users=0;
		m_iLinkQuality=LinkQuality;
		m_sID=ID;
		m_iMacAddress=MacAddress;
		m_bIsConnected=false;
		m_tFirstDetected=m_tLastDetected=time(NULL);
		for(int i=5;i>=0;--i)
		{
			u_int64_t power = (u_int64_t) pow((long double) 256, i);
			int hex = (int) (MacAddress / power);
			MacAddress -= (hex * power);
			char sHex[5];
			snprintf(sHex, 5, "%02x", hex);
			m_sMacAddress += sHex;
			if( i!=0 )
				m_sMacAddress += ":";
			
		}
		m_sMacAddress=StringUtils::ToUpper(m_sMacAddress);
		CheckForPlutoID();
	}

	PhoneDevice(const PhoneDevice &aDevice)
	{
		m_sID = aDevice.m_sID;
		m_sMacAddress = aDevice.m_sMacAddress;
		m_UserVersion = aDevice.m_UserVersion;
		m_PKID_Users = aDevice.m_PKID_Users;
		m_iMacAddress = aDevice.m_iMacAddress;
		m_bIsConnected = aDevice.m_bIsConnected;
		m_tFirstDetected = aDevice.m_tFirstDetected;
		m_tLastDetected = aDevice.m_tLastDetected;
		m_iLinkQuality = aDevice.m_iLinkQuality;

#if !defined(WIN32) && !defined(NOBLUETOOTH) && !defined(__APPLE_CC__)
		m_bdaddrDongle = aDevice.m_bdaddrDongle;
#endif

		// These are just for using the Socket communication & the tester
		m_sIPAddress = aDevice.m_sIPAddress;
		m_iPort = aDevice.m_iPort;
	}

	void CheckForPlutoID()
	{
		m_sID = StringUtils::ToUpper(m_sID);
		if( m_sID.substr(0,2)=="PV" )
		{
			m_PKID_Users=0;
			for(int i=0;i<=5;++i)
			{
				char Digit = m_sID[i+2];
				unsigned long Value = 0;
				if( Digit<='9' )
					Value = Digit-48;
				else if( Digit<'O' )
					Value = Digit-55; // (A=65)
				else if( Digit>'O' )
					Value = Digit-56; // Skip 'O', which we treat as 0

				unsigned long p = (unsigned long) pow((double) 35,(double)5-i);
				m_PKID_Users += p * Value;
			}
			m_UserVersion = m_sID.substr(8,3);
		}
	}
};


#endif
