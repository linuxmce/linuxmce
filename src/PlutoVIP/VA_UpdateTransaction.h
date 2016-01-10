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
#ifndef VA_UpdateTransaction_H
#define VA_UpdateTransaction_H

#include "RA/RA_Action.h"
#include "PlutoVIPRequests.h"

class VA_UpdateTransaction : public RA_Action
{
public:
	unsigned long m_iTransNumber,m_iStatus;
	string m_sMessage;

	VA_UpdateTransaction(unsigned long TransNumber,unsigned long Status,string Message);
	VA_UpdateTransaction() {}

	virtual uint32_t ID() { return ACTION_UPDATETRANSACTION; }
	virtual void SetupSerialization(int iSC_Version)
	{
		RA_Action::SetupSerialization(iSC_Version);
		StartSerializeList() + m_iTransNumber + m_iStatus
			+ m_sMessage;
	}

    virtual void ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor);
};


#endif

