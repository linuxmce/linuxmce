/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/** @file DCERouter.h
 The DCE Router class.
 */

#ifndef Router_H
#define Router_H

#include "SocketListener.h"
#include "DCE/Logger.h"
#include "AlarmManager.h"
#include "SerializeClass/SerializeClass.h"
#include "DeviceData_Router.h"
#include "PlutoUtils/DBHelper.h"

#include <set>
#include <queue>

#include "Command_Impl.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"

class Database_pluto_main;
class Row_Device;

typedef class Command_Impl * (* RAP_FType) (class Router *, int, Logger *);
typedef list<Message *> ListMessage;

/*

Classes to hold pointers to the call back functions, and general helper classes

*/

/**
@namespace DCE
The Data Commands and Events (DCE) namespace
*/
namespace DCE
{

/**
@class MessageInterceptorCallBack
This class is for ???
*/
//class ClientSocket;
    // Contains a pointer to the message interceptor
    class MessageInterceptorCallBack
    {
    public:
        class Command_Impl *m_pPlugIn;
        int m_dwPK_Device;
        int m_dwID;
        MessageInterceptorFn m_pMessageInterceptorFn;

		// The list that contains this interceptor in case we want to remove it.  Since external devices
		// can register as interceptors, and they can go away, and then re-register, there needs to be a
		// way to cleanly remove its interceptors
		list<class MessageInterceptorCallBack *> *m_plistMessageInterceptor;

        MessageInterceptorCallBack(class Command_Impl *pPlugIn,MessageInterceptorFn pMessageInterceptorFn)
        {
            m_dwPK_Device=0;
            m_pPlugIn=pPlugIn;
            m_pMessageInterceptorFn=pMessageInterceptorFn;
			m_plistMessageInterceptor=NULL;
        }
        MessageInterceptorCallBack(int PK_Device,int ID)
        {
            m_dwPK_Device=PK_Device;
            m_dwID=ID;
            m_pPlugIn=NULL;
            m_pMessageInterceptorFn=NULL;
			m_plistMessageInterceptor=NULL;
        }
    };


/**
@class MessageFromInterceptor
This class is for ???
*/
    // Store information about MessageInterceptors

    class MessageFromInterceptor
    {
    public:
        list<class MessageInterceptorCallBack *> m_listMessageInterceptor;
		~MessageFromInterceptor()
		{
			for(list<class MessageInterceptorCallBack *>::iterator it=m_listMessageInterceptor.begin();it!=m_listMessageInterceptor.end();++it)
				delete *it;
		}
    };

/**
@class MessageCategoryInterceptor
This class is for ???
*/
    class MessageCategoryInterceptor
    {
    public:
        map<int,class MessageFromInterceptor *> m_mapMessageFromInterceptor;
		~MessageCategoryInterceptor()
		{
			for(map<int,class MessageFromInterceptor *>::iterator it=m_mapMessageFromInterceptor.begin();it!=m_mapMessageFromInterceptor.end();++it)
				delete (*it).second;
		}
    };

/**
@class MessageToInterceptor
This class is for ???
*/
    class MessageToInterceptor
    {
    public:
        map<int,class MessageCategoryInterceptor *> m_mapMessageCategoryInterceptor;
		~MessageToInterceptor()
		{
			for(map<int,class MessageCategoryInterceptor *>::iterator it=m_mapMessageCategoryInterceptor.begin();it!=m_mapMessageCategoryInterceptor.end();++it)
				delete (*it).second;
		}
    };

 /**
@class MessageMDLInterceptor
This class is for ???
*/
   class MessageMDLInterceptor
    {
    public:
        map<int,class MessageToInterceptor *> m_mapMessageToInterceptor;
		~MessageMDLInterceptor()
		{
			for(map<int,class MessageToInterceptor *>::iterator it=m_mapMessageToInterceptor.begin();it!=m_mapMessageToInterceptor.end();++it)
				delete (*it).second;
		}
    };

/**
@class MessageIDInterceptor
This class is for ???
*/
    class MessageIDInterceptor
    {
    public:
        map<int,class MessageMDLInterceptor *> m_mapMessageMDLInterceptor;
		~MessageIDInterceptor()
		{
			for(map<int,class MessageMDLInterceptor *>::iterator it=m_mapMessageMDLInterceptor.begin();it!=m_mapMessageMDLInterceptor.end();++it)
				delete (*it).second;
		}
    };

/**
@class MessageTypeInterceptor
This class is for ???
*/
    class MessageTypeInterceptor
    {
    public:
        map<int,class MessageIDInterceptor *> m_mapMessageIDInterceptor;
		~MessageTypeInterceptor()
		{
			for(map<int,class MessageIDInterceptor *>::iterator it=m_mapMessageIDInterceptor.begin();it!=m_mapMessageIDInterceptor.end();++it)
				delete (*it).second;
		}
    };

/**
@class Router
The DCE Router class

There are 3 layers to Device classes: _Base, _Impl and _Router.
Each layer adds more data.
m_DeviceStructure contains the minimal information, _Base,
 which is sent to all devices when they register
*/

