/*
 PhoneDetection_Simulate
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "PhoneDetection_Simulate.h"
#include "BD/PhoneDevice.h"

bool PhoneDetection_Simulate::ScanningLoop()
{
	// I just read that sleep causes all the threads to sleep???  But I don't see a pthread_sleep to make
	// just this one sleep
 	Sleep(500);
#ifdef WIN32
	FILE *file = fopen("C:\\Phones.txt","r");
#else
	FILE *file = fopen("/home/Phones.txt","r");
#endif

	if( m_bAbortScanLoop )
		return false;

	m_mapDevicesDetectedThisScan.clear();

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Inquiry started (simulation)");	

	if( file )
	{
		while(true)
		{
			// The file will be in the format:
			// Bluetooth ID \t MacAddress \t LinkQuality \t	IPAddress \t Port \n
			// Where IPAddress & Port is for testing purposes only with socket connection
			char Line[100]="";
			fgets(Line,100,file);
			string sLine(Line);

			string::size_type pos=0;
			string BlueToothID = StringUtils::Tokenize(sLine,"\t",pos);
			string MacAddress = StringUtils::Tokenize(sLine,"\t",pos);
			string LinkQuality = StringUtils::Tokenize(sLine,"\t",pos);
			string IPAddress = StringUtils::Tokenize(sLine,"\t",pos);
			string Port = StringUtils::Tokenize(sLine,"\t",pos);

			if ( MacAddress.length()>10 )
			{
				// See if this is a new device
				PLUTO_SAFETY_LOCK(mm,m_MapMutex);

				PhoneDevice *pDNew = new PhoneDevice(BlueToothID,MacAddress,atoi(LinkQuality.c_str()));

				string sMac = MacAddress;
				vector<string> bda;
				StringUtils::Tokenize(sMac, ":", bda);

				u_int64_t iMacAddress=0;
				for(int i=0;i<6;++i)
				{
					u_int64_t power = (u_int64_t) pow(256., 5-i);
					int Digit = StringUtils::HexByte2Num(const_cast<char *>(bda[i].c_str()));
					iMacAddress += (power * Digit);
				}

				pDNew->m_iMacAddress = iMacAddress;
				AddDeviceToDetectionList(pDNew);
			}
			else
				break;
		}
		fclose(file);
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Inquiry completed (simulation)");

	Sleep(10000);  // Don't this run wild
	return !m_bAbortScanLoop;
}
