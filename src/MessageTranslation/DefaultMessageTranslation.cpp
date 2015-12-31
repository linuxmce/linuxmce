/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "DCE/Logger.h"
#include "DCE/DCEConfig.h"
#include "DefaultMessageTranslation.h"

#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_CommandParameter.h"
#include "PlutoUtils/DBHelper.h"
#include <assert.h>

namespace DCE {

/*****************************************************************
DefaultMessageTranslator
*****************************************************************/

DefaultMessageTranslator::DefaultMessageTranslator() 
{
}

DefaultMessageTranslator::~DefaultMessageTranslator() {
}


bool 
DefaultMessageTranslator::Translate(MessageReplicator& inrepl, MessageReplicatorList& outrepls) {
	DeviceData_Base* pTargetDev = FindTargetDevice(inrepl.getMessage().m_dwPK_Device_To);
	if(!pTargetDev) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Target Device %d Not Found.", inrepl.getMessage().m_dwPK_Device_To);
		return false;
	}
	int IR_PowerDelay = 0;
	long devtemplid = pTargetDev->m_dwPK_DeviceTemplate;
	if( InitDelaysMap(devtemplid) )
	{
		IR_PowerDelay = map_PowerDelay[devtemplid];
	}
	
	/**************************************************************************************
	Determine message queue attributes
	**************************************************************************************/
	int delay = 0, repeat = 1;

	/*general attributes*/	
	if(inrepl.getMessage().m_mapParameters.count(COMMANDPARAMETER_Repeat_Command_CONST) > 0 && 
		inrepl.getMessage().m_mapParameters[COMMANDPARAMETER_Repeat_Command_CONST].length() > 0) {
		repeat = atoi(inrepl.getMessage().m_mapParameters[COMMANDPARAMETER_Repeat_Command_CONST].c_str());
		if(repeat <= 0) {
			repeat = 1;
		}
	}
	
	/*command specific attributes*/	
	switch(inrepl.getMessage().m_dwID) {
		case COMMAND_Generic_On_CONST:
		case COMMAND_Generic_Off_CONST:
		case COMMAND_Toggle_Power_CONST: {
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got power command for Device: %d", pTargetDev->m_dwPK_Device);
			delay = IR_PowerDelay;
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Using delay of: %ld.", delay);
		} break;
		default: {
		/*
			if (inrepl.getMessage().m_mapParameters.count(COMMANDPARAMETER_PK_Command_Input_CONST)>0) {
				delay = IR_ModeDelay;
			} */
		} break;
	}

	inrepl.setPostDelay(delay);
	inrepl.setCount(repeat);
	return false;
}

bool
DefaultMessageTranslator::InitDelaysMap(long devtemplid)
{
	static char sql_buff[1024];
	if(map_PowerDelay.find(devtemplid) == map_PowerDelay.end())
	{
		DCEConfig dceconf;
		DBHelper mySqlHelper(dceconf.m_sDBHost, dceconf.m_sDBUser, dceconf.m_sDBPassword, dceconf.m_sDBName,dceconf.m_iDBPort);
		PlutoSqlResult result_set;
		DB_ROW row=NULL;
		sprintf(sql_buff,"SELECT IR_PowerDelay FROM DeviceTemplate_AV WHERE FK_DeviceTemplate='%d'", (int)devtemplid);
		if( (result_set.r=mySqlHelper.db_wrapper_query_result(sql_buff)) && (row = db_wrapper_fetch_row(result_set.r)) )
		{
			map_PowerDelay[devtemplid] = atoi(row[0]);
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Device has no AV properties");
			return false;
		}
	}
	
	return true;
}

/*****************************************************************
DefaultMessageDispatcher
*****************************************************************/
void 
DefaultMessageDispatcher::DispatchMessage(MessageReplicator& inrepl) {
	for(int i = 0; i < inrepl.getCount(); i++) {
		if( inrepl.isUseless() )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "GSD-DispatchMessage - ignoring %d because is useless.", inrepl.getMessage().m_dwID);
		}
		else
		{
			int sleep_delay = inrepl.getPreDelay();
			if( sleep_delay < 0 )
				sleep_delay = 0;
			else if( 19 > sleep_delay )
				sleep_delay *= 1000;
			else if( 20000 < sleep_delay )
				sleep_delay /= 1000;
			if( sleep_delay != 0 )
				Sleep(sleep_delay);
			
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "GSD-Sleep Pre %d : %d", inrepl.getMessage().m_dwID, sleep_delay);
			
			DispatchMessage(&inrepl.getMessage());
			
			sleep_delay = inrepl.getPostDelay();
			if( sleep_delay < 0 )
				sleep_delay = 0;
			else if( 19 > sleep_delay )
				sleep_delay *= 1000;
			else if( 20000 < sleep_delay )
				sleep_delay /= 1000;
			if( sleep_delay != 0 )
				Sleep(sleep_delay);
			
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "GSD-Sleep Post %d : %d", inrepl.getMessage().m_dwID, sleep_delay);
		}
	}
}

};
