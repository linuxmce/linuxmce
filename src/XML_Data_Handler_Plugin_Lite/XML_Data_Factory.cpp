/*
Copyright (C) 2004 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

*/

#include "XML_Data_Factory.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

XML_Data_Factory::XML_Data_Factory( Command_Impl *pCommand_Impl ) 
	: m_DataFactoryMutex( "XML_Data_Factory" )
{
	m_pCommand_Impl=pCommand_Impl;
	m_DataFactoryMutex.Init( NULL );
}

XML_Data_Factory::~XML_Data_Factory()
{
	for(map<string,XML_Data_Source_Info *>::iterator it=m_mapXML_Data_Source_Info.begin();it!=m_mapXML_Data_Source_Info.end();++it)
		delete it->second;
	m_mapXML_Data_Source_Info.clear();
}

int XML_Data_Factory::RegisterForeignFactory(const char *szDataID,int PK_Device)
{
	PLUTO_SAFETY_LOCK(fm,m_DataFactoryMutex);
	map<string,int>::iterator it=m_mapForeignFactories.find(szDataID);
	if( it!=m_mapForeignFactories.end() )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XML_Data_Factory::RegisterForeignFactory ID %s already exists", szDataID);
		return -1;
	}

	m_mapForeignFactories[szDataID]=PK_Device;
	return 0;
}

int XML_Data_Factory::Register(XML_Data_Source_Instantantiate _XML_Data_Source_Instantantiate,
							   XML_Data_Source_DataID _XML_Data_Source_DataID,
							   XML_Data_InstantiationInfo *pXML_Data_InstantiationInfo,
							   XML_Data_Source_Info::enumCanCache _enumCanCache,bool bSpecificToRequestor)
{
	PLUTO_SAFETY_LOCK(fm,m_DataFactoryMutex);

	string sDataID = (*_XML_Data_Source_DataID)();
	map<string,XML_Data_Source_Info *>::iterator it=m_mapXML_Data_Source_Info.find(sDataID);
	if( it!=m_mapXML_Data_Source_Info.end() )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XML_Data_Factory::RegisterForeignFactory ID %s already exists", *_XML_Data_Source_DataID());
		return -1;
	}

	XML_Data_Source_Info *pXML_Data_Source_Info = new XML_Data_Source_Info(_XML_Data_Source_Instantantiate,
		_XML_Data_Source_DataID,pXML_Data_InstantiationInfo,_enumCanCache,bSpecificToRequestor);
	m_mapXML_Data_Source_Info[sDataID] = pXML_Data_Source_Info;
	return 0;
}

int XML_Data_Factory::Populate(int PK_Device_Requestor,string sDataID,string sXmlIn,string *sXmlOut)
{
	PLUTO_SAFETY_LOCK(fm,m_DataFactoryMutex);

	map<string,XML_Data_Source_Info *>::iterator it=m_mapXML_Data_Source_Info.find(sDataID);
	// If it's something we handle locally, there's another Populate that takes the XML_Data_Source_Info *
	if( it!=m_mapXML_Data_Source_Info.end() )
	{
		XML_Data_Source_Info *pXML_Data_Source_Info = it->second;
		return pXML_Data_Source_Info->Populate(PK_Device_Requestor,sXmlIn,sXmlOut);
	}
	// Handle the situation that sDataID isn't something we handle localling
	else
	{
		// This means we either can't handle it at all
		map<string,int>::iterator itFF=m_mapForeignFactories.find(sDataID);
		if( itFF==m_mapForeignFactories.end() )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"XML_Data_Factory::Populate nothing to handle %s", sDataID.c_str());
			return -1;
		}

		fm.Release(); // This can take a while.  We don't need any member variables, so release the mutex
		// Or there's a foreign factory that registered to handle it
//		DCE::CMD_Populate_Datagrid CMD_Populate_Datagrid(m_pCommand_Impl->m_dwPK_Device,it->second,sDataID,sXmlIn,sXmlOut);
		bool bResult = false; //m_pCommand_Impl->SendCommand(CMD_Populate_Datagrid);
		return bResult ? 0 : -1;
	}
}
