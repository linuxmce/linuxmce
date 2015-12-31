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
#include "PlutoVIPRequests.h"
#include "RA/RA_Processor.h"
#include "VR_RequestSecureTransaction.h"
#include "VR_RequestPayment.h"
#include "VR_PhoneRespondedToRequest.h"
#include "VR_PhoneInstalledBinary.h"
#include "VR_ManuallyMatchPhone.h"
#include "VR_IdentifyPhone.h"

#include "VA_UpdateTransaction.h"
#include "VA_SendFileToPhone.h"
#include "VA_SendMenuToPhone.h"

RA_Request *RA_Processor::BuildRequestFromData(uint32_t dwSize, const char *pcData, uint32_t dwRequestID)
{
	RA_Request *pRequest=NULL;
	switch( dwRequestID )
	{
/*
	case VRP_REQUEST_SHOW_MENU:
		pRequest=new VR_ShowMenu();
		pRequest->CreateRequest(dwSize,pcData);
		return pRequest;
*/
	case VRP_REQUEST_SEC_TRANS:
		pRequest=new VR_RequestSecureTransaction();
		pRequest->CreateRequest(dwSize,pcData);
		return pRequest;
	case VRS_REQUEST_PAYMENT:
		pRequest=new VR_RequestPayment();
		pRequest->CreateRequest(dwSize,pcData);
		return pRequest;
	case VRS_PHONE_RESPONDED:
		pRequest=new VR_PhoneRespondedToRequest();
		pRequest->CreateRequest(dwSize,pcData);
		return pRequest;
	case VRS_PHONE_INSTALLED_BIN:
		pRequest=new VR_PhoneInstalledBinary();
		pRequest->CreateRequest(dwSize,pcData);
		return pRequest;
	case VRS_MANUALLY_MATCH_PHONE:
		pRequest=new VR_ManuallyMatchPhone();
		pRequest->CreateRequest(dwSize,pcData);
		return pRequest;
	case VRS_IDENFITY_PHONE:
		pRequest=new VR_IdentifyPhone();
		pRequest->CreateRequest(dwSize,pcData);
		return pRequest;
	};

	return pRequest;
}

RA_Action *RA_Processor::BuildActionFromData( uint32_t dwSize, const char *pcData, uint32_t dwActionID)
{
	RA_Action *pRA_Action=NULL;
	switch( dwActionID )
	{
	case ACTION_UPDATETRANSACTION:
		pRA_Action=new VA_UpdateTransaction();
		pRA_Action->CreateAction(dwSize,pcData);
		return pRA_Action;
	case ACTION_SENDFILE_TOPHONE:
		pRA_Action=new VA_SendFileToPhone();
		pRA_Action->CreateAction(dwSize,pcData);
		return pRA_Action;
	case ACTION_SENDMENU_TOPHONE:
		pRA_Action=new VA_SendMenuToPhone();
		pRA_Action->CreateAction(dwSize,pcData);
		return pRA_Action;
	}
	return pRA_Action;
}

RA_Processor *RA_Processor::CreateRA_Processor(class RAServerSocket *pRAServerSocket, class RA_Config *pRA_Config/* = NULL*/)
{
	return new RA_Processor(0, 1, pRA_Config);
}