    class Router : public SocketListener, AlarmEvent, public DBHelper
    {
    private:
        /*
        Data

        */

        // There are 3 layers to Device classes: _Base, _Impl and _Router.  Each layer adds
        // more data.  m_DeviceStructure contains the minimal information, _Base, which is sent to
        // all devices when they register
        char *m_pDeviceStructure;
        unsigned long m_dwIDeviceStructure_Size;
        char *m_pBufferForDeviceCategories;

        // The Plug-in's and interceptors
        map<int,class Command_Impl *> m_mapPlugIn;
        map<int,ListCommand_Impl *> m_mapPlugIn_DeviceTemplate;
		map<int,pair<time_t,time_t> > m_mapDeviceUpStatus; // For each device (ie each md), 2 times: first when the machine first came up (or 0 if it's down), the second, the last time there was communication from it
		map<int,ListMessage *> m_mapPendingMessages; // int=the device id, ListMessage = list of messages to send the device after it registers.  See Message::m_eRetry

        map<int,class MessageTypeInterceptor *> m_mapMessageTypeInterceptor;
        list<class MessageInterceptorCallBack *> m_listMessageInterceptor_Global;   // Interceptors that want all messages
		list<void *> m_listPluginHandles; // The handles to the .so and .dll files

        // Miscellaneous Data for internal use
        AlarmManager* m_pAlarmManager;
        pluto_pthread_mutex_t m_CoreMutex, // Protects all the map's within DCERouter
			m_InterceptorMutex;
        pluto_pthread_mutex_t m_MessageQueueMutex;
        pthread_cond_t m_MessageQueueCond;
        int m_Port,m_dwPK_Language;
        long m_dwPK_Device_Largest;
		bool m_bStopProcessingMessages; // We're going to kill ourselves
        string m_sBasePath;
        pthread_t m_pthread_queue_id;
        map<int,string> m_dwPK_Device_To_CommandLine;
        map<int,int> m_Routing_DeviceToController;
        map<int,int> m_Routing_VideoDeviceToController;
        map<int,string> m_Routing_ControllerToIP;
        bool m_bStartup, m_bIsMaster, m_bReload, m_bQuit, m_bIsLoading;
        int m_dwPK_Device,m_iFileVersion,m_dwPK_Installation;
        string m_sDBHost,m_sDBUser,m_sDBPassword,m_sDBName;
        int m_dwIDBPort;
        Row_Device *m_pRow_Device_Me;
        set<int> m_RunningDevices;
        list<Message *> m_MessageQueue;
        Database_pluto_main *m_pDatabase_pluto_main;
		Row_Installation *m_pRow_Installation;

        // Lots of maps
        map<int,class Room *> m_mapRoom;
        map<int,class Command *> m_mapCommand;
        map<int,class Event_Router *> m_mapEvent_Router;
        map<int,class CommandGroup *> m_mapCommandGroup;
        map<int,string> m_mapCommandParmNames;
        map<int,string> m_mapEventParmNames;
        map<int,class DeviceData_Router *> m_mapDeviceData_Router;
        map<int,ListDeviceData_Router *> m_mapDeviceByTemplate;
        map<int,ListDeviceData_Router *> m_mapDeviceByCategory;
        map<int,class DeviceGroup *> m_mapDeviceGroup;
        Map_DeviceCategory m_mapDeviceCategory;

