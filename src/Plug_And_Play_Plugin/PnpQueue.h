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
#ifndef PnpQueue_h
#define PnpQueue_h

using namespace std;
class Database_pluto_main;
class Row_PnpQueue;

#define TIMEOUT_PROMPTING_USER					120  // If the user doesn't respond in this many seconds, try asking again
#define TIMEOUT_DETECTION_SCRIPT				20	// Maximum number of seconds to wait for a detction script to return
#define TIMEOUT_WAITING_FOR_DEVICE				20	// Maximum number of seconds to wait for a device to come online
#define TIMEOUT_PRE_PNP_SCRIPT					120  // How long to wait for the pre pnp script
#define TIMEOUT_WAITING_ORBITERS				60 // How long to wait for orbiters to register that can handle this device

#define	PNP_DETECT_STAGE_DETECTED				10
#define	PNP_DETECT_STAGE_CONFIRMING_POSSIBLE_DT	11
#define	PNP_DETECT_STAGE_RUNNING_DETECTION_SCRIPTS	12
#define	PNP_DETECT_STAGE_PROMPTING_USER_FOR_DT	13
#define	PNP_DETECT_STAGE_RUNNING_PRE_PNP_SCRIPT	14
#define	PNP_DETECT_STAGE_PROMPTING_USER_FOR_OPT	15
#define	PNP_DETECT_STAGE_ADD_DEVICE				16
#define	PNP_DETECT_STAGE_ADD_SOFTWARE			17
#define	PNP_DETECT_STAGE_START_DEVICE			18
#define	PNP_DETECT_STAGE_DONE					19

#define PNP_REMOVE_STAGE_REMOVED				20
#define PNP_REMOVE_STAGE_DONE					21


// We want devices to be processed in the order they're detected because NAS devices are detected before fileservers
// and you want to ask the user if it's a known NAS before asking if it's a generic file server.  If both entries are queued
// then when ReleaseQueuesBlockedFromPromptingState releases them, the first one will immediately block in BlockIfOtherQueuesAtPromptingState
// over the 2nd one.  So, when devices were just released in ReleaseQueuesBlockedFromPromptingState we give them the 
// temporary stage: PNP_TEMP_STAGE_GO_TO_PROMPTING_USER_FOR_DT so that the Process function, which handles them in order
// won't block over them.
#define	PNP_TEMP_STAGE_GO_TO_PROMPTING_USER_FOR_DT	30



class Database_pluto_main;
class Row_Device;

#include <pthread.h>
#include "Pnp_PreCreateOptions.h"

namespace DCE
{
	// A function for locating devices of specific categories since some require special handling
	typedef bool( PnpQueue::*fnLocateDevice )( PnpQueueEntry *pPnpQueueEntry );

	
	/** @brief
    */
    class PnpQueue
    {
		friend class Plug_And_Play_Plugin;
		friend class Pnp_PreCreateOptions;
		map<int,class PnpQueueEntry *> m_mapPnpQueueEntry;
		PnpQueueEntry *m_mapPnpQueueEntry_Find(int PK_PnpQueue) { map<int,class PnpQueueEntry *>::iterator it = m_mapPnpQueueEntry.find(PK_PnpQueue); return it==m_mapPnpQueueEntry.end() ? NULL : (*it).second; }
		class Plug_And_Play_Plugin *m_pPlug_And_Play_Plugin;
		Database_pluto_main *m_pDatabase_pluto_main;
		bool m_bThreadRunning;
		Pnp_PreCreateOptions m_Pnp_PreCreateOptions;
		map<string,fnLocateDevice> m_mapCategoryLocateDevice;
		pthread_t m_PnpQueueThread_Id;

		// When the Detected Device event has a value in the 'Signature' parameter, then we will hold up processing all events
		// from that signature until we get a Done Detecting Devices event.  When we do get a Done Detecting Devices, then 
		// before starting to process those events, we will first disable any events that were not detected.  So, if the user has 
		// a device X attached detected by signature "16,HAL", and then the user removes X while the system is off, so we don't get 
		// the removed event, at bootup, HAL will fire all the Device Detected events.  When it fires Done Detecting Devices we'll
		// see that it didn't detect X and disable it, before we process the new detected events since maybe X is there but on 
		// a different USB port/PCI Slot
		map<string,bool> m_mapSignaturesReady;

	public:
		PnpQueue(class Plug_And_Play_Plugin *pPlug_And_Play_Plugin);
		~PnpQueue();
		void Run();  // The main loop

		// Main entry points
		void NewEntry(PnpQueueEntry *pPnpQueueEntry);

		// Helper functions
		bool LocateDevice(PnpQueueEntry *pPnpQueueEntry);
		bool CheckForDeviceTemplateOnWeb(PnpQueueEntry *pPnpQueueEntry);
		void RunPnpDetectionScript(PnpQueueEntry *pPnpQueueEntry);
		void ReleaseQueuesBlockedFromPromptingState(PnpQueueEntry *pPnpQueueEntry);
		bool BlockIfOtherQueuesAtPromptingState(PnpQueueEntry *pPnpQueueEntry);
		void ReadOutstandingQueueEntries();
		bool DetermineOrbitersForPrompting(PnpQueueEntry *pPnpQueueEntry,bool bBlockIfNone);
		string GetDescription(PnpQueueEntry *pPnpQueueEntry);
		string GetDeviceName(PnpQueueEntry *pPnpQueueEntry);  // Get the default name to use for this device
		bool DeviceMatchesCriteria(Row_Device *pRow_Device,PnpQueueEntry *pPnpQueueEntry);
		void SetDisableFlagForDeviceAndChildren(Row_Device *pRow_Device,bool bDisabled);

		bool ReenableDevice(PnpQueueEntry *pPnpQueueEntry,Row_Device *pRow_Device);
		Row_Device *FindDisabledDeviceTemplateOnPC(int PK_Device_PC,int PK_DeviceTemplate);
		void RunSetupScript(PnpQueueEntry *pPnpQueueEntry); // Run any setup script in the parameter 'Setup Script' with the parms Device Location_ON_PCI_Bus
		void DoneDetectingDevices(string sSignature);

		// The various process functions,
		bool Process(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Detected(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Confirm_Possible_DT(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Running_Detction_Scripts(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Prompting_User_For_DT(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Running_Pre_Pnp_Script(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Prompting_User_For_Options(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Add_Device(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Add_Software(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Detect_Stage_Start_Device(PnpQueueEntry *pPnpQueueEntry);
		bool Process_Remove_Stage_Removed(PnpQueueEntry *pPnpQueueEntry);

		// Specialized LocateDevice methods
		bool LocateFileShare(PnpQueueEntry *pPnpQueueEntry);
		bool LocateFileServer(PnpQueueEntry *pPnpQueueEntry);
	};
}

#endif
