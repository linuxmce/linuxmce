/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/**
@file Command_Impl.h
Header file for the Command_Impl class

@todo notcommented
*/


#ifndef COMMAND_IMPL_H
#define COMMAND_IMPL_H

#include "HandleRequestSocket.h"
#include "DeviceData_Impl.h"
#include "MessageBuffer.h"

/**
@namespace DCE
The Data Commands and Events (DCE) namespace
*/
namespace DCE
{
	class ClientSocket;
	class Message;
	class DeviceData_Impl;
	class Event_Impl;


/**
@class PendingTask
A helper class for reporting this device's pending tasks
*/
	class PendingTask
	{
	public:
		int m_dwID; /**< A unique id to identify this task.  Can be a job ID. */
		int m_dwPK_Device_Processing; /**< The device that is processing this task. */
		int m_dwPK_Device_Abort; /**< The device to send the ABORT_TASK syscommand to with the ID. */
		string m_sType,m_sDescription;
		char m_cPercentComplete; /**< From 0 - 100. */
		int m_dwSecondsLeft; /**< How many seconds are left before this task will be done. */
		bool m_bCanAbort; /**< True if the task can be aborted. */

        /** Constructor
        @param dwID is a unique ID.
        @param dwPK_Device_Processing is ???
        @param dwPK_Device_Abort is ???
        @param sDescription is the description of the device.
        @param cPercentComplete is a byte 0..100.
        @param dwSecondsLeft is the time left for this task.
        @param bCanAbort is a flag to allow aborting.
        */
		PendingTask(int dwID, int dwPK_Device_Processing, int dwPK_Device_Abort,
            string sType, string sDescription, char cPercentComplete, int dwSecondsLeft,
			bool bCanAbort)
		{
			m_dwID=dwID;
			m_dwPK_Device_Processing=dwPK_Device_Processing;
			m_dwPK_Device_Abort=dwPK_Device_Abort;
			m_sType=sType;
			m_sDescription=sDescription;
			m_cPercentComplete=cPercentComplete;
			m_dwSecondsLeft=dwSecondsLeft;
			m_bCanAbort=bCanAbort;
		}

		/** Constructor
        @param sValue is a string with all the calling values tab separated.

        Parse from a string to obtain the calling parameters.
        */
		PendingTask(string sValue)
		{
			string::size_type pos=0;
			m_dwID=atoi( StringUtils::Tokenize( sValue, "\t", pos ).c_str() );
			m_dwPK_Device_Processing=atoi( StringUtils::Tokenize( sValue, "\t", pos ).c_str() );
			m_dwPK_Device_Abort=atoi( StringUtils::Tokenize( sValue, "\t", pos ).c_str() );
			m_sType=StringUtils::Tokenize( sValue, "\t", pos );
			m_sDescription=StringUtils::Tokenize( sValue, "\t", pos );
			m_cPercentComplete=(unsigned char) atoi( StringUtils::Tokenize( sValue, "\t", pos ).c_str() );
			m_dwSecondsLeft=atoi( StringUtils::Tokenize( sValue, "\t", pos ).c_str() );
			m_bCanAbort=StringUtils::Tokenize( sValue, "\t", pos )=="1";
		}

        /** Base class destructor */
		virtual ~PendingTask() {}

        /** ToString
        stringifies (tab seperated) the calling parameters.

        This is to sinplify all the calling parameters.
        */
		string ToString()
		{
			return StringUtils::itos(m_dwID) + "\t"
				+ StringUtils::itos(m_dwPK_Device_Processing) + "\t"
				+ StringUtils::itos(m_dwPK_Device_Abort) + "\t"
				+ m_sType + "\t"
				+ m_sDescription + "\t"
				+ StringUtils::itos(m_cPercentComplete) + "\t"
				+ StringUtils::itos(m_dwSecondsLeft) + "\t"
				+ (m_bCanAbort ? "1" : "0");
		}
	};

/**
@class PendingTaskList
A wrapper for a list of tasks that will automatically delete any allocated tasks.

Create this on the stack to have all allocated memory clean up on exit
*/
	class PendingTaskList
	{
	public:
		list<PendingTask *> m_listPendingTask;
		PendingTaskList() {}
		~PendingTaskList()
		{
			for(list<PendingTask *>::iterator it=m_listPendingTask.begin();it!=m_listPendingTask.end();++it)
				delete *it;
		}

