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

#define BASIC_TYPE_CONTROLLER                           0x01

#define GENERIC_TYPE_GENERIC_CONTROLLER                 0x01
#define GENERIC_TYPE_STATIC_CONTROLLER                  0x02
#define GENERIC_TYPE_AV_CONTROL_POINT                   0x03
#define GENERIC_TYPE_THERMOSTAT                         0x08
#define GENERIC_TYPE_WINDOW_COVERING                    0x09
#define GENERIC_TYPE_SWITCH_BINARY                      0x10
#define GENERIC_TYPE_SWITCH_MULTILEVEL                  0x11
#define GENERIC_TYPE_SWITCH_REMOTE                      0x12
#define GENERIC_TYPE_SENSOR_BINARY                      0x20
#define GENERIC_TYPE_SENSOR_MULTILEVEL                  0x21
#define GENERIC_TYPE_SENSOR_ALARM			0xa1
#define GENERIC_TYPE_METER			        0x31

#define SPECIFIC_TYPE_NOT_USED				0x00
#define SPECIFIC_TYPE_POWER_SWITCH_MULTILEVEL		0x01
#define SPECIFIC_TYPE_MOTOR_MULTIPOSITION		0x03
#define SPECIFIC_TYPE_SCENE_SWITCH_MULTILEVEL		0x04
#define SPECIFIC_TYPE_CLASS_A_MOTOR_CONTROL		0x05
#define SPECIFIC_TYPE_CLASS_B_MOTOR_CONTROL		0x06
#define SPECIFIC_TYPE_CLASS_C_MOTOR_CONTROL		0x07

#define SPECIFIC_TYPE_ADV_ZENSOR_NET_SMOKE_SENSOR	0x0a
#define SPECIFIC_TYPE_BASIC_ROUTING_SMOKE_SENSOR	0x06
#define SPECIFIC_TYPE_BASIC_ZENSOR_NET_SMOKE_SENSOR	0x08
#define SPECIFIC_TYPE_ROUTING_SMOKE_SENSOR		0x07
#define SPECIFIC_TYPE_ZENSOR_NET_SMOKE_SENSOR		0x09


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
		string m_sLogFile;
	public:
		ZWInterface();
		~ZWInterface();

		NodeInfo* GetNodeInfo ( OpenZWave::Notification const* _notification);
		NodeInfo* GetNodeInfo( uint32 homeId, uint8 nodeId);
		list<NodeInfo*> GetNodes() { return g_nodes; }
		uint32 GetHomeId();
		void Lock();
		void UnLock();

		bool Init(ZWConfigData* data);
		void SetZWave(ZWave* pZWave);
		bool RequireRestart(ZWConfigData* data);
		bool IsReady();
		void SetLogFileName(string sFileName) { m_sLogFile = sFileName; };

		static void OnNotification_static(OpenZWave::Notification const* _notification, void* _context);
		void OnNotification(OpenZWave::Notification const* _notification);

		OpenZWave::ValueID* GetValueIdByNodeInstanceLabel(int iNodeId, int iInstance, string label);
		OpenZWave::ValueID* GetValueIdByLabel(NodeInfo* pNodeInfo, int iInstance, string label);
		bool SetIntValue(int iNodeId, int iInstance, string label, int iValue);
		bool SetWakeUp(int iNodeId, int iValue);
	};
}
#endif
