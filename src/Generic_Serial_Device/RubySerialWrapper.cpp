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
#include "RubySerialWrapper.h"

namespace DCE {

	RubySerialWrapper::RubySerialWrapper()
		: pdce_(NULL)	
	{
	}

	RubySerialWrapper::~RubySerialWrapper()
	{
	}

	void RubySerialWrapper::SendCommand(RubyCommandWrapper* pcmd) 
	{
		if(pdce_) {
			pdce_->SendCommand(pcmd);
		}
	}

	void RubySerialWrapper::SendReceiveCommand(RubyCommandWrapper* pcmd) 
	{
		if(pdce_) {
			pdce_->SendReceiveCommand(pcmd);
		}
	}

	void RubySerialWrapper::DisableDevice( int PK_Device, bool bDisable ) 
	{
		if( pdce_ != NULL /*&& device_ != NULL*/ ) {

			// 791 = PK_Command for disable device
			RubyCommandWrapper *pcmd = new RubyCommandWrapper( device_.getDevId(), 0, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Set_Enable_Status_CONST );

			pcmd->setTemplate( DEVICETEMPLATE_General_Info_Plugin_CONST );

			pcmd->params_[COMMANDPARAMETER_PK_Device_CONST] = StringUtils::itos( PK_Device );

			if ( bDisable )
				pcmd->params_[COMMANDPARAMETER_Enable_CONST] = "0";
			else pcmd->params_[COMMANDPARAMETER_Enable_CONST] = "1";
			
			pdce_->SendCommand(pcmd);

			delete pcmd;
		}

	}

	void RubySerialWrapper::SetDeviceDataInDB( int PK_Device, int PK_DeviceData, const char* value )
	{
		if(pdce_) {

			RubyCommandWrapper *pcmd = new RubyCommandWrapper( device_.getDevId(), 
				DEVICEID_DCEROUTER, PRIORITY_NORMAL, MESSAGETYPE_DATAPARM_CHANGE, PK_Device ? PK_Device : device_.getDevId() );

			pcmd->params_[PK_DeviceData] = value;

			pdce_->SendCommand(pcmd);

			delete pcmd;

		}
	}

	const char* RubySerialWrapper::GetCurrentDeviceData( int PK_Device, int PK_DeviceData )
	{
		if(pdce_) {

			// TODO: make sure the "to" and "from" fields are filled corectlly
			// message type is irrelevant, from SendCommandReceiveString it's set this right anyway
			RubyCommandWrapper *pcmd = new RubyCommandWrapper( DEVICEID_DCEROUTER, 
				device_.getDevId(), PRIORITY_NORMAL, MESSAGETYPE_DATAPARM_REQUEST, PK_DeviceData );

			string s_response = pdce_->SendCommandReceiveString(pcmd);

			delete pcmd;
			return s_response.c_str();

		}
		// Message *pMessage = new Message(m_dwPK_Device,PK_Device,PRIORITY_NORMAL,MESSAGETYPE_DATAPARM_REQUEST, PK_DeviceData,0);
		
		return NULL;
	}
};
