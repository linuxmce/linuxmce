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
#include "Advanced_IP_Camera.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Advanced_IP_Camera::Advanced_IP_Camera(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Advanced_IP_Camera_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_eventThread = 0;
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
Advanced_IP_Camera::Advanced_IP_Camera(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: Advanced_IP_Camera_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
Advanced_IP_Camera::~Advanced_IP_Camera()
//<-dceag-dest-e->
{

	// wait for other threads
	m_bRunning = false;
	if (m_eventThread != 0)
		pthread_join (m_eventThread, NULL);

	curl_easy_cleanup(m_pCurl);
	curl_global_cleanup();

        for( vector<MotionDetector*>::const_iterator it = m_vectMotionDetector.begin();
                        it != m_vectMotionDetector.end(); ++it )
	{
		delete (*it);
	}
}

void *
eventThread (void *param)
{
	Advanced_IP_Camera* pCameraDevice = (Advanced_IP_Camera*)param;
	pCameraDevice->EventThread();
	pthread_exit (NULL);
}


//<-dceag-getconfig-b->
bool Advanced_IP_Camera::GetConfig()
{
	if( !Advanced_IP_Camera_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	m_sBaseURL = "http://"+GetIpAddress();
	if (DATA_Get_TCP_Port())
	{
		m_sBaseURL += ":" + StringUtils::itos(DATA_Get_TCP_Port());
	}
	m_sBaseURL += "/";
	m_sImgPath = DATA_Get_Path();

	m_sUser = DATA_Get_AuthUser();
	m_sPasswd = DATA_Get_AuthPassword();

	m_sEventPath = DATA_Get_Configuration();

	curl_global_init (CURL_GLOBAL_ALL);
	m_pCurl = curl_easy_init ();
	if (!m_pCurl)
		return false;

	// See what child devices we have - it will determine if we have for example motion detection
	DeviceData_Impl* pChildDevice;
	LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "getConfig() Children:");
        for( VectDeviceData_Impl::const_iterator it = m_pData->m_vectDeviceData_Impl_Children.begin();
                        it != m_pData->m_vectDeviceData_Impl_Children.end(); ++it )
	{
                pChildDevice = (*it);
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "getConfig() PK_Device= %d", pChildDevice->m_dwPK_Device);
		if (pChildDevice->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Motion_Detector_CONST)
		{
			MotionDetector* pMotionDetector = new MotionDetector();
			pMotionDetector->PK_Device = pChildDevice->m_dwPK_Device;
			pMotionDetector->status = 0;
			string value;
			GetChildDeviceData(pChildDevice->m_dwPK_Device, DEVICEDATA_Capabilities_CONST, value);
			LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "getConfig() Motion detector, capabilities = %s", value.c_str());
			size_t pos = value.find("\n");
			if (pos != string::npos)
			{
				pMotionDetector->triggerOn = value.substr(0, pos-1);
				pMotionDetector->triggerOff = value.substr(pos+1);
			}
			LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "getConfig() Motion detector, triggerOn = %s, triggerOff = %s", pMotionDetector->triggerOn.c_str(), pMotionDetector->triggerOff.c_str());
			m_vectMotionDetector.push_back(pMotionDetector);
		}
	}

	m_bRunning = true;
	pthread_create (&m_eventThread, NULL, eventThread, (void*)this);

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Advanced_IP_Camera::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Advanced_IP_Camera_Command *Create_Advanced_IP_Camera(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new Advanced_IP_Camera(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Advanced_IP_Camera::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Advanced_IP_Camera::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/

struct CallbackData {
	char* buffer;
	size_t size;
};

size_t Advanced_IP_Camera::WriteCallback(void *ptr, size_t size, size_t nmemb, void *ourpointer) 
{
	struct CallbackData* data = (CallbackData*)ourpointer;
	size_t realsize = size * nmemb;
	data->buffer = (char*)realloc(data->buffer, data->size + realsize + 1);
	if (data->buffer == NULL)
	{
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "CMD_Get_Video_Frame: Unable to allocate memory for receive buffer!");
		return 0;
	}
	memcpy(&(data->buffer[data->size]), ptr, realsize);
	data->size += realsize;
	data->buffer[data->size] = 0;
	return realsize;
}

//<-dceag-c84-b->

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

void Advanced_IP_Camera::CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage)
//<-dceag-c84-e->
{

	CallbackData data;
	data.buffer = (char*)malloc(1);
	data.size = 0;
	string sUrl = m_sBaseURL + "/" + m_sImgPath;


	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "CMD_Get_Video_Frame: sUrl: %s", sUrl.c_str ());
	curl_easy_setopt(m_pCurl, CURLOPT_URL, sUrl.c_str());

        /* send all data to this function  */ 
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, WriteCallback);
 	curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, (void *)&data);
	curl_easy_setopt(m_pCurl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	if (!m_sUser.empty())
	{
		curl_easy_setopt(m_pCurl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
		curl_easy_setopt(m_pCurl, CURLOPT_USERNAME, m_sUser.c_str());
		if (!m_sPasswd.empty()) {
			curl_easy_setopt(m_pCurl, CURLOPT_PASSWORD, m_sPasswd.c_str());
		}
	}

	CURLcode res = curl_easy_perform(m_pCurl);

	if (res != 0)
	{
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "CMD_Get_Video_Frame: failed to get data: curl error: %s", curl_easy_strerror(res));
		
	} else {
		long code;
		curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &code);
		if (code == 200)
		{
			// http OK
			LoggerWrapper::GetInstance ()->Write (LV_STATUS, "CMD_Get_Video_Frame: data size: %d", data.size);
			*pData = data.buffer;
			*iData_Size = data.size;
		} else {
			LoggerWrapper::GetInstance ()->Write (LV_STATUS, "CMD_Get_Video_Frame: http code: %d, response:",  code, data.buffer);

		}
	}

}

