/*
     Copyright (C) 2015 LinuxMCE
     www.linuxmce.org

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "CameraDevice.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

CameraDevice::CameraDevice(Advanced_IP_Camera* pAIPC) : m_pAIPC(pAIPC)
{
  m_sUser = pAIPC->GetUser();
  m_sPasswd = pAIPC->GetPassword();
}

CameraDevice::~CameraDevice()
{
}

bool CameraDevice::Get_Image(int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat)
{
    return false;
}
