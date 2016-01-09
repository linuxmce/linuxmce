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

namespace DCE
{
	class CameraDevice
	{
	protected:
		Advanced_IP_Camera* m_pAIPC;
		string m_sUser;
		string m_sPasswd;

	public:
	        CameraDevice(Advanced_IP_Camera* pAIPC);
	        virtual ~CameraDevice();
            virtual bool LoadConfig(vector<string> parameters) { return true; }
        virtual bool Get_Image(int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat);
		virtual bool MoveLeft(int step) { return false; }
		virtual bool MoveRight(int step) { return false; }
        virtual bool MoveUp(int step) { return false; }
        virtual bool MoveDown(int step) { return false; }
        virtual bool ZoomIn(int step) { return false; }
        virtual bool ZoomOut(int step) { return false; }
    };
}

#endif