		string ToString()
		{
			string sResponse;
			for(list<PendingTask *>::iterator it=m_listPendingTask.begin();it!=m_listPendingTask.end();++it)
				sResponse += (*it)->ToString() + "\n";
			return sResponse;
		}
	};

	typedef map<int, class Command_Impl *> MapCommand_Impl;
	typedef list<class Command_Impl *> ListCommand_Impl;

	/**
	 * @brief Function definition for a callback, return true to stop all further processing of the message and abort it's delivery to the destination
	 */
	typedef  bool ( Command_Impl::*MessageInterceptorFn )( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );


	/**
    @class Command_Impl
	 A derived class implementating the command from the DCE namespace (Data Commands and Events).

    Classes derived from this one handle commmands for specific devices
	 */
	class Command_Impl : public HandleRequestSocket
	{
	protected:
		MessageBuffer *m_pMessageBuffer;

	private:

		list<Message *> m_listMessageQueue;  /**< there are two ways of sending a message: realtime and queued (in a sepparted thread); this is the queue of messages */
		map<int,string> m_mapSpawnedDevices;  /**< Keep track of all the devices we spawned so we can kill them on create PK_Device->Command */

	public:
		// Set the following two flags in your DCE Device's constructor since the flags are passed in on the Connect()
		bool m_bAskBeforeReload, // If set to true ask this device if it's ok to reload before processing a reload request
			m_bImplementsPendingTasks, // If set to true, this device implements the PendingTasks() to report pending tasks it may be doing
			m_bRouterReloading; // Set to true if the router is reloading

		Command_Impl *m_pParent; /**< if the command was created as an embedded command, keep a pointer to it's parent */
		MapCommand_Impl m_mapCommandImpl_Children; /**< map containing the commands that this command has created */
		pluto_pthread_mutex_t m_listMessageQueueMutex; /**< for protecin the access to the MessageQueue */
		pthread_t m_pthread_queue_id; /**< the thread id for the tread that's treating the messages from the message queue */
		pthread_cond_t m_listMessageQueueCond; /**< condition for the messages in the queue */
		bool m_bKillSpawnedDevicesOnExit;  /**< a derived class can set this to false if we should not kill the devices when we die */
		class Router *m_pRouter; /**< this will be NULL if this is not a plug-in being loaded in the same memory space, otherwise it will point to the router that can see the shared mamory space */

		Command_Impl *m_pPrimaryDeviceCommand; /**< pointer to the command for the primary device (if this one was spawned by it) */
		DeviceData_Impl *m_pData; /**< a pointer to a device data implementation class (one derived from it) */
		Event_Impl *m_pEvent; /**< a pointer to the event implementation class (one derived from it) */
		Event_Impl *m_pcRequestSocket;  /**< Create a second socket for handling requests separately */

		int m_iTargetDeviceID; /**< the device targeted by the command */
		/** Normally a device can open multiple socket connections to the Router, and if any one dies the Router should kill them all.
		Since it's possible another instance may also connect, the Router needs to know which sockets belong together.  This is assigned
		to the current time(NULL) in the Command_Impl constructor.  During GetConfig, all sockets will send their device ID, plus the
		instanceID.  When any one socket dies, all other sockets with the same device/instance combination will also die.  If you don't
		want this behavior, set m_iInstanceID to 0 in your constructor */
		int m_iInstanceID;

		bool m_bWatchdogRunning; /**< specifies if the watchdog is running */
		bool m_bStopWatchdog; /**< specifies if the watchdog is stoped @todo ask is it used? */
		bool m_bMessageQueueThreadRunning; /**< specifies if the MessageQueueThread is running */
		pthread_t m_pThread; /**< the wachdog thread */

