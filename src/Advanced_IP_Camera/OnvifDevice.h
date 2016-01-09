/*
     Copyright (C) 2015 LinuxMCE
     www.linuxmce.org

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef OnvifDevice_h
#define OnvifDevice_h

#include "CameraDevice.h"
#include "onvif/soapDeviceBindingProxy.h"
#include "onvif/soapPTZBindingProxy.h"
#include "onvif/soapMediaBindingProxy.h"

namespace DCE
{
	class OnvifDevice : public CameraDevice
	{
	private:
		DeviceBindingProxy* m_pDeviceProxy;
		PTZBindingProxy* m_pPTZProxy;
		MediaBindingProxy* m_pMediaProxy;

        string m_sMediaProfileToken;
        string m_sImageURI;

		bool PTZ(float panx,float pany, float zoom);

    public:
        OnvifDevice(Advanced_IP_Camera* pAIPC);
		virtual ~OnvifDevice();
        bool Get_Image(int iWidth, int iHeight, char **pData, int *iData_Size, string *sFormat);
        bool MoveLeft(int step);
		bool MoveRight(int step);
        bool MoveUp(int step);
        bool MoveDown(int step);
        bool ZoomIn(int step);
        bool ZoomOut(int step);
    };
}

#endif
