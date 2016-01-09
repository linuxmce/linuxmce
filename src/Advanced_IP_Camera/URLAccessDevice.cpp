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

URLAccessDevice::URLAccessDevice(Advanced_IP_Camera* pAIPC) : CameraDevice(pAIPC)
{

	
}

URLAccessDevice::~URLAccessDevice()
{
}

bool URLAccessDevice::LoadConfig(vector<string> parameters)
{
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
}

bool URLAccessDevice::ChangeOutput(OutputDevice* pDevice, bool newState)
{
	string sUrl = m_pAIPC->GetBaseURL();
	if ( newState )
	{
		sUrl += pDevice->m_sOn;
	} else {
		sUrl += pDevice->m_sOff;
	}
    return m_pAIPC->DoURLAccess(sUrl);
}

bool URLAccessDevice::Get_Image(int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat)
{
    string sUrl = m_pAIPC->GetBaseURL() + m_pAIPC->GetImgPath();
    return m_pAIPC->HttpGet(sUrl, pData, iData_Size);
}

bool URLAccessDevice::MoveLeft(int step)
{
    if (!m_sPanLeftCmd.empty())
    {
        string sUrl = m_pAIPC->GetBaseURL() + m_sControlURL + m_sPanLeftCmd;
        return m_pAIPC->DoURLAccess(sUrl);
    }
    return false;
}

bool URLAccessDevice::MoveRight(int step)
{
    if (!m_sPanRightCmd.empty())
    {
        string sUrl = m_pAIPC->GetBaseURL() + m_sControlURL + m_sPanRightCmd;
        return m_pAIPC->DoURLAccess(sUrl);
    }
    return false;
}

bool URLAccessDevice::MoveUp(int step)
{
    if (!m_sPanUpCmd.empty())
    {
        string sUrl = m_pAIPC->GetBaseURL() + m_sControlURL + m_sPanUpCmd;
        return m_pAIPC->DoURLAccess(sUrl);
    }
    return false;
}

bool URLAccessDevice::MoveDown(int step)
{
    if (!m_sPanDownCmd.empty())
    {
        string sUrl = m_pAIPC->GetBaseURL() + m_sControlURL + m_sPanDownCmd;
        return m_pAIPC->DoURLAccess(sUrl);
    }
    return false;
}

bool URLAccessDevice::ZoomIn(int step)
{
    if (!m_sZoomInCmd.empty())
    {
        string sUrl = m_pAIPC->GetBaseURL() + m_sControlURL + m_sZoomInCmd;
        return m_pAIPC->DoURLAccess(sUrl);
    }
    return false;
}

bool URLAccessDevice::ZoomOut(int step)
{
    if (!m_sZoomOutCmd.empty())
    {
        string sUrl = m_pAIPC->GetBaseURL() + m_sControlURL + m_sZoomOutCmd;
        return m_pAIPC->DoURLAccess(sUrl);
    }
    return false;
}
