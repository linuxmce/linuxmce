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
//<-dceag-d-b->
#include "Event_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DCERouter.h"
#include "Event.h"
#include "EventInfo.h"
#include "EventInstance.h"
#include "Criteria.h"
#include "CriteriaParm.h"
#include "CriteriaParmNesting.h"

#include "TimedEvent.h"
#include "sunrise.h"

#include "../General_Info_Plugin_Lite/General_Info_Plugin.h"

#define ALARM_TIMED_EVENT	1
#define ALARM_SUNRISE_SUNSET	2


//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Event_Plugin::Event_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Event_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_EventMutex("security")
{
	pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_EventMutex.Init(&m_MutexAttr);

	m_dwID_EventInstance=0;
	m_pTimedEvent_Next=NULL;
	m_pAlarmManager=NULL;
}

//<-dceag-getconfig-b->
bool Event_Plugin::GetConfig()
{
	if( !Event_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

/*
	vector<Row_Criteria *> vectRow_Criteria;
	if(pRow_Installation)
		pRow_Installation->Criteria_FK_Installation_getrows(&vectRow_Criteria);
	for(size_t s=0;s<vectRow_Criteria.size();++s)
	{
		Row_Criteria *pRow_Criteria = vectRow_Criteria[s];
		CriteriaParmNesting *pCriteriaParmNesting = LoadCriteriaParmNesting(NULL,pRow_Criteria->FK_CriteriaParmNesting_getrow());
		Criteria *pCriteria = new Criteria(pRow_Criteria->PK_Criteria_get(),pCriteriaParmNesting);
		m_mapCriteria[pRow_Criteria->PK_Criteria_get()] = pCriteria;
	}
	*/

	DataLayer_JSON *pDataLayer_JSON = (DataLayer_JSON *) m_pRouter->DataLayer();

	{
		struct json_object_iter iter;
		json_object_object_foreachC(pDataLayer_JSON->m_root_json_obj_PM_get(), iter) 
		{
			string sValue = iter.key;

			if(sValue == "CannedEvents")
				ParseCannedEvents(iter.val);
			else if(sValue == "CriteriaParmList")
				ParseCriteriaParmList(iter.val);
		}
	}

	{
		struct json_object_iter iter;
		json_object_object_foreachC(pDataLayer_JSON->m_root_json_obj_NonDevices_get(), iter) 
		{
			string sValue = iter.key;

			if(sValue == "Timer")
				ParseTimers(iter.val);
			else if(sValue == "Event")
				ParseEvents(iter.val);
			else if(sValue == "longitude")
				m_fLongitude = json_object_get_double(iter.val);
			else if(sValue == "latitude")
				m_fLatitude = json_object_get_double(iter.val);
		}
	}

	m_pAlarmManager = new AlarmManager();
    m_pAlarmManager->Start(2);      //4 = number of worker threads

	PLUTO_SAFETY_LOCK(em,m_EventMutex);

	if( m_fLongitude!=0 || m_fLatitude!=0 )
		SetFirstSunriseSunset();

	SetNextTimedEventCallback();
	return true;
}

void Event_Plugin::ParseTimers(struct json_object *json_obj)
{
	struct json_object *obj_timers = json_obj;
	struct json_object_iter iter_timers;
	json_object_object_foreachC(obj_timers, iter_timers) 
	{
		string sTimerID = iter_timers.key;

		StringUtils::Replace(&sTimerID, "PK_Timer_", "");
		int nTimerID = atoi(sTimerID.c_str());
		
		TimedEvent *pTimedEvent = new TimedEvent(nTimerID,iter_timers.val,this);
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Adding timed event %d",nTimerID);
#endif
		if( pTimedEvent->m_mapCommands.empty() )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Timed event %d has no commands",nTimerID);
			delete pTimedEvent;
		}
		else
			m_mapTimedEvent[pTimedEvent->m_ID] = pTimedEvent;
	}
}