		/** If this is a plug-in, the message interceptors will be registered directly.  Otherwise there will be 'callbacks' stored in this map */
		int m_dwMessageInterceptorCounter;
		map<int, MessageInterceptorFn> m_mapMessageInterceptorFn;
        MessageInterceptorFn m_mapMessageInterceptorFn_Find(int dwMessageInterceptorCounter) { map<int,MessageInterceptorFn>::iterator it = m_mapMessageInterceptorFn.find(dwMessageInterceptorCounter); return it==m_mapMessageInterceptorFn.end() ? NULL : (*it).second; }

		/** flags */

		bool m_bReload;
		bool m_bHandleChildren; /**< used by ReceivedMessage() */
		bool m_bLocalMode; /**< no router */
		bool m_bGeneric; /**< This is not a custom generated command handler.  Just a generic child with no handlers. */


		/**
		 * @brief create a top-level command
		 * will establish a connection to the server download the data, and attempt to run CreateCommand to instantiate child objects
		 */
		Command_Impl( int DeviceID, string ServerAddress, bool bLocalMode=false, class Router *pRouter=NULL );

		/**
		 * @brief create a child command
		 * assumes a pre-existing connection to the server and requires that the data and event classes be already generated
		 */
		Command_Impl( Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, class Router *pRouter );

		/**
		 * @brief frees memory allocated on the heap and calls KillSpawnedDevices if the flag is set
		 * @see KillSpawnedDevices
		 */
		virtual ~Command_Impl();

		/**
		 * @brief Plugins may contain pointers to each other.  If Plugin a is deleted while Plugin b still has a pointer to a
		 * it can crash.  To prevent this, before deleting a device this function is called, which should stop all threads.
		 */
		virtual void PrepareToDelete();

		/**
		 * @brief  now all this used to do is done automatically; keeping it for now *just in case*
		 * If your implementation can handle child devices, it MUST override and process
		 * CreateCommand to instantiate your customized command processors.
		 * If DeviceManager passes a child device and this returns NULL, your
		 * app will terminate with an error.
		 */
		virtual Command_Impl *CreateCommand( int iPK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent );

		/**
		 * @brief used by devices witch are plug-ins to register
		 * @return true if successfull
		 */
		virtual bool Register() { return true; };

		/**
		 * @brief accessor for the m_bHandleChildren data member
		 */
		void HandleChildren() { m_bHandleChildren = true; };

		/**
		 * @brief creates a new command embedded into this one
		 */
		virtual void CreateChildren();
		virtual void CreateNewChildren();  // Spawn any new child devices that were created after the load (ie temporary so devices start without a reload router)

		/**
		 * @brief Called during the getconfig in the auto-generated classes to do misc cleanup such as assigning m_pCategory for all the devices
		 */
		virtual void PostConfigCleanup();
		virtual void PostConfigCleanup(DeviceData_Impl *pDevice);

		/**
		 * @brief this will try to spawn the children as separate sessions (for example different displays under Linux or different comman prompts for Windows)
		 * derived classes should implement this to handle special cases, such as if the child requires the GUI interface
		 */
		virtual bool SpawnChildDevice( int PK_Device, string sCommand, string sDisplay="" );  // Another implementation may override this, passing in the display to export -- Linux only

		/**
		 * @brief this will check to see that all related devices on the same pc that ImplementDCE and are not embedded
		 * have registered.  It returns the number of unregistered devices.  The map will have int=PK_Device, bool=registered
		 * for all related devices
		 */
		int FindUnregisteredRelatives(map<int,bool> *p_mapUnregisteredRelatives);
		void FindUnregisteredRelativesLoop(DeviceData_Base *pDevice,map<int,bool> *p_mapUnregisteredRelatives,bool bScanParent=true,int PK_Device_ExcludeChild=0);
		char DeviceIsRegistered(int PK_Device); // Returns Y, N, D (for disabled), E (for error with sockets)

		virtual void KillSpawnedDevices();

		/**
		 * @brief replaces the values for all the parameters in m_pData with new values specified in sReplacement
		 * @param sReplacement looks like this: param_index,param_new_value,param_index,param_new_value ...
		 */
		virtual void ReplaceParams( ::std::string sReplacement );