    public:
        // Accessors
        Command_Impl *m_mapPlugIn_Find(int PK_Device) { map<int,class Command_Impl *>::iterator it = m_mapPlugIn.find(PK_Device); return it==m_mapPlugIn.end() ? NULL : (*it).second; }
		pair<time_t,time_t> m_mapDeviceUpStatus_Find(int PK_Device) { PLUTO_SAFETY_LOCK(cm,m_CoreMutex); map<int,pair<time_t,time_t> >::iterator it = m_mapDeviceUpStatus.find(PK_Device); return it==m_mapDeviceUpStatus.end() ? make_pair<time_t,time_t> (0,0) : (*it).second; }
		ListCommand_Impl *m_mapPlugIn_DeviceTemplate_Find(int PK_DeviceTemplate) { map<int,ListCommand_Impl *>::iterator it = m_mapPlugIn_DeviceTemplate.find(PK_DeviceTemplate); return it==m_mapPlugIn_DeviceTemplate.end() ? NULL : (*it).second; }
        DeviceCategory *m_mapDeviceCategory_Find(int PK_DeviceCategory) { Map_DeviceCategory::iterator it = m_mapDeviceCategory.find(PK_DeviceCategory); return it==m_mapDeviceCategory.end() ? NULL : (*it).second; }
        DeviceGroup *m_mapDeviceGroup_Find(int PK_DeviceGroup) {map<int,class DeviceGroup *>::iterator it = m_mapDeviceGroup.find(PK_DeviceGroup); return it==m_mapDeviceGroup.end() ? NULL : (*it).second; }
        ListDeviceData_Router *m_mapDeviceByTemplate_Find(int PK_DeviceTemplate) { map<int,ListDeviceData_Router *>::iterator it = m_mapDeviceByTemplate.find(PK_DeviceTemplate); return it==m_mapDeviceByTemplate.end() ? NULL : (*it).second; }
        ListDeviceData_Router *m_mapDeviceByCategory_Find(int PK_DeviceCategory) { map<int,ListDeviceData_Router *>::iterator it = m_mapDeviceByCategory.find(PK_DeviceCategory); return it==m_mapDeviceByCategory.end() ? NULL : (*it).second; }
        DeviceData_Router *m_mapDeviceData_Router_Find(int PK_Device) { map<int,class DeviceData_Router *>::iterator it = m_mapDeviceData_Router.find(PK_Device); return it==m_mapDeviceData_Router.end() ? NULL : (*it).second; }
        CommandGroup *m_mapCommandGroup_Find(int PK_CommandGroup) { map<int,class CommandGroup *>::iterator it = m_mapCommandGroup.find(PK_CommandGroup); return it==m_mapCommandGroup.end() ? NULL : (*it).second;}
        Command *m_mapCommand_Find(int PK_Command) { map<int,class Command *>::iterator it = m_mapCommand.find(PK_Command); return it==m_mapCommand.end() ? NULL : (*it).second; }
        Event_Router *m_mapEvent_Router_Find(int PK_Event_Router) { map<int,class Event_Router *>::iterator it = m_mapEvent_Router.find(PK_Event_Router); return it==m_mapEvent_Router.end() ? NULL : (*it).second; }
        Room *m_mapRoom_Find(int PK_Room) { map<int,class Room *>::iterator it = m_mapRoom.find(PK_Room); return it==m_mapRoom.end() ? NULL : (*it).second; }
        const map<int,class DeviceData_Router *> *m_mapDeviceData_Router_get() { return &m_mapDeviceData_Router; };
        const map<int,class DeviceGroup *> *m_mapDeviceGroup_get() { return &m_mapDeviceGroup; };
		const map<int,class Room *> *m_mapRoom_get() { return &m_mapRoom; }
		const map<int,class Command_Impl *> *m_mapPlugIn_get() { return &m_mapPlugIn; }
        const map<int,class CommandGroup *> *m_mapCommandGroup_get() { return &m_mapCommandGroup; }

        int iPK_Installation_get() { return m_dwPK_Installation; }
		Row_Installation *m_pRow_Installation_get() { return m_pRow_Installation; }
		int iPK_Device_get() { return m_dwPK_Device; }
        int iPK_Language_get() { return m_dwPK_Language; }
        string sBasePath_get() { return m_sBasePath; }

        string sDBHost_get() { return m_sDBHost; }
        string sDBUser_get() { return m_sDBUser; }
        string sDBPassword_get() { return m_sDBPassword; }
        string sDBName_get() { return m_sDBName; }
        int iDBPort_get() { return m_dwIDBPort; }

        /*
        METHODS
        */

