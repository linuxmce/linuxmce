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
#ifndef VA_ShowMenu_H
#define VA_ShowMenu_H

#include "RA/RA_Action.h"
#include "PlutoVIPRequests.h"

class VA_SendMenuToPhone : public RA_Action
{
public:
	PlutoDataBlock m_pdbMenu;
	unsigned long long m_iMacAddress;

	VA_SendMenuToPhone(string Filename, u_int64_t MacAddress);
	VA_SendMenuToPhone() { m_pdbMenu.m_pBlock = NULL; m_pdbMenu.m_dwSize = 0;}

	virtual uint32_t ID() { return ACTION_SENDMENU_TOPHONE; }
	virtual void SetupSerialization(int iSC_Version)
	{
		RA_Action::SetupSerialization(iSC_Version);
		StartSerializeList() + m_pdbMenu + m_iMacAddress;
	}

    virtual void ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor);
};


#endif

