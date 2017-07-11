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
#include "../pluto_main/Define_DeviceData.h"
#include "../pluto_main/Define_CommandParameter.h"

#include <iostream>
using namespace std;
using namespace DCE;

CameraDevice::CameraDevice(Advanced_IP_Camera* pAIPC, DeviceData_Impl* pData) : m_pAIPC(pAIPC), m_pDevice(pData)
{
    m_sUser = pData->m_mapParameters_Find(DEVICEDATA_AuthUser_CONST);
    m_sPasswd = pData->m_mapParameters_Find(DEVICEDATA_AuthPassword_CONST);
    m_sBaseURL = "http://"+pData->m_sIPAddress;
    int port = atoi(pData->m_mapParameters[DEVICEDATA_TCP_Port_CONST].c_str());
    if (port == 0) {
        port = 80;
    } else {
        if (port != 80)
        {
              m_sBaseURL += ":" + StringUtils::itos(port);
        }
    }
}

CameraDevice::~CameraDevice()
{
}

bool CameraDevice::Get_Image(int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat)
{
    return false;
}

bool CameraDevice::HasChild(long PK_Device)
{
    if (m_pDevice->m_dwPK_Device == PK_Device) {
        return true;
    } else {
        map<int, OutputDevice*>::iterator it = m_mapPK_Device_OutputDevice.find(PK_Device);
        if ( it != m_mapPK_Device_OutputDevice.end() )
            return true;
    }
    return false;
}

void CameraDevice::ReceiveCommandForChild(long pkDevice, string &sCMD_Result, Message* pMessage) {
    LoggerWrapper::GetInstance ()->Write (LV_STATUS, "CameraDevice.ReceiveCommandForChild() device = %d", pkDevice);
    if (pkDevice == m_pDevice->m_dwPK_Device) {
        switch (pMessage->m_dwID) {
        case COMMAND_Get_Video_Frame_CONST: {
            int iWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
            int iHeight = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Height_CONST].c_str());
            string sFormat = pMessage->m_mapParameters[COMMANDPARAMETER_Format_CONST];
            char *pData=pMessage->m_mapData_Parameters[COMMANDPARAMETER_Data_CONST];
            int iData_Size=pMessage->m_mapData_Lengths[COMMANDPARAMETER_Data_CONST];
            if (Get_Image(iWidth, iHeight, &pData, &iData_Size, &sFormat)) {
                LoggerWrapper::GetInstance ()->Write (LV_STATUS, "CameraDevice.ReceiveCommandForChild() sending response message");
                sCMD_Result = "OK";
                pMessage->m_bRespondedToMessage=true;
                Message *pMessageOut=new Message(pkDevice,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
                pMessageOut->m_mapData_Parameters[COMMANDPARAMETER_Data_CONST]=pData;
                pMessageOut->m_mapData_Lengths[COMMANDPARAMETER_Data_CONST]=iData_Size;
                pMessageOut->m_mapParameters[COMMANDPARAMETER_Format_CONST]=sFormat;
                pMessageOut->m_mapParameters[0]=sCMD_Result;
                m_pAIPC->SendMessage(pMessageOut);
            }
            break;
        }
        case COMMAND_Generic_On_CONST:
        case COMMAND_Generic_Off_CONST:
            // TODO: motion on/off ?
            break;
        case COMMAND_Move_Up_CONST:
            if (MoveUp(1))
                sCMD_Result = "OK";
            break;
        case COMMAND_Move_Left_CONST:
            if (MoveLeft(1))
                sCMD_Result = "OK";
            break;
        case COMMAND_Move_Right_CONST:
            if (MoveRight(1))
                sCMD_Result = "OK";
            break;
        case COMMAND_Move_Down_CONST:
            if (MoveDown(1))
                sCMD_Result = "OK";
            break;
        default:
            sCMD_Result = "UNKNOWN COMMAND";
            break;
        }
    } else {
        map<int, OutputDevice*>::iterator it = m_mapPK_Device_OutputDevice.find(pkDevice);
        if ( it != m_mapPK_Device_OutputDevice.end() )
        {
            OutputDevice* pDevice = (*it).second;

            switch (pMessage->m_dwID) {
            case COMMAND_Generic_On_CONST:
            case COMMAND_Generic_Off_CONST:
                if (ChangeOutput(pDevice, pMessage->m_dwID == COMMAND_Generic_On_CONST))
                {
                    sCMD_Result = "OK";
                }
                break;
            default:
                sCMD_Result = "UNKNOWN COMMAND";
                break;
            }
        }
    }
}

