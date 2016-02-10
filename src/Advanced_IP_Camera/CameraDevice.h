/*
     Copyright (C) 2015 LinuxMCE
     www.linuxmce.org

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef CameraDevice_h
#define CameraDevice_h

#include "Advanced_IP_Camera.h"
#include "OutputDevice.h"

namespace DCE
{
	class CameraDevice
	{
	protected:
		Advanced_IP_Camera* m_pAIPC;
        DeviceData_Impl* m_pDevice;
        string m_sUser;
		string m_sPasswd;
        string m_sBaseURL;

        map<int, OutputDevice*> m_mapPK_Device_OutputDevice;

	public:
            CameraDevice(Advanced_IP_Camera* pAIPC, DeviceData_Impl* pData);
	        virtual ~CameraDevice();
            virtual bool LoadConfig(vector<string> parameters) { return true; }
            string getUser() { return m_sUser; }
            string getPasswd() { return m_sPasswd; }
            string getBaseURL() { return m_sBaseURL; }
        virtual bool Get_Image(int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat);
		virtual bool MoveLeft(int step) { return false; }
		virtual bool MoveRight(int step) { return false; }
        virtual bool MoveUp(int step) { return false; }
        virtual bool MoveDown(int step) { return false; }
        virtual bool ZoomIn(int step) { return false; }
        virtual bool ZoomOut(int step) { return false; }
        virtual void ReceiveCommandForChild(long pkDevice, string &sCMD_Result, Message* pMessage);
        virtual bool HasChild(long pkDevice);
        virtual bool ChangeOutput(OutputDevice* pDevice, bool newState) {}

    };
}

#endif
