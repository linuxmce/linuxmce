/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "RubyIOManager.h"
#include "RubyEmbeder.h"

#include "SerialIOPool.h"
#include "NetworkIOPool.h"
#include "PlutoUtils/LinuxSerialUSB.h"
#include "DCE/DeviceData_Impl.h"
#include "DCE/Command_Impl.h"
#include "DCE/Logger.h"
#include "DCE/Message.h"
#include "DCE/Command_Impl.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Table_DeviceData.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include <signal.h>

using namespace std;
using namespace EMBRUBY;

#define DEFAULT_POOL_TIME		10
#define DEFAULT_DELAY_TIME		2000
//this one is in seconds
#define DEFAULT_RESPONSE_TIME	18

namespace DCE {

RubyIOManager* RubyIOManager::s_instance_ = NULL;

RubyIOManager::RubyIOManager()
	: m_MsgMutex("message mutex",true), pevdisp_(NULL), rootnode_(NULL)
{
}


RubyIOManager::~RubyIOManager() {
}

RubyIOManager* 
RubyIOManager::getInstance() {
	if(!s_instance_) {
		static RubyIOManager instance;
		s_instance_ = &instance;
	}
	return s_instance_;
}

RubyDCEDeviceNode* 
RubyIOManager::InstantiateNode(Command_Impl* pcmdimpl, DeviceData_Impl* pdevdata) {
	RubyDCEDeviceNode* pNode = NULL;
	pcmdimpl_=pcmdimpl;
	string sport = pcmdimpl->m_pEvent->GetDeviceDataFromDatabase(pcmdimpl->m_dwPK_Device,DEVICEDATA_COM_Port_on_PC_CONST);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"RubyIOManager::InstantiateNode port: %s",sport.c_str());
	PORTTYPE porttype = PORTTYPE_UNKNOWN;
	if(!sport.empty() && sport != "0" && StringUtils::StartsWith(sport,"ERR")==false ) {
		porttype = PORTTYPE_SERIAL;
	} else {
		sport = pdevdata->m_mapParameters[DEVICEDATA_TCP_Port_CONST];
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Checking TCP Port %s",sport.c_str());
		if(!sport.empty()) {
			porttype = PORTTYPE_NETWORK;
		} else {
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "GSD Port is not specified. Instantiating non-IO Wrapper.");
		}
	}

	if(porttype == PORTTYPE_UNKNOWN) {
		pNode = new RubyDCEDeviceNode();
		cs_.addCode(pcmdimpl, pdevdata, false);
		
	} else {
		cs_.addCode(pcmdimpl, pdevdata, true);
	    IOPool* pnewpool = NULL;
    	switch(porttype) {
        	/*** Serial Device Initialization *********************************/
    	    case PORTTYPE_SERIAL: {
	            int bps = 9600;

        	    string serport = pcmdimpl->m_pEvent->GetDeviceDataFromDatabase(pcmdimpl->m_dwPK_Device,DEVICEDATA_COM_Port_on_PC_CONST);
				serport = TranslateSerialUSB(serport);

        	    string sbaudrate = pdevdata->m_mapParameters[DEVICEDATA_COM_Port_BaudRate_CONST];
            	if(!sbaudrate.empty() )
				{
					int tbps;
					if( sbaudrate[0] == 'B')
						tbps = atoi(sbaudrate.substr(1, sbaudrate.length() -1).c_str());
					else
						tbps = atoi(sbaudrate.c_str());
    	            if(tbps != 0) {
        	            bps = tbps;
            	    }
	            } else {
    	            LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ba>=udRate not specified or invalid, using default: %d.", bps);
        	    }

	            LoggerWrapper::GetInstance()->Write(LV_STATUS, "Using port %s, at bps: %d.", serport.c_str(), bps);

    	        pnewpool = new SerialIOPool();
        	    SerialIOConnection* pio = reinterpret_cast<SerialIOConnection*>(pnewpool->getConnection());
            	pio->setBPS(bps);
	            pio->setSerialPort(serport.c_str());

	            string sparbitstop = pdevdata->m_mapParameters[DEVICEDATA_COM_Port_ParityBitStop_CONST];
    	        if(!sparbitstop.empty()) {
        	        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Using paritybit/stopbit: %s.", sparbitstop.c_str());
                	if(sparbitstop == "N81") {
            	        pio->setParityBitStop(epbsN81);
	                } else
    	            if(sparbitstop == "E81") {
        	            pio->setParityBitStop(epbsE81);
            	    } else
                	if(sparbitstop == "O81") {
	                    pio->setParityBitStop(epbsO81);
        	        } else {
    	                /*more to be implemented*/
            	        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Paritybit/Stopbit %s NOT supported.", sparbitstop.c_str());
                	}
	            }
    	        } break;
	       /*** Network Device Initialization *********************************/
    	    case PORTTYPE_NETWORK: {
        	    string netaddr;
	            int netport = -1;

	            int lpar = sport.find("("), rpar = sport.find(")", lpar + 1);
    	        if(lpar > 0 && rpar > 0) {
        	        netport = atoi(sport.substr(lpar + 1, rpar - lpar - 1).c_str());
            	    netaddr = sport.substr(0, lpar);
	            } else {
    	            /*get device ip addr*/
        	        netport = atoi(sport.c_str());
            	    netaddr = pdevdata->GetIPAddress();
	            }

    	        if(!netaddr.empty()) {
        	        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Using network device with address <%s>, at port: <%d>.", netaddr.c_str(), netport);
            	} else {
    	            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Could not determine network address for device %d.", pdevdata->m_dwPK_Device);
	                return NULL;
        	    }

	            pnewpool = new NetworkIOPool();
    	        NetworkIOConnection* pio = reinterpret_cast<NetworkIOConnection*>(pnewpool->getConnection());
        	    pio->setHost(netaddr.c_str());
            	pio->setPort(netport);

	            } break;

    	    default:
        	    /*we checked for unknown before, so we cannot get here*/
	            return NULL;
	    }

		pNode = new RubyIOPool(pnewpool);
	}

	int delay = atoi(pdevdata->m_mapParameters[DEVICEDATA_Idle_Delay_CONST].c_str());
	if(delay == 0) {
		delay = DEFAULT_DELAY_TIME;
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Using Idle Delay: %d.", delay);
	pNode->setIdleDelay(delay);
    pNode->setDeviceData(pdevdata);
	return pNode;
}

