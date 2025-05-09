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
#include "onvif/soapPullPointSubscriptionBindingProxy.h"

namespace DCE
{
	class OnvifDevice : public CameraDevice
	{
	private:
		DeviceBindingProxy* m_pDeviceProxy;
		PTZBindingProxy* m_pPTZProxy;
		MediaBindingProxy* m_pMediaProxy;
        PullPointSubscriptionBindingProxy* m_pEventsProxy;
        PullPointSubscriptionBindingProxy* m_pPullPointProxy;

        string m_sMediaProfileToken;
        string m_sImageURI;

        string m_sSubscriptionAddress;
        pthread_t m_OnvifThread;
        bool m_bRunning;
        void Start();
        void Stop();

		bool PTZ(float panx,float pany, float zoom);

    public:
        OnvifDevice(Advanced_IP_Camera* pAIPC, DeviceData_Impl* pData);
		virtual ~OnvifDevice();
        void PullPointThread();
        bool Get_Image(int iWidth, int iHeight, char **pData, int *iData_Size, string *sFormat);
        bool MoveLeft(int step);
		bool MoveRight(int step);
        bool MoveUp(int step);
        bool MoveDown(int step);
        bool ZoomIn(int step);
        bool ZoomOut(int step);
        void ReceiveCommandForChild(unsigned long pkDevice, string &sCMD_Result, Message* pMessage) {};
        bool ChangeOutput(OutputDevice* pDevice, bool newState) {}
        string GetStreamURI(string sMediaToken);
    };
}

#endif
