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
#include "../pluto_main/Define_DeviceData.h"
#include "../pluto_main/Define_DeviceTemplate.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
#include "OnvifDevice.h"
#include "URLAccessDevice.h"
#include "CameraDevice.h"
#include "onvif/DeviceBinding.nsmap"

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
    : Advanced_IP_Camera_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter), m_CurlMutex("curl_mutex")
//<-dceag-const2-e->
{
    m_CurlMutex.Init (NULL);

}

//<-dceag-dest-b->
Advanced_IP_Camera::~Advanced_IP_Camera()
//<-dceag-dest-e->
{
    for (size_t i = 0; i < m_dequeCameraDevice.size();i++) {
        if (m_dequeCameraDevice[i] != NULL)
            delete m_dequeCameraDevice[i];
    }

    curl_easy_cleanup(m_pCurl);
    curl_global_cleanup();
}

//<-dceag-getconfig-b->
bool Advanced_IP_Camera::GetConfig()
{
	if( !Advanced_IP_Camera_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

    curl_global_init (CURL_GLOBAL_ALL);
    m_pCurl = curl_easy_init ();
    if (!m_pCurl)
        return false;

    // Check for old style config
    string config = DATA_Get_Configuration();
    if (StringUtils::StartsWith(config, "multi")) {
        // New config
        // See what child devices we have, decode their configuration
        DeviceData_Impl* pChildDevice;
        LoggerWrapper::GetInstance ()->Write (LV_STATUS, "getConfig() Children:");
            for( VectDeviceData_Impl::const_iterator it = m_pData->m_vectDeviceData_Impl_Children.begin();
                            it != m_pData->m_vectDeviceData_Impl_Children.end(); ++it )
        {
            pChildDevice = (*it);
            LoggerWrapper::GetInstance ()->Write (LV_STATUS, "getConfig() PK_Device= %d", pChildDevice->m_dwPK_Device);
            CreateCameraChild(pChildDevice);
        }
    } else {
        // assume old config
        CreateCameraChild(m_pData);
    }

    return true;
}


bool Advanced_IP_Camera::CreateCameraChild(DeviceData_Impl* pData)
{
    string config = pData->m_mapParameters[DEVICEDATA_Configuration_CONST];
    LoggerWrapper::GetInstance ()->Write (LV_STATUS, "CreateCameraChild() Configuration = %s", config.c_str());
    vector<string> parameters;
    SplitConfig(config, parameters);
    LoggerWrapper::GetInstance ()->Write (LV_STATUS, "CreateCameraChild() parameters.size = %d", parameters.size());

    // First use DT to detect type
    CameraDevice* pCameraDevice = NULL;
    if (pData->m_dwPK_DeviceTemplate == DEVICETEMPLATE_ONVIF_camera_CONST) {
        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "CreateCameraChild() creating onvif device");
        pCameraDevice = new OnvifDevice(this, pData);
    } else {
        // Then fall back to configuration data
        if (parameters.size() > 0)
        {
            string val = parameters[0];
            if (StringUtils::StartsWith(val, "type="))
            {
                LoggerWrapper::GetInstance ()->Write (LV_STATUS, "CreateCameraChild() type = %s", val.substr(5).c_str());
                if (StringUtils::EndsWith(val, "onvif"))
                {
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "CreateCameraChild() creating onvif device");
                    pCameraDevice = new OnvifDevice(this, pData);
                }
            }
        }
    }
    // Last, lets assume it is a URL access device
    if (!pCameraDevice) {
        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "CreateCameraChild() Assuming URL Access Device - which may be wrong, you should update your config.");
        pCameraDevice = new URLAccessDevice(this, pData);
    }
    if  (!pCameraDevice->LoadConfig(parameters))
        return false;

    m_dequeCameraDevice.push_back(pCameraDevice);

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

void Advanced_IP_Camera::CreateChildren()
{

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

    unsigned long PK_Device = pDeviceData_Impl->m_dwPK_Device;
    CameraDevice* pDevice = FindCameraDevice(PK_Device);
    if (pDevice != NULL) {
        pDevice->ReceiveCommandForChild(PK_Device,sCMD_Result,pMessage);
    }
}

CameraDevice* Advanced_IP_Camera::FindCameraDevice(unsigned long PK_Device)
{
    LoggerWrapper::GetInstance ()->Write (LV_STATUS, "FindCameraDevice() looking for device = %d", PK_Device);
    for (size_t i = 0; i < m_dequeCameraDevice.size();i++) {
        if (m_dequeCameraDevice[i]->HasChild(PK_Device))
            return m_dequeCameraDevice[i];
    }
    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "FindCameraDevice() device = %d not found", PK_Device);
    return NULL;
}

