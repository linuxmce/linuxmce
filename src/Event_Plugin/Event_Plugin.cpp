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
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Table_Criteria.h"
#include "pluto_main/Table_CriteriaParm.h"
#include "pluto_main/Table_CriteriaParmList.h"
#include "pluto_main/Table_CriteriaParmNesting.h"
#include "pluto_main/Table_EventHandler.h"
#include "pluto_main/Table_City.h"
#include "pluto_main/Define_DataGrid.h"

#include "DataGrid.h"
#include "TimedEvent.h"
#include "sunrise.h"

#define ALARM_TIMED_EVENT	1
#define ALARM_SUNRISE_SUNSET	2
#define ALARM_CHECK_DST         3

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
	m_pDatabase_pluto_main=NULL;
	m_pAlarmManager=NULL;
	m_isDst = -1;
}

//<-dceag-getconfig-b->
bool Event_Plugin::GetConfig()
{
	if( !Event_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

    m_pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());
    if( !m_pDatabase_pluto_main->Connect( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), m_pRouter->sDBName_get( ), m_pRouter->iDBPort_get( ) ) )
    {
        LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot connect to database!" );
        m_bQuit_set(true);
        return false;
    }
    m_pAlarmManager = new AlarmManager();
    m_pAlarmManager->Start(2);      //4 = number of worker threads

    time_t t=time(NULL);
    struct tm tm_DstNow;
    localtime_r(&t,&tm_DstNow);
    m_isDst = tm_DstNow.tm_isdst;
    
    m_pAlarmManager->AddRelativeAlarm(60,this,ALARM_CHECK_DST,NULL);

    Initialize(false);
	return true;
}

void Event_Plugin::Initialize(bool bReload)
{
	Row_Installation *pRow_Installation = m_pDatabase_pluto_main->Installation_get()->GetRow( m_pRouter->iPK_Installation_get() );
	if (bReload)
		pRow_Installation->Reload();

	vector<Row_Criteria *> vectRow_Criteria;
	if(pRow_Installation)
		pRow_Installation->Criteria_FK_Installation_getrows(&vectRow_Criteria);
	for(size_t s=0;s<vectRow_Criteria.size();++s)
	{
		Row_Criteria *pRow_Criteria = vectRow_Criteria[s];
		if (bReload) {
			pRow_Criteria->Reload();
		}
		
		CriteriaParmNesting *pCriteriaParmNesting = LoadCriteriaParmNesting(NULL,pRow_Criteria->FK_CriteriaParmNesting_getrow(), bReload);
		Criteria *pCriteria = new Criteria(pRow_Criteria->PK_Criteria_get(),pCriteriaParmNesting);
		m_mapCriteria[pRow_Criteria->PK_Criteria_get()] = pCriteria;
	}

	vector<Row_EventHandler *> vectRow_EventHandler;
	if(pRow_Installation)
		pRow_Installation->EventHandler_FK_Installation_getrows(&vectRow_EventHandler);
	for(size_t s=0;s<vectRow_EventHandler.size();++s)
	{
		Row_EventHandler *pRow_EventHandler = vectRow_EventHandler[s];
		if (bReload)
			pRow_EventHandler->Reload();

		if( pRow_EventHandler->TimedEvent_get() )
		{
			// Load all timed events, even disabled ones - this is needed to be able to enable/disable them from the orbiter
			TimedEvent *pTimedEvent = new TimedEvent(pRow_EventHandler, m_mapCriteria_Find(pRow_EventHandler->FK_Criteria_get()));
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Adding timed event %d",pRow_EventHandler->PK_EventHandler_get());
#endif
			// TODO: as the commands are kept in the router itself, we wont get the most up to date values using this approach
			pTimedEvent->m_pCommandGroup = m_pRouter->m_mapCommandGroup_Find(pRow_EventHandler->FK_CommandGroup_get());
			if( !pTimedEvent->m_pCommandGroup )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Timed event %d has no commands",pRow_EventHandler->PK_EventHandler_get());
				delete pTimedEvent;
			}
			else
				m_mapTimedEvent[pTimedEvent->m_pRow_EventHandler->PK_EventHandler_get()] = pTimedEvent;
		}
		else
		{
			if( pRow_EventHandler->Disabled_get() )
				continue;

			EventHandler *pEventHandler = new EventHandler(pRow_EventHandler->PK_EventHandler_get(),
				pRow_EventHandler->FK_Event_get(),m_pRouter->m_mapCommandGroup_Find(pRow_EventHandler->FK_CommandGroup_get()),
				m_mapCriteria_Find(pRow_EventHandler->FK_Criteria_get()),pRow_EventHandler->OncePerSeconds_get());

			ListEventHandler *pListEventHandler = m_mapListEventHandler_Find(pRow_EventHandler->FK_Event_get());
			if( !pListEventHandler )
			{
				pListEventHandler = new ListEventHandler();
  				m_mapListEventHandler[pRow_EventHandler->FK_Event_get()] = pListEventHandler;
			}
			pListEventHandler->push_back(pEventHandler);
		}
	}
	m_pAlarmManager->Clear();

	PLUTO_SAFETY_LOCK(em,m_EventMutex);
	m_fLongitude = DATA_Get_Longitude();
	m_fLatitude = DATA_Get_Latitude();

	if( m_fLongitude==0 && m_fLatitude==0 && m_pRouter->m_pRow_Installation_get() )
	{
		Row_City *pRow_City = m_pRouter->m_pRow_Installation_get()->FK_City_getrow();
		if( pRow_City )
		{
			m_fLongitude = pRow_City->Longitude_get();
			m_fLatitude = pRow_City->Latitude_get();
		}
	}

	if( m_fLongitude!=0 || m_fLatitude!=0 )
		SetFirstSunriseSunset();
	
	SetNextTimedEventCallback();
}

