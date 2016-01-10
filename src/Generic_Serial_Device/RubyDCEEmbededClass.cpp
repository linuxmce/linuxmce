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
#include "RubyDCEEmbededClass.h"
#include "RubyDeviceWrapper.h"
#include "RubyCommandWrapper.h"
#include "RubyIOManager.h"
#include "DCE/Message.h"
#include "DCE/Logger.h"
#include "DCE/Event_Impl.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_ParameterType.h"

#include <ruby.h>

using namespace std;
using namespace EMBRUBY;

namespace DCE {

RubyDCEEmbededClass::RubyDCEEmbededClass() {
	m_dwPK_Device=0;
}

RubyDCEEmbededClass::RubyDCEEmbededClass(RubyDCECodeSupplier* pcs, int dwPK_Device) 
	: RubyEmbededClassImpl<RubyDCEEmbededClass>(NULL), pcs_(pcs)
{
	m_dwPK_Device=dwPK_Device;
	char buff[20];
	sprintf(buff, "Device_%d", m_dwPK_Device);
	Init(buff);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Class: %s instantiated.", buff);
}


RubyDCEEmbededClass::~RubyDCEEmbededClass()
{
}

bool
RubyDCEEmbededClass::CallCmdHandler(Message *pMessage) {
	if(!pcs_->isCmdImplemented(m_dwPK_Device, pMessage->m_dwID)) {
		if(pMessage->m_dwID != COMMAND_Process_IDLE_CONST) {
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Command %d not supported.", pMessage->m_dwID);
			pMessage->m_bRespondedToMessage=true;
		}
		if(pMessage->m_dwID == COMMAND_Process_Initialize_CONST) 
			return true; // It's ok if there's no initialize specified
		return false;
	}
	
	std::list<int> paramids;
	pcs_->getParamsOrderForCmd(m_dwPK_Device, pMessage->m_dwID, paramids);
	
	// build the params
	std::list<VALUE> params;
	RubyEmbededClassImpl<>* pembclass = NULL;
	try {	
		pembclass = new RubyEmbededClassImpl<>("Command");
		RubyCommandWrapper* pWrapper = reinterpret_cast<RubyCommandWrapper*>(DATA_PTR(pembclass->getValue()));
		
		if(pWrapper == NULL) {
			delete pembclass;
			return false;
		} else {
			pWrapper->setDevIdFrom(pMessage->m_dwPK_Device_From);
			pWrapper->setDevIdTo(pMessage->m_dwPK_Device_To);
			pWrapper->setPriority(pMessage->m_dwPriority);
			pWrapper->setType(pMessage->m_dwMessage_Type);
			pWrapper->setId(pMessage->m_dwID);
			
			pWrapper->setParams(pMessage->m_mapParameters);
		};
	} catch(RubyException e) {
		pembclass = NULL;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Exception in Ruby occured: %s.", e.getMessage());
		return false;
	}
	
	for(std::list<int>::iterator pmit = paramids.begin(); pmit != paramids.end(); pmit++) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Parameter: %s", (pMessage->m_mapParameters[*pmit]).c_str());
		params.push_back(StrToValue((pMessage->m_mapParameters[*pmit]).c_str()));
	}
	// the last parameter is the command itself
	params.push_back(pembclass->getValue());
	VALUE result;