void Event_Plugin::ParseCannedEvents(struct json_object *json_obj)
{
	struct json_object *obj_events = json_obj;
	struct json_object_iter iter_events;
	json_object_object_foreachC(obj_events, iter_events) 
	{
		string sCannedEventID = iter_events.key;

		StringUtils::Replace(&sCannedEventID, "PK_CannedEvents_", "");
		int PK_CannedEvents = atoi(sCannedEventID.c_str());

		string sDescription;
		int FK_Event=0;
		bool bIsNot=false,bIsAnd=false;
		struct json_object_iter iter_eventParms;
		json_object_object_foreachC(iter_events.val, iter_eventParms)
		{
			if( iter_eventParms.val->o_type!=json_type_int && iter_eventParms.val->o_type!=json_type_string )
				continue;

			string sKey = iter_eventParms.key;
			if( sKey=="Description" )
				sDescription = json_object_get_string(iter_eventParms.val);
			else if( sKey=="FK_Event" )
				FK_Event = json_object_get_int(iter_eventParms.val);
			else if( sKey=="bIsAnd" )
				bIsAnd = json_object_get_int(iter_eventParms.val)==1;
			else if( sKey=="bIsNot" )
				bIsNot = json_object_get_int(iter_eventParms.val)==1;
		}

		m_mapCannedEvents[PK_CannedEvents] = new CannedEvent(FK_Event,sDescription,bIsNot,bIsAnd);
	}
}

void Event_Plugin::ParseCriteriaParmList(struct json_object *json_obj)
{
	struct json_object *obj_parmlist = json_obj;
	struct json_object_iter iter_parmlist;
	json_object_object_foreachC(obj_parmlist, iter_parmlist) 
	{
		string sCriteriaParmListID = iter_parmlist.key;

		StringUtils::Replace(&sCriteriaParmListID, "PK_CriteriaParmList_", "");
		int PK_CriteriaParmList = atoi(sCriteriaParmListID.c_str());

		int FK_ParameterType=0;
		struct json_object_iter iter_parmlistParms;
		json_object_object_foreachC(iter_parmlist.val, iter_parmlistParms)
		{
			if( iter_parmlistParms.val->o_type!=json_type_int && iter_parmlistParms.val->o_type!=json_type_string )
				continue;

			string sKey = iter_parmlistParms.key;
			if( sKey=="FK_ParameterType" )
				FK_ParameterType = json_object_get_int(iter_parmlistParms.val);
		}

		m_mapCriteriaParmList_ParameterType[PK_CriteriaParmList] = FK_ParameterType;
	}
}

void Event_Plugin::ParseEvents(struct json_object *json_obj)
{
	struct json_object *obj_events = json_obj;
	struct json_object_iter iter_events;
	json_object_object_foreachC(obj_events, iter_events) 
	{
		string sEventID = iter_events.key;

		StringUtils::Replace(&sEventID, "PK_Event_", "");
		int nEventID = atoi(sEventID.c_str());
		
		EventHandler *pEventHandler = new EventHandler(nEventID,this,iter_events.val);

		if( pEventHandler->m_mapCommands.empty() )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Event %d has no commands",nEventID );
			delete pEventHandler;
		}
		else
		{
			ListEventHandler *pListEventHandler = m_mapListEventHandler_Find(pEventHandler->m_PK_Event);
			if( !pListEventHandler )
			{
				pListEventHandler = new ListEventHandler();
  				m_mapListEventHandler[pEventHandler->m_PK_Event] = pListEventHandler;
			}
			pListEventHandler->push_back(pEventHandler);
		}
	}
}