//<-dceag-const2-b->!

CriteriaParmNesting *Event_Plugin::LoadCriteriaParmNesting(CriteriaParmNesting *pCriteriaParmNesting_Parent,Row_CriteriaParmNesting *pRow_CriteriaParmNesting, bool bReload)
{
	if (bReload)
		pRow_CriteriaParmNesting->Reload();

	CriteriaParmNesting *pCriteriaParmNesting = new CriteriaParmNesting(pRow_CriteriaParmNesting->IsNot_get()==1,pRow_CriteriaParmNesting->IsAnd_get()==1);
	if( pCriteriaParmNesting_Parent )
		pCriteriaParmNesting_Parent->m_vectCriteriaParmNesting.push_back(pCriteriaParmNesting);
	vector<Row_CriteriaParmNesting *> vectRow_CriteriaParmNesting;
	pRow_CriteriaParmNesting->CriteriaParmNesting_FK_CriteriaParmNesting_Parent_getrows(&vectRow_CriteriaParmNesting);
	for(size_t s=0;s<vectRow_CriteriaParmNesting.size();++s)
	{
		Row_CriteriaParmNesting *pRow_CriteriaParmNesting = vectRow_CriteriaParmNesting[s];
		if (bReload)
			pRow_CriteriaParmNesting->Reload();
		LoadCriteriaParmNesting(pCriteriaParmNesting,pRow_CriteriaParmNesting, bReload);
	}
	vector<Row_CriteriaParm *> vectRow_CriteriaParm;
	pRow_CriteriaParmNesting->CriteriaParm_FK_CriteriaParmNesting_getrows(&vectRow_CriteriaParm);
	for(size_t s=0;s<vectRow_CriteriaParm.size();++s)
	{
		Row_CriteriaParm *pRow_CriteriaParm = vectRow_CriteriaParm[s];
		if (bReload)
			pRow_CriteriaParm->Reload();

		if(NULL != pRow_CriteriaParm->FK_CriteriaParmList_getrow())
		{
			if (bReload)
				pRow_CriteriaParm->FK_CriteriaParmList_getrow()->Reload();

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
}

//<-dceag-dest-b->
Event_Plugin::~Event_Plugin()
//<-dceag-dest-e->
{
	delete m_pDatabase_pluto_main;
	m_pDatabase_pluto_main = NULL;

	DeleteMembers();
}

void Event_Plugin::DeleteMembers()
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

	for(map<int,TimedEvent *>::iterator it = m_mapTimedEvent.begin(), end =
		m_mapTimedEvent.end(); it != end; ++it)
	{
	        delete it->second;
	}
	m_mapTimedEvent.clear();
	
	for(map<int,Criteria *>::iterator itc = m_mapCriteria.begin(), endc = m_mapCriteria.end(); itc != endc; ++itc)
		delete itc->second;
	m_mapCriteria.clear();

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
	m_pDatagrid_Plugin=( Datagrid_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Datagrid_Plugin_CONST);
	if( !m_pDatagrid_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to event plugin");
		return false;
	}

    m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &Event_Plugin::AlarmsInRoom) )
        , DATAGRID_Alarms_In_Room_CONST,PK_DeviceTemplate_get() );

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