	RubyIOManager* pmanager=NULL;
	char buff[20];
	sprintf(buff, "cmd_%d", (int)pMessage->m_dwID);
	try {
		pmanager = RubyIOManager::getInstance();
		result=callmethod(buff, params);
		string sCMD_Result="OK";
		PLUTO_SAFETY_LOCK(mm,pmanager->m_MsgMutex);
		if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
		{
			Message *pMessageOut=new Message(pMessage->m_dwPK_Device_To,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
			if(TYPE(result)==T_ARRAY)
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got returnParamArray from Ruby");
#ifdef RUBY2_0
			    	char *tmpbuf[RARRAY_LEN(result)];
				for(int i=0;i<RARRAY_LEN(result);i++)
				{
					VALUE v=RARRAY_PTR(result)[i];
#else
				char *tmpbuf[RARRAY(result)->len];
				for(int i=0;i<RARRAY(result)->len;i++)
				{
					VALUE v=RARRAY(result)->ptr[i];
#endif
					if(TYPE(v)!=T_NIL)
					{
						switch(this->pcs_->getParamType(i))
						{
							case PARAMETERTYPE_string_CONST:
#ifdef RUBY2_0
								tmpbuf[i]=strdup(StringValuePtr(v));
#else
								tmpbuf[i]=strdup(STR2CSTR(v));
#endif
								pMessageOut->m_mapParameters[i]=tmpbuf[i];
								break;
							case PARAMETERTYPE_int_CONST:
								tmpbuf[i]=(char*)malloc(20);
								sprintf(tmpbuf[i],"%d",NUM2INT(v));
								pMessageOut->m_mapParameters[i]=tmpbuf[i];
								break;
							case PARAMETERTYPE_bool_CONST:
								pMessageOut->m_mapParameters[i]=(TYPE(v)==T_TRUE?true:false);
								break;
							case PARAMETERTYPE_Data_CONST:
#ifdef RUBY2_0
								tmpbuf[i]=(char*)calloc(RSTRING_LEN(v),sizeof(char));
								memcpy(tmpbuf[i],RSTRING_PTR(v),RSTRING_LEN(v));
								pMessageOut->m_mapData_Parameters[i]=tmpbuf[i];
								pMessageOut->m_mapData_Lengths[i]=RSTRING_LEN(v);
#else
								tmpbuf[i]=(char*)calloc(RSTRING(v)->len,sizeof(char));
								memcpy(tmpbuf[i],RSTRING(v)->ptr,RSTRING(v)->len);
								pMessageOut->m_mapData_Parameters[i]=tmpbuf[i];
								pMessageOut->m_mapData_Lengths[i]=RSTRING(v)->len;
#endif
								break;
							case PARAMETERTYPE_double_CONST:
						    	tmpbuf[i]=(char*)malloc(20);
								sprintf(tmpbuf[i],"%lf",NUM2DBL(v));
								pMessageOut->m_mapParameters[i]=tmpbuf[i];
								break;
						}
						if(this->pcs_->getParamType(i) != PARAMETERTYPE_Data_CONST)
							LoggerWrapper::GetInstance()->Write(LV_STATUS, "    Parameter %d = \"%s\"",i,pMessageOut->m_mapParameters[i].c_str());
						else
							LoggerWrapper::GetInstance()->Write(LV_STATUS, "    Parameter %d is %d bytes long",i,pMessageOut->m_mapData_Lengths[i]);
					}
				}
			}
			pMessageOut->m_mapParameters[0]=sCMD_Result;
			mm.Release();
			pmanager->SendMessage(pMessageOut);
			mm.Relock();
			pMessage->m_bRespondedToMessage=true;
		}
		else
		{
			if( pMessage->m_eExpectedResponse==ER_DeliveryConfirmation && !pMessage->m_bRespondedToMessage )
			{
				mm.Release();
				pmanager->SendString(sCMD_Result);
				mm.Relock();
			}
			pMessage->m_bRespondedToMessage=true;
			mm.Release();
		}
	} catch(RubyException e) {
		delete pembclass;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (string("Error while calling method: ") + e.getMessage()).c_str());
		return false;
	}

	delete pembclass;
	return true;
}

void 
RubyDCEEmbededClass::CallCmdForChildHandler(unsigned devid, Message *pMessage) {
	if(!pcs_->isProcChildCommandAssigned(devid)) {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Command For Child not supported.");
		pMessage->m_bRespondedToMessage=true;
		return;
	}

	std::list<VALUE> params;

	RubyEmbededClassImpl<>* pembclass = NULL;
	try {	
		pembclass = new RubyEmbededClassImpl<>("Command");
		RubyCommandWrapper* pWrapper = reinterpret_cast<RubyCommandWrapper*>(DATA_PTR(pembclass->getValue()));
		
		if(pWrapper == NULL) {
			delete pembclass;
			return;
		} else {
			pWrapper->setDevIdFrom(pMessage->m_dwPK_Device_From);
			pWrapper->setDevIdTo(pMessage->m_dwPK_Device_To);
			pWrapper->setPriority(pMessage->m_dwPriority);
			pWrapper->setType(pMessage->m_dwMessage_Type);
			pWrapper->setId(pMessage->m_dwID);
			
			pWrapper->setParams(pMessage->m_mapParameters);
		};
	} catch(RubyException e) {
		pembclass = NULL;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Exception in Ruby occured: %s.", e.getMessage());
		return;
	}
	
	params.push_back(pembclass->getValue());
	
	try {
		callmethod("cmd_ReceiveCommandForChild", params);
		pMessage->m_bRespondedToMessage=true;
	} catch(RubyException e) {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (string("Error while calling method: ") + e.getMessage()).c_str());
	}

	delete pembclass;
}

};
