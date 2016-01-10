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
#ifndef PnpQueueEntry_h
#define PnpQueueEntry_h

#include "Plug_And_Play_Plugin.h"

using namespace std;
class Database_pluto_main;
class Row_PnpQueue;
class Row_DHCPDevice;
class Row_Device;

namespace DCE
{
    /** @brief
    */
    class PnpQueueEntry
    {
		friend class PnpQueue;
		friend class Plug_And_Play_Plugin;
		friend class Pnp_PreCreateOptions;
		friend class Pnp_PostCreateOptions;

		bool m_bCreateWithoutPrompting,  // Create the device without prompting
			m_bUseAllOrbitersForPrompt;  // The user isn't responding on the default orbiters, ask on all orbiters so this entry isn't blocked forever
		typedef enum { pnpqe_blocked_none=0, 
			pnpqe_blocked_running_detection_scripts=1, 
			pnpqe_blocked_prompting_device_template=2, 
			pnpqe_blocked_pre_pnp_script=3, 
			pnpqe_blocked_prompting_options=4, 
			pnpqe_blocked_installing_software=5, 
			pnpqe_blocked_waiting_for_other_prompting=6, // Waiting for other orbiters to respond to the pnp message
			pnpqe_block_processing_suspended=7, 
			pnpqe_blocked_waiting_for_other_device=8,
			pnpqe_blocked_waiting_for_orbiters=9,  // There are no orbiters registered, block this until there are
			pnpqe_blocked_waiting_for_initial_detection=10  // Waiting for the 'Done Detecting Devices' event for this signature
		} EBlockedState;

		EBlockedState m_EBlockedState;
		time_t m_tTimeBlocked;
		Database_pluto_main *m_pDatabase_pluto_main;
		Row_PnpQueue *m_pRow_PnpQueue;
		class OH_Orbiter *m_pOH_Orbiter_Active; // The Orbiter to use for displaying messages
		map<int,Row_DHCPDevice *> m_mapPK_DHCPDevice_possible;
		int m_iPK_DHCPDevice,m_dwPK_Device_TopLevel,m_dwPK_PnpQueue_BlockingFor,m_iPK_Room;
		map<int,string> m_mapPK_DeviceData;
		string m_mapPK_DeviceData_Find(int PK_DeviceData) { map<int,string>::iterator it = m_mapPK_DeviceData.find(PK_DeviceData); return it==m_mapPK_DeviceData.end() ? "" : (*it).second; }
		Row_Device *m_pRow_Device_Reported;
		string m_sPK_Orbiter_List_For_Prompts;
		string m_sDetectionScript_Running;
		Plug_And_Play_Plugin *m_pPlug_And_Play_Plugin;

	public:
		// Constructor for device detected
		PnpQueueEntry(Plug_And_Play_Plugin *pPlug_And_Play_Plugin,
			Database_pluto_main *pDatabase_pluto_main,
			string sDeviceData,
			string sIPAddress,
			string sMacAddress,
			string sCategory,
			int PK_CommMethod,
			int PK_DeviceTemplate,
			int PK_Device_Reported,
			int PK_PnpProtocol,
			string sPnpSerialNumber,
			string sText,
			string sVendorModelId,
			string sSignature);

		// Constructor for device removed
		PnpQueueEntry(Plug_And_Play_Plugin *pPlug_And_Play_Plugin,
			Database_pluto_main *pDatabase_pluto_main,
			string sDeviceData,
			string sIPAddress,
			string sMacAddress,
			int PK_CommMethod,
			int PK_Device_Created,
			int PK_Device_Reported,
			int PK_DeviceTemplate,
			int PK_PnpProtocol,
			string sPnpSerialNumber,
			string sText,
			string sVendorModelId);

		// Constructor for restarting unfinished ones in the queue
		PnpQueueEntry(Plug_And_Play_Plugin *pPlug_And_Play_Plugin,Row_PnpQueue *pRow_PnpQueue);

		void Stage_set(int Stage);

		void Block(EBlockedState eBlockedState);
		string DeviceDataAsString();
		void ParseDeviceData(string sDeviceData);
		void FindTopLevelDevice();
		void AssignDeviceData(Row_Device *pRow_Device);
		void RemoveBlockedDeviceData();  // Remove any device data from HAL that is blocked per the device data Pnp Ignore Device Data in DeviceTemplate_DeviceData
		bool IsDuplicate(PnpQueueEntry *pPnpQueueEntry);  // Returns true if the entry passed in is the same as this one
		bool CompareShortSerialNumberAndPCILocation(PnpQueueEntry *pPnpQueueEntry);  // String any trailing _XX from the serial #'s.  If they're the same, and the location on PCI bus is the same, it's a match and return true
		string BaseSerialNumber(string sString); // If the serial number is in the format: /a/b/c/d_e  strip the e.  But only with that syntax
		bool CompareParms(PnpQueueEntry *pPnpQueueEntry);  // Compare the parameters and return true if they're the same
		string StageAsText();
		string ToString();

		void m_pOH_Orbiter_Active_set(int PK_Device);   // The that is the user is currently responding to questions with
		void m_pOH_Orbiter_Active_set(OH_Orbiter *pOH_Orbiter);   // Called by the prior
		class OH_Orbiter *m_pOH_Orbiter_Active_get() { return m_pOH_Orbiter_Active; }
		void UseAllOrbitersForPrompt(); // Call this to use all orbiter for the prompts because the user isn't responding to the default ones
	};
}

#endif
