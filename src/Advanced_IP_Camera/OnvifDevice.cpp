/*
     Copyright (C) 2015 LinuxMCE
     www.linuxmce.org

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "OnvifDevice.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "openssl/ssl.h"
#include "plugin/wsaapi.h"
#include "plugin/wsseapi.h"

#include <iostream>
using namespace std;
using namespace DCE;

OnvifDevice::OnvifDevice(Advanced_IP_Camera* pAIPC, DeviceData_Impl* pData) : CameraDevice(pAIPC, pData),
    m_pDeviceProxy(NULL), m_pPTZProxy(NULL), m_pMediaProxy(NULL)
{
    string url = m_sBaseURL;
	url += "/onvif/device_service"; // default onvif entry point
	LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): url = %s", url.c_str());
    m_pDeviceProxy = new DeviceBindingProxy(url.c_str());

	_tds__GetCapabilities capabilities;
	_tds__GetCapabilitiesResponse response;
#if GSOAP_VERSION >= 20822
    if (m_pDeviceProxy->GetCapabilities(&capabilities, response) == SOAP_OK)
#else
    if (m_pDeviceProxy->GetCapabilities(&capabilities, &response) == SOAP_OK)
#endif
	{
		LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Got capabilities");
		tt__Capabilities* pCaps = response.Capabilities;
		if (pCaps != NULL) {
            if (pCaps->Media != NULL)
            {
                LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): * Media");
                LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): creating Media proxy for: %s", pCaps->Media->XAddr.c_str());
                m_pMediaProxy = new MediaBindingProxy(pCaps->Media->XAddr.c_str());
                soap_wsse_add_Security(m_pMediaProxy);
                soap_wsse_add_UsernameTokenDigest(m_pMediaProxy, "Id", m_pAIPC->DATA_Get_AuthUser().c_str(), m_pAIPC->DATA_Get_AuthPassword().c_str());

                bool hasMediaProfile = false;
                m_sMediaProfileToken = "LinuxMCE";
                _trt__GetProfiles getprofiles;
                _trt__GetProfilesResponse getprofilesresp;
#if GSOAP_VERSION >= 20822
                if (m_pMediaProxy->GetProfiles(&getprofiles, getprofilesresp) == SOAP_OK) {
#else
                if (m_pMediaProxy->GetProfiles(&getprofiles, &getprofilesresp) == SOAP_OK) {
#endif
                    for (size_t i = 0; i < getprofilesresp.Profiles.size(); i++) {
                        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Media profile: %s", getprofilesresp.Profiles[i]->Name.c_str());
                        if (getprofilesresp.Profiles[i]->token == m_sMediaProfileToken)
                            hasMediaProfile = true;
                    }
                }

                /*_trt__DeleteProfile dp;
                _trt__DeleteProfileResponse dpr;
                dp.ProfileToken = "LinuxMCE2";
                soap_wsse_add_UsernameTokenDigest(m_pMediaProxy, "Id", m_pAIPC->DATA_Get_AuthUser().c_str(), m_pAIPC->DATA_Get_AuthPassword().c_str());
                if (m_pMediaProxy->DeleteProfile(&dp, &dpr) == SOAP_OK) {
                }*/
                if (!hasMediaProfile) {
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Creating LinuxMCE media profile, token: %s", m_sMediaProfileToken.c_str());
                    _trt__CreateProfile cp;
                    cp.Name = m_sMediaProfileToken;
                    cp.Token = &m_sMediaProfileToken;
                    _trt__CreateProfileResponse cpr;
                    soap_wsse_add_UsernameTokenDigest(m_pMediaProxy, "Id", m_pAIPC->DATA_Get_AuthUser().c_str(), m_pAIPC->DATA_Get_AuthPassword().c_str());
#if GSOAP_VERSION >= 20822
                    if (m_pMediaProxy->CreateProfile(&cp, cpr) == SOAP_OK) {
#else
                    if (m_pMediaProxy->CreateProfile(&cp, &cpr) == SOAP_OK) {
#endif
                        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Profile %s created, token = %s", m_sMediaProfileToken.c_str(), cpr.Profile->token.c_str());

                    } else {
                        m_pMediaProxy->soap_stream_fault(std::cerr);
                        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Error creating media profile");
                    }

                }
                // Add video source to media profile
                // First, find available sources
                _trt__GetVideoSourceConfigurations gvsc;
                _trt__GetVideoSourceConfigurationsResponse gvscr;
                soap_wsse_add_UsernameTokenDigest(m_pMediaProxy, "Id", m_pAIPC->DATA_Get_AuthUser().c_str(), m_pAIPC->DATA_Get_AuthPassword().c_str());
#if GSOAP_VERSION >= 20822
                if (m_pMediaProxy->GetVideoSourceConfigurations(&gvsc, gvscr) == SOAP_OK) {
#else
                if (m_pMediaProxy->GetVideoSourceConfigurations(&gvsc, &gvscr) == SOAP_OK) {
#endif
		  if (gvscr.Configurations.size() > 0) {
                        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Video Source %s", gvscr.Configurations[0]->token.c_str());
                        _trt__AddVideoSourceConfiguration avsc;
                        _trt__AddVideoSourceConfigurationResponse avscr;
                        avsc.ProfileToken = m_sMediaProfileToken;
                        avsc.ConfigurationToken = gvscr.Configurations[0]->token;
                        soap_wsse_add_UsernameTokenDigest(m_pMediaProxy, "Id", m_pAIPC->DATA_Get_AuthUser().c_str(), m_pAIPC->DATA_Get_AuthPassword().c_str());
#if GSOAP_VERSION >= 20822
                        if (m_pMediaProxy->AddVideoSourceConfiguration(&avsc, avscr) == SOAP_OK) {
#else
                        if (m_pMediaProxy->AddVideoSourceConfiguration(&avsc, &avscr) == SOAP_OK) {
#endif
                            LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Video Source %s added to media profile", avsc.ConfigurationToken.c_str());
                        } else {
                            m_pMediaProxy->soap_stream_fault(std::cerr);
                            LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Error adding video source configurations to media profile");
                        }
                    }
                } else {
                    m_pMediaProxy->soap_stream_fault(std::cerr);
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Error getting video source configurations");
                }
                if (!m_sMediaProfileToken.empty()) {
                    _trt__GetSnapshotUri gsu;
                    gsu.ProfileToken = m_sMediaProfileToken;
                    _trt__GetSnapshotUriResponse gsur;
                    soap_wsse_add_Security(m_pMediaProxy);
                    soap_wsse_add_UsernameTokenDigest(m_pMediaProxy, NULL, m_pAIPC->DATA_Get_AuthUser().c_str(), m_pAIPC->DATA_Get_AuthPassword().c_str());
#if GSOAP_VERSION >= 20822
                    if (m_pMediaProxy->GetSnapshotUri(&gsu, gsur) == SOAP_OK) {
#else
                    if (m_pMediaProxy->GetSnapshotUri(&gsu, &gsur) == SOAP_OK) {
#endif
                        if (gsur.MediaUri)
                            m_sImageURI = gsur.MediaUri->Uri;
                        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Image URI: %s", m_sImageURI.c_str());
                    } else {
                        m_pMediaProxy->soap_stream_fault(std::cerr);
                        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Error getting Image URI");
                    }
                }
            }
            if (pCaps->PTZ != NULL)
            {
                LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): * PTZ");
                LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): creating PTZ proxy for: %s", pCaps->PTZ->XAddr.c_str());
                m_pPTZProxy = new PTZBindingProxy(pCaps->PTZ->XAddr.c_str());
                soap_wsse_add_Security(m_pPTZProxy);
                soap_wsse_add_UsernameTokenDigest(m_pPTZProxy, NULL,  m_pAIPC->DATA_Get_AuthUser().c_str(), m_pAIPC->DATA_Get_AuthPassword().c_str());
                _tptz__GetNodes getNodes;
                _tptz__GetNodesResponse pGnresp;
#if GSOAP_VERSION >= 20822
                if (m_pPTZProxy->GetNodes(&getNodes, pGnresp) == SOAP_OK) {
#else
                if (m_pPTZProxy->GetNodes(&getNodes, &pGnresp) == SOAP_OK) {
#endif
                    if (pGnresp.PTZNode.size() > 0) {
                        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): PTZNode: %s",
                                                              pGnresp.PTZNode[0]->Name != NULL ? pGnresp.PTZNode[0]->Name->c_str() : "NULL");
                        _tptz__GetNode getNode;
                        getNode.NodeToken = "pGnresp.PTZNode[0]->Name->c_str()";
                        _tptz__GetNodeResponse pGnoderesp;
#if GSOAP_VERSION >= 20822
                        if (m_pPTZProxy->GetNode(&getNode, pGnoderesp) == SOAP_OK) {
#else
                        if (m_pPTZProxy->GetNode(&getNode, &pGnoderesp) == SOAP_OK) {
#endif
                            //pGnoderesp.PTZNode->
                        }
                    }
                } else {
                    m_pPTZProxy->soap_stream_fault(std::cerr);
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Error getting PTZ nodes");
                }

                // Get PTZ configurations
                _tptz__GetConfigurations gptzc;
                _tptz__GetConfigurationsResponse gptzr;
                soap_wsse_add_UsernameTokenDigest(m_pPTZProxy, NULL, m_pAIPC->DATA_Get_AuthUser().c_str(), m_pAIPC->DATA_Get_AuthPassword().c_str());
                string sPtzConfigToken;
#if GSOAP_VERSION >= 20822
                if (m_pPTZProxy->GetConfigurations(&gptzc, gptzr) == SOAP_OK) {
#else
                if (m_pPTZProxy->GetConfigurations(&gptzc, &gptzr) == SOAP_OK) {
#endif
                    for (size_t i = 0; i < gptzr.PTZConfiguration.size(); i++) {
                        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): PTZConfiguration: %s", gptzr.PTZConfiguration[i]->token.c_str());
                    }
                    if (gptzr.PTZConfiguration.size() > 0) {
                        tt__PTZConfiguration *ptzConfig = gptzr.PTZConfiguration[0];
                        sPtzConfigToken = ptzConfig->token;
                        if (ptzConfig->PanTiltLimits != NULL && ptzConfig->PanTiltLimits->Range != NULL) {
                            tt__Space2DDescription *range = ptzConfig->PanTiltLimits->Range;
                            LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): : X range : %f - %f", range->XRange->Min, range->XRange->Max);
                            LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): : Y range : %f - %f", range->YRange->Min, range->YRange->Max);
                        }
                        if (ptzConfig->ZoomLimits != NULL && ptzConfig->ZoomLimits->Range != NULL) {
                            LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): : zoom range : %f - %f", ptzConfig->ZoomLimits->Range->XRange->Min, ptzConfig->ZoomLimits->Range->XRange->Max);
                        }
                        if (ptzConfig->DefaultPTZSpeed != NULL && ptzConfig->DefaultPTZSpeed->PanTilt != NULL) {
                            LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): : speed: X: %f, Y: %f", ptzConfig->DefaultPTZSpeed->PanTilt->x, ptzConfig->DefaultPTZSpeed->PanTilt->y);
                        }
                    }
                } else {
                    m_pPTZProxy->soap_stream_fault(std::cerr);
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Error getting PTZ configurations");
                }

                if (m_pMediaProxy != NULL) {
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): AddPTZConfiguration %s to media profile %s", sPtzConfigToken.c_str(), m_sMediaProfileToken.c_str());
                    _trt__AddPTZConfiguration apc;
                    apc.ProfileToken = m_sMediaProfileToken;
                    apc.ConfigurationToken = sPtzConfigToken;
                    _trt__AddPTZConfigurationResponse apcr;
                    soap_wsse_add_UsernameTokenDigest(m_pMediaProxy, NULL,  m_pAIPC->DATA_Get_AuthUser().c_str(), m_pAIPC->DATA_Get_AuthPassword().c_str());
