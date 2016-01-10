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
#include "VR_RequestSecureTransaction.h"
#include "VIPShared/PlutoConfig.h"
#include "VIPShared/VIPMenu.h"
#include "RA/RA_Processor.h"
#include "DCE/DCEMySqlConfig.h"

#include <assert.h>

VR_RequestSecureTransaction::VR_RequestSecureTransaction(unsigned long FKID_PlutoId_Establishment,unsigned long FKID_PlutoId_User) 
	: RA_Request()
{
	m_FKID_PlutoId_Establishment=FKID_PlutoId_Establishment;
	m_FKID_PlutoId_User=FKID_PlutoId_User;
}

bool VR_RequestSecureTransaction::ProcessRequest(RA_Processor *pRA_Processor)
{
	//DCEMySqlConfig *pDCEMySqlConfig = dynamic_cast<DCEMySqlConfig *>(pRA_Processor->m_pRA_Config);
	//assert(NULL != pDCEMySqlConfig);

	m_lMacAddress = 1234567890;
	m_iConfirmEstablishmentId = m_FKID_PlutoId_Establishment;
	m_pdbKey.m_dwSize = 100;
	m_pdbKey.m_pBlock = new char[101];
	strcpy(m_pdbKey.m_pBlock,"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

	return true;
}