void Event_Plugin::GetHouseModes(Message* pMessage) {

	PLUTO_SAFETY_LOCK(em,m_EventMutex);
        // find PK_Device of security plugin
	map<int,string> mapDevices;
	GetDevicesByTemplate(DEVICETEMPLATE_Security_Plugin_CONST, &mapDevices);
	int PK_SecurityPlugin;
	if( mapDevices.begin() == mapDevices.end() )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to event plugin (Security Plugin)");
		return;
	} else {
	        PK_SecurityPlugin = mapDevices.begin()->first;
	}

	Row_Device_DeviceData *pRow_Device_DeviceData =
	        m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(PK_SecurityPlugin,DEVICEDATA_Configuration_CONST);
	if( !pRow_Device_DeviceData )
	        return;

	pRow_Device_DeviceData->Reload();
	string sData=pRow_Device_DeviceData->IK_DeviceData_get();

	// decode int list to int,int map
        m_mapPK_HouseMode.clear();
	string::size_type pos=0;
	while( pos<sData.size() && pos!=string::npos )
	{
	        int PK_DeviceGroup = atoi( StringUtils::Tokenize(sData,",",pos).c_str() );
		m_mapPK_HouseMode[PK_DeviceGroup]=atoi( StringUtils::Tokenize(sData,",",pos).c_str() );
	}
	if (pMessage->m_dwID == EVENT_House_Mode_Changed_CONST)
	{
		int iPK_DeviceGroup = atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_DeviceGroup_CONST].c_str());
		int iPrevValue = atoi(pMessage->m_mapParameters[EVENTPARAMETER_Previous_Value_CONST].c_str());
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"House mode changed event, previous house mode = %d", iPrevValue);
		m_mapPK_HouseMode[iPK_DeviceGroup] = iPrevValue;
	}
}

bool Event_Plugin::ProcessEvent(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	PLUTO_SAFETY_LOCK(em,m_EventMutex);
	ListEventHandler *pListEventHandler = m_mapListEventHandler_Find(pMessage->m_dwID);
	if( pListEventHandler==NULL ) // No handlers for this type of event
	{
		LoggerWrapper::GetInstance()->Write(LV_EVENT,"Event #%d has no handlers",pMessage->m_dwID);
		return false;
	}

	GetHouseModes(pMessage); // Update house modes
	EventInfo *pEventInfo = new EventInfo(pMessage->m_dwID,pMessage,(DeviceData_Router *)pDeviceFrom, m_mapPK_HouseMode[0]);
//	m_listEventInfo.push_back(pEventInfo);

//	LoggerWrapper::GetInstance()->Write(LV_EVENT,"Event #%d has %d handlers",pEventInfo->m_iPK_Event,(int)pEventInfo->m_vectEventHandlers.size());
	for(ListEventHandler::iterator it=pListEventHandler->begin();it!=pListEventHandler->end();++it)
	{
		EventHandler *pEventHandler = *it;
//		pEventInfo->m_vectEventHandlers.push_back(pEventHandler);
//		pEventInfo->pEventHandler = pEventHandler;

		if(pEventHandler->m_pCommandGroup==NULL) {
			LoggerWrapper::GetInstance()->Write(LV_EVENT,"no command group for this eventhandler");
			continue;  // This event handler doesn't have anything to do anyway
		}

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
	                        LoggerWrapper::GetInstance()->Write(LV_DEBUG,"Trying to Evaluate the Criteria for this event");
				if( !pEventHandler->m_pCriteria->Evaluate(pEventInfo,(void *) m_pRouter) )
					bResult=false;
			}
			catch(exception e)
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Problem with criteria for Event ID: %d - result: %s",pEventHandler->m_PK_EventHander,"??");
				bResult=false;
			}
		}
                LoggerWrapper::GetInstance()->Write(LV_EVENTHANDLER,"Criteria probably evaluated to %i", bResult);

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
	ExecCommandGroup(pEventInstance->m_ptrEventHandler->m_pCommandGroup->m_PK_CommandGroup);
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
			m_pTimedEvent_Next->m_pRow_EventHandler->Description_get().c_str(),
			m_pTimedEvent_Next->m_tTime - time(NULL));
	}
}

