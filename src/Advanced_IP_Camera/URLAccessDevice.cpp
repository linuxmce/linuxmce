/*
     Copyright (C) 2015 LinuxMCE
     www.linuxmce.org

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "URLAccessDevice.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

URLAccessDevice::URLAccessDevice(Advanced_IP_Camera* pAIPC, DeviceData_Impl* pData) : CameraDevice(pAIPC, pData)
{
    m_sImgPath = pData->m_mapParameters[DEVICEDATA_Path_CONST];

}

URLAccessDevice::~URLAccessDevice()
{
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

bool URLAccessDevice::LoadConfig(vector<string> parameters)
{
    m_vectEventMethod.resize(5);
    for( size_t i = 0; i < m_vectEventMethod.size(); i++ )
    {
        m_vectEventMethod[i] = NULL;
    }

    for (size_t i = 0; i < parameters.size(); i++)
    {
        size_t pos = parameters[i].find_first_of("=");
        if (pos != string::npos)
        {
            string key = parameters[i].substr(0, pos);
            key = StringUtils::TrimSpaces(key);
            string value = parameters[i].substr(pos+1);
            value = StringUtils::TrimSpaces(value);
            LoggerWrapper::GetInstance ()->Write (LV_STATUS, "getConfig() key = %s, value = %s", key.c_str(), value.c_str());
            if (StringUtils::StartsWith(key, "controlURL")) {
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
    for (size_t i = 0; i < parameters.size(); i++)
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
            }
        }
    }
    // See what child devices we have, decode their configuration and assign them to an Event Method
    DeviceData_Impl* pChildDevice;
    LoggerWrapper::GetInstance ()->Write (LV_STATUS, "getConfig() Children:");
        for( VectDeviceData_Impl::const_iterator it = m_pDevice->m_vectDeviceData_Impl_Children.begin();
                        it != m_pDevice->m_vectDeviceData_Impl_Children.end(); ++it )
    {
                pChildDevice = (*it);
        LoggerWrapper::GetInstance ()->Write (LV_STATUS, "getConfig() PK_Device= %d", pChildDevice->m_dwPK_Device);
        if (pChildDevice->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Motion_Detector_CONST ||
            pChildDevice->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Generic_Sensor_CONST)
        {
            InputDevice* pInputDevice = new InputDevice(pChildDevice->m_dwPK_Device, pChildDevice->m_dwPK_DeviceTemplate);
            pInputDevice->status = 0;
            string value;
            m_pAIPC->GetChildDeviceData(pChildDevice->m_dwPK_Device, DEVICEDATA_Capabilities_CONST, value);
            LoggerWrapper::GetInstance ()->Write (LV_STATUS, "getConfig() Motion detector, capabilities = %s", value.c_str());
            Advanced_IP_Camera::SplitConfig(value, parameters);
            EventMethod* pEventMethod = NULL;
            for (size_t i = 0; i < parameters.size(); i++)
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
            m_pAIPC->GetChildDeviceData(pChildDevice->m_dwPK_Device, DEVICEDATA_Capabilities_CONST, value);
            LoggerWrapper::GetInstance ()->Write (LV_STATUS, "getConfig() Generic Input Output, capabilities = %s", value.c_str());
            Advanced_IP_Camera::SplitConfig(value, parameters);
            for (size_t i = 0; i < parameters.size(); i++)
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
    for (size_t i = 0; i < m_vectEventMethod.size(); i++)
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

EventMethod* URLAccessDevice::GetEventMethod(int i)
{
    LoggerWrapper::GetInstance ()->Write (LV_STATUS, "GetEventMethod() requested EventMethod num = %d", i);
    EventMethod* pEventMethod = NULL;
    if (i > 0 && i <= 5)
    {
        pEventMethod = m_vectEventMethod[i-1];
        if (pEventMethod == NULL)
        {
            LoggerWrapper::GetInstance ()->Write (LV_STATUS, "GetEventMethod() new EventMethod num = %d", i);
            pEventMethod = new EventMethod(m_pAIPC, this);
            m_vectEventMethod[i-1] = pEventMethod;
        }
    }
    return pEventMethod;
}

bool URLAccessDevice::ChangeOutput(OutputDevice* pDevice, bool newState)
{
    string sUrl = m_sBaseURL;
	if ( newState )
	{
		sUrl += pDevice->m_sOn;
	} else {
		sUrl += pDevice->m_sOff;
	}
    return m_pAIPC->DoURLAccess(sUrl, m_sUser, m_sPasswd);
}

bool URLAccessDevice::Get_Image(int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat)
{
    string sUrl = m_sBaseURL + m_sImgPath;
    return m_pAIPC->HttpGet(sUrl, m_sUser, m_sPasswd, pData, iData_Size);
}

bool URLAccessDevice::MoveLeft(int step)
{
    if (!m_sPanLeftCmd.empty())
    {
        string sUrl = m_sBaseURL + m_sControlURL + m_sPanLeftCmd;
        return m_pAIPC->DoURLAccess(sUrl, m_sUser, m_sPasswd);
    }
    return false;
}

bool URLAccessDevice::MoveRight(int step)
{
    if (!m_sPanRightCmd.empty())
    {
        string sUrl = m_sBaseURL + m_sControlURL + m_sPanRightCmd;
        return m_pAIPC->DoURLAccess(sUrl, m_sUser, m_sPasswd);
    }
    return false;
}

bool URLAccessDevice::MoveUp(int step)
{
    if (!m_sPanUpCmd.empty())
    {
        string sUrl = m_sBaseURL + m_sControlURL + m_sPanUpCmd;
        return m_pAIPC->DoURLAccess(sUrl, m_sUser, m_sPasswd);
    }
    return false;
}

bool URLAccessDevice::MoveDown(int step)
{
    if (!m_sPanDownCmd.empty())
    {
        string sUrl = m_sBaseURL + m_sControlURL + m_sPanDownCmd;
        return m_pAIPC->DoURLAccess(sUrl, m_sUser, m_sPasswd);
    }
    return false;
}

bool URLAccessDevice::ZoomIn(int step)
{
    if (!m_sZoomInCmd.empty())
    {
        string sUrl = m_sBaseURL + m_sControlURL + m_sZoomInCmd;
        return m_pAIPC->DoURLAccess(sUrl, m_sUser, m_sPasswd);
    }
    return false;
}

bool URLAccessDevice::ZoomOut(int step)
{
    if (!m_sZoomOutCmd.empty())
    {
        string sUrl = m_sBaseURL + m_sControlURL + m_sZoomOutCmd;
        return m_pAIPC->DoURLAccess(sUrl, m_sUser, m_sPasswd);
    }
    return false;
}
