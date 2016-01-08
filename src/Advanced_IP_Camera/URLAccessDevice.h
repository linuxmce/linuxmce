/*
     Copyright (C) 2015 LinuxMCE
     www.linuxmce.org

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef URLAccessDevice_h
#define URLAccessDevice_h

#include "CameraDevice.h"

namespace DCE
{
	class URLAccessDevice : public CameraDevice
	{
	private:
        string m_sEventURL;
        string m_sControlURL;
        string m_sPanUpCmd, m_sPanDownCmd, m_sPanLeftCmd, m_sPanRightCmd, m_sZoomInCmd, m_sZoomOutCmd;

		bool ChangeOutput(OutputDevice* pDevice, bool newState);

    public:
	        URLAccessDevice(Advanced_IP_Camera* pAIPC);
		virtual ~URLAccessDevice();
        bool LoadConfig(vector<string> parameters);
        bool Get_Image(int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat);
		bool MoveLeft(int step);
		bool MoveRight(int step);
        bool MoveUp(int step);
        bool MoveDown(int step);
        bool ZoomIn(int step);
        bool ZoomOut(int step);
    };
}

#endif