void Event_Plugin::AlarmCallback(int id, void* param)
{
	PLUTO_SAFETY_LOCK(em,m_EventMutex);
	if( id==ALARM_TIMED_EVENT )
	{
		TimedEvent *pTimedEvent = (TimedEvent *) param;

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Timer: %s timed out",
						    pTimedEvent->m_pRow_EventHandler->Description_get().c_str());
 
		if ( !pTimedEvent->m_pRow_EventHandler->Disabled_get() )
		{
			bool bResult = true;
			if( pTimedEvent->m_pCriteria!=NULL )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Timer: %s - checking criteria.");
				// Create some fake message and and use the event plugin as from device to avoid passing NULLs
				Message* pMessage = new Message();
				GetHouseModes(pMessage); // Update house modes
				EventInfo *pEventInfo = new EventInfo(m_dwPK_Device, pMessage , (DeviceData_Router *)this, m_mapPK_HouseMode[0]);
				pEventInfo->m_bTimedEvent = true;
				try
				{
					if( !pTimedEvent->m_pCriteria->Evaluate(pEventInfo,(void *) m_pRouter) )
						bResult=false;
				}
				catch(exception e)
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Problem with criteria for timed event : %s - result: %s",pTimedEvent->m_pRow_EventHandler->Description_get().c_str(),"??");
					bResult=false;
				}
			}
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Timer: criteria evaluated as : %d", bResult);
		
			if (bResult)
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Timer: %s firing command group %d",
								    pTimedEvent->m_pRow_EventHandler->Description_get().c_str(),pTimedEvent->m_pCommandGroup->m_PK_CommandGroup);
				ExecCommandGroup(pTimedEvent->m_pCommandGroup->m_PK_CommandGroup);
			}
		}
		pTimedEvent->CalcNextTime();
		SetNextTimedEventCallback();
	}
	else if( id==ALARM_SUNRISE_SUNSET )
		FireSunriseSunsetEvent();
	else if ( id==ALARM_CHECK_DST )
	  {
	    time_t t=time(NULL);
	    struct tm tm_DstNow;
	    localtime_r(&t,&tm_DstNow);
	    int isDst = tm_DstNow.tm_isdst;
	    if (isDst != m_isDst)
	      {
		// DST has changed, reload everything.
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Daylight Savings time has changed. Reloading events.");
		Initialize(true);
	      }
	    m_isDst = isDst;
	    m_pAlarmManager->AddRelativeAlarm(1,this,ALARM_CHECK_DST,NULL);
	  }
}