int 
RubyIOManager::addDevice(Command_Impl* pcmdimpl, DeviceData_Impl* pdevdata) {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Adding device: %d.", pdevdata->m_dwPK_Device);

	/*parse Code nodes and insert node*/
	RubyDCEDeviceNode* pNewNode = addDevice(pcmdimpl, pdevdata, rootnode_);
	if(!pNewNode) {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Could not add new Device Node.");
		return -1;
	}

	if(!rootnode_) {
		rootnode_ = pNewNode;
	}
	
	return 0;
}

RubyDCEDeviceNode* 
RubyIOManager::addDevice(Command_Impl* pcmdimpl, DeviceData_Impl* pdevdata, RubyDCEDeviceNode* pNode) {
	if(pNode == NULL) {
		return InstantiateNode(pcmdimpl, pdevdata);
	} 
	RubyDCEDeviceNode* pNewNode = NULL;
	if(pdevdata->m_dwPK_Device_ControlledVia == pNode->getDeviceData()->m_dwPK_Device) {
		pNewNode = InstantiateNode(pcmdimpl, pdevdata);
		if(pNewNode) {
			pNode->addChild(pNewNode);
		}
	} else {
		std::list<RubyDCEDeviceNode*>& children = pNode->getChildren();
		for(std::list<RubyDCEDeviceNode*>::iterator it = children.begin(); it != children.end(); it++) {
			pNewNode = addDevice(pcmdimpl, pdevdata, *it);
			if(pNewNode) {
				break;
			}
		}
	}
	return pNewNode;
}

int 
RubyIOManager::removeDevice(DeviceData_Impl* pdevdata) {
	/*NOT Implemented*/
	return -1;
}
bool 
RubyIOManager::hasDevice(DeviceData_Base* pdevdata) {
	return hasDevice(pdevdata, rootnode_);
}

bool 
RubyIOManager::hasDevice(DeviceData_Base* pdevdata, RubyDCEDeviceNode* pNode) {
	if(!pNode) {
		return false;
	}

	if(pNode->getDeviceData()->m_dwPK_Device == pdevdata->m_dwPK_Device) {
		return true;
	}

	std::list<RubyDCEDeviceNode*>& children = pNode->getChildren();
    for(std::list<RubyDCEDeviceNode*>::iterator it = children.begin(); it != children.end(); it++) {
		if(hasDevice(pdevdata, *it)) {
			return true;
		}
    }
	return false;
}