#if GSOAP_VERSION >= 20822
                    if (m_pMediaProxy->AddPTZConfiguration(&apc, apcr) == SOAP_OK) {
#else
                    if (m_pMediaProxy->AddPTZConfiguration(&apc, &apcr) == SOAP_OK) {
#endif

                        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Added PTZ config to media profile");
                    } else {
                        m_pMediaProxy->soap_stream_fault(std::cerr);
                        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Error adding PTZ config to media profile");
                    }
                }
            }

		}
	}
	else
	{
		m_pDeviceProxy->soap_stream_fault(std::cerr);
		LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Error getting device capabililties");

    }

/*    _tds__GetServices gs;
    _tds__GetServicesResponse gsr;
    if (m_pDeviceProxy->GetServices(&gs, &gsr) == SOAP_OK)  {
        for (size_t i = 0; i < gsr.Service.size(); i++) {
            LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): service addr : %s", gsr.Service.at(i)->XAddr.c_str());
        }
    } else 	{
        m_pDeviceProxy->soap_stream_fault(std::cerr);
        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Error getting device services");
    }
*/
}

OnvifDevice::~OnvifDevice()
{
	if (m_pDeviceProxy != NULL)
		delete m_pDeviceProxy;
	if (m_pPTZProxy != NULL)
		delete m_pPTZProxy;
    if (m_pMediaProxy != NULL)
        delete m_pMediaProxy;
}