void Event_Plugin::ParseCommands(map<int, Command_Data>& mapCommands, struct json_object *json_obj)
{
	struct json_object *obj_commands = json_obj;
	struct json_object_iter iter_commands;
	json_object_object_foreachC(obj_commands, iter_commands) 
	{
		string sCommand = iter_commands.key;
		StringUtils::Replace(&sCommand, "command_", "");
		int nCommandIndex = atoi(sCommand.c_str());
		Command_Data aCommand_data;

		if(iter_commands.val->o_type == json_type_object)
		{
			struct json_object *obj_command = iter_commands.val;
			struct json_object_iter iter_command;
			json_object_object_foreachC(obj_command, iter_command) 
			{
				string sKey = iter_command.key;

				if(iter_command.val->o_type == json_type_int || iter_command.val->o_type == json_type_string)
				{
					int nValue = json_object_get_int(iter_command.val);

					if(sKey == "Device_From")
						aCommand_data.Device_From(nValue);
					else if(sKey == "Device_To")
						aCommand_data.Device_To(nValue);
					else if(sKey == "FK_Command")
						aCommand_data.PK_Command(nValue);
					else if(sKey == "IsTemporary")
						aCommand_data.IsTemporary(nValue);
					else if(sKey == "CancelIfOther")
						aCommand_data.CancelIfOther(nValue);
					else if(sKey == "Delay")
						aCommand_data.Delay(nValue);
				}
				else if(sKey == "params" && iter_command.val->o_type == json_type_object)
				{
					ParseCommandParameters(aCommand_data.Params(), iter_command.val);
				}
			}
		}

		mapCommands[nCommandIndex] = aCommand_data;
	}
}
//----------------------------------------------------------------------------------------------
void Event_Plugin::ParseCommandParameters(std::map<int, string>& mapParams, struct json_object *json_obj)
{
	struct json_object *obj_params = json_obj;
	struct json_object_iter iter_params;
	json_object_object_foreachC(obj_params, iter_params) 
	{
		string sFK_CommandParameter = iter_params.key;
		StringUtils::Replace(&sFK_CommandParameter, "FK_CommandParameter_", "");
		int nFK_CommandParameter = atoi(sFK_CommandParameter.c_str());
		
		if(iter_params.val->o_type == json_type_int || iter_params.val->o_type == json_type_string)
		{
			string sValue = json_object_get_string(iter_params.val);
			mapParams[nFK_CommandParameter] = sValue;
		}
	}
}
//<-dceag-const2-b->!

CriteriaParmNesting *Event_Plugin::LoadCriteriaParmNesting(CriteriaParmNesting *pCriteriaParmNesting_Parent,Row_CriteriaParmNesting *pRow_CriteriaParmNesting)
{
	/*
	CriteriaParmNesting *pCriteriaParmNesting = new CriteriaParmNesting(pRow_CriteriaParmNesting->IsNot_get()==1,pRow_CriteriaParmNesting->IsAnd_get()==1);
	if( pCriteriaParmNesting_Parent )
		pCriteriaParmNesting_Parent->m_vectCriteriaParmNesting.push_back(pCriteriaParmNesting);
	vector<Row_CriteriaParmNesting *> vectRow_CriteriaParmNesting;
	pRow_CriteriaParmNesting->CriteriaParmNesting_FK_CriteriaParmNesting_Parent_getrows(&vectRow_CriteriaParmNesting);
	for(size_t s=0;s<vectRow_CriteriaParmNesting.size();++s)
	{
		Row_CriteriaParmNesting *pRow_CriteriaParmNesting = vectRow_CriteriaParmNesting[s];
		LoadCriteriaParmNesting(pCriteriaParmNesting,pRow_CriteriaParmNesting);
	}
	vector<Row_CriteriaParm *> vectRow_CriteriaParm;
	pRow_CriteriaParmNesting->CriteriaParm_FK_CriteriaParmNesting_getrows(&vectRow_CriteriaParm);
	for(size_t s=0;s<vectRow_CriteriaParm.size();++s)
	{
		Row_CriteriaParm *pRow_CriteriaParm = vectRow_CriteriaParm[s];

		if(NULL != pRow_CriteriaParm->FK_CriteriaParmList_getrow())
		{
			pCriteriaParmNesting->m_vectCriteriaParm.push_back( new CriteriaParm(
				pRow_CriteriaParm->PK_CriteriaParm_get(),pRow_CriteriaParm->FK_CriteriaParmList_get(),
				pRow_CriteriaParm->FK_CriteriaParmList_getrow()->FK_ParameterType_get(),pRow_CriteriaParm->Operator_get(),
				pRow_CriteriaParm->Value_get(),pRow_CriteriaParm->Parm_get()) );
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "In CriteriaParm we have a wrong value for FK_CriteriaParmList. "
				"PK_CriteriaParam is %d", pRow_CriteriaParm->PK_CriteriaParm_get());
		}
	}

	return pCriteriaParmNesting;
	*/
return NULL;
}

