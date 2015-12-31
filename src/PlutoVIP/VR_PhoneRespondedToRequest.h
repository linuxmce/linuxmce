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
#ifndef VR_PhoneRespondedToRequest_H
#define VR_PhoneRespondedToRequest_H

#include "RA/RA_Request.h"
#include "PlutoVIPRequests.h"

class VR_PhoneRespondedToRequest : public RA_Request
{
	// Request Variables
	unsigned long m_iRequestID;
	PlutoDataBlock m_pdbOriginalRequest,m_pdbOriginalResponse;

	// Response Variables

public:
	// The establishment will call this constructor, then ConvertRequestToBinary
	VR_PhoneRespondedToRequest(unsigned long RequestID,unsigned long RequestSize,const char *RequestData,unsigned long ResponseSize,const char *ResponseData);
	VR_PhoneRespondedToRequest() {}

	virtual uint32_t ID() { return VRS_PHONE_RESPONDED; }

	virtual void SetupSerialization_Request()
	{
		RA_Request::SetupSerialization_Request();
		StartSerializeList() + m_iRequestID + m_pdbOriginalRequest + m_pdbOriginalResponse;
	}
	virtual bool ProcessRequest(class RA_Processor *pRA_Processor);
};


#endif

