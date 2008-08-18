/*
Copyright (C) 2004 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

*/

#include "DP_Catalog.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include "Gen_Devices/AllCommandsRequests.h"
#include "DCE/Command_Impl.h"

#include <iostream>
using namespace std;
using namespace DCE;

DP_Catalog::DP_Catalog( Command_Impl *pCommand_Impl ) 
{
	m_pCommand_Impl=pCommand_Impl;
}

DP_Catalog::~DP_Catalog()
{
}

void DP_Catalog::Register_Data_Provider(int iPK_Device,string sID,string sFormat_Input,string sFormat_Output)
{
	if( sID.size()==0 )
		return; // Shouldn't happen

	// If there's a trailing *, set bWildcard to true and strip it
	bool bWildcard=false;
	if( sID[ sID.size()-1 ] == '*' )
	{
		bWildcard=true;
		sID = sID.substr(0, sID.size()-1);
	}

	// Do we have a list already for this ID?
	ListRegDevice *pListRegDevice;
	map<string, ListRegDevice *>::iterator itMap = m_mapRegisters.find( sID );
	if( itMap==m_mapRegisters.end() )
	{
		pListRegDevice = new ListRegDevice();
		m_mapRegisters[ sID ] = pListRegDevice;
	}
	else
		pListRegDevice = itMap->second;

	// Do we already have this same entry in the list?  Avoid double registration
	for( ListRegDevice::iterator it=pListRegDevice->begin(); it!=pListRegDevice->end(); ++it )
	{
		RegDevice *pRegDevice = *it;
		if( pRegDevice->m_bWildcard==bWildcard &&
			 pRegDevice->m_dwPK_Device==iPK_Device &&
			 pRegDevice->m_sFormatIn==sFormat_Input &&
			 pRegDevice->m_sFormatOut==sFormat_Output )
		{
		    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Data_Provider_Catalog::CMD_Register_Data_Provider duplicate registration from %d of %s", iPK_Device, sID.c_str());
			return;
		}
	}

	RegDevice *pRegDevice = new RegDevice();
	pRegDevice->m_bWildcard=bWildcard;
	pRegDevice->m_dwPK_Device=iPK_Device;
	pRegDevice->m_sFormatIn=sFormat_Input;
	pRegDevice->m_sFormatOut=sFormat_Output;
	pListRegDevice->push_back(pRegDevice);
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Data_Provider_Catalog::CMD_Register_Data_Provider registered from %d of %s", iPK_Device, sID.c_str());
}

bool DP_Catalog::Request_Data_Provider(int PK_Device,string sID,string sParameters,bool bMultiple,string sFormat_Input,string sFormat_Output,string sCache,int *iValue,string *sData_String)
{
	map<string, ListRegDevice *>::iterator itMap = m_mapRegisters.find( sID );
	if( itMap==m_mapRegisters.end() )
	{
	    LoggerWrapper::GetInstance()->Write(LV_WARNING,"DP_Catalog::Request_Data_Provider nothing for %s", sID.c_str());
		return false;
	}

	ListRegDevice *pListRegDevice = itMap->second;

    LoggerWrapper::GetInstance()->Write(LV_STATUS,"DP_Catalog::Request_Data_Provider found %d for %s", (int) pListRegDevice->size(), sID.c_str());

	// TODO: use upper_bound to find wild cards
	for( ListRegDevice::iterator it=pListRegDevice->begin(); it!=pListRegDevice->end(); ++it )
	{
		RegDevice *pRegDevice = *it;

		// TODO: compare input/output format
		DCE::CMD_Request_Data_Provider CMD_Request_Data_Provider(PK_Device, pRegDevice->m_dwPK_Device, sID, sParameters, bMultiple, sFormat_Input, sFormat_Output, sCache, iValue, sData_String);
		string sResponse;
		if( m_pCommand_Impl->SendCommand(CMD_Request_Data_Provider, &sResponse) && sResponse.size()>=2 && sResponse.substr(0,2)=="OK" )
		{
		    LoggerWrapper::GetInstance()->Write(LV_STATUS,"DP_Catalog::Request_Data_Provider got data from %d", pRegDevice->m_dwPK_Device);
			return true;
		}
		else
		    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"DP_Catalog::Request_Data_Provider cannot get data from %d", pRegDevice->m_dwPK_Device);
	}
	return false;
}