        Router(int PK_Device,int PK_Installation,string BasePath,string DBHost,string DBUser,string DBPassword,string DBName,int DBPort,int ListenPort);
        virtual ~Router();
		void Reload()
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"DCERouter::Reload");
			m_bReload=true; m_bClosed=true;
		}

        // Plug-In's
        void RegisterAllPlugins(); // Iterates through all plug-ins, calling the register method
		bool m_bIsLoading_get() { return m_bIsLoading; }

        void RegisterMsgInterceptor(Message *pMessage);

        // HACK -- We need to do this inline since a lot of plug-ins will be calling this method and we don't have a way for dcerouter to export it's classes and we don't want to add dcerouter.cpp to the plug-ins
        void RegisterMsgInterceptor(class MessageInterceptorCallBack *pCallBack,int PK_Device_From,int PK_Device_To,int PK_DeviceTemplate,int PK_DeviceCategory,int MessageType,int MessageID)
        {
			PLUTO_SAFETY_LOCK(im,m_InterceptorMutex);  // Protect the interceptor map

			// Add this to the device's list
			DeviceData_Router *pDevice_RegisteringInterceptor=NULL;
			if( pCallBack->m_pPlugIn )
				pDevice_RegisteringInterceptor = m_mapDeviceData_Router_Find(pCallBack->m_pPlugIn->m_dwPK_Device);
			else
				pDevice_RegisteringInterceptor = m_mapDeviceData_Router_Find(pCallBack->m_dwPK_Device);

			if( pDevice_RegisteringInterceptor )
				pDevice_RegisteringInterceptor->m_vectMessageInterceptorCallBack.push_back(pCallBack);
			else
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find the device registering this interceptor"); // Should never happen

			if( PK_Device_From==0 && PK_Device_To==0 && PK_DeviceTemplate==0 && PK_DeviceCategory==0 && MessageType==0 && MessageID==0 )
            {
                m_listMessageInterceptor_Global.push_back(pCallBack);
				pCallBack->m_plistMessageInterceptor = &m_listMessageInterceptor_Global;
                return;
            }
            MessageTypeInterceptor *pMessageTypeInterceptor = m_mapMessageTypeInterceptor[MessageType];
            if( !pMessageTypeInterceptor )
            {
                pMessageTypeInterceptor = new MessageTypeInterceptor();
                m_mapMessageTypeInterceptor[MessageType] = pMessageTypeInterceptor;
            }

            MessageIDInterceptor *pMessageIDInterceptor = pMessageTypeInterceptor->m_mapMessageIDInterceptor[MessageID];
            if( !pMessageIDInterceptor )
            {
                pMessageIDInterceptor = new MessageIDInterceptor();
                pMessageTypeInterceptor->m_mapMessageIDInterceptor[MessageID] = pMessageIDInterceptor;
            }

            MessageMDLInterceptor *pMessageMDLInterceptor = pMessageIDInterceptor->m_mapMessageMDLInterceptor[PK_DeviceTemplate];
            if( !pMessageMDLInterceptor )
            {
                pMessageMDLInterceptor = new MessageMDLInterceptor();
                pMessageIDInterceptor->m_mapMessageMDLInterceptor[PK_DeviceTemplate] = pMessageMDLInterceptor;
            }

            MessageToInterceptor *pMessageToInterceptor = pMessageMDLInterceptor->m_mapMessageToInterceptor[PK_Device_To];
            if( !pMessageToInterceptor )
            {
                pMessageToInterceptor = new MessageToInterceptor();
                pMessageMDLInterceptor->m_mapMessageToInterceptor[PK_Device_To] = pMessageToInterceptor;
            }

            MessageCategoryInterceptor *pMessageCategoryInterceptor = pMessageToInterceptor->m_mapMessageCategoryInterceptor[PK_DeviceCategory];
            if( !pMessageCategoryInterceptor )
            {
                pMessageCategoryInterceptor = new MessageCategoryInterceptor();
                pMessageToInterceptor->m_mapMessageCategoryInterceptor[PK_DeviceCategory] = pMessageCategoryInterceptor;
            }

            MessageFromInterceptor *pMessageFromInterceptor = pMessageCategoryInterceptor->m_mapMessageFromInterceptor[PK_Device_From];
            if( !pMessageFromInterceptor )
            {
                pMessageFromInterceptor = new MessageFromInterceptor();
                pMessageCategoryInterceptor->m_mapMessageFromInterceptor[PK_Device_From] = pMessageFromInterceptor;
            }

            pMessageFromInterceptor->m_listMessageInterceptor.push_back(pCallBack);
			pCallBack->m_plistMessageInterceptor = &pMessageFromInterceptor->m_listMessageInterceptor;
        }

        // Internal use
        bool Run();
		virtual void RegisteredEventHandler(ServerSocket *pSocket, int DeviceID);
		virtual void RegisteredCommandHandler(ServerSocket *pSocket, int DeviceID);
        virtual bool ReceivedString(Socket *Socket, string Line, int nTimeout = -1);
		virtual void ReceivedMessage( Socket *pSocket, Message* pMessage ) { ReceivedMessage(pSocket,pMessage,true); }
        virtual void ReceivedMessage(Socket *pSocket, Message *pMessage, bool bAutoDelete);
        virtual void OnDisconnected(int DeviceID);
        void AddMessageToQueue(Message *pMessage);
        void ProcessQueue();
        void HandleCommandPipes(Socket *pSocket,SafetyMessage *pSafetyMessage);
		bool HandleRouterMessage(Message *pMessage); // Handle a message for the router internally
        void RealSendMessage(Socket *pSocket,SafetyMessage *pSafetyMessage);
        void CreatedDevice(DeviceData_Router *pDevice);
        void ParseDevice(int MasterDeviceID, int ParentDeviceID, class DeviceData_Impl *pDevice);
        void StopAllDevices(bool bReload);
		void DoLogRotation();
		void DoLogReload();
		bool RequestReload(int PK_Device_Requesting)
		{
			// If it's not safe to do a reload now returns false and fires a EVENT_Reload_Aborted_CONST event
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Received reload command from %d",PK_Device_Requesting);

			//get the list with devices
			PLUTO_SAFETY_LOCK(lm,m_ListenerMutex);

			list<int> listServerSocketDevices;
			for(ServerSocketMap::iterator itSS = m_mapServerSocket.begin(); itSS!=m_mapServerSocket.end(); ++itSS)
				listServerSocketDevices.push_back(itSS->first);

			lm.Release();

            //got the list; let's iterate and get server socket for each devices and send safe to reload
			for(list<int>::iterator it = listServerSocketDevices.begin(); it != listServerSocketDevices.end(); ++it)
			{
				ServerSocket *pServerSocket;
				GET_SERVER_SOCKET(gs, pServerSocket, *it);

				LoggerWrapper::GetInstance()->Write(LV_STATUS, "SafeToReload for device %d, server socket %p ask %d", *it, pServerSocket,
					pServerSocket ? (int) pServerSocket->m_bAskBeforeReload : -1 );

				if(NULL != pServerSocket)
				{
					if( pServerSocket->m_bAskBeforeReload )
					{
						bool bAbortReload = false;
						string sReason;
						if(IsPlugin(pServerSocket->m_dwPK_Device))
						{
							Command_Impl *pCommand_Impl = m_mapPlugIn_Find(pServerSocket->m_dwPK_Device);
							bAbortReload = !pCommand_Impl->SafeToReload(sReason);
						}
						else
						{
							PLUTO_SAFETY_LOCK(slConnMutex,(pServerSocket->m_ConnectionMutex))
							bAbortReload = !pServerSocket->SafeToReload(sReason);
						}

						if(bAbortReload)
						{
							LoggerWrapper::GetInstance()->Write(LV_STATUS,"Aborting reload per device %d = %s", pServerSocket->m_dwPK_Device, sReason.c_str());
							if( PK_Device_Requesting )
							{
								ReceivedMessage(NULL,new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT,
									EVENT_Reload_Aborted_CONST,3,
									EVENTPARAMETER_PK_Device_CONST,StringUtils::itos(pServerSocket->m_dwPK_Device).c_str(),
									EVENTPARAMETER_Text_CONST,sReason.c_str(),
									EVENTPARAMETER_PK_Orbiter_CONST,StringUtils::itos(PK_Device_Requesting).c_str()));
							}
							return false;
						}
					}
				}
			}

			LoggerWrapper::GetInstance()->Write(LV_STATUS,"RELOAD OK");
			return true;
		}
        void OutputChildren(class DeviceData_Impl *pDevice,string &Response);
        void AlarmCallback(int id, void* param);
        void Configure(); // Build the config information from the database

		void CheckForRecursiveControlledVia(DeviceData_Router *pDevice,vector<int> *pvect_Device_ControlledVia);
		void CheckForRecursiveRouteTo(DeviceData_Router *pDevice,vector<int> *pvect_Device_RouteTo);
		void CheckForRecursivePipes(DeviceData_Router *pDevice,vector<int> *pvect_Device_Pipe);

