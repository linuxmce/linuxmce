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
#ifndef VA_SendFileToPhone_H
#define VA_SendFileToPhone_H

#include "RA/RA_Action.h"
#include "PlutoVIPRequests.h"

#if defined(VIPESTABLISHMENT) && !defined(BT_SOCKET)
#include "VIPShared/BtSendFile.h"
#endif

class VA_SendFileToPhone : public RA_Action
#if defined(VIPESTABLISHMENT) && !defined(BT_SOCKET)
	,CBtSendFileCallback
#endif
{
public:
	PlutoDataBlock m_pdbFile;
	string m_sFileName;
	unsigned long long m_iMacAddress;

	// For an incoming menu
	VA_SendFileToPhone(string sPathName, string sFileName,unsigned long long MacAddress);
	VA_SendFileToPhone() {}

	virtual uint32_t ID() { return ACTION_SENDFILE_TOPHONE; }
	virtual void SetupSerialization(int iSC_Version)
	{
		RA_Action::SetupSerialization(iSC_Version);
		StartSerializeList() + m_pdbFile + m_sFileName
			+ m_iMacAddress;
	}

    virtual void ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor);

#if defined(VIPESTABLISHMENT) && !defined(BT_SOCKET)
	void OnSendFileEventReceived(  eSendFileCBEvents e_event_code, long param1, long param2);
#endif
};


#endif

