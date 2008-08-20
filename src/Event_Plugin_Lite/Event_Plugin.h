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
#ifndef Event_Plugin_h
#define Event_Plugin_h

//	DCE Implemenation for #52 Event Plug-in

#include "Gen_Devices/Event_PluginBase.h"
//<-dceag-d-e->

#include "EventHandler.h"
#include "AlarmManager.h"
#include "TimedEvent.h"

class Criteria;
class CriteriaParmNesting;
class EventInstance;

//<-dceag-decl-b->!
namespace DCE
{
	class CannedEvent
	{
	public:
		int m_FK_Event;
		string m_sDescription;
		bool m_bIsNot,m_bIsAnd;

		CannedEvent(int FK_Event, string sDescription, bool bIsNot, bool bIsAnd)
		{
			m_FK_Event=FK_Event;
			m_sDescription=sDescription;
			m_bIsNot=bIsNot;
			m_bIsAnd=bIsAnd;
		}
	};

	class UserNotification
	{
	public:
		int m_PK_Users;
		string m_sName,m_sPhone,m_sEmail,m_sVoicePhone;
		bool m_bNotifySms,m_bNotifyEmail,m_bNotifyVoice;

		UserNotification(int PK_Users) 
		{ 
			m_PK_Users=PK_Users; 
			m_bNotifySms=m_bNotifyEmail=m_bNotifyVoice=false;
		}
	};

	class Event_Plugin : public Event_Plugin_Command, public AlarmEvent
	{
//<-dceag-decl-e->
		// Private member variables
		pluto_pthread_mutex_t m_EventMutex;
		pthread_mutexattr_t m_MutexAttr; /** < make it recursive */
		class AlarmManager *m_pAlarmManager;
		map<int,Criteria *> m_mapCriteria;
		map<int,ListEventHandler *> m_mapListEventHandler;
		MapTimedEvent m_mapTimedEvent;
		TimedEvent *m_mapTimedEvent_Find(int PK_TimedEvent) { MapTimedEvent::iterator it = m_mapTimedEvent.find(PK_TimedEvent); return it==m_mapTimedEvent.end() ? NULL : (*it).second; }
		ListEventHandler *m_mapListEventHandler_Find(int PK_Event) { map<int,ListEventHandler *>::iterator it = m_mapListEventHandler.find(PK_Event); return it==m_mapListEventHandler.end() ? NULL : (*it).second; }
		Criteria *m_mapCriteria_Find(int PK_Criteria) { map<int,class Criteria *>::iterator it = m_mapCriteria.find(PK_Criteria); return it==m_mapCriteria.end() ? NULL : (*it).second; }
		int m_dwID_EventInstance;
		TimedEvent *m_pTimedEvent_Next;
		double m_fLongitude,m_fLatitude;
		bool m_bIsDaytime;
		time_t m_tNextSunriseSunset;
		map<int,CannedEvent *> m_mapCannedEvents;
		map<int,int> m_mapCriteriaParmList_ParameterType;  // Map of criteriaparmlist to parameter type
		class General_Info_Plugin *m_pGeneral_Info_Plugin;
		map<int,UserNotification *> m_mapUserNotification;

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Event_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Event_Plugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		void ParseTimers(struct json_object *json_obj);
		void ParseEvents(struct json_object *json_obj);
		void ParseUsers(struct json_object *json_obj);
		void ParseCannedEvents(struct json_object *json_obj);
		void ParseCriteriaParmList(struct json_object *json_obj);
		void ParseCommands(map<int, Command_Data>& mapCommands, struct json_object *json_obj);
		void ParseCommandParameters(std::map<int, string>& mapParams, struct json_object *json_obj);

		virtual void PrepareToDelete();
		bool ProcessEvent(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
		CriteriaParmNesting *LoadCriteriaParmNesting(CriteriaParmNesting *pCriteriaParmNesting_Parent,class Row_CriteriaParmNesting *pRow_CriteriaParmNesting);
		void ExecuteEvent(EventInstance *pEventInstance);
        void AlarmCallback(int id, void* param);
		void SetNextTimedEventCallback();

		void SetFirstSunriseSunset();
		void FireSunriseSunsetEvent();
		void HandleAlertNotification(EventInstance *pEventInstance);

		bool IsDaytime() { return m_bIsDaytime; }

		CannedEvent *m_mapCannedEvents_Find(int PK_CannedEvent) { map<int,CannedEvent *>::iterator it = m_mapCannedEvents.find(PK_CannedEvent); return it==m_mapCannedEvents.end() ? NULL : (*it).second; }
		int m_mapCriteriaParmList_ParameterType_Find(int PK_CriteriaParmList) { map<int,int>::iterator it = m_mapCriteriaParmList_ParameterType.find(PK_CriteriaParmList); return it==m_mapCriteriaParmList_ParameterType.end() ? NULL : (*it).second; }

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_PK_City();
	double DATA_Get_Longitude();
	double DATA_Get_Latitude();

			*****EVENT***** accessors inherited from base class
	void EVENT_Sunrise();
	void EVENT_Sunset();

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #263 - Toggle Event Handler */
	/** Turns an event handler on/off by toggling the 'disabled' flag. */
		/** @param #107 PK_EventHandler */
			/** The event handler to toggle. */

	virtual void CMD_Toggle_Event_Handler(int iPK_EventHandler) { string sCMD_Result; CMD_Toggle_Event_Handler(iPK_EventHandler,sCMD_Result,NULL);};
	virtual void CMD_Toggle_Event_Handler(int iPK_EventHandler,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
