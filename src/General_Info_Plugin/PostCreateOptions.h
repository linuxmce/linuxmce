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
#ifndef PostCreateOptions_h
#define PostCreateOptions_h

using namespace std;

class Database_pluto_main;
class Row_Device;

namespace DCE
{
	class OH_Orbiter;

	/** @brief
    */
    class PostCreateOptions
    {
		Database_pluto_main *m_pDatabase_pluto_main;
		class Router *m_pRouter;
		class Orbiter_Plugin *m_pOrbiter_Plugin;
		class Command_Impl *m_pCommand_Impl;
	public:
		PostCreateOptions(Database_pluto_main *pDatabase_pluto_main,Router *pRouter,Command_Impl *pCommand_Impl);

		void PostCreateDevice(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter);
		void PostCreateDevice_AlarmPanel(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter);
		void PostCreateSecurityDevice(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter);
		void PostCreateDevice_NetworkStorage(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter);
		void PostCreateDevice_FileServer(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter);
		void PostCreateDevice_Cameras(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter);
		void PostCreateDevice_DisklessMD(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter);
		void PostCreateDevice_CaptureCard(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter);
		void PostCreateDevice_LightingInterface(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter);
		void PostCreateDevice_TV(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter);
		void PostCreateDevice_Receiver(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter);
	};
}

#endif