class DataGridTable *Event_Plugin::AlarmsInRoom( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    DataGridTable *pDataGrid = new DataGridTable();	
    DataGridCell *pCell = NULL;

	if( Parms.length()==0 )
		return pDataGrid;

	string sSQL = "JOIN CommandGroup ON EventHandler.FK_CommandGroup=PK_CommandGroup JOIN CommandGroup_Room ON CommandGroup_Room.FK_CommandGroup=PK_CommandGroup WHERE TimedEvent IS NOT NULL AND FK_Room=" + Parms;
	vector<Row_EventHandler *> vectRow_EventHandler;
	m_pDatabase_pluto_main->EventHandler_get()->GetRows(sSQL,&vectRow_EventHandler);
	int iRow=0;
	for(size_t s=0;s<vectRow_EventHandler.size();++s)
	{
		Row_EventHandler *pRow_EventHandler = vectRow_EventHandler[s];
		TimedEvent *pTimedEvent = m_mapTimedEvent_Find(pRow_EventHandler->PK_EventHandler_get());

		string sDescription;
		switch(pRow_EventHandler->TimedEvent_get())
		{
		case INTERVAL_EVENT:
			sDescription = "Interval";
			break;
		case DAY_OF_WEEK:
			sDescription = "Day of Week";
			break;
		case DAY_OF_MONTH:
			sDescription = "Day of Month";
			break;
		case ABSOLUTE_TIME:
			sDescription = "Absolute Time";
			break;
		}

		string sTime;
		vector<Row_CriteriaParm *> vectRow_CriteriaParm;
		pRow_EventHandler->FK_Criteria_getrow()->FK_CriteriaParmNesting_getrow()->CriteriaParm_FK_CriteriaParmNesting_getrows(&vectRow_CriteriaParm);
		for(size_t s=0;s<vectRow_CriteriaParm.size();++s)
		{
			Row_CriteriaParm *pRow_CriteriaParm = vectRow_CriteriaParm[s];
			if( pRow_CriteriaParm->FK_CriteriaParmList_get()==CRITERIAPARMLIST_Time_of_day_CONST )
				sTime += " " + pRow_CriteriaParm->Value_get();
			else if( pRow_CriteriaParm->FK_CriteriaParmList_get()==CRITERIAPARMLIST_Day_Of_Week_CONST )
			{
				string::size_type pos=0;
				string sDow = pRow_CriteriaParm->Value_get();
				while( pos<sDow.length() )
					sDescription += StringUtils::GetDow( atoi(StringUtils::Tokenize(sDow,",",pos).c_str()) ) + " ";
			}
			else if( pRow_CriteriaParm->FK_CriteriaParmList_get()==CRITERIAPARMLIST_Day_Of_Month_CONST )
				sDescription += " " + pRow_CriteriaParm->Value_get();
		}

		if( pTimedEvent )
		{
			int Minutes = (pTimedEvent->m_tTime - time(NULL))/60;
			sTime += " left: " + StringUtils::itos( Minutes / 60 ) + ":" + StringUtils::itos( Minutes % 60 );
		}
		pCell = new DataGridCell(pRow_EventHandler->Disabled_get()==1 ? "" : "ON",StringUtils::itos(pRow_EventHandler->PK_EventHandler_get()));
		pDataGrid->SetData(0,iRow,pCell);

		pCell = new DataGridCell(pRow_EventHandler->Description_get() + "\n" + sTime + "\n" + sDescription,StringUtils::itos(pRow_EventHandler->PK_EventHandler_get()));
		pCell->m_Colspan = 3;
		pDataGrid->SetData(1,iRow++,pCell);
	}

	return pDataGrid;
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
	Row_EventHandler *pRow_EventHandler = m_pDatabase_pluto_main->EventHandler_get()->GetRow(iPK_EventHandler);
	if( !pRow_EventHandler )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot toggle event handler: %d",iPK_EventHandler);
		return;
	}
	pRow_EventHandler->Disabled_set( pRow_EventHandler->Disabled_get()!=1 );
	m_pDatabase_pluto_main->EventHandler_get()->Commit();
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
//<-dceag-c757-b->

	/** @brief COMMAND: #757 - Download Configuration */
	/** Request event plugint to reload its configuration */
		/** @param #9 Text */
			/** Any information the device may want to do the download */

void Event_Plugin::CMD_Download_Configuration(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c757-e->
{
	PLUTO_SAFETY_LOCK(em,m_EventMutex);
	DeleteMembers();
	Initialize(true);
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Event_Plugin::CMD_Download_Configuration() : Done");
}
