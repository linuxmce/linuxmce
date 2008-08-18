/*
Copyright (C) 2004 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

*/

#include "Data_Provider_Factory.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCE/Command_Impl.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "Gen_Devices/AllCommandsRequests.h"

#include <iostream>
using namespace std;
using namespace DCE;

Data_Provider_Factory::Data_Provider_Factory( Command_Impl *pCommand_Impl ) 
	: m_DataFactoryMutex( "Data_Provider_Factory" )
{
	m_pCommand_Impl=pCommand_Impl;
	m_DataFactoryMutex.Init( NULL );
}

Data_Provider_Factory::~Data_Provider_Factory()
{
	for(map<string,ListData_Provider_Source_Info *>::iterator it=m_mapListData_Provider_Source_Info.begin();it!=m_mapListData_Provider_Source_Info.end();++it)
		delete it->second;
	m_mapListData_Provider_Source_Info.clear();
}

int Data_Provider_Factory::Register(Data_Provider_Source_Instantantiate _Data_Provider_Source_Instantantiate,
								string sID, string sFormatIn, string sFormatOut,
								Data_Provider_InstantiationInfo *pData_Provider_InstantiationInfo,
								Data_Provider_Source_Info::enumCanCache _enumCanCache,bool bSpecificToRequestor)
{
	PLUTO_SAFETY_LOCK(fm,m_DataFactoryMutex);

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Data_Provider_Factory::Register %s", sID.c_str());

	if( sID.size()==0 )
		return 0; // Shouldn't happen

	// If there's a trailing *, set bWildcard to true and strip it
	bool bWildcard=false;
	if( sID[ sID.size()-1 ] == '*' )
	{
		bWildcard=true;
		sID = sID.substr(0, sID.size()-1);
	}

	// Do we have a list already for this ID?
	ListData_Provider_Source_Info *pListData_Provider_Source_Info;
	map<string,ListData_Provider_Source_Info *>::iterator itMap = m_mapListData_Provider_Source_Info.find( sID );
	if( itMap==m_mapListData_Provider_Source_Info.end() )
	{
		pListData_Provider_Source_Info = new ListData_Provider_Source_Info();
		m_mapListData_Provider_Source_Info[ sID ] = pListData_Provider_Source_Info;
	}
	else
		pListData_Provider_Source_Info = itMap->second;

	Data_Provider_Source_Info *pData_Provider_Source_Info = new Data_Provider_Source_Info(_Data_Provider_Source_Instantantiate,
		sID, sFormatIn, sFormatOut, pData_Provider_InstantiationInfo,_enumCanCache,bSpecificToRequestor,bWildcard);

	pListData_Provider_Source_Info->push_back( pData_Provider_Source_Info );
	return 0;
}

void Data_Provider_Factory::RegisterCatalog()
{
	for(map<string,ListData_Provider_Source_Info *>::iterator itMap = m_mapListData_Provider_Source_Info.begin(); itMap!=m_mapListData_Provider_Source_Info.end(); ++itMap)
	{
		ListData_Provider_Source_Info *pListData_Provider_Source_Info = itMap->second;
		for(ListData_Provider_Source_Info::iterator itList = pListData_Provider_Source_Info->begin(); itList != pListData_Provider_Source_Info->end(); ++itList)
		{
			Data_Provider_Source_Info *pData_Provider_Source_Info = *itList;
			DCE::CMD_Register_Data_Provider_Cat CMD_Register_Data_Provider_Cat(m_pCommand_Impl->m_dwPK_Device, DEVICECATEGORY_Data_Provider_Catalog_Plugins_CONST, true, BL_SameHouse, 
				m_pCommand_Impl->m_dwPK_Device, pData_Provider_Source_Info->m_sID, pData_Provider_Source_Info->m_sFormatIn, pData_Provider_Source_Info->m_sFormatOut);
			m_pCommand_Impl->SendCommand( CMD_Register_Data_Provider_Cat );
		}
	}
}

bool Data_Provider_Factory::Populate(int PK_Device,string sID,string sParameters,bool bMultiple,string sFormat_Input,string sFormat_Output,string sCache,int *iValue,string *sData_String)
{
	PLUTO_SAFETY_LOCK(fm,m_DataFactoryMutex);

	// handle all the wildcards

	ListData_Provider_Source_Info *pListData_Provider_Source_Info;
	map<string,ListData_Provider_Source_Info *>::iterator itMap = m_mapListData_Provider_Source_Info.find( sID );
	if( itMap==m_mapListData_Provider_Source_Info.end() )
		return false;

	pListData_Provider_Source_Info = itMap->second;

	for(ListData_Provider_Source_Info::iterator it=pListData_Provider_Source_Info->begin();it!=pListData_Provider_Source_Info->end();++it)
	{
		Data_Provider_Source_Info *pData_Provider_Source_Info = *it;
		Data_Provider_Source_Base *pData_Provider_Source_Base = pData_Provider_Source_Info->Data_Provider_Source_Base_get(PK_Device);
		if( pData_Provider_Source_Base )
		{
			if( pData_Provider_Source_Base->Populate(PK_Device,sID,sParameters,bMultiple,sFormat_Input,sFormat_Output,sCache,iValue,sData_String) )
				return true;
		}
	}

	return false;
}
