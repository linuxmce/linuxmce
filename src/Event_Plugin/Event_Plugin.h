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
#include "pluto_main/Database_pluto_main.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"

class Criteria;
class CriteriaParmNesting;
class Row_CriteriaParmNesting;
class EventInstance;

//<-dceag-decl-b->!
namespace DCE
{	
	class Event_Plugin : public Event_Plugin_Command, public AlarmEvent, public DataGridGeneratorPlugIn
	{
//<-dceag-decl-e->
		// Private member variables
		pluto_pthread_mutex_t m_EventMutex;
		pthread_mutexattr_t m_MutexAttr; /** < make it recursive */
		class AlarmManager *m_pAlarmManager;
	    Database_pluto_main *m_pDatabase_pluto_main;
		map<int,Criteria *> m_mapCriteria;
		map<int,ListEventHandler *> m_mapListEventHandler;
		MapTimedEvent m_mapTimedEvent;
		TimedEvent *m_mapTimedEvent_Find(int PK_TimedEvent) { MapTimedEvent::iterator it = m_mapTimedEvent.find(PK_TimedEvent); return it==m_mapTimedEvent.end() ? NULL : (*it).second; }
		ListEventHandler *m_mapListEventHandler_Find(int PK_Event) { map<int,ListEventHandler *>::iterator it = m_mapListEventHandler.find(PK_Event); return it==m_mapListEventHandler.end() ? NULL : (*it).second; }
		Criteria *m_mapCriteria_Find(int PK_Criteria) { map<int,class Criteria *>::iterator it = m_mapCriteria.find(PK_Criteria); return it==m_mapCriteria.end() ? NULL : (*it).second; }
		int m_dwID_EventInstance;
		TimedEvent *m_pTimedEvent_Next;
	    class Datagrid_Plugin *m_pDatagrid_Plugin;
		double m_fLongitude,m_fLatitude;
		bool m_bIsDaytime;
		time_t m_tNextSunriseSunset;
		map<int,int> m_mapPK_HouseMode;
		int m_isDst;

		// Private methods
		void GetHouseModes(Message* pMessage);
		void DeleteMembers();
		void Initialize(bool reload);
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

		virtual void PrepareToDelete();
		bool ProcessEvent(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo);
		CriteriaParmNesting *LoadCriteriaParmNesting(CriteriaParmNesting *pCriteriaParmNesting_Parent,Row_CriteriaParmNesting *pRow_CriteriaParmNesting, bool bReload);
		void ExecuteEvent(EventInstance *pEventInstance);
        void AlarmCallback(int id, void* param);
		void SetNextTimedEventCallback();

		void SetFirstSunriseSunset();
		void FireSunriseSunsetEvent();

		bool IsDaytime() { return m_bIsDaytime; }

		// Datagrid
		class DataGridTable *AlarmsInRoom( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage );

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


	/** @brief COMMAND: #757 - Download Configuration */
	/** Request event plugint to reload its configuration */
		/** @param #9 Text */
			/** Any information the device may want to do the download */

	virtual void CMD_Download_Configuration(string sText) { string sCMD_Result; CMD_Download_Configuration(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Download_Configuration(string sText,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