#ifdef AUDIDEMO
        class Command_Impl *CreatePlugInHardCoded(int PK_Device, int PK_DeviceTemplate, string sCommandLine);  // Load the plug-in on the command line
#endif

		RAP_FType PlugIn_Load(int PK_Device, int PK_DeviceTemplate, string sCommandLine); // Load the plug-in on the command line
		class Command_Impl * PlugIn_Activate(int PK_Device, RAP_FType RegisterAsPlugin, string sLogFile); // Call specified plug-in registering function
        int DynamicallyLoadPlugin(string sFile);
        string GetDevicesByDeviceTemplate(DeviceData_Router *pDeviceData_From,int PK_DeviceTemplate,eBroadcastLevel BroadcastLevel);
		string GetDevicesByCategory(DeviceData_Router *pDeviceData_From,int PK_DeviceCategory,eBroadcastLevel BroadcastLevel);
		string GetDevicesByGroup(int PK_DeviceGroup);
        void ErrorResponse(Socket *pSocket,Message *pMessage); // Respond with an error condition if the sender is waiting for a response
#ifdef LL_DEBUG
		void ShowSockets();
#endif

		bool IsPlugin(int iPK_Device);
        // Public
        void Die(const char *Why);
        void CrashWithinPlugin(int iPK_Device)
		{
			Reload();
		}
        virtual void DispatchMessage(Message *pMessage) { ReceivedMessage(NULL,pMessage); };
        bool GetParameterWithDefinedMessage(Message *sendMessage, string &sResult);
        bool GetParameter(int ToDevice,int ParmType, string &sResult);
        string GetPublicIP();
        void CleanFileName(string &FileName);
        Message *GetActionForInput(int PK_Device,int PK_Input);

		bool DeviceIsRegistered(int PK_Device)
		{
			PLUTO_SAFETY_LOCK(slCore,m_CoreMutex);

			int RouteToDevice = DEVICEID_NULL;

			map<int,int>::iterator iRoute;
			iRoute = m_Routing_DeviceToController.find(PK_Device);
			if (iRoute!=m_Routing_DeviceToController.end())
			{
				RouteToDevice = (*iRoute).second;
			}
			slCore.Release();

			if (!RouteToDevice)
				return false;

			ServerSocket *pServerSocket;
			GET_SERVER_SOCKET(gs, pServerSocket, PK_Device );
			return pServerSocket!=NULL;
		}

		void HandleMessageFailure(SafetyMessage *pSafetyMessage);  // Called when we failed to deliver a message
		void HandleMessageFailure(Message *pMessage,bool bSkipSerialization=false);
		void CheckForMessageRetries(int DeviceID);
		void ReadPersistentMessages();
		void SerializePersistentMessages();

		void SetDeviceDataInDB(int PK_Device,int PK_DeviceData,string sValue,bool bOnlyIfNotAlreadyThere=false)
		{
			Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(PK_Device,PK_DeviceData);
			if( bOnlyIfNotAlreadyThere && pRow_Device_DeviceData && pRow_Device_DeviceData->IK_DeviceData_get().size() )
				return;
			if( !pRow_Device_DeviceData )
			{
				pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->AddRow();
				pRow_Device_DeviceData->FK_Device_set(PK_Device);
				pRow_Device_DeviceData->FK_DeviceData_set(PK_DeviceData);
			}
			else
				pRow_Device_DeviceData->Reload();
			pRow_Device_DeviceData->IK_DeviceData_set( sValue );
			pRow_Device_DeviceData->Table_Device_DeviceData_get()->Commit();
		}

        void ExecuteCommandGroup(int PK_CommandGroup,DeviceData_Router *pDevice_Sender,int sStartingCommand=0);
		void StopPendingCommandGroup(int PK_CommandGroup,string sDescription,string sHint);

        Database_pluto_main *GetDatabase() { return m_pDatabase_pluto_main; }
        void StartListening() { SocketListener::StartListening(m_Port); }
        void Quit() { m_bQuit=true; }
		virtual void PingFailed( ServerSocket *pServerSocket, int dwPK_Device );
		virtual void RemoveAndDeleteSocket( ServerSocket *pServerSocket, bool bDontDelete=false );

		virtual int GetDeviceID( int iPK_DeviceTemplate, string sMacAddress, string sIPAddress );
		virtual int ConfirmDeviceTemplate( int iPK_Device, int iPK_DeviceTemplate );

		Command_Impl *FindPluginByTemplate(int PK_DeviceTemplate)
		{
			ListCommand_Impl *pListCommand_Impl = m_mapPlugIn_DeviceTemplate_Find( PK_DeviceTemplate );
			if( !pListCommand_Impl || pListCommand_Impl->size( )!=1 )
				return NULL;
			return pListCommand_Impl->front( );
		}

        // declared inline since i use it from some plugins.
        int FindClosestRelative(int MasterDeviceType, int CurrentDevice)
        {
            DeviceData_Router *pDevice = m_mapDeviceData_Router_Find(CurrentDevice);
            if( pDevice )
            {
                int DeviceID = pDevice->m_dwPK_Device_ControlledVia;
                if( DeviceID && (pDevice = m_mapDeviceData_Router_Find(DeviceID)))
                {
                    for(int i=0;i<(int)pDevice->m_vectDeviceData_Impl_Children.size();++i)
                    {
                        DeviceData_Impl *pChildDevice = pDevice->m_vectDeviceData_Impl_Children[i];
                        if (pChildDevice->m_dwPK_DeviceTemplate == MasterDeviceType)
                        {
                            return pChildDevice->m_dwPK_Device;
                        }
                    }
                    // Not here.  Go up one.
                    return FindClosestRelative(MasterDeviceType, DeviceID);
                }
            }
            return -1;
        }

        // Helper functions for message interceptors, declared inline for performance
        void CheckInterceptor(class MessageTypeInterceptor *pMessageTypeInterceptor,class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo,list<class MessageInterceptorCallBack *> &listMessageInterceptor)
        {
            map<int,class MessageIDInterceptor *>::iterator itMessageID = pMessageTypeInterceptor->m_mapMessageIDInterceptor.find(pMessage->m_dwID);
            if( itMessageID != pMessageTypeInterceptor->m_mapMessageIDInterceptor.end() )
            {
                CheckInterceptor( (*itMessageID).second, pSocket, pMessage, pDeviceFrom, pDeviceTo,listMessageInterceptor );
            }
            if( pMessage->m_dwID )
            {
                itMessageID = pMessageTypeInterceptor->m_mapMessageIDInterceptor.find(0);
                if( itMessageID != pMessageTypeInterceptor->m_mapMessageIDInterceptor.end() )
                {
                    CheckInterceptor( (*itMessageID).second, pSocket, pMessage, pDeviceFrom, pDeviceTo,listMessageInterceptor );
                }
            }
        }

        void CheckInterceptor(class MessageIDInterceptor *pMessageIDInterceptor,class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo,list<class MessageInterceptorCallBack *> &listMessageInterceptor)
        {
            map<int,class MessageMDLInterceptor *>::iterator itMessageMDL;
            if( pDeviceFrom )
            {
                itMessageMDL = pMessageIDInterceptor->m_mapMessageMDLInterceptor.find(pDeviceFrom->m_dwPK_DeviceTemplate);
                if( itMessageMDL != pMessageIDInterceptor->m_mapMessageMDLInterceptor.end() )
                {
                    CheckInterceptor( (*itMessageMDL).second, pSocket, pMessage, pDeviceFrom, pDeviceTo,listMessageInterceptor );
                }
            }
            if( pDeviceTo )
            {
                itMessageMDL = pMessageIDInterceptor->m_mapMessageMDLInterceptor.find(pDeviceTo->m_dwPK_DeviceTemplate);
                if( itMessageMDL != pMessageIDInterceptor->m_mapMessageMDLInterceptor.end() )
                {
                    CheckInterceptor( (*itMessageMDL).second, pSocket, pMessage, pDeviceFrom, pDeviceTo,listMessageInterceptor );
                }
            }
            if( !pDeviceFrom || pDeviceFrom->m_dwPK_DeviceTemplate!=0 )
            {
                itMessageMDL = pMessageIDInterceptor->m_mapMessageMDLInterceptor.find(0);
                if( itMessageMDL != pMessageIDInterceptor->m_mapMessageMDLInterceptor.end() )
                {
                    CheckInterceptor( (*itMessageMDL).second, pSocket, pMessage, pDeviceFrom, pDeviceTo,listMessageInterceptor );
                }
            }
        }

        void CheckInterceptor(class MessageMDLInterceptor *pMessageMDLInterceptor,class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo,list<class MessageInterceptorCallBack *> &listMessageInterceptor)
        {
            map<int,class MessageToInterceptor *>::iterator itMessageTo = pMessageMDLInterceptor->m_mapMessageToInterceptor.find(pMessage->m_dwPK_Device_To);
            if( itMessageTo != pMessageMDLInterceptor->m_mapMessageToInterceptor.end() )
            {
                CheckInterceptor( (*itMessageTo).second, pSocket, pMessage, pDeviceFrom, pDeviceTo,listMessageInterceptor );
            }
            // If we didn't already check for 0, check now
            if( pMessage->m_dwPK_Device_To )
            {
                itMessageTo = pMessageMDLInterceptor->m_mapMessageToInterceptor.find(0);
                if( itMessageTo != pMessageMDLInterceptor->m_mapMessageToInterceptor.end() )
                {
                    CheckInterceptor( (*itMessageTo).second, pSocket, pMessage, pDeviceFrom, pDeviceTo,listMessageInterceptor );
                }
            }
            if( pMessage->m_sPK_Device_List_To.length() )
            {
                int PK_Device;  size_t pos=0;
                while( true )
                {
                    PK_Device=atoi(StringUtils::Tokenize(pMessage->m_sPK_Device_List_To,",",pos).c_str());
                    itMessageTo = pMessageMDLInterceptor->m_mapMessageToInterceptor.find(PK_Device);
                    if( itMessageTo != pMessageMDLInterceptor->m_mapMessageToInterceptor.end() )
                    {
                        CheckInterceptor( (*itMessageTo).second, pSocket, pMessage, pDeviceFrom, pDeviceTo,listMessageInterceptor );
                    }
                    if( pos>=pMessage->m_sPK_Device_List_To.length() || pos==string::npos )
                        break;
                }
            }
        }

        void CheckInterceptor(class MessageToInterceptor *pMessageToInterceptor,class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo,list<class MessageInterceptorCallBack *> &listMessageInterceptor)
        {
            map<int,class MessageCategoryInterceptor *>::iterator itMessageToCat;
            if( pDeviceTo )
            {
                itMessageToCat = pMessageToInterceptor->m_mapMessageCategoryInterceptor.find(pDeviceTo->m_dwPK_DeviceCategory);
                if( itMessageToCat != pMessageToInterceptor->m_mapMessageCategoryInterceptor.end() )
                {
                    CheckInterceptor( (*itMessageToCat).second, pSocket, pMessage, pDeviceFrom, pDeviceTo,listMessageInterceptor );
                }
            }
            if( pDeviceFrom )
            {
                itMessageToCat = pMessageToInterceptor->m_mapMessageCategoryInterceptor.find(pDeviceFrom->m_dwPK_DeviceCategory);
                if( itMessageToCat != pMessageToInterceptor->m_mapMessageCategoryInterceptor.end() )
                {
                    CheckInterceptor( (*itMessageToCat).second, pSocket, pMessage, pDeviceFrom, pDeviceTo,listMessageInterceptor );
                }
            }
            itMessageToCat = pMessageToInterceptor->m_mapMessageCategoryInterceptor.find(0);
            if( itMessageToCat != pMessageToInterceptor->m_mapMessageCategoryInterceptor.end() )
            {
                CheckInterceptor( (*itMessageToCat).second, pSocket, pMessage, pDeviceFrom, pDeviceTo,listMessageInterceptor );
            }
            if( pMessage->m_sPK_Device_List_To.length() )
            {
                int PK_Device;  size_t pos=0;
                while( true )
                {
                    PK_Device=atoi(StringUtils::Tokenize(pMessage->m_sPK_Device_List_To,",",pos).c_str());
                    DeviceData_Router *pDeviceData_Router = m_mapDeviceData_Router_Find(PK_Device);
                    if( pDeviceData_Router )
                    {
                        itMessageToCat = pMessageToInterceptor->m_mapMessageCategoryInterceptor.find(pDeviceData_Router->m_dwPK_DeviceCategory);
                        if( itMessageToCat != pMessageToInterceptor->m_mapMessageCategoryInterceptor.end() )
                        {
                            CheckInterceptor( (*itMessageToCat).second, pSocket, pMessage, pDeviceFrom, pDeviceTo,listMessageInterceptor );
                        }
                    }
                    if( pos>=pMessage->m_sPK_Device_List_To.length() || pos==string::npos )
                        break;
                }
            }
        }

        void CheckInterceptor(class MessageCategoryInterceptor *pMessageCategoryInterceptor,class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo,list<class MessageInterceptorCallBack *> &listMessageInterceptor)
        {
            map<int,class MessageFromInterceptor *>::iterator itMessageFrom = pMessageCategoryInterceptor->m_mapMessageFromInterceptor.find(pMessage->m_dwPK_Device_From);
            if( itMessageFrom != pMessageCategoryInterceptor->m_mapMessageFromInterceptor.end() )
            {
                CheckInterceptor( (*itMessageFrom).second, pSocket, pMessage, pDeviceFrom, pDeviceTo,listMessageInterceptor);
            }
            if( pMessage->m_dwPK_Device_From )
            {
                itMessageFrom = pMessageCategoryInterceptor->m_mapMessageFromInterceptor.find(0);
                if( itMessageFrom != pMessageCategoryInterceptor->m_mapMessageFromInterceptor.end() )
                {
                    CheckInterceptor( (*itMessageFrom).second, pSocket, pMessage, pDeviceFrom, pDeviceTo,listMessageInterceptor);
                }
            }
        }

        void CheckInterceptor(class MessageFromInterceptor *pMessageFromInterceptor,class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo,list<class MessageInterceptorCallBack *> &listMessageInterceptor)
        {
            list<class MessageInterceptorCallBack *>::iterator itMessageInterceptor;
            for(itMessageInterceptor=pMessageFromInterceptor->m_listMessageInterceptor.begin();
                itMessageInterceptor!=pMessageFromInterceptor->m_listMessageInterceptor.end();
                ++itMessageInterceptor)
            {
                class MessageInterceptorCallBack *pMessageInterceptorCallBack = (*itMessageInterceptor);
				listMessageInterceptor.push_back(pMessageInterceptorCallBack);
            }
        }
    };
}

#endif