int 
RubyIOManager::RouteMessage(DeviceData_Base* pdevdata, Message *pMessage) {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Dispatching Message %d to %d...", pMessage->m_dwID, pMessage->m_dwPK_Device_To);

	RubyIOManager* pmanager = RubyIOManager::getInstance();
	int routeToDevice = 0;
	/*find device*/
	if(!pdevdata)
	{
	    LoggerWrapper::GetInstance()->Write(LV_WARNING, "Running without DeviceData");
		routeToDevice = pMessage->m_dwPK_Device_To;
	}
	else
	{
		DeviceData_Base *ptmpdevdata = pdevdata->m_AllDevices.m_mapDeviceData_Base_Find(pMessage->m_dwPK_Device_To);
		while(ptmpdevdata != NULL && !pmanager->hasDevice(ptmpdevdata)) {
			pdevdata = ptmpdevdata;
			ptmpdevdata =
				pdevdata->m_AllDevices.m_mapDeviceData_Base_Find(ptmpdevdata->m_dwPK_Device_ControlledVia);
		}
		if(ptmpdevdata == NULL) {
		    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Command will be handled in Parent device.");
			ptmpdevdata = pdevdata;
		}
		routeToDevice = ptmpdevdata->m_dwPK_Device;
	}
		
	mmsg_.Lock();
	std::pair<unsigned, Message*> msg(routeToDevice, pMessage);
	msgqueue_.push_back(msg);
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Routing message ID=%d to device %d.", pMessage->m_dwID, routeToDevice);
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Process Queue = %u", msgqueue_.size());
	mmsg_.Unlock();
	emsg_.Signal();
	
	time_t tTimeout=time(NULL)+DEFAULT_RESPONSE_TIME;
	while( tTimeout>time(NULL) )
	{
		Sleep(1);
		PLUTO_SAFETY_LOCK(mm,m_MsgMutex);
		if( pMessage->m_bRespondedToMessage )
		{
			return 0;
		}
		mm.Release();
	}
	PLUTO_SAFETY_LOCK(mm,m_MsgMutex);
	if( !pMessage->m_bRespondedToMessage )
	{
		pMessage->m_bRespondedToMessage=true;
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Ruby was unable to handle the command in reasonable amount of time");
	}
	return 0;
}

void handler_for_bad_things(int signal)
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "A CRITICAL ERROR HAS OCCURED (CHECK YOUR RUBY CODE) %i", signal);
	exit(1);
}

void* 
RubyIOManager::_Run() {
	signal(SIGABRT,handler_for_bad_things);
	while(!isStopRequested()) {
		bool msgarrived = emsg_.Wait(DEFAULT_POOL_TIME);
		mmsg_.Lock();
		if(!msgarrived && msgqueue_.size() == 0) {
			mmsg_.Unlock();
			if(rootnode_) {
				rootnode_->handleNoMessage();
				usleep(10000);
			}
			/*	
			POOLMAP::iterator it = pools_.begin();
			while(it != pools_.end()) {
				if((*it).second != NULL) {
					(*it).second->handleIteration();
				}
				it++;
			}
			*/
		} else {
LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "_Run: handle message");
			mmsg_.Unlock();
			
			Message *pmsg = NULL;
			unsigned deviceid = 0;
	
			while(1) {		
				mmsg_.Lock();
				if(msgqueue_.size() > 0) {
					deviceid = (*msgqueue_.begin()).first;
					pmsg = (*msgqueue_.begin()).second;
					msgqueue_.pop_front();
				}
				mmsg_.Unlock();
				
				if(pmsg != NULL) {
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Routing message to Ruby Interpreter...");
					if (!rootnode_->handleMessage(pmsg))
					{
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "For obscure reasons could not handle the message");
						pmsg->m_bRespondedToMessage = true;
					}
					pmsg = NULL;
				} else {
					break;
				}
			}
LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "_Run: message handling complete");
		}
	}
LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "_Run: stop requested");
	return 0;
}

void 
RubyIOManager::SendCommand(RubyCommandWrapper* pcmd) {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ruby code requested to send command %d, from %d to %d... Sending...", 
								pcmd->getId(), pcmd->getDevIdFrom(), pcmd->getDevIdTo());

	Message* pmsg;
	if( pcmd->getDevIdTo() == DEVICEID_CATEGORY )
		pmsg = new Message(pcmd->getDevIdFrom(), pcmd->getCategory(), true, BL_SameHouse, pcmd->getPriority(), pcmd->getType(), pcmd->getId(), 0);
	else if( pcmd->getDevIdTo() == DEVICEID_MASTERDEVICE )
		pmsg = new Message(pcmd->getDevIdFrom(), pcmd->getTemplate(), BL_SameHouse, pcmd->getPriority(), pcmd->getType(), pcmd->getId(), 0);
	else
		pmsg = new Message(pcmd->getDevIdFrom(), pcmd->getDevIdTo(), pcmd->getPriority(), pcmd->getType(), pcmd->getId(), 0);
	pmsg->m_mapParameters = pcmd->getParams();
	pevdisp_->SendMessage(pmsg);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Command sent.");
}