//<-dceag-dest-b->
Event_Plugin::~Event_Plugin()
//<-dceag-dest-e->
{
	for(map<int,ListEventHandler *>::iterator it = m_mapListEventHandler.begin(), end =
		m_mapListEventHandler.end(); it != end; ++it)
	{
		for(list<EventHandler *>::iterator it_event = it->second->begin(), end_event =
			it->second->end(); it_event != end_event; ++it_event)
		{
			delete *it_event;
		}

		delete it->second;
	}
	m_mapListEventHandler.clear();	

	for(map<int,Criteria *>::iterator itc = m_mapCriteria.begin(), endc = m_mapCriteria.end(); itc != endc; ++itc)
		delete itc->second;
	m_mapCriteria.clear();

	for(map<int,CannedEvent *>::iterator it=m_mapCannedEvents.begin();it!=m_mapCannedEvents.end();++it)
		delete it->second;
}

void Event_Plugin::PrepareToDelete()
{
	PLUTO_SAFETY_LOCK(em,m_EventMutex);
	Command_Impl::PrepareToDelete();
	delete m_pAlarmManager;
	m_pAlarmManager = NULL;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Event_Plugin::Register()
//<-dceag-reg-e->
{
	m_pGeneral_Info_Plugin=( General_Info_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_General_Info_Plugin_CONST);

	if( !m_pGeneral_Info_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to Event_Plugin plugin");
		return false;
	}

	RegisterMsgInterceptor((MessageInterceptorFn)(&Event_Plugin::ProcessEvent) ,0,0,0,0,MESSAGETYPE_EVENT,0);
	return Connect(PK_DeviceTemplate_get()); 
}

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Event_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Event_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

bool Event_Plugin::ProcessEvent(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	ListEventHandler *pListEventHandler = m_mapListEventHandler_Find(pMessage->m_dwID);
	if( pListEventHandler==NULL ) // No handlers for this type of event
	{
		LoggerWrapper::GetInstance()->Write(LV_EVENT,"Event #%d has no handlers",pMessage->m_dwID);
		return false;
	}

	EventInfo *pEventInfo = new EventInfo(pMessage->m_dwID,pMessage,(DeviceData_Router *)pDeviceFrom,1 /*m_iPKID_C_HouseMode*/);
//	m_listEventInfo.push_back(pEventInfo);

//	LoggerWrapper::GetInstance()->Write(LV_EVENT,"Event #%d has %d handlers",pEventInfo->m_iPKID_Event,(int)pEvent->m_vectEventHandlers.size());
	for(ListEventHandler::iterator it=pListEventHandler->begin();it!=pListEventHandler->end();++it)
	{
		EventHandler *pEventHandler = *it;
//		pEventInfo->m_vectEventHandlers.push_back(pEventHandler);
//		pEventInfo->pEventHandler = pEventHandler;

		if(pEventHandler->m_mapCommands.empty())
			continue;  // This event handler doesn't have anything to do anyway

		bool bResult = true;  // it's true unless there's a criteria that evaluates to false

		if( pEventHandler->m_OncePerSeconds && pEventHandler->m_tLastFired && time(NULL)-pEventHandler->m_tLastFired<pEventHandler->m_OncePerSeconds )
		{
			LoggerWrapper::GetInstance()->Write(LV_EVENTHANDLER,"Skipping Event Handler: %d last fired %d (time is %d)",pEventHandler->m_PK_EventHander,(int) pEventHandler->m_tLastFired,(int) time(NULL));
			bResult=false;
		}
		else if( pEventHandler->m_pCriteria!=NULL )
		{
			try
			{
				if( !pEventHandler->m_pCriteria->Evaluate(pEventInfo,(void *) m_pRouter) )
					bResult=false;
			}
			catch(exception e)
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Problem with criteria for Event ID: %d - result: %s",pEventHandler->m_PK_EventHander,"??");
				bResult=false;
			}
		}

		LoggerWrapper::GetInstance()->Write(LV_EVENTHANDLER,"Evaluated Event Handler: %d to: %d once per: %d last fired %d (time is %d)",
			pEventHandler->m_PK_EventHander,(int) bResult,
			pEventHandler->m_OncePerSeconds,(int) pEventHandler->m_tLastFired,(int) time(NULL));
		if( bResult ) 
		{
			EventInstance *pEventInstance = new EventInstance(m_dwID_EventInstance++,pEventHandler);
			pEventInstance->pMessage = new Message(pMessage);
			pEventInstance->m_PKID_Device_OriginatedThisEvent = pMessage->m_dwPK_Device_From;
			pEventHandler->m_tLastFired = time(NULL);
			ExecuteEvent(pEventInstance);
		}
	}

	delete pEventInfo;
	return false;
}