size_t Advanced_IP_Camera::StaticEventWriteCallback(void *ptr, size_t size, size_t nmemb, void *ourpointer) 
{
	return ((Advanced_IP_Camera*)ourpointer)->EventWriteCallback(ptr, size, nmemb);
}

size_t Advanced_IP_Camera::EventWriteCallback(void *ptr, size_t size, size_t nmemb)  {
	string separatorToken = "\n";

	if (m_bRunning) {
		m_sEventBuffer.append((char*)ptr, size*nmemb);
//		LoggerWrapper::GetInstance ()->Write (LV_WARNING, "EventWriteCallback: size*nmemb = %d", size*nmemb);

		// Check what we have got, are there any recognized events
		// split string on line endings
		size_t lastPos = 0;
		string::size_type loc = m_sEventBuffer.find( separatorToken, lastPos );
		while (loc < m_sEventBuffer.length()) {
			if( loc != string::npos ) {
				string s = m_sEventBuffer.substr(lastPos, loc);
				LoggerWrapper::GetInstance ()->Write (LV_WARNING, "EventWriteCallback: s = %s", s.c_str());
				for( vector<MotionDetector*>::const_iterator it = m_vectMotionDetector.begin();
				     it != m_vectMotionDetector.end(); ++it )
				{
					if ((*it)->Matches(s))
						MotionStatusChanged(*it, s);
				}
				lastPos = loc+1;
			}
			loc = m_sEventBuffer.find( separatorToken, lastPos );
		}
		// discard checked data
		if (lastPos != string::npos)
		{
			m_sEventBuffer = m_sEventBuffer.substr(lastPos);
			LoggerWrapper::GetInstance ()->Write (LV_WARNING, "EventWriteCallback: compacting m_sEventBuffer, new length = %d", m_sEventBuffer.length());
		}
		
		return size*nmemb;
	} else {
		return -1;
	}
}

void Advanced_IP_Camera::MotionStatusChanged(MotionDetector* motionDetector, string trigger)
{
	LoggerWrapper::GetInstance ()->Write (LV_WARNING, "MotionStatusChanged: trigger = %s", trigger.c_str());
	int newStatus = motionDetector->GetNewStatus(trigger);

	Message *pMessage = new Message(motionDetector->PK_Device, 0, PRIORITY_NORMAL, MESSAGETYPE_EVENT, EVENT_Sensor_Tripped_CONST,
					1, EVENTPARAMETER_Tripped_CONST, StringUtils::itos(newStatus).c_str());
	QueueMessageToRouter(pMessage);
	motionDetector->status = newStatus;

}
void Advanced_IP_Camera::EventThread() {

	// Set up connection
	string data;
	string sUrl = m_sBaseURL + "/" + m_sEventPath;

	CURLM* eventCurl = curl_easy_init();

	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventThread(): sUrl: %s", sUrl.c_str ());
	curl_easy_setopt(eventCurl, CURLOPT_URL, sUrl.c_str());

	/* send all data to this function  */ 
	curl_easy_setopt(eventCurl, CURLOPT_WRITEFUNCTION, StaticEventWriteCallback);
	curl_easy_setopt(eventCurl, CURLOPT_WRITEDATA, (void *)this);
	curl_easy_setopt(eventCurl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	if (!m_sUser.empty())
	{
		curl_easy_setopt(eventCurl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
		curl_easy_setopt(eventCurl, CURLOPT_USERNAME, m_sUser.c_str());
		if (!m_sPasswd.empty()) {
			curl_easy_setopt(eventCurl, CURLOPT_PASSWORD, m_sPasswd.c_str());
		}
	}
	CURLcode res = curl_easy_perform(eventCurl);
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventThread(): curl_Easy_perform has returned");
	if (res != 0)
	{
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventThread(): failed to get connection: curl error: %s", curl_easy_strerror(res));
		
	} else {
		long code;
		curl_easy_getinfo(eventCurl, CURLINFO_RESPONSE_CODE, &code);
		if (code == 200)
		{
			// http OK
//			LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventThread(): data size: %d", data.);
//			*pData = data.buffer;
//			*iData_Size = data.size;
		} else {
//			LoggerWrapper::GetInstance ()->Write (LV_STATUS, "EventThread(): http code: %d, response:",  code, data.buffer);

		}
	}

	while (m_bRunning)
	{

	}



}
//<-dceag-c200-b->

	/** @brief COMMAND: #200 - Move Up */
	/** Move camera up */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Advanced_IP_Camera::CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c200-e->
{
}
//<-dceag-c201-b->

	/** @brief COMMAND: #201 - Move Down */
	/** Move camera down */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Advanced_IP_Camera::CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c201-e->
{
}
//<-dceag-c202-b->

	/** @brief COMMAND: #202 - Move Left */
	/** Move camera to the left */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Advanced_IP_Camera::CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c202-e->
{
}
//<-dceag-c203-b->

	/** @brief COMMAND: #203 - Move Right */
	/** Move camera to the right */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Advanced_IP_Camera::CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c203-e->
{
}
//<-dceag-c684-b->

	/** @brief COMMAND: #684 - Zoom In */
	/** Zoom in */

void Advanced_IP_Camera::CMD_Zoom_In(string &sCMD_Result,Message *pMessage)
//<-dceag-c684-e->
{
}
//<-dceag-c685-b->

	/** @brief COMMAND: #685 - Zoom Out */
	/** Zoom out */

void Advanced_IP_Camera::CMD_Zoom_Out(string &sCMD_Result,Message *pMessage)
//<-dceag-c685-e->
{
}
