/*
     Copyright (C) 2012 LinuxMCE
     www.linuxmce.org

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
	: Advanced_IP_Camera_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter),
	  m_CurlMutex("curl_mutex")
//<-dceag-const-e->
{
  m_CurlMutex.Init (NULL);

}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
Advanced_IP_Camera::Advanced_IP_Camera(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: Advanced_IP_Camera_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter),
	  m_CurlMutex("curl_mutex")
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
Advanced_IP_Camera::~Advanced_IP_Camera()
//<-dceag-dest-e->
{

	curl_easy_cleanup(m_pCurl);
	curl_global_cleanup();

        for( vector<EventMethod*>::const_iterator it = m_vectEventMethod.begin();
                        it != m_vectEventMethod.end(); ++it )
	{
		if ( *it != NULL )
		{
			(*it)->Stop();
			delete (*it);
		}
	}
        for( map<int, OutputDevice*>::const_iterator it = m_mapPK_Device_OutputDevice.begin();
                        it != m_mapPK_Device_OutputDevice.end(); ++it )
	{
		delete (*it).second;
	}
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
	m_sImgPath = DATA_Get_Path();

	m_sUser = DATA_Get_AuthUser();
	m_sPasswd = DATA_Get_AuthPassword();

	m_vectEventMethod.resize(5);
        for( int i = 0; i < m_vectEventMethod.size(); i++ )
	{
		m_vectEventMethod[i] = NULL;
	}
	string config = DATA_Get_Configuration();
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "getConfig() Configuration = %s", config.c_str());
	vector<string> parameters;
	SplitConfig(config, parameters);
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "getConfig() parameters.size = %d", parameters.size());
	for (int i = 0; i < parameters.size(); i++)
	{
		size_t pos = parameters[i].find_first_of("=");
		if (pos != string::npos)
		{
			string key = parameters[i].substr(0, pos);
			key = StringUtils::TrimSpaces(key);
			string value = parameters[i].substr(pos+1);
			value = StringUtils::TrimSpaces(value);
			LoggerWrapper::GetInstance ()->Write (LV_STATUS, "getConfig() key = %s, value = %s", key.c_str(), value.c_str());
			if (StringUtils::StartsWith(key, "eventMethod")) {
				int num = atoi(key.substr(11).c_str());
				if (num > 0)
				{
					EventMethod* pEventMethod = GetEventMethod(num);
					if (pEventMethod != NULL)
						pEventMethod->m_sMethod = value;
				}
			} else if (StringUtils::StartsWith(key, "eventURL")) {
				int num = atoi(key.substr(8).c_str());
				if (num > 0)
				{
					EventMethod* pEventMethod = GetEventMethod(num);
					if (pEventMethod != NULL)
						pEventMethod->m_sURL = value;
				}
			} else if (StringUtils::StartsWith(key, "eventInterval")) {
				int num = atoi(key.substr(8).c_str());
				if (num > 0)
				{
					EventMethod* pEventMethod = GetEventMethod(num);
					if (pEventMethod != NULL && atoi(value.c_str()) > 0)
						pEventMethod->m_iInterval = atoi(value.c_str());
				}
			} else if (StringUtils::StartsWith(key, "controlURL")) {
				m_sControlURL = value;
			} else if (StringUtils::StartsWith(key, "panUpCmd")) {
				m_sPanUpCmd = value;
			} else if (StringUtils::StartsWith(key, "panDownCmd")) {
				m_sPanDownCmd = value;
			} else if (StringUtils::StartsWith(key, "panLeftCmd")) {
				m_sPanLeftCmd = value;
			} else if (StringUtils::StartsWith(key, "panRightCmd")) {
				m_sPanRightCmd = value;
			} else if (StringUtils::StartsWith(key, "zoomInCmd")) {
				m_sZoomInCmd = value;
			} else if (StringUtils::StartsWith(key, "zoomOutCmd")) {
				m_sZoomOutCmd = value;
			}
		}
	}

	curl_global_init (CURL_GLOBAL_ALL);
	m_pCurl = curl_easy_init ();
	if (!m_pCurl)
		return false;

	// See what child devices we have, decode their configuration and assign them to an Event Method
	DeviceData_Impl* pChildDevice;
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "getConfig() Children:");
        for( VectDeviceData_Impl::const_iterator it = m_pData->m_vectDeviceData_Impl_Children.begin();
                        it != m_pData->m_vectDeviceData_Impl_Children.end(); ++it )
	{
                pChildDevice = (*it);
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "getConfig() PK_Device= %d", pChildDevice->m_dwPK_Device);
		if (pChildDevice->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Motion_Detector_CONST ||
			pChildDevice->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Generic_Sensor_CONST)
		{
			InputDevice* pInputDevice = new InputDevice(pChildDevice->m_dwPK_Device, pChildDevice->m_dwPK_DeviceTemplate);
			pInputDevice->status = 0;
			string value;
			GetChildDeviceData(pChildDevice->m_dwPK_Device, DEVICEDATA_Capabilities_CONST, value);
			LoggerWrapper::GetInstance ()->Write (LV_STATUS, "getConfig() Motion detector, capabilities = %s", value.c_str());
			SplitConfig(value, parameters);
			EventMethod* pEventMethod = NULL;
			for (int i = 0; i < parameters.size(); i++)
			{
				size_t pos = parameters[i].find_first_of("=");
				if (pos != string::npos)
				{
					string key = parameters[i].substr(0, pos);
					key = StringUtils::TrimSpaces(key);
					string value = parameters[i].substr(pos+1);
					value = StringUtils::TrimSpaces(value);
					LoggerWrapper::GetInstance ()->Write (LV_STATUS, "getConfig() key = %s, value = %s", key.c_str(), value.c_str());
					if (StringUtils::StartsWith(key, "method")) {
						int num = atoi(value.c_str());
						pEventMethod = GetEventMethod(num);
						if (pEventMethod != NULL) 
							pEventMethod->Add(pInputDevice);
					} else if (StringUtils::StartsWith(key, "triggerMethod")) {
						pInputDevice->m_sTriggerMethod = value;
					} else if (StringUtils::StartsWith(key, "patternOn")) {
						pInputDevice->m_sPatternOn = value;
					} else if (StringUtils::StartsWith(key, "patternOff")) {
						pInputDevice->m_sPatternOff = value;
					}
				}
			}
		} else if (pChildDevice->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Generic_Input_Ouput_CONST)
		{
			OutputDevice* pOutputDevice = new OutputDevice(pChildDevice->m_dwPK_Device, pChildDevice->m_dwPK_DeviceTemplate);
			pOutputDevice->status = 0;
			string value;
			GetChildDeviceData(pChildDevice->m_dwPK_Device, DEVICEDATA_Capabilities_CONST, value);
			LoggerWrapper::GetInstance ()->Write (LV_STATUS, "getConfig() Generic Input Output, capabilities = %s", value.c_str());
			SplitConfig(value, parameters);
			for (int i = 0; i < parameters.size(); i++)
			{
				size_t pos = parameters[i].find_first_of("=");
				if (pos != string::npos)
				{
					string key = parameters[i].substr(0, pos);
					key = StringUtils::TrimSpaces(key);
					string value = parameters[i].substr(pos+1);
					value = StringUtils::TrimSpaces(value);
					LoggerWrapper::GetInstance ()->Write (LV_STATUS, "getConfig() key = %s, value = %s", key.c_str(), value.c_str());
					if (StringUtils::StartsWith(key, "controlMethod")) {
						pOutputDevice->m_sControlMethod = value;
					} else if (StringUtils::StartsWith(key, "on")) {
						pOutputDevice->m_sOn = value;
					} else if (StringUtils::StartsWith(key, "off")) {
						pOutputDevice->m_sOff = value;
					}
				}
			}
			m_mapPK_Device_OutputDevice[pChildDevice->m_dwPK_Device] = pOutputDevice;
		}
	}
	
	LoggerWrapper::GetInstance ()->Write (LV_WARNING, "m_vectEventMethod.size() = %d. Configured EventMethods:", m_vectEventMethod.size());
	for (int i = 0; i < m_vectEventMethod.size(); i++)
	{
		EventMethod* pEventMethod = m_vectEventMethod[i];
		if (pEventMethod != NULL)
		{
			LoggerWrapper::GetInstance ()->Write (LV_WARNING, "GetConfig(): EventMethod %d, %s", i+1, pEventMethod->ToString().c_str());
			pEventMethod->Start();
		}

	}
	LoggerWrapper::GetInstance ()->Write (LV_WARNING, "m_mapPK_Device_OutputDevice.size() = %d. Configured OutputDevices:", m_mapPK_Device_OutputDevice.size());
        for( map<int, OutputDevice*>::const_iterator it = m_mapPK_Device_OutputDevice.begin();
                        it != m_mapPK_Device_OutputDevice.end(); ++it )
	{
		OutputDevice* pOutputDevice = (*it).second;
		if (pOutputDevice != NULL)
		{
			LoggerWrapper::GetInstance ()->Write (LV_WARNING, "GetConfig(): OutputDevice %s", pOutputDevice->ToString().c_str());
		}

	}
	return true;
}

void Advanced_IP_Camera::SplitConfig(string config, vector<string> &parameters)
{
	parameters.clear();
	if ( config.find(":") != string::npos )
	{
		parameters = StringUtils::Split(config, ":");
	} else {
		parameters = StringUtils::Split(config, "\n");
	}
}

EventMethod* Advanced_IP_Camera::GetEventMethod(int i)
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "GetEventMethod() requested EventMethod num = %d", i);
	EventMethod* pEventMethod = NULL;
	if (i > 0 && i <= 5)
	{
		pEventMethod = m_vectEventMethod[i-1];
		if (pEventMethod == NULL)
		{
			LoggerWrapper::GetInstance ()->Write (LV_STATUS, "GetEventMethod() new EventMethod num = %d", i);
			pEventMethod = new EventMethod(this);
			m_vectEventMethod[i-1] = pEventMethod;
		}
	}
	return pEventMethod;
}

string Advanced_IP_Camera::GetBaseURL()
{
	return m_sBaseURL;
}

string Advanced_IP_Camera::GetPassword()
{
	return m_sPasswd;
}

string Advanced_IP_Camera::GetUser()
{
	return m_sUser;
}

void Advanced_IP_Camera::InputStatusChanged(InputDevice* pInputDevice, int newStatus)
{
	// TODO: we might need to use a device setting to tell what event to send.
	// An input might be connected to a sensor too, so that is should use sensor_tripped and not state_changes
	if (pInputDevice->m_dwFK_DeviceTemplate == DEVICETEMPLATE_Motion_Detector_CONST)
	{
		// Send sensor tripped event
		Message *pMessage = new Message(pInputDevice->m_dwPK_Device, 0, PRIORITY_NORMAL, MESSAGETYPE_EVENT, EVENT_Sensor_Tripped_CONST,
					1, EVENTPARAMETER_Tripped_CONST, StringUtils::itos(newStatus).c_str());
		QueueMessageToRouter(pMessage);
	} else {
                // Send State_Changed event
		Message *pMessage = new Message(pInputDevice->m_dwPK_Device, 0, PRIORITY_NORMAL, MESSAGETYPE_EVENT, EVENT_State_Changed_CONST,
					1, EVENTPARAMETER_State_CONST, StringUtils::itos(newStatus).c_str());
		QueueMessageToRouter(pMessage);
	}

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
	
	map<int, OutputDevice*>::iterator it = m_mapPK_Device_OutputDevice.find(pDeviceData_Impl->m_dwPK_Device);
	if ( it != m_mapPK_Device_OutputDevice.end() ) 
	{
		OutputDevice* pDevice = (*it).second;

		if (pMessage->m_dwID == COMMAND_Generic_On_CONST ||
		    pMessage->m_dwID == COMMAND_Generic_Off_CONST)
		{
			if (ChangeOutput(pDevice, pMessage->m_dwID == COMMAND_Generic_On_CONST)) 
			{
				sCMD_Result = "OK";
			}
		}
	}
}

bool Advanced_IP_Camera::ChangeOutput(OutputDevice* pDevice, bool newState)
{
	string sUrl = GetBaseURL();
	if ( newState )
	{
		sUrl += pDevice->m_sOn;
	} else {
		sUrl += pDevice->m_sOff;
	}
	return DoURLAccess(sUrl);
}

void Advanced_IP_Camera::SetupCurl(string sUrl)
{
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "SetupCurl() start: sUrl: %s", sUrl.c_str ());
	curl_easy_setopt(m_pCurl, CURLOPT_URL, sUrl.c_str());
		
	curl_easy_setopt(m_pCurl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	if (!m_sUser.empty())
	{
		curl_easy_setopt(m_pCurl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
		curl_easy_setopt(m_pCurl, CURLOPT_USERNAME, m_sUser.c_str());
		if (!m_sPasswd.empty()) {
			curl_easy_setopt(m_pCurl, CURLOPT_PASSWORD, m_sPasswd.c_str());
		}
	}
}

bool Advanced_IP_Camera::DoURLAccess(string sUrl)
{
	PLUTO_SAFETY_LOCK (cm, m_CurlMutex);
	SetupCurl(sUrl);
	CallbackData data;
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, WriteCallback);
 	curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, (void *)&data);
	CURLcode res = curl_easy_perform(m_pCurl);
	
	bool statusOK = false;
	if (res != 0)
	{
		LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DoURLAccess(): failed to get data: curl error: %s", curl_easy_strerror(res));
	} else {
		long code;
		curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &code);
		if (code == 200)
		{
			// http OK
			statusOK = true;
		} else {
			LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DoURLAccess(): http code: %d, response:",  code);
		}
	}
	data.Clear();
	curl_easy_reset(m_pCurl);
	LoggerWrapper::GetInstance ()->Write (LV_STATUS, "DoURLAccess(): end.");
	return statusOK;
}

size_t Advanced_IP_Camera::WriteCallback(void *ptr, size_t size, size_t nmemb, void *ourpointer) 
{
	CallbackData* data = (CallbackData*)ourpointer;
	size_t realsize = size * nmemb;
	char* temp = (char*)realloc(data->buffer, data->size + realsize + 1);
	if (temp == NULL)
	{
		LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "WriteCallback(): Unable to allocate memory for receive buffer!");
		return 0;
	}
	data->buffer = temp;
	memcpy(&(data->buffer[data->size]), ptr, realsize);
	data->size += realsize;
	data->buffer[data->size] = 0;
	return realsize;
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
	string sUrl = m_sBaseURL + m_sImgPath;

	PLUTO_SAFETY_LOCK (gm, m_CurlMutex);
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

//<-dceag-c200-b->

	/** @brief COMMAND: #200 - Move Up */
	/** Move camera up */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Advanced_IP_Camera::CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c200-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
	if (!m_sPanUpCmd.empty())
	{
		string sUrl = GetBaseURL() + m_sControlURL + m_sPanUpCmd;
		if (DoURLAccess(sUrl))
			sCMD_Result = "OK";
	}
}
//<-dceag-c201-b->

	/** @brief COMMAND: #201 - Move Down */
	/** Move camera down */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Advanced_IP_Camera::CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c201-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
	if (!m_sPanDownCmd.empty())
	{
		string sUrl = GetBaseURL() + m_sControlURL + m_sPanDownCmd;
		if (DoURLAccess(sUrl))
			sCMD_Result = "OK";
	}
}
//<-dceag-c202-b->

	/** @brief COMMAND: #202 - Move Left */
	/** Move camera to the left */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Advanced_IP_Camera::CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c202-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
	if (!m_sPanLeftCmd.empty())
	{
		string sUrl = GetBaseURL() + m_sControlURL + m_sPanLeftCmd;
		if (DoURLAccess(sUrl))
			sCMD_Result = "OK";
	}
}
//<-dceag-c203-b->

	/** @brief COMMAND: #203 - Move Right */
	/** Move camera to the right */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Advanced_IP_Camera::CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c203-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
	if (!m_sPanRightCmd.empty())
	{
		string sUrl = GetBaseURL() + m_sControlURL + m_sPanRightCmd;
		if (DoURLAccess(sUrl))
			sCMD_Result = "OK";
	}
}
//<-dceag-c684-b->

	/** @brief COMMAND: #684 - Zoom In */
	/** Zoom in */

void Advanced_IP_Camera::CMD_Zoom_In(string &sCMD_Result,Message *pMessage)
//<-dceag-c684-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
	if (!m_sZoomInCmd.empty())
	{
		string sUrl = GetBaseURL() + m_sControlURL + m_sZoomInCmd;
		if (DoURLAccess(sUrl))
			sCMD_Result = "OK";
	}
}
//<-dceag-c685-b->

	/** @brief COMMAND: #685 - Zoom Out */
	/** Zoom out */

void Advanced_IP_Camera::CMD_Zoom_Out(string &sCMD_Result,Message *pMessage)
//<-dceag-c685-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
	if (!m_sZoomOutCmd.empty())
	{
		string sUrl = GetBaseURL() + m_sControlURL + m_sZoomOutCmd;
		if (DoURLAccess(sUrl))
			sCMD_Result = "OK";
	}
}