void Event_Plugin::ExecuteEvent(EventInstance *pEventInstance)
{
	m_pGeneral_Info_Plugin->ExecuteCommandData(&(pEventInstance->m_ptrEventHandler->m_mapCommands),m_dwPK_Device);
	delete pEventInstance;  // We will probably need to keep this for a while for some events like security problems
}

void Event_Plugin::SetNextTimedEventCallback()
{
	PLUTO_SAFETY_LOCK(em,m_EventMutex);
	if( m_mapTimedEvent.size()==0 )
	{
		m_pTimedEvent_Next=NULL;
		return;
	}

	m_pTimedEvent_Next=NULL;
	for(MapTimedEvent::iterator it=m_mapTimedEvent.begin();it!=m_mapTimedEvent.end();++it)
	{
		TimedEvent *pTimedEvent = (*it).second;
		if( pTimedEvent->m_tTime==0 )
			continue;

		if( !m_pTimedEvent_Next )
			m_pTimedEvent_Next = pTimedEvent;

		if( pTimedEvent->m_tTime < m_pTimedEvent_Next->m_tTime )
			m_pTimedEvent_Next = pTimedEvent;
	}

	if( m_pTimedEvent_Next )
	{
		m_pAlarmManager->AddAbsoluteAlarm( m_pTimedEvent_Next->m_tTime, this, ALARM_TIMED_EVENT, (void *) m_pTimedEvent_Next );
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Timer: next event is %s in %d seconds",
			m_pTimedEvent_Next->m_sDescription.c_str(),
			m_pTimedEvent_Next->m_tTime - time(NULL));
	}
}

void Event_Plugin::AlarmCallback(int id, void* param)
{
	PLUTO_SAFETY_LOCK(em,m_EventMutex);
	if( id==ALARM_TIMED_EVENT )
	{
		TimedEvent *pTimedEvent = (TimedEvent *) param;

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Timer: %s firing",
			pTimedEvent->m_sDescription.c_str());

		m_pGeneral_Info_Plugin->ExecuteCommandData(&(pTimedEvent->m_mapCommands),m_dwPK_Device);
		pTimedEvent->CalcNextTime();
		SetNextTimedEventCallback();
	}
	else if( id==ALARM_SUNRISE_SUNSET )
		FireSunriseSunsetEvent();
}


//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/



//<-dceag-c263-b->

	/** @brief COMMAND: #263 - Toggle Event Handler */
	/** Turns an event handler on/off by toggling the 'disabled' flag. */
		/** @param #107 PK_EventHandler */
			/** The event handler to toggle. */

void Event_Plugin::CMD_Toggle_Event_Handler(int iPK_EventHandler,string &sCMD_Result,Message *pMessage)
//<-dceag-c263-e->
{
	/*
	Row_EventHandler *pRow_EventHandler = m_pDatabase_pluto_main->EventHandler_get()->GetRow(iPK_EventHandler);
	if( !pRow_EventHandler )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot toggle event handler: %d",iPK_EventHandler);
		return;
	}
	pRow_EventHandler->Disabled_set( pRow_EventHandler->Disabled_get()!=1 );
	m_pDatabase_pluto_main->EventHandler_get()->Commit();
	*/
}
//<-dceag-createinst-b->!

