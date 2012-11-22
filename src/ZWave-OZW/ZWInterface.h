/*
     Copyright (C) 2012 LinuxMCE

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef ZWInterface_h
#define ZWInterface_h

#include "ZWave.h"
#include "ZWConfigData.h"
#include "NodeInfo.h"

namespace DCE
{
	class ZWInterface
	{
	private:
		uint32 g_homeId;
		bool   g_initFailed;
		bool   m_bInitDone;
		pthread_cond_t  initCond; 
		pthread_mutex_t initMutex;
		pthread_mutex_t g_criticalSection;

		list<NodeInfo*> g_nodes;

		ZWave* m_pZWave;

	public:
		ZWInterface();
		~ZWInterface();

		NodeInfo* GetNodeInfo ( OpenZWave::Notification const* _notification);

		uint32 GetHomeId();
		void Lock();
		void UnLock();

		bool Init(ZWConfigData* data);
		void SetZWave(ZWave* pZWave);
		bool RequireRestart(ZWConfigData* data);
		bool IsReady();
		
		static void OnNotification_static(OpenZWave::Notification const* _notification, void* _context);
		void OnNotification(OpenZWave::Notification const* _notification);
	};
}
#endif
