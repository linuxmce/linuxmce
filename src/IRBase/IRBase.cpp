/*
	IRBase

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

 	Phone: +1 (877) 758-8648

	This program is distributed according to the terms of the Pluto Public License, available at:
	http://plutohome.com/index.php?section=public_license

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "IRBase.h"

#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "Infrared_Plugin/Infrared_Plugin.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "DCE/Logger.h"
#include "Serial/SerialPort.h"
#include "IR/IRDevice.h"
#include "DCE/DCEConfig.h"
#include <math.h>
#include <time.h>

void 
IRBase::handleStart() {
	// get details (IR sequences, Delays, etc.) of all child devices
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "In IRBase::ParseDevices");
	Command_Impl *pCommand_Impl = getCommandImpl();
	if(!pCommand_Impl) {
		return;
	}

	MapCommand_Impl::iterator it;
	for(MapCommand_Impl::iterator it = pCommand_Impl->m_mapCommandImpl_Children.begin(); 
			it != pCommand_Impl->m_mapCommandImpl_Children.end(); ++it)
	{
		handleStart(it->second);
	}
}

void 
IRBase::handleStart(Command_Impl *pCommand_Impl) {
	long devid = pCommand_Impl->m_pData->m_dwPK_Device;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Requested IR cored for device: %lu", devid);
	
	map<int,string> mapClass;  
	// this will have all the Ruby code, where int is the PK_Command and string is the codeint 
	int iSize=0; char* pData = NULL; // Place holders for the 'out' parameter
	DCE::CMD_Get_Infrared_Codes_DT CMD_Get_Infrared_Codes_DT(devid, DEVICETEMPLATE_Infrared_Plugin_CONST,
				BL_SameHouse, devid, &pData, &iSize);
	getCommandImpl()->SendCommand(CMD_Get_Infrared_Codes_DT);  // Get the codes from I/R Plugin
	if( !iSize || !pData )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot get i/r codes for %d",devid);
		return;
	}
	IRDevice irDevice;
	irDevice.SerializeRead(iSize, pData); // De-serialize the data
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "IR Code count: %d", mapClass.size());
	m_mapDevice_IRRepeat[devid] = make_pair (irDevice.m_iRepeatIR,irDevice.m_iRepeatVolume);
			
	for(map<int,string>::iterator it = irDevice.m_mapCodes.begin(); it != irDevice.m_mapCodes.end(); it++ ) {
		long cmdid = (*it).first;
		if( m_bMustConvertRC5_6 && (it->second[0]=='5' || it->second[0]=='6') )
			codemap_[longPair(devid, cmdid)] = ConvertRC5_6((*it).second);
		else
			codemap_[longPair(devid, cmdid)] = (*it).second;
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Loaded IR code for Device %ld, Action %ld", devid, cmdid);
	}
	MapCommand_Impl::iterator it;
	for(MapCommand_Impl::iterator it = pCommand_Impl->m_mapCommandImpl_Children.begin(); 
			it != pCommand_Impl->m_mapCommandImpl_Children.end(); ++it)
	{
		handleStart(it->second);
	}
}

void 
IRBase::handleStop()
{
	m_bQuit_ = true;
}

bool IRBase::Translate(MessageReplicator& inrepl, MessageReplicatorList& outrepls) {
	Message* pmsg = &inrepl.getMessage();
	if(pmsg == NULL)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "IRBase::Translate : null message");
		return false;
	}
	
	DeviceData_Base *pTargetDev = AVMessageTranslator::FindTargetDevice(pmsg->m_dwPK_Device_To);
	if(!pTargetDev) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Target Device %d Not Found.", pmsg->m_dwPK_Device_To);
		return false;
	}
	long devid = pTargetDev->m_dwPK_Device;
	
	// try to find if there is the IR code available
	bool irFound = false;
	map <longPair, string>::iterator it = codemap_.find(longPair(devid, pmsg->m_dwID));
	if(it != codemap_.end()) {
		irFound = true;
		inrepl.setImplemented(true);
	}
	
	if(AVMessageTranslator::Translate(inrepl, outrepls)) {
		return true;
	}
	
	/********************************************************************************************************
	COMMAND_Send_Code_CONST
	********************************************************************************************************/
	if(pmsg->m_dwID == COMMAND_Send_Code_CONST) {
		outrepls.push_back(inrepl);
		return true;
	} 
	
	if( !irFound ) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Infrared Code not found for Command %d. Will not be processed by IRBase.", pmsg->m_dwID);
		return false;
	} else {
		outrepls.push_back(inrepl);
		return true;
	}
	
	// Ok, not a system-handled action.
	return false;
}

