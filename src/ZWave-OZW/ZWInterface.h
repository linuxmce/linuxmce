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

namespace DCE
{
	class NodeInfo;
	class ZWConfigData;

	class ZWInterface
	{
	private:
		uint32 g_homeId;
		uint32 m_ourNodeId;
		bool   g_initFailed;
		bool   m_bInitDone;
		pthread_cond_t  initCond; 
		pthread_mutex_t initMutex;
		pthread_mutex_t g_criticalSection;

		list<NodeInfo*> g_nodes;
		uint32 m_iPolledValues;

		ZWave* m_pZWave;
		ZWConfigData* m_pConfigData;
		string m_sLogFile;
	public:
		ZWInterface();
		~ZWInterface();

		NodeInfo* GetNodeInfo ( OpenZWave::Notification const* _notification);
		NodeInfo* GetNodeInfo( uint32 homeId, uint8 nodeId);
		list<NodeInfo*> GetNodes() { return g_nodes; }
		uint32 GetHomeId();
		uint32 GetNodeId() { return m_ourNodeId; }
		void Lock();
		void UnLock();

		bool Init(ZWConfigData* data);
		void SetZWave(ZWave* pZWave);
		bool RequireRestart(ZWConfigData* data);
		bool IsReady();
		void SetLogFileName(string sFileName) { m_sLogFile = sFileName; };
		void UpdatePolling();
		static void OnNotification_static(OpenZWave::Notification const* _notification, void* _context);
		void OnNotification(OpenZWave::Notification const* _notification);

		OpenZWave::ValueID* GetValueIdByNodeInstanceLabel(int iNodeId, unsigned int iInstance, string label);
		OpenZWave::ValueID* GetValueIdByLabel(NodeInfo* pNodeInfo, unsigned int iInstance, string label);
		bool SetIntValue(int iNodeId, int iInstance, string label, int iValue);
		bool SetWakeUp(int iNodeId, int iValue);
	};
}
#endif
