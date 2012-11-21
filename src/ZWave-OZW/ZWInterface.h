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

		ZWave* m_pZWave;

	public:
		ZWInterface()
		{
			m_pZWave = NULL;
			g_homeId = 0;
			g_initFailed = false;
			m_bInitDone = false;
			initCond  = PTHREAD_COND_INITIALIZER;
			initMutex = PTHREAD_MUTEX_INITIALIZER;
			pthread_mutexattr_t mutexattr;

			pthread_mutexattr_init ( &mutexattr );
			pthread_mutexattr_settype( &mutexattr, PTHREAD_MUTEX_RECURSIVE );

			pthread_mutex_init( &g_criticalSection, &mutexattr );
			pthread_mutexattr_destroy( &mutexattr );

		}

		uint32 GetHomeId() {
			return g_homeId;
		}

		void Lock() {
			pthread_mutex_lock( &g_criticalSection );
		}

		void UnLock() {
			pthread_mutex_unlock( &g_criticalSection );
		}

		bool Init(ZWConfigData* data) {
			LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::Init() start");

			pthread_mutex_lock( &initMutex );

			// Create the OpenZWave Manager.
			// The first argument is the path to the config files (where the manufacturer_specific.xml file is located
			// The second argument is the path for saved Z-Wave network state and the log file.  If you leave it NULL 
			// the log file will appear in the program's working directory.
			OpenZWave::Options::Create( "/etc/openzwave/config/", "", "" );
			OpenZWave::Options::Get()->Lock();

			OpenZWave::Manager::Create();
			
			OpenZWave::Manager::Get()->AddWatcher( OnNotification_static, this );
			
			OpenZWave::Manager::Get()->AddDriver(data->m_sPort );
			
			LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::Init() waiting for OZW init");
			// Now we just wait for the driver to become ready
			pthread_cond_wait( &initCond, &initMutex );

			m_bInitDone = true;
			
			if( !g_initFailed )
			{
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::Init() init successfull");
				OpenZWave::Manager::Get()->WriteConfig( g_homeId );
				
				OpenZWave::Driver::DriverData data;
				OpenZWave::Manager::Get()->GetDriverStatistics( g_homeId, &data );
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "SOF: %d ACK Waiting: %d Read Aborts: %d Bad Checksums: %d\n", data.s_SOFCnt, data.s_ACKWaiting, data.s_readAborts, data.s_badChecksum);
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Reads: %d Writes: %d CAN: %d NAK: %d ACK: %d Out of Frame: %d\n", data.s_readCnt, data.s_writeCnt, data.s_CANCnt, data.s_NAKCnt, data.s_ACKCnt, data.s_OOFCnt);
				LoggerWrapper::GetInstance()->Write(LV_ZWAVE,"Dropped: %d Retries: %d\n", data.s_dropped, data.s_retries);
				
				// int ourNodeId = Manager::Get()->GetControllerNodeId(g_homeId);

				return true;
			}
			LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::Init() init failed!");
			return false;
		}

		void SetZWave(ZWave* pZWave) {
			m_pZWave = pZWave;
		}

		bool RequireRestart(ZWConfigData* data) {
			// TODO compare new config with old config and decide what to do
			return false;
		}

		bool IsReady() {
			// perhaps we should also check the status of the init, has ozw
			// sent driver ready? not a problem as long as ozw init not in its own thread...
			bool ready = m_bInitDone && !g_initFailed;
			LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "ZWInterface::IsReady() returns %s", ready ? "true" : "false");
			return ready;
		}
		
		static void OnNotification_static(OpenZWave::Notification const* _notification, void* _context) {
			ZWInterface *zwI = (ZWInterface*)_context;
			zwI->OnNotification(_notification);
		}

		void OnNotification(OpenZWave::Notification const* _notification) {

			switch( _notification->GetType() )
			{
				
			case OpenZWave::Notification::Type_DriverReady:
			{
				g_homeId = _notification->GetHomeId();
				break;
			}


			case OpenZWave::Notification::Type_DriverFailed:
			{
				g_initFailed = true;
				pthread_cond_broadcast(&initCond);
				break;
			}

			case OpenZWave::Notification::Type_AwakeNodesQueried:
			case OpenZWave::Notification::Type_AllNodesQueried:
			{
				pthread_cond_broadcast(&initCond);
				break;
			}

			default:
			{
				if (m_pZWave != NULL)
					m_pZWave->OnNotification( _notification);
			}
			}
		}
	};
}
#endif
