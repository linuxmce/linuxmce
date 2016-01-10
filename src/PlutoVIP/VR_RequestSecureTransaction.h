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
#ifndef VR_RequestSecureTransaction_H
#define VR_RequestSecureTransaction_H

#include "RA/RA_Request.h"
#include "PlutoVIPRequests.h"

class VR_RequestSecureTransaction : public RA_Request
{
	// Request Variables
	unsigned long m_FKID_PlutoId_Establishment,m_FKID_PlutoId_User;

	// Response Variables
	unsigned long long m_lMacAddress;
	unsigned long m_iConfirmEstablishmentId;
	PlutoDataBlock m_pdbKey;

public:
	// The establishment will call this constructor, then ConvertRequestToBinary
	VR_RequestSecureTransaction(unsigned long FKID_PlutoId_Establishment,unsigned long FKID_PlutoId_User);
	VR_RequestSecureTransaction() {};

	virtual uint32_t ID() { return VRP_REQUEST_SEC_TRANS; }
	
	virtual void SetupSerialization_Request()
	{
		RA_Request::SetupSerialization_Request();
		StartSerializeList() + m_FKID_PlutoId_Establishment + m_FKID_PlutoId_User;
	}
	virtual void SetupSerialization_Response()
	{
		RA_Request::SetupSerialization_Response();
		StartSerializeList() + m_lMacAddress + m_iConfirmEstablishmentId + m_pdbKey;
	}
	virtual bool ProcessRequest(class RA_Processor *pRA_Processor);
};


#endif

