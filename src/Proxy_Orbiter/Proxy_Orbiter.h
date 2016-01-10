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
#ifndef __Proxy_Orbiter_H__
#define __Proxy_Orbiter_H__

//-----------------------------------------------------------------------------------------------------
#include <iostream> 
using namespace std; 
//-----------------------------------------------------------------------------------------------------
#include "../Orbiter/Orbiter.h"
#include "SocketListener.h"
//-----------------------------------------------------------------------------------------------------
#define CURRENT_SCREEN_IMAGE        "screen.png"
#define CURRENT_SCREEN_XML          "screen.xml"
//-----------------------------------------------------------------------------------------------------

class DataGridRenderer_Proxy;

namespace DCE
{
	class Proxy_OrbiterRenderer_SDL;

	class Proxy_Orbiter : public Orbiter, public SocketListener
	{
		friend class Proxy_OrbiterRenderer_SDL;
		friend class ::DataGridRenderer_Proxy;

		bool IsProcessingRequest();
		void StartProcessingRequest();
		void EndProcessingRequest();
		void *PushRefreshEventWatchdog(void *);

		int m_iListenPort;
		int m_ImageQuality;
		int m_iImageCounter;  // We will increment this each time we have a new image so we can keep track of whether a connected device has the lateest
		int m_iLastImageSent;

		map<int,int> m_mapID_ImageCounter; // Map of connection ID's to the latest image counter
		deque<string> m_dequeXMLItems;
		deque<string> m_dequeCellXMLItems;

		string m_sRequestUrl;
		string m_sPngImageUrl;
		string m_sBaseUrl;

		pluto_pthread_mutex_t m_ActionMutex; 
		pthread_cond_t m_ActionCond;

		string sNoMediaSoftKeysXml;
		string sAllSoftKeysXml;

		bool m_bPhoneRespondedToPush;
		bool m_bProcessingRequest;
		string m_sInternalServerAddress;
		string m_sRemotePhoneIP;

		int m_nCurrentScreenId;
		pluto_pthread_mutex_t m_ResourcesMutex;

		bool m_bNews;
		string m_sBitsDirectory;

	public:
		Proxy_Orbiter(int DeviceID, 
			int PK_DeviceTemplate, string ServerAddress);
		virtual ~Proxy_Orbiter();

		void StopProcessingRequest(void *p);

		// Public virtual methods
		virtual bool GetConfig();

		virtual unsigned long ImageQuality() { return m_ImageQuality; }
		virtual void ImageQuality(unsigned long nImageQuality) { m_ImageQuality = nImageQuality; }
		virtual void FireImageQualityChanged(unsigned long nImageQuality);

		virtual bool ReceivedString( Socket *pSocket, string sLine, int nTimeout = - 1 );
		virtual void ReceivedMessage( Socket *pSocket, Message* pMessage ) {} // We don't do messages

		virtual void Run();
		virtual void SaveXML(string sFileName);
		virtual void SaveBits(DesignObj_Orbiter *Data = NULL);
		virtual void SaveDataGrid(DesignObj_DataGrid *pDesignObj_DataGrid);
		virtual void GenerateXMLItems(DesignObj_Orbiter *pObj); //recursive
		virtual string GenerateSoftKeys(DesignObj_Orbiter *pObj);

		virtual void ParseHardKeys();

		inline string GetDevicePngFileName();
		inline string GetDeviceXmlFileName();

		virtual void CMD_Quit(string &sCMD_Result,Message *pMessage);
		virtual void CMD_Terminate_Orbiter(string &sCMD_Result,Message *pMessage);
		virtual void OnReload();
		virtual void CMD_Regen_Screen(string &sCMD_Result,Message *pMessage);

		virtual bool IsRepeatedKeyForScreen(DesignObj_Orbiter* pObj, int iPK_Button);
		virtual void ImageGenerated();
		
		bool PushRefreshEvent(bool bForce, bool bIgnoreMinimumInterval = false);
		void *PushRefreshEventTask(void *p);
	};
}
//-----------------------------------------------------------------------------------------------------
#endif //__Proxy_Orbiter_H__ 