void Advanced_IP_Camera::SetupCurl(string sUrl, string sUser, string sPasswd)
{
    LoggerWrapper::GetInstance ()->Write (LV_STATUS, "SetupCurl() start: sUrl: %s", sUrl.c_str ());
    curl_easy_setopt(m_pCurl, CURLOPT_URL, sUrl.c_str());

    curl_easy_setopt(m_pCurl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    if (!sUser.empty())
    {
        curl_easy_setopt(m_pCurl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(m_pCurl, CURLOPT_USERNAME, sUser.c_str());
        if (!sPasswd.empty()) {
            curl_easy_setopt(m_pCurl, CURLOPT_PASSWORD, sPasswd.c_str());
        }
    }
}

bool Advanced_IP_Camera::DoURLAccess(string sUrl, string sUser, string sPasswd)
{
    PLUTO_SAFETY_LOCK (cm, m_CurlMutex);
    SetupCurl(sUrl,sUser, sPasswd);
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

bool Advanced_IP_Camera::HttpGet(string sUrl, string sUser, string sPasswd, char **pData,int *iData_Size)
{
    CallbackData data;

    PLUTO_SAFETY_LOCK (gm, m_CurlMutex);
    LoggerWrapper::GetInstance ()->Write (LV_STATUS, "HttpGet: sUrl: %s", sUrl.c_str ());
    curl_easy_setopt(m_pCurl, CURLOPT_URL, sUrl.c_str());

        /* send all data to this function  */
    curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, (void *)&data);
    curl_easy_setopt(m_pCurl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    if (!sUser.empty())
    {
        curl_easy_setopt(m_pCurl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(m_pCurl, CURLOPT_USERNAME, sUser.c_str());
	curl_easy_setopt(m_pCurl, CURLOPT_PASSWORD, sPasswd.c_str());
    }

    CURLcode res = curl_easy_perform(m_pCurl);

    if (res != 0)
    {
        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "HttpGet: failed to get data: curl error: %s", curl_easy_strerror(res));
        return false;
    } else {
        long code;
        curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &code);
        if (code == 200)
        {
            // http OK
            LoggerWrapper::GetInstance ()->Write (LV_STATUS, "HttpGet: data size: %d", data.size);
            *pData = data.buffer;
            *iData_Size = data.size;
            return true;
        } else {
            LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "HttpGet: http code: %d, response:",  code, data.buffer);
        }
    }
    return false;
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
    CameraDevice* pDevice = FindCameraDevice(m_dwPK_Device);
    if (pDevice != NULL)
        pDevice->Get_Image(iWidth, iHeight, pData, iData_Size, sFormat);
    sCMD_Result = "OK";
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
    CameraDevice* pDevice = FindCameraDevice(m_dwPK_Device);
    if (pDevice != NULL) {
        if (pDevice->MoveUp(1))
        {
            sCMD_Result = "OK";
        }
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
    CameraDevice* pDevice = FindCameraDevice(m_dwPK_Device);
    if (pDevice != NULL) {
        if (pDevice->MoveDown(1))
        {
            sCMD_Result = "OK";
        }
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
    CameraDevice* pDevice = FindCameraDevice(m_dwPK_Device);
    if (pDevice != NULL) {
        if (pDevice->MoveLeft(1))
        {
            sCMD_Result = "OK";
        }
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
    CameraDevice* pDevice = FindCameraDevice(m_dwPK_Device);
    if (pDevice != NULL) {
        if (pDevice->MoveDown(1))
        {
            sCMD_Result = "OK";
        }
    }
}
//<-dceag-c684-b->

	/** @brief COMMAND: #684 - Zoom In */
	/** Zoom in */

void Advanced_IP_Camera::CMD_Zoom_In(string &sCMD_Result,Message *pMessage)
//<-dceag-c684-e->
{
    sCMD_Result = "UNKNOWN COMMAND";
    CameraDevice* pDevice = FindCameraDevice(m_dwPK_Device);
    if (pDevice != NULL) {
        if (pDevice->ZoomIn(1))
        {
            sCMD_Result = "OK";
        }
    }
}
//<-dceag-c685-b->

	/** @brief COMMAND: #685 - Zoom Out */
	/** Zoom out */

void Advanced_IP_Camera::CMD_Zoom_Out(string &sCMD_Result,Message *pMessage)
//<-dceag-c685-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
    CameraDevice* pDevice = FindCameraDevice(m_dwPK_Device);
    if (pDevice != NULL) {
        if (pDevice->ZoomOut(1))
        {
            sCMD_Result = "OK";
        }
    }
}