void Event_Plugin::SetFirstSunriseSunset()
{
	m_bIsDaytime=true;
	m_tNextSunriseSunset=0;
	if( m_fLongitude==0 && m_fLatitude==0 )
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"No sunrise/sunset since no city or longitude/latitude set");
	else
	{
		time_t tNow=time(NULL);
		time_t tSunrise,tSunset,tSunriseTomorrow,tSunsetTomorrow;
		GetSunriseSunset(tSunrise,tSunset,tSunriseTomorrow,tSunsetTomorrow,m_fLatitude,m_fLongitude);

		//chris m. 12/12/05: asctime is crashing - don't have time to debug! :(
		//probably, localtime returns NULL
		tm tmm;
		localtime_r(&tSunrise,&tmm);

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"tSunrise: %s",asctime(localtime(&tSunrise)));
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"tSunset: %s",asctime(localtime(&tSunset)));
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"tSunriseTomorrow: %s",asctime(localtime(&tSunriseTomorrow)));
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"tSunsetTomorrow: %s",asctime(localtime(&tSunsetTomorrow)));

		if( tSunset>tSunrise )  // I think this is always the case unless in the Arctic somewhere sunset is at 1am and sunrise at 2am????
		{
			if( tNow<tSunrise )
			{
				m_bIsDaytime=false;
				m_tNextSunriseSunset=tSunrise;
			}
			else if( tNow>=tSunrise && tNow<tSunset )
				m_tNextSunriseSunset=tSunset;
			else if( tNow>=tSunset && tNow<tSunriseTomorrow )
			{
				m_bIsDaytime=false;
				m_tNextSunriseSunset=tSunriseTomorrow;
			}
			else
				m_tNextSunriseSunset=tSunsetTomorrow; // Don't think this is possible
		}
		else // Don't think this is possible
		{
			if( tNow<tSunset )
				m_tNextSunriseSunset=tSunset;
			else if( tNow>=tSunset && tNow<tSunrise )
			{
				m_bIsDaytime=false;
				m_tNextSunriseSunset=tSunrise;
			}
			else if( tNow>=tSunrise && tNow<tSunsetTomorrow )
				m_tNextSunriseSunset=tSunsetTomorrow;
			else
			{
				m_bIsDaytime=false;
				m_tNextSunriseSunset=tSunriseTomorrow; // Don't think this is possible
			}
		}

		int Seconds = m_tNextSunriseSunset-tNow;
		int Hours = Seconds / 3600;
		Seconds -= Hours * 3600;
		int Minutes = Seconds / 60;
		Seconds -= Minutes * 60;
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Currently %s next event in %d:%d:%d",
			(m_bIsDaytime ? "daytime" : "night"), Hours, Minutes, Seconds);

		m_pAlarmManager->AddAbsoluteAlarm( m_tNextSunriseSunset, this, ALARM_SUNRISE_SUNSET, NULL );
	}
}

void Event_Plugin::FireSunriseSunsetEvent()
{
	PLUTO_SAFETY_LOCK(em,m_EventMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Event_Plugin::FireSunriseSunsetEvent: time %d daytime %d", (int) time(NULL), (int) m_bIsDaytime);

	m_bIsDaytime=!m_bIsDaytime;
	if( m_bIsDaytime )
		EVENT_Sunrise();
	else
		EVENT_Sunset();

	time_t tNow=time(NULL);
	time_t tSunrise,tSunset,tSunriseTomorrow,tSunsetTomorrow;
	GetSunriseSunset(tSunrise,tSunset,tSunriseTomorrow,tSunsetTomorrow,m_fLatitude,m_fLongitude);

	if( m_bIsDaytime )
		m_tNextSunriseSunset= tSunset>tNow ? tSunset : tSunsetTomorrow;
	else
		m_tNextSunriseSunset= tSunrise>tNow ? tSunrise : tSunriseTomorrow;

	m_pAlarmManager->AddAbsoluteAlarm( m_tNextSunriseSunset, this, ALARM_SUNRISE_SUNSET, NULL );

}