		/**
		 * @brief tries to connect to a client socket
		 * @return true on success, false otherwise
		 */
		virtual bool Connect(int iPK_DeviceTemplate, std::string s="");
		virtual void PostConnect() {}  // Placeholder function that is called after a connect

		/**
		 * @brief Reports to the user on the console that the device cannot load because
		 * the router needs to be reloaded, and asks the user if he wants to
		 * reload the router.  Returns true if the user chose yes and the reload was sent
		 * and connection should be re-attempted.  False means abort.
		 */
		virtual bool RouterNeedsReload();

		/**
		 * @brief Reports to the user on the console that the device ID is invalid.  It will
		 * ask the server for a list of devices with this template and ask the user which device
		 * to use.  Returns 0 meaning abort, or a positive number representing the new device ID
		 */
		virtual int DeviceIdInvalid();

		/**
		 * @brief Reports to the user that the router cannot be reloaded now
		 */
		virtual void CannotReloadRouter();

		/**
		 * @brief Fill the map with a list of all devices matching the template
		 */
		virtual void GetDevicesByTemplate(int PK_DeviceTemplate,map<int,string> *p_mapDevices);

		/**
		 * @brief Fill the map with a list of all devices matching the category
		 */
		virtual void GetDevicesByCategory(int PK_DeviceTemplate,map<int,string> *p_mapDevices);

		/** events */

		/**
		 * @brief override to get notified when the server has changed a data parameter
		 */
		virtual void OnDataChange( int /*ID*/, string /*ValueOld*/, string /*ValueNew*/ ) {};

		/**
		 * @brief sets the m_bQuit_get()mb data; used when DeviceManager wants the app to terminate
		 * @todo check: 1/4/2004 - AB removed this to try to stop socket failures Disconnect()
		 */
		virtual void OnQuit();

		/**
		 * @brief when DeviceManager wants the app to recheck its config, by default it exits.
		 * sets the m_bReload, m_bQuit, m_bTerminate flags to true
		 * @todo test if it's a plug-in, just die and it will be reloaded (same as above Disconnect());
		 */
		virtual void OnReload();

		/**
		* @brief If your device implements this function set m_bImplementsPendingTasks to true
		* in the constructor before Connect().
		* Return true if you have pending tasks, and if listPendingTask is not NULL
		* add info on your tasks to it
		*/
		virtual bool ReportPendingTasks(PendingTaskList * /*pPendingTaskList*/ =NULL) { return false; }

		/**
		* @brief Clients can call this to get the list of pending tasks from PK_Device, where the pair is task type, description
		* Pass in the request socket to use to get the response
		*/
		static bool ReportPendingTasksFromDevice(Socket *pSocket,int PK_Device_Requestor,int PK_Device,PendingTaskList *pPendingTaskList=NULL);

		virtual bool AbortTask(int /*PK_Device_Requestor*/,int /*ID*/) { return true; } // Abort the task with ID per PK_Device_Requestor

		// Override this if there are times when you can't allow a reload, and if so, return false and fill in sReason
		virtual bool SafeToReload(string & /*sReason*/) { return true; }

		/**
		 * @brief just calls OnQuit()
		 * @see m_bQuit
		 */
		virtual void OnUnexpectedDisconnect() { OnReload(); };

		/**
		 * @brief called when the socket disconnects, should be overriden
		 */
		virtual void OnDisconnect() {};

		/**
		 * @brief The derived classes must implement this.  We can't make it pure virtual, though, since a device
		 * with children of an unknown type may create generic CommandImpl.
		 */
		virtual int PK_DeviceTemplate_get() { return 0; };

		/**
		 * @brief override to handle specifying real-time parameter data (like running time)
		 */
		virtual void RequestingParameter( int /*iPK_DeviceData*/ ) {};

		/**
		 * @brief when it receives a strin it sends it to all the command impl children
		 */
		virtual void ReceivedString( string sLine, int nTimeout = -1 );

		/**
		 * @brief performes a specific action bases on the message type
		 */
		virtual ReceivedMessageResult ReceivedMessage( Message *pMessage );