void 
IRBase::DispatchMessage(Message* pmsg) {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "In IRBase::DispatchMessage");
	
	if( m_bQuit_ )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Didn't process the message, quiting");
	}
	
	string irport, ircode;
	int cmd = 0, devid = pmsg->m_dwPK_Device_To;

	DeviceData_Base *pTargetDev = AVMessageDispatcher::FindTargetDevice(devid);
	if(!pTargetDev) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Could not find target device %d...", devid);
		return;
	}
	
	if(pmsg->m_dwID == COMMAND_Send_Code_CONST) {
		irport = pmsg->m_mapParameters[COMMANDPARAMETER_Absolute_Channel_CONST],
		ircode = pmsg->m_mapParameters[COMMANDPARAMETER_Tokens_CONST];
		
		if(ircode.empty()) {
			if(pmsg->m_mapParameters.count(COMMANDPARAMETER_ID_CONST) > 0) {
				// Is an action ID specified?
				cmd = atoi(pmsg->m_mapParameters[COMMANDPARAMETER_ID_CONST].c_str());
			} else {
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Command Send Code does not contain any info about infrared code...");
				return;
			}
		} else {
			string::size_type pos=0;
			while(pos<ircode.size() && pos!=string::npos && !m_bQuit_)
			{
				string _ircode = StringUtils::Tokenize(ircode,"&",pos);
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"pos %d size %d Checking %s\n for multiple codes, got: %s",pos,(int) ircode.size(),ircode.c_str(),_ircode.c_str());
				SendIR(irport,_ircode,1);
				if( pos<ircode.size() && !m_bQuit_ )
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Sleeping for 500 ms since there are multiple codes");
					Sleep(500);
				}
			}
		}
	} else {
		cmd = pmsg->m_dwID;
		m_pCommand_Impl->GetChildDeviceData(pTargetDev->m_dwPK_Device,DEVICEDATA_PortChannel_Number_CONST,irport);
		// if the device doesn't have a port (which is normal), try to find a port on its parent (which should have this setting)
		if (irport == "" && pTargetDev->m_dwPK_Device_ControlledVia > 0)
			m_pCommand_Impl->GetChildDeviceData(pTargetDev->m_dwPK_Device_ControlledVia, DEVICEDATA_PortChannel_Number_CONST, irport);
	}
		
	map <longPair, string>::iterator it = codemap_.find(longPair(devid, cmd));
	if(it == codemap_.end()) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Could not find Infrared Code for Command %d.", cmd);
		return;
	} else {
		ircode = (*it).second;
	}
	
	if(!ircode.empty()) {
		int iRepeat = 4;
		if( pmsg->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST)!=pmsg->m_mapParameters.end() )
			iRepeat = atoi( pmsg->m_mapParameters[COMMANDPARAMETER_Repeat_Command_CONST].c_str() );
		else
		{
			if( m_mapDevice_IRRepeat.find(devid)!=m_mapDevice_IRRepeat.end() )
			{
				pair<int,int> p = m_mapDevice_IRRepeat[devid];
				if( cmd==COMMAND_Vol_Up_CONST || cmd==COMMAND_Vol_Down_CONST )
					iRepeat = p.second;
				else
					iRepeat = p.first;
			}
		}
		if( iRepeat<1 )
			iRepeat=4;

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending Infrared Code for dev <%d> cmd <%d>, channel <%s>, repeat <%d> code <%s>", 
					devid, cmd, irport.c_str(), iRepeat, ircode.c_str());

		string::size_type pos=0;
		while(pos<ircode.size() && pos!=string::npos && !m_bQuit_)
		{
			string _ircode = StringUtils::Tokenize(ircode,"&",pos);

			LoggerWrapper::GetInstance()->Write(LV_STATUS,"pos %d size %d Checking %s\n for multiple codes, got: %s",pos,(int) ircode.size(),ircode.c_str(),_ircode.c_str());

			SendIR(irport,_ircode,iRepeat);
			if( pos<ircode.size() && !m_bQuit_ )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Sleeping for 500 ms since there are multiple codes");
				Sleep(500);
			}
		}
	} else {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Infrared Code Data is Empty...");
	}
}