bool OnvifDevice::PTZ(float panx,float pany, float zoomx)
{
	if (m_pPTZProxy != NULL)
	{
        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "PTZ() start, X: %f Y: %f, Z: %f", panx, pany, zoomx);
 		_tptz__RelativeMove relativeMove;
        relativeMove.ProfileToken = m_sMediaProfileToken;
		tt__PTZVector vector;
		tt__Vector2D pantilt;
		tt__Vector1D zoom;
		pantilt.x = panx;
		pantilt.y = pany;
		vector.PanTilt = &pantilt;
		zoom.x = zoomx;
		vector.Zoom = &zoom;
		relativeMove.Translation = &vector;
/*		tt__PTZSpeed speed;
		tt__Vector2D ptSpeed;
		ptSpeed.x = 1;
		speed.PanTilt = &ptSpeed;
        relativeMove.Speed = &speed; // optional*/
		_tptz__RelativeMoveResponse response;
		soap_wsse_add_Security(m_pPTZProxy);
        soap_wsse_add_UsernameTokenDigest(m_pPTZProxy, "Id", m_pAIPC->DATA_Get_AuthUser().c_str(), m_pAIPC->DATA_Get_AuthPassword().c_str());
#if GSOAP_VERSION >= 20822
		if (m_pPTZProxy->RelativeMove(&relativeMove, response) != SOAP_OK)
#else
		if (m_pPTZProxy->RelativeMove(&relativeMove, &response) != SOAP_OK)
#endif
		{
			LoggerWrapper::GetInstance ()->Write (LV_WARNING, "PTZ() Error");
			m_pPTZProxy->soap_stream_fault(std::cerr);
            return false;
		} else {
            LoggerWrapper::GetInstance ()->Write (LV_WARNING, "PTZ() success");
			return true;
		}
	} else {
		LoggerWrapper::GetInstance ()->Write (LV_WARNING, "PTZ() PTZ not supported");
	}
    return false;
}

bool OnvifDevice::Get_Image(int iWidth, int iHeight, char **pData, int *iData_Size, string *sFormat)
{
    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "Get_Image() start, m_sImageURI = %s", m_sImageURI.c_str());
    if (!m_sImageURI.empty())
        return m_pAIPC->HttpGet(m_sImageURI, m_sUser, m_sPasswd, pData, iData_Size);
    return false;
}

bool OnvifDevice::MoveLeft(int step)
{
  return PTZ((float)-step/50, 0, 0);
}

bool OnvifDevice::MoveRight(int step)
{
  return PTZ((float)step/50, 0, 0);
}

bool OnvifDevice::MoveUp(int step)
{
  return PTZ(0, (float)step/50, 0);
}
bool OnvifDevice::MoveDown(int step)
{
  return PTZ(0, (float)-step/50, 0);
}
bool OnvifDevice::ZoomIn(int step)
{
  return PTZ(0, 0, (float)step/10);
}
bool OnvifDevice::ZoomOut(int step)
{
  return PTZ(0, 0, (float)-step/10);
}