		/**
		 * @brief adds the message to the message queue
		 * Most of the time when a device wants to send a message it doesn't want to block it's thread
		 * until the message is sent by using the normal SendMessageWithConfirm.  Also, SendMessageWithConfirm introduces
		 * the possibility that the receiving device may send something back in response, creating a
		 * potential race condition if the thread sending the message is blocking the same mutex
		 * as the thread on which the new command comes in on.  To prevent this, call QueueMessageToRouter,
		 * which causes the message to be put in a queue and sent on a separate thread.  This is like
		 * the difference between SendMessage and PostMessage in Windows.
		 */
		void QueueMessageToRouter( Message *pMessage );

		/**
		 * @brief sends a message.  This will send the message on the event socket since that is the correct outgoing socket
		 */
		void SendMessageToRouter( Message *pMessage ) { m_pEvent->SendMessage(pMessage); }

		/**
		 * @brief sends all the messges in the message queue
		 * @warning Do not call directly.  For internal use only (called by the queue handler only)
		 */
		virtual void ProcessMessageQueue();

		/**
		 * @brief
		 * Send a command.  If the command includes out parameters, the framework
		 * will wait for a response so it can fill those parameters.  Otherwise, it will
		 * send without waiting for a reply
		 */

		bool SendCommand( class PreformedCommand &pPreformedCommand ) { return InternalSendCommand( pPreformedCommand,-1,NULL ); }

		/**
		 * @brief
		 * Send a command and wait for the reply, which will be put into p_sReponse.
		 * If the command is successful, it will be "OK".
		 */
		bool SendCommand(class  PreformedCommand &pPreformedCommand, string *p_sResponse ) { return InternalSendCommand( pPreformedCommand,1,p_sResponse ); }

		/**
		 * @brief
		 * Send a command and don't wait for a reply even if the command has out
		 * parameters.  The out parameters will be unmodified
		 */
		bool SendCommandNoResponse( class PreformedCommand &pPreformedCommand ) { return InternalSendCommand( pPreformedCommand,0,NULL ); }
		bool InternalSendCommand( class PreformedCommand &pPreformedCommand, int iConfirmation, string *p_sResponse );

		/**
		 * @brief
		 * Tells the router to execute the given command group (ie group of commands)
		 */
		void ExecCommandGroup(int PK_CommandGroup);

		/**
		 * @brief Register a function that we want to get called back when a message matching a given criteria is received.
		 * Any of the criteria may be 0, indicating all messages will be a match.
		 * If this is not a plugin, it will return the counter ID for this interceptor, otherwise it returns 0
		 */
		int RegisterMsgInterceptor(MessageInterceptorFn pMessageInterceptorFn,int PK_Device_From,int PK_Device_To,int PK_DeviceTemplate,int PK_DeviceCategory,int MessageType,int MessageID);

		/**
		 * @brief If a plug-in dies, the router also dies, and all interceptors are cleared.  However if this
		 * is a normal external device, it can stop and restart itself at will, and therefore may end up
		 * registering the same interceptor more than once.  To prevent this a device can call this function
		 * when it starts to purge any interceptors the router has for it.
		 */
		void PurgeInterceptors();

		/**
		 * @brief If 1 message is sent on a queue, and a later message is sent in realtime, it's possible
		 * for the later message to actually get sent first.  This function blocks the thread until all
		 * messages in the queue have been sent.
		 */
		void WaitForMessageQueue();

		/**
		 * @brief If this is not a plugin loaded into the Router's memory space, the callback will be in the form of a message to this function
		 */
		void InterceptedMessage(Message *pMessage);

		/**
		 * @brief This device is processing commands which may not complete immediately, so incoming messages should be put into a queue
		 * and duplicate messages that aren't yet processed should be purged.  To accomadate lighting and a/v, on/off/set level/set volume are treated as
		 * alike.  In other words, an immediate succession of on/off/on/off/set level/off, all will be purged except the last off
		 */
		void BufferMessages() { if( !m_pMessageBuffer ) m_pMessageBuffer = new MessageBuffer(this); }

		/** Re-route the primitives through the primary device command. */

		/** @brief sending a string directly or through the parent device */
		virtual bool SendString(string s)
		{
			if ( this != m_pPrimaryDeviceCommand )
				return m_pPrimaryDeviceCommand->SendString( s );
			else
				return Socket::SendString( s );
		};

