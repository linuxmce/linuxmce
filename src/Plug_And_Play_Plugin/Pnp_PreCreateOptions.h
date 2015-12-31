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
#ifndef Pnp_PreCreateOptions_h
#define Pnp_PreCreateOptions_h

using namespace std;

class Database_pluto_main;
class Row_DeviceTemplate;

namespace DCE
{
	class OH_Orbiter;
	class PnpQueueEntry;

    /** @brief
    */
    class Pnp_PreCreateOptions
    {
		class PnpQueue *m_pPnpQueue;
		Database_pluto_main *m_pDatabase_pluto_main;
	public:
		Pnp_PreCreateOptions(class PnpQueue *pPnpQueue,class Plug_And_Play_Plugin *pPlug_And_Play_Plugin);

		bool OkayToCreateDevice(PnpQueueEntry *pPnpQueueEntry);
		bool OkayToCreateDevice_Presets(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate);
		bool OkayToCreateDevice_AlarmPanel(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate);
		bool OkayToCreateDevice_NetworkStorage(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate);
		bool OkayToCreate_Cameras(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate);
		bool OkayToCreate_MobilePhone(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate);
		bool OkayToCreateDevice_Username(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate);
		bool OkayToCreateDevice_Room(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate);
		bool OkayToCreate_CaptureCard(PnpQueueEntry *pPnpQueueEntry,Row_DeviceTemplate *pRow_DeviceTemplate);
	};
}

#endif
