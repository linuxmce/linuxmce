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
// OCDeviceTemplate.cpp : Defines the entry point for the console application.
//

// Hack this in for this test app since we're not using VIPIncludes
#include <iostream>

#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "Message.h"
#include "../VIPShared/MyStl.h"
#include "OCDevice.h"
#include "RA/RA_Request.h"
#include "../VIPShared/VR_RequestPayment.h"
#include "../VIPShared/VR_IdentifyPhone.h"

#include "../VIPShared/VA_ForwardRequestToPhone.h"
#include "RA/RA_Processor.h"
#include "../VIPShared/VIPConfig.h"
#include "../VIPShared/VR_RequestSecureTransaction.h"

#include "../VIP_POS/VIP_POS.h"

#include "CellPhoneEmulator.h"

class VIPConfig *gp_VIPConfig;


unsigned long Vip_Callback_Payment_Requested(long EstablishmentId, long UsersPlutoId, LPTSTR InvoiceNumber, 
		LPTSTR Description, LPTSTR CashierName,unsigned char DynamicInvoice)
{
	return 2;
}

unsigned long Vip_Callback_Add_Lineitem_To_Payment(long TransactionId, unsigned char ClearFirst, 
		unsigned char Finalized, LPTSTR ProductID, LPTSTR Description, long Quantity, long Amount, long Total)
{
	return 2;
}


//HINSTANCE m_hinstPlutoVIPLib; 
//VIP_PLUTO_MEMBER_IDENTIFIED m_cbVIP_PlutoMemberIdentified; 
//VIP_REGISTER_CALLBACK m_cbVIP_Register_Callback; 
//BOOL m_bRunTimeLinkSuccess;


#ifdef WIN32
long _tmain(long argc, _TCHAR* argv[])
{
	long err;
	WSADATA wsaData;
	err = WSAStartup(MAKEWORD( 1, 1 ),(LPWSADATA)  &wsaData);
#else
long main(long argc, char *argv[])
{
#endif

	string ServerAddress=(argc==1 ? "vs.plutovip.com:3460" : argv[1]);
	ServerAddress="localhost:3460";
	long DeviceID=10000;

	Logger* Logger = CreateLogger(argc,argv);
	if(Logger == NULL)
	{
		fprintf(stderr,"Problem creating logger.  Check params.\n");
		exit(1);
	}

	string s, Cmd, page;
	string::size_type pos = 0;

	VIPConfig pConfig;
	RA_Processor rp(&pConfig,990,2);
	pConfig.m_pcRA_Processor=&rp;

//    m_hinstPlutoVIPLib = LoadLibrary("C:\\ha\\level4\\VIP_POS.dll"); 
//    m_bRunTimeLinkSuccess = FALSE;

    // If the handle is valid, try to get the function address.
//    if (m_hinstPlutoVIPLib != NULL) 
//    { 
//        m_cbVIP_Register_Callback = (VIP_REGISTER_CALLBACK) GetProcAddress(m_hinstPlutoVIPLib, "fnVIP_Register_Callback"); 
//        m_cbVIP_PlutoMemberIdentified = (VIP_PLUTO_MEMBER_IDENTIFIED) GetProcAddress(m_hinstPlutoVIPLib, "fnVIP_PlutoMemberIdentified"); 
 
        // If the function address is valid, call the function.
//        if (m_cbVIP_Register_Callback != NULL) 
//		{
    fnVIP_Register_Callback(Vip_Callback_Payment_Requested,Vip_Callback_Add_Lineitem_To_Payment);
	fnVIP_PlutoMemberIdentified(12,"185","John","Doe",
		"Nickname","Address","City","State","Zip","Country","Gender","Birthdate",
		"Email","Comments","LocalPathToPic");
//		}
//    } 

	VR_RequestSecureTransaction *pRST = new VR_RequestSecureTransaction(&pConfig,14,12);
	pRST->ConvertRequestToBinary();
	CellPhoneEmulator ce(&pConfig,Logger);
	long ResponseSize;
	const char *Response=ce.ReceiveRequest(pRST->RequestSize(),pRST->Request(),&ResponseSize);
	pRST->StoreResponse(ResponseSize,Response); 

	class VR_RequestPayment *pRA_RequestPayment = new VR_RequestPayment(&pConfig,1250,"938-82","Misc Videos","Mrs. John Doe",14,12,pRST);

	pRA_RequestPayment->m_listInvoiceDetail.push_back(new InvoiceDetail("l1","p283","The Matrix",2818));
	pRA_RequestPayment->m_listInvoiceDetail.push_back(new InvoiceDetail("l2","p383","The Matrix 2",2920));
	pRA_RequestPayment->m_listInvoiceDetail.push_back(new InvoiceDetail("l3","p483","The Matrix 3",3050));
	rp.AddRequest(pRA_RequestPayment);
	while( rp.SendRequests(ServerAddress) );


	//53528182340 = mac address 000C76873E44
//	VR_IdentifyPhone *pRA_Request = new VR_IdentifyPhone(&pConfig,1234,"PV00000C168",(u_int64_t) 0x000C76873E44,-1,-1,12,"168");
//	cout << "Sending request to identify phone with Mac " << pRA_Request->m_iMacAddress << " Identified ID " << pRA_Request->m_iIdentifiedPlutoId << "\n";
//	rp.AddRequest(pRA_Request);
//	while( rp.SendRequests(ServerAddress) );

//	cout << "Response was " << (int) pRA_Request->m_cProcessOutcome << "\n";
//	cout << "Name " << pRA_Request->m_sNickname << " " << pRA_Request->m_sFirstName << " email " << pRA_Request->m_sEmail << " pic size: " << pRA_Request->m_iImageSize << "\n";

	delete Logger;

    Sleep(50);	
#ifdef _WIN32
	WSACleanup();
#endif

	return 0;
}

