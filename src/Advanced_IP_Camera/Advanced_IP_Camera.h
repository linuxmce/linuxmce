/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef Advanced_IP_Camera_h
#define Advanced_IP_Camera_h

//	DCE Implemenation for #2208 Advanced IP Camera

#include "Gen_Devices/Advanced_IP_CameraBase.h"
//<-dceag-d-e->

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include <pthread.h>

//<-dceag-decl-b->
namespace DCE
{
	class MotionDetector {
	public:
		int PK_Device;
		int status;
		string triggerOn;
		string triggerOff;
		
		bool Matches(string s) {
			return s.compare(triggerOn) == 0 || s.compare(triggerOff) == 0;
		}
		
		int GetNewStatus(string s) {
			if (s.compare(triggerOn) == 0) {
				return 1;
			} else if (s.compare(triggerOff) == 0) {
				return 0;
			}
			return 0;
		}
	};
  
	class Advanced_IP_Camera : public Advanced_IP_Camera_Command
	{
//<-dceag-decl-e->
		// Private member variables
	  string m_sBaseURL;
	  string m_sImgPath;
	  string m_sUser;
	  string m_sPasswd;
	  string m_sEventPath;
	  
	  vector<MotionDetector*> m_vectMotionDetector;

	  CURLM* m_pCurl;
	  
	  pthread_t m_eventThread;
	  string m_sEventBuffer;
	        // Private methods
public:
		// Public member variables
	  bool m_bRunning;

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Advanced_IP_Camera(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Advanced_IP_Camera();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		static size_t WriteCallback(void *ptr, size_t size, size_t nmemb, void *ourpointer);
		static size_t StaticEventWriteCallback(void *ptr, size_t size, size_t nmemb, void *ourpointer);
		size_t EventWriteCallback(void *ptr, size_t size, size_t nmemb);
		void EventThread();
		void MotionStatusChanged(MotionDetector* motionDetector, string trigger);
		
//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Advanced_IP_Camera(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Path();
	int DATA_Get_PK_FloorplanObjectType();
	string DATA_Get_Alert();
	string DATA_Get_File_Name_and_Path();
	int DATA_Get_TCP_Port();
	string DATA_Get_Audio_settings();
	string DATA_Get_Video_settings();
	string DATA_Get_AuthUser();
	string DATA_Get_AuthPassword();
	string DATA_Get_Sound_Card();
	bool DATA_Get_Video_Support();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #84 - Get Video Frame */
	/** Get's a picture from a specified surveilance camera */
		/** @param #19 Data */
			/** The video frame */
		/** @param #20 Format */
			/** Format of the frame */
		/** @param #23 Disable Aspect Lock */
			/** Disable Aspect Ratio */
		/** @param #41 StreamID */
			/** The ID of the stream */
		/** @param #60 Width */
			/** Frame width */
		/** @param #61 Height */
			/** Frame height */

	virtual void CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat) { string sCMD_Result; CMD_Get_Video_Frame(sDisable_Aspect_Lock.c_str(),iStreamID,iWidth,iHeight,pData,iData_Size,sFormat,sCMD_Result,NULL);};
	virtual void CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
