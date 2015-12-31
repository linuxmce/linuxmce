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
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "VR_PhoneInstalledBinary.h"
#include "VA_UpdateTransaction.h"
#include "VIPShared/VIPMenu.h"

#include "RA/RA_Processor.h"
#include "DCE/DCEMySqlConfig.h"

#include <assert.h>
#include <iostream>
#include <sstream>

VR_PhoneInstalledBinary::VR_PhoneInstalledBinary(unsigned long long MacAddress,unsigned long Revision)
	: RA_Request()
{
	m_iMacAddress=MacAddress;
	m_iRevision=Revision;
}


bool VR_PhoneInstalledBinary::ProcessRequest(RA_Processor *pRA_Processor)
{
#ifdef VIPSERVER

	DCEMySqlConfig *pDCEMySqlConfig = dynamic_cast<DCEMySqlConfig *>(pRA_Processor->m_pRA_Config);
	assert(NULL != pDCEMySqlConfig);

	std::ostringstream s;
	s << "UPDATE MacAddress SET Revision=" << m_iRevision << " WHERE PKID_MacAddress=" << m_iMacAddress;

	//TODO ... ?!

#endif
	return true;
}