void 
RubyIOManager::SendReceiveCommand(RubyCommandWrapper* pcmd) {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ruby code requested to send +receive command %d, from %d to %d... Sending...", 
								pcmd->getId(), pcmd->getDevIdFrom(), pcmd->getDevIdTo());

	Message* pmsg;
	if( pcmd->getDevIdTo() == DEVICEID_CATEGORY )
		pmsg = new Message(pcmd->getDevIdFrom(), pcmd->getCategory(), true, BL_SameHouse, pcmd->getPriority(), pcmd->getType(), pcmd->getId(), 0);
	else if( pcmd->getDevIdTo() == DEVICEID_MASTERDEVICE )
		pmsg = new Message(pcmd->getDevIdFrom(), pcmd->getTemplate(), BL_SameHouse, pcmd->getPriority(), pcmd->getType(), pcmd->getId(), 0);
	else
		pmsg = new Message(pcmd->getDevIdFrom(), pcmd->getDevIdTo(), pcmd->getPriority(), pcmd->getType(), pcmd->getId(), 0);
	pmsg->m_mapParameters = pcmd->getParams();
	Message* pResponse = pevdisp_->SendReceiveMessage(pmsg);

	if (pResponse)
	{
	        map<long, string>::iterator it;
		for(it = pResponse->m_mapParameters.begin(); it != pResponse->m_mapParameters.end(); it++) {
	                pcmd->setParam(it->first, it->second.c_str());
		}
	}
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Command sent.");
}

string 
RubyIOManager::SendCommandReceiveString(RubyCommandWrapper* pcmd) {

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ruby code requested to send command %d, from %d to %d, and is processing the RESPONSE... Sending...", 
								pcmd->getId(), pcmd->getDevIdFrom(), pcmd->getDevIdTo());

	Message* pmsg;

	pmsg = new Message( pcmd->getDevIdFrom(), pcmd->getDevIdTo(), pcmd->getPriority(), MESSAGETYPE_DATAPARM_REQUEST, pcmd->getId(), 0 );
	pmsg->m_mapParameters = pcmd->getParams();

	Message *pResponse = pcmdimpl_->m_pEvent->SendReceiveMessage( pmsg );

	if( pResponse )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Received a valid response (%s), returning it.", pResponse->m_mapParameters[pcmd->getId()].c_str() );
		return pResponse->m_mapParameters[pcmd->getId()];
	}
	
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "No valid response received, returning an empty string");
	return "";
}

void 
RubyIOManager::SendMessage(Message* pmsg) {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ruby code sending message...");
	if(!pcmdimpl_->SendMessage(pmsg))
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Failed to send message.");
	else
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Message was sent.");
}

void 
RubyIOManager::SendString(string str) {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ruby code sending string...");
	if(!pcmdimpl_->SendString(str))
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Failed to send string.");
	else
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "String was sent.");
}

/** this is sort of obsolete, since using it (and moving it's definition back to 
the base RubyDCEConnector) whould turn the framework for ruby into a mess */
void
RubyIOManager::SetDeviceData( int PK_Device, int PK_DeviceData, string Value ) {
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ruby code setting device data.");
	pcmdimpl_->SetDeviceDataInDB( PK_Device, PK_DeviceData, Value );
}


bool 
RubyIOManager::handleStartup() {
	try {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Loading generated code...");
		RubyEmbeder::getInstance()->loadCode(&cs_);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Generated code loaded.");
	} catch(RubyException e) {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed loading code: %s.", e.getMessage());
		return false;
	}

	/*start serial pools*/
	if(rootnode_) {
		rootnode_->Init(&cs_);
		if( !rootnode_->handleStartup() )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "RubyIOManager::handleStartup Disabling because initialization failed");
			DCE::CMD_Set_Enable_Status_DT CMD_Set_Enable_Status_DT(pcmdimpl_->m_dwPK_Device,DEVICETEMPLATE_General_Info_Plugin_CONST,BL_SameHouse,pcmdimpl_->m_dwPK_Device,false);
			pcmdimpl_->SendCommand(CMD_Set_Enable_Status_DT);
		}
		rootnode_->handleNoMessage(); // added one iteration processing in order for idle to be called first	
	}
	return true;
}

void
RubyIOManager::handleTerminate() {
	if(rootnode_) {
		rootnode_->handleTerminate();
	}
}

};
