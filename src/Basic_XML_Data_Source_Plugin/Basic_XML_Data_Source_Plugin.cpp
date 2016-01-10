/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "Basic_XML_Data_Source_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "../XML_Data_Handler_Plugin/XML_Data_Handler_Plugin.h"
#include "XML_Data_Source.h"
#include "../DCE/DCERouter.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Basic_XML_Data_Source_Plugin::Basic_XML_Data_Source_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Basic_XML_Data_Source_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pDatabase_pluto_main = NULL;
	m_pXML_Data_InstantiationInfo_Basic=NULL;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Basic_XML_Data_Source_Plugin::~Basic_XML_Data_Source_Plugin()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool Basic_XML_Data_Source_Plugin::GetConfig()
{
	if( !Basic_XML_Data_Source_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	m_pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());
	if(!m_pDatabase_pluto_main->Connect(m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get()) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot connect to database!");
		m_bQuit_set(true);
		return false;
	}

	m_pXML_Data_InstantiationInfo_Basic = new XML_Data_InstantiationInfo_Basic(this);

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Basic_XML_Data_Source_Plugin::Register()
//<-dceag-reg-e->
{
	m_pXML_Data_Handler_Plugin=( XML_Data_Handler_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_XML_Data_Handler_Plugin_CONST);
	if( !m_pXML_Data_Handler_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to Basic_XML_Data_Source_Plugin");
		return false;
	}

	m_pXML_Data_Handler_Plugin->m_XML_Data_Factory_get().Register( DeviceList::Instantiate, DeviceList::DataID, m_pXML_Data_InstantiationInfo_Basic, XML_Data_Source_Info::cache_never, true );
	m_pXML_Data_Handler_Plugin->m_XML_Data_Factory_get().Register( CommandGroupList::Instantiate, CommandGroupList::DataID, m_pXML_Data_InstantiationInfo_Basic, XML_Data_Source_Info::cache_never, true );
	return Connect(PK_DeviceTemplate_get()); 
}

//<-dceag-createinst-b->!

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Basic_XML_Data_Source_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Basic_XML_Data_Source_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/



