/*
     Copyright (C) 2012 LinuxMCE
     www.linuxmce.org

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
#include <curl/easy.h>

#include <pthread.h>
#include <deque>

#include "EventMethod.h"

//<-dceag-decl-b->
namespace DCE
{
	class CameraDevice;

	class Advanced_IP_Camera : public Advanced_IP_Camera_Command
	{
//<-dceag-decl-e->
		// Private member variables
        pluto_pthread_mutex_t m_CurlMutex;
        CURLM* m_pCurl;
        void SetupCurl(string sUrl, string sUser, string sPasswd);
        static size_t WriteCallback(void *ptr, size_t size, size_t nmemb, void *ourpointer);
        deque<CameraDevice*> m_dequeCameraDevice;
	  
	        // Private methods
public:
		// Public member variables

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

		void InputStatusChanged(InputDevice* inputDevice, string trigger);
		void InputStatusChanged(InputDevice* pInputDevice, int newStatus);

        bool DoURLAccess(string sUrl, string sUser, string sPass);
        bool HttpGet(string sURL, string sUser, string sPass, char **pData,int *iData_Size);

        static void SplitConfig(string value, vector<string> &parameters);
        bool CreateCameraChild(DeviceData_Impl* pData);
        CameraDevice* FindCameraDevice(unsigned long PK_Device);
        void CreateChildren();
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
	string DATA_Get_Configuration();
	int DATA_Get_TCP_Port();
	string DATA_Get_Audio_settings();
	string DATA_Get_Video_settings();
	string DATA_Get_AuthUser();
	string DATA_Get_AuthPassword();
	bool DATA_Get_Video_Support();

			*****EVENT***** accessors inherited from base class
	void EVENT_Sensor_Tripped(bool bTripped);

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


	/** @brief COMMAND: #200 - Move Up */
	/** Move camera up */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Up(int iStreamID) { string sCMD_Result; CMD_Move_Up(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #201 - Move Down */
	/** Move camera down */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Down(int iStreamID) { string sCMD_Result; CMD_Move_Down(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #202 - Move Left */
	/** Move camera to the left */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Left(int iStreamID) { string sCMD_Result; CMD_Move_Left(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #203 - Move Right */
	/** Move camera to the right */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Right(int iStreamID) { string sCMD_Result; CMD_Move_Right(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #684 - Zoom In */
	/** Zoom in */

	virtual void CMD_Zoom_In() { string sCMD_Result; CMD_Zoom_In(sCMD_Result,NULL);};
	virtual void CMD_Zoom_In(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #685 - Zoom Out */
	/** Zoom out */

	virtual void CMD_Zoom_Out() { string sCMD_Result; CMD_Zoom_Out(sCMD_Result,NULL);};
	virtual void CMD_Zoom_Out(string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
