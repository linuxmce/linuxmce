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
#include "VIPShared/VIPIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "VA_SendFileToPhone.h"
#include "VIPShared/VIPMenu.h"
#include "BD/PhoneDevice.h"
#include "RA/RA_Request.h"
#include "VR_PhoneRespondedToRequest.h"
#include "PlutoVIPRequests.h"
#include "VIPShared/PlutoConfig.h"

#include "RA/RA_Processor.h"
#include "DCE/DCEMySqlConfig.h"

#include <iostream>
#include <sstream>
#include <assert.h>


// todo - is there a cross platform (linux/symbian/windows) way to dynamically allocate a growable memory block?
VA_SendFileToPhone::VA_SendFileToPhone(string sPathName, string sFileName,unsigned long long MacAddress) 
	: RA_Action()
{ 
	m_sFileName=sFileName;
	m_iMacAddress=MacAddress;
	size_t s;
	m_pdbFile.m_pBlock = FileUtils::ReadFileIntoBuffer(sPathName + "\\" + m_sFileName.c_str(), s);
	m_pdbFile.m_dwSize = (unsigned long) s;
}

#if defined(VIPESTABLISHMENT) && !defined(BT_SOCKET)

pluto_pthread_mutex_t m_FileSendMutex("FileSend");

void VA_SendFileToPhone::ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor)
{
	// Be sure 2 threads don't both try to do this at the same time
	PLUTO_SAFETY_LOCK(fs,m_FileSendMutex);

	// This class will delete itself when it's finished
	CBtSendFile *ptrBtSendFile = new CBtSendFile( this );

	PhoneDevice p("",m_iMacAddress,0);  // just to get a string
	FILE *f = fopen((g_pPlutoConfig->m_sMenuPath + "\\" + m_sFileName).c_str(),"wb");
	if( f )
	{
		fwrite(m_pdbFile.m_pBlock,m_pdbFile.m_dwSize,1,f);
		fclose(f);

		ptrBtSendFile->SendFile( g_pPlutoConfig->m_sMenuPath + "\\" + m_sFileName, p.m_sMacAddress, CBtSendFile::BY_OBEX_THEN_NONE );
	}
}


void VA_SendFileToPhone::OnSendFileEventReceived(  eSendFileCBEvents e_event_code, long param1, long param2)
{
	/*
	if ( e_event_code == FILE_SEND_PROGRESS )
	{
		m_lCurrent = param1;
		m_lTotal   = param2;
		PostMessage( WM_SEND_PROGRESS );
	}
	if ( e_event_code == FILE_SEND_COMPLETED )
		PostMessage( WM_SEND_COMPLETED );
	else if ( e_event_code == FILE_SEND_ABORTED )
		PostMessage( WM_SEND_ABORTED );
	else if ( e_event_code == FILE_SEND_FAILED )
		PostMessage( WM_SEND_FAILED );
*/
}

#else
void VA_SendFileToPhone::ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor)
{
}
#endif

