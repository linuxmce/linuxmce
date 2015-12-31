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
/**
 *
 * @file VA_SendMenuToPhone.cpp
 * @brief implementation of the VA_SendMenuToPhone class
 * @author
 *
 */

/**
 *
 * Copyright Notice goes here
 *
 */

#include "VIPShared/VIPIncludes.h"
#include "VA_SendMenuToPhone.h"
#include "VIPShared/VIPMenu.h"
#include "RA/RA_Request.h"
#include "VIPShared/PlutoConfig.h"
#include "VR_PhoneRespondedToRequest.h"
#include "BD/PhoneDevice.h"
#include "VR_IdentifyPhone.h"
#include "PlutoVIP/Customer.h"
#include "VIPShared/BD_CP_ShowVMC.h"

#ifdef VIPESTABLISHMENT
#include "VIPEstablishment/CellPhoneEmulator.h"
#include "RA/RA_Processor.h"
#ifndef BT_SOCKET
#include "BD/BDCommandProcessor.h"
#endif
//#include "PlutoVIP/VIPSocket/VIPUIRequest.h"
#endif

/** @todo - is there a cross platform (linux/symbian/windows) way to dynamically allocate a growable memory block? */
VA_SendMenuToPhone::VA_SendMenuToPhone(string Filename, u_int64_t MacAddress) 
	: RA_Action()
{ 
	m_iMacAddress=MacAddress;

	size_t size;
	m_pdbMenu.m_pBlock = FileUtils::ReadFileIntoBuffer(Filename, size);
	m_pdbMenu.m_dwSize = (unsigned long)size;
}

#ifdef VIPESTABLISHMENT

#include "BD/BDCommandProcessor.h"
#include "BD/BDCommandProcessor.h"
#include "VIPShared/BD_CP_SendMeKeystrokes.h"
#include "VIPShared/BD_CP_ShowVMC.h"
#include "VIPEstablishment/EstablishmentSocket.h"

void *HandleBDCommandProcessorThread(void *p)
{
	BDCommandProcessor *pt = (BDCommandProcessor *)p;
	BDCommand *pCommand = new BD_CP_SendMeKeystrokes(0);
	pt->AddCommand(pCommand);
	while( pt->ReceiveCommand(0,0,NULL) );
	return NULL;
}

void VA_SendMenuToPhone::ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor)
{
	if( !pRequest )
		throw "VA_SendMenuToPhone needs a pointer to the request that originated the action";

	/** We need to get the customer that the request originated from */
	if( pRequest->ID()!=/** @test VRS_IDENTIFY_PHONE*/ VRS_IDENFITY_PHONE )
		throw "We can't forward a request to a phone unless the original request was an identify phone";

	VR_IdentifyPhone *pRA_Request_Original = (VR_IdentifyPhone *) pRequest;

	string sFileName("PlutoVIP.vmc");

	BD_CP_ShowVMC *pVMC = new BD_CP_ShowVMC(
		1, /** store */
		m_pdbMenu.m_dwSize, 
		m_pdbMenu.m_pBlock,
		sFileName.size(),
		const_cast<char *>(sFileName.c_str())

	);

	BDCommandProcessor *pProcessor = pRA_Request_Original->m_pCustomer->GetCommandProcessor(); /** This will create it if it doesn't exist */

	if( !pProcessor && pProcessor->m_bDead )
	{
		delete pProcessor;
		pProcessor = NULL;
	}

	if( !pProcessor )
		return; /** This could be normal, if the phone went out of range or doesn't support RFCOM */

	pProcessor->AddCommand(pVMC);
	pthread_create(&pProcessor->m_BDSockThreadID, NULL, HandleBDCommandProcessorThread, (void*)pProcessor);
}
#else
void VA_SendMenuToPhone::ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor)
{
}
#endif