		/** @brief recieving a string directly or through the parent device */
		virtual bool ReceiveString( string &s, int nTimeout = -1 )
		{
			if ( this != m_pPrimaryDeviceCommand )
				return m_pPrimaryDeviceCommand->ReceiveString( s );
			else
				return Socket::ReceiveString( s, nTimeout );
		}

		/** @brief sending a data block directly or through the parent device */
		virtual bool SendData( int iSize, const char *pcData )
		{
			if ( this != m_pPrimaryDeviceCommand )
				return m_pPrimaryDeviceCommand->SendData( iSize, pcData );
			else
				return Socket::SendData( iSize, pcData );
		}

		/** @brief recieving a data block directly or through the parent device */
		virtual bool ReceiveData( int iSize, char *pcData, int nTimeout = -1)
		{
			if ( this != m_pPrimaryDeviceCommand )
				return m_pPrimaryDeviceCommand->ReceiveData( iSize, pcData, nTimeout );
			else
				return Socket::ReceiveData( iSize, pcData, nTimeout );
		}

		/** @brief Gets the "IPAddress" for the given device, or this device by default */
		string GetIpAddress( int dwPK_Device=0 );

		/** @brief Gets the "state" for the given device, or this device by default */
		virtual string GetState( int dwPK_Device=0 );

		/** @brief Gets the "status" for the given device, or this device by default */
		virtual string GetStatus( int dwPK_Device=0 );

		/** @brief Sets the "state" for the given device, or this device by default */
		virtual bool SetState( string sState, int dwPK_Device=0 );

		/** @brief Sets the "status" for the given device, or this device by default */
		virtual bool SetStatus( string sStatus, int dwPK_Device=0 );

		virtual void EnableDevice( int PK_Device, bool bEnabled ); // Sets the given device to enabled if true, or disable if false

		/**
		 * @brief Gets the current value of a given device data from one of our children
		 */
		bool GetChildDeviceData( int PK_Device, int PK_DeviceData, string &sValue, DeviceData_Impl *pDeviceData_Impl=NULL); // Leave pDeviceData_Impl=NULL

		/**
		 * @brief Gets the current value of a given device data directly from the device
		 */
		string GetCurrentDeviceData( int PK_Device, int PK_DeviceData ) { return m_pEvent->GetCurrentDeviceData(PK_Device,PK_DeviceData); }

		/**
		 * @brief Sends a message to DCERouter to set the device data for the given device
		 */
		void SetDeviceDataInDB(int PK_Device,int PK_DeviceData,string Value);
		void SetDeviceDataInDB(int PK_Device,int PK_DeviceData,const char *Value) { SetDeviceDataInDB(PK_Device,PK_DeviceData,string(Value)); }
		void SetDeviceDataInDB(int PK_Device,int PK_DeviceData,int Value) { SetDeviceDataInDB(PK_Device,PK_DeviceData,StringUtils::itos(Value)); }
		void SetDeviceDataInDB(int PK_Device,int PK_DeviceData,bool Value) { SetDeviceDataInDB(PK_Device,PK_DeviceData,Value ? "1" : "0"); }

		/**
		 *
		 * Watchdog will kill the connection if it exceeds Timeout.
		 * Eventually SendMessage will need to be overridden to create
		 * a watchdog and retransmit if they don't go through.
		 *
		 */

		/** Starts the watchdog thread with the specified timeout
        @param Timeoutis the thread timeout.
		 */
		void StartWatchDog( clock_t Timeout );


		/** Stops the watchdog thread
		 */
		void StopWatchDog();


		/** For internal use only.

        There are a couple global pointers allocated and if a device wants
         to run a memory leak tool it can call this to delete them as it's exiting
		 */
		void DeleteGlobalAllocs();

		// Misc Cleanup tasks after we connect
		void MiscCleanup();
		void SetDeviceCategories(DeviceData_Impl *pData);

		// When we're running as a test device, without an actual router, this lets us control
		// the device with the keyboard
		void RunLocalMode();
		Message *GetLocalModeResponse();
	};

}

#endif
