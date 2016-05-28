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
#include "Gen_Devices/AllCommandsRequests.h"

#include <iostream>
using namespace std;
using namespace DCE;

OnvifDevice::OnvifDevice(Advanced_IP_Camera* pAIPC, DeviceData_Impl* pData) : CameraDevice(pAIPC, pData),
    m_pDeviceProxy(NULL), m_pPTZProxy(NULL), m_pMediaProxy(NULL), m_pEventsProxy(NULL), m_pPullPointProxy(NULL)
{
    m_OnvifThread = 0;

    string firstProfile = "";
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
                soap_wsse_add_UsernameTokenDigest(m_pMediaProxy, "Id", m_sUser.c_str(), m_sPasswd.c_str());

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
                        if (i == 0)
                            firstProfile = getprofilesresp.Profiles[i]->token;
                        if (getprofilesresp.Profiles[i]->token == m_sMediaProfileToken)
                            hasMediaProfile = true;
                    }
		} else {
			m_pMediaProxy->soap_stream_fault(std::cerr);
            LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "OnvifDevice(): Error gettting media profiles");
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
                    soap_wsse_add_UsernameTokenDigest(m_pMediaProxy, "Id", m_sUser.c_str(), m_sPasswd.c_str());
#if GSOAP_VERSION >= 20822
                    if (m_pMediaProxy->CreateProfile(&cp, cpr) == SOAP_OK) {
#else
                    if (m_pMediaProxy->CreateProfile(&cp, &cpr) == SOAP_OK) {
#endif
                        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Profile %s created, token = %s", m_sMediaProfileToken.c_str(), cpr.Profile->token.c_str());

                    } else {
                        m_pMediaProxy->soap_stream_fault(std::cerr);
                        LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "OnvifDevice(): Error creating media profile");
                    }

                }
                // Add video source to media profile
                // First, find available sources
                _trt__GetVideoSourceConfigurations gvsc;
                _trt__GetVideoSourceConfigurationsResponse gvscr;
                soap_wsse_add_UsernameTokenDigest(m_pMediaProxy, "Id", m_sUser.c_str(), m_sPasswd.c_str());
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
                        soap_wsse_add_UsernameTokenDigest(m_pMediaProxy, "Id", m_sUser.c_str(), m_sPasswd.c_str());
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
                    LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "OnvifDevice(): Error getting video source configurations");
                }
                if (!m_sMediaProfileToken.empty()) {
                    _trt__GetSnapshotUri gsu;
                    gsu.ProfileToken = m_sMediaProfileToken;
                    _trt__GetSnapshotUriResponse gsur;
                    soap_wsse_add_Security(m_pMediaProxy);
                    soap_wsse_add_UsernameTokenDigest(m_pMediaProxy, NULL, m_sUser.c_str(), m_sPasswd.c_str());
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
                        LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "OnvifDevice(): Error getting Image URI");
                    }
                }
                if (firstProfile != "") {
                    string streamURI = GetStreamURI(firstProfile);
                    CMD_Set_Device_Data_DT cmd(pAIPC->m_dwPK_Device, DEVICETEMPLATE_General_Info_Plugin_CONST,
                                           BL_SameHouse, pData->m_dwPK_Device,
                                           streamURI, DEVICEDATA_Video_settings_CONST);
                    pAIPC->SendCommand(cmd);
                }
            } // if pCaps->Media != null
            if (pCaps->PTZ != NULL)
            {
                LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): * PTZ");
                LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): creating PTZ proxy for: %s", pCaps->PTZ->XAddr.c_str());
                m_pPTZProxy = new PTZBindingProxy(pCaps->PTZ->XAddr.c_str());
                soap_wsse_add_Security(m_pPTZProxy);
                soap_wsse_add_UsernameTokenDigest(m_pPTZProxy, NULL, m_sUser.c_str(), m_sPasswd.c_str());
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
                    LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "OnvifDevice(): Error getting PTZ nodes");
                }

                // Get PTZ configurations
                _tptz__GetConfigurations gptzc;
                _tptz__GetConfigurationsResponse gptzr;
                soap_wsse_add_UsernameTokenDigest(m_pPTZProxy, NULL, m_sUser.c_str(), m_sPasswd.c_str());
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
                    LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "OnvifDevice(): Error getting PTZ configurations");
                }

                if (m_pMediaProxy != NULL) {
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): AddPTZConfiguration %s to media profile %s", sPtzConfigToken.c_str(), m_sMediaProfileToken.c_str());
                    _trt__AddPTZConfiguration apc;
                    apc.ProfileToken = m_sMediaProfileToken;
                    apc.ConfigurationToken = sPtzConfigToken;
                    _trt__AddPTZConfigurationResponse apcr;
                    soap_wsse_add_UsernameTokenDigest(m_pMediaProxy, NULL, m_sUser.c_str(), m_sPasswd.c_str());
#if GSOAP_VERSION >= 20822
                    if (m_pMediaProxy->AddPTZConfiguration(&apc, apcr) == SOAP_OK) {
#else
                    if (m_pMediaProxy->AddPTZConfiguration(&apc, &apcr) == SOAP_OK) {
#endif

                        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Added PTZ config to media profile");
                    } else {
                        m_pMediaProxy->soap_stream_fault(std::cerr);
                        LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "OnvifDevice(): Error adding PTZ config to media profile");
                    }
                }
            }
            if (pCaps->Events != NULL)
            {
                LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): * Events");
                LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): - WSPausableSubscriptionManagerInterfaceSupport: %s", pCaps->Events->WSPausableSubscriptionManagerInterfaceSupport ? "Yes" : "No");
                LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): - WSPullPointSupport: %s", pCaps->Events->WSPullPointSupport ? "Yes" : "No");
                LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): - WSSubscriptionPolicySupport: %s", pCaps->Events->WSSubscriptionPolicySupport ? "Yes" : "No");
                LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): creating Events proxy for: %s", pCaps->Events->XAddr.c_str());
                m_pEventsProxy = new PullPointSubscriptionBindingProxy(pCaps->Events->XAddr.c_str());
                soap_wsse_add_Security(m_pEventsProxy);
                soap_wsse_add_UsernameTokenDigest(m_pEventsProxy, NULL, m_sUser.c_str(), m_sPasswd.c_str());

                _tev__GetEventProperties gep;
                _tev__GetEventPropertiesResponse gepr;
#if GSOAP_VERSION >= 20822
                if (m_pEventsProxy->GetEventProperties(&gep, gepr) == SOAP_OK) {
#else
                if (m_pEventsProxy->GetEventProperties(&gep, &gepr) == SOAP_OK) {
#endif
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Event: TopicNamespaceLocation");
                    for (size_t i = 0; i < gepr.TopicNamespaceLocation.size(); i++) {
                         LoggerWrapper::GetInstance ()->Write (LV_WARNING, " - %s", gepr.TopicNamespaceLocation.at(i).c_str());
                    }
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Event: fixedTopicSet = %s", gepr.wsnt__FixedTopicSet ? "Yes" : "No");
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Event: Topics:");
                    if (gepr.wstop__TopicSet != NULL && gepr.wstop__TopicSet->documentation != NULL && gepr.wstop__TopicSet->documentation->__mixed != NULL) {
                         LoggerWrapper::GetInstance ()->Write (LV_WARNING, " - %s", gepr.wstop__TopicSet->documentation->__mixed);
                    } else {
                         LoggerWrapper::GetInstance ()->Write (LV_WARNING, " - None");
                    }
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Event: MessageContentFilterDialect");
                    for (size_t i = 0; i < gepr.MessageContentFilterDialect.size(); i++) {
                         LoggerWrapper::GetInstance ()->Write (LV_WARNING, " - %s", gepr.MessageContentFilterDialect.at(i).c_str());
                    }
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Event: MessageContentSchemaLocation");
                    for (size_t i = 0; i < gepr.MessageContentSchemaLocation.size(); i++) {
                         LoggerWrapper::GetInstance ()->Write (LV_WARNING, " - %s", gepr.MessageContentSchemaLocation.at(i).c_str());
                    }
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Event: ProducerPropertiesFilterDialect");
                    for (size_t i = 0; i < gepr.ProducerPropertiesFilterDialect.size(); i++) {
                         LoggerWrapper::GetInstance ()->Write (LV_WARNING, " - %s", gepr.ProducerPropertiesFilterDialect.at(i).c_str());
                    }
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Event: wsnt__TopicExpressionDialect");
                    for (size_t i = 0; i < gepr.wsnt__TopicExpressionDialect.size(); i++) {
                         LoggerWrapper::GetInstance ()->Write (LV_WARNING, " - %s", gepr.wsnt__TopicExpressionDialect.at(i).c_str());
                    }

                } else {
                    m_pEventsProxy->soap_stream_fault(std::cerr);
                    LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "OnvifDevice(): Error getting event profiles");
                }
                soap_wsse_add_UsernameTokenDigest(m_pEventsProxy, NULL, m_sUser.c_str(), m_sPasswd.c_str());
                _tev__GetServiceCapabilities gsc;
                _tev__GetServiceCapabilitiesResponse gscr;
#if GSOAP_VERSION >= 20822
                if (m_pEventsProxy->GetServiceCapabilities(&gsc, gscr) == SOAP_OK) {
#else
                if (m_pEventsProxy->GetServiceCapabilities(&gsc, &gscr) == SOAP_OK) {
#endif
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Event: Service capabilities:");
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Event: WSSubscriptionPolicySupport: %s", gscr.Capabilities->WSSubscriptionPolicySupport != NULL && gscr.Capabilities->WSSubscriptionPolicySupport ? "Yes" : "No");
                } else {
                    m_pEventsProxy->soap_stream_fault(std::cerr);
                    LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "OnvifDevice(): Error getting service capabilities");
                }

                LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Event: Creating pull point:");
                soap_wsse_add_UsernameTokenDigest(m_pEventsProxy, NULL, m_sUser.c_str(), m_sPasswd.c_str());
/*
                _wsnt__CreatePullPoint cpps;
                _wsnt__CreatePullPointResponse cppr;
#if GSOAP_VERSION >= 20822
                if (m_pEventsProxy->CreatePullPoint(&cpps, cppr) == SOAP_OK) {
#else
                if (m_pEventsProxy->CreatePullPoint(&cpps, &cppr) == SOAP_OK) {
#endif
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Event: pull point subscription created:");
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Event: address : %s", cppr.PullPoint.Address);
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): size %d", cppr.PullPoint.__size);
//                    for (int i = 0; i < cppr.PullPoint.__size; i++) {
  //                      LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): size %d", cppr.PullPoint.__anyAttribute);
//                    }
                    if (cppr.PullPoint.Metadata != NULL)
                        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): metadata size %d", cppr.PullPoint.Metadata->__size);
                    if (cppr.PullPoint.ReferenceParameters != NULL)
*/
                _tev__CreatePullPointSubscription cpps;
/*                tt__EventFilter filter;
                filter.
/*                wsnt__FilterType filter;
                filter.
                        |__mixed = "tns1:Device//.".c_str();
                cpps.Filter = filter;
  //cpps.SubscriptionPolicy
*/

//                wstop__FullTopicExpression expr = "";
  //              cpps.Filter = expr;
                string termtime = "PT60S";
                cpps.InitialTerminationTime = &termtime;
  //              cpps.Filter = &filter;
//                cpps.SubscriptionPolicy = ;
                _tev__CreatePullPointSubscriptionResponse cppr;
#if GSOAP_VERSION >= 20822
                if (m_pEventsProxy->CreatePullPointSubscription(&cpps, cppr) == SOAP_OK) {
#else
                if (m_pEventsProxy->CreatePullPointSubscription(&cpps, &cppr) == SOAP_OK) {
#endif
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Event: pull point subscription created:");
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): termination time %d", cppr.wsnt__TerminationTime);
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): SubscriptionReference.Address %s", cppr.SubscriptionReference.Address);
                    m_sSubscriptionAddress = string(cppr.SubscriptionReference.Address);

                    if (cppr.SubscriptionReference.__anyAttribute != NULL) {
                        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): __anyAttribute %s",
                                                              cppr.SubscriptionReference.__anyAttribute);
                    }
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): SubscriptionReference.__size %d",
                                                          cppr.SubscriptionReference.__size);
                    for (int i = 0; i < cppr.SubscriptionReference.__size; i++) {
                        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): __any %s",
                                                              cppr.SubscriptionReference.__any[i]);
                    }


                    if (cppr.SubscriptionReference.Metadata != NULL) {
                        if (cppr.SubscriptionReference.Metadata->__anyAttribute != NULL) {
                            LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Metadata->__anyAttribute %s",
                                                                  cppr.SubscriptionReference.ReferenceParameters->__anyAttribute);
                        }
                        for (int i = 0; i < cppr.SubscriptionReference.Metadata->__size; i++) {
                            LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): MetaData__any %s",
                                                                  cppr.SubscriptionReference.Metadata->__any[i]);
                        }
                    }
                    if (cppr.SubscriptionReference.ReferenceParameters != NULL) {
                        if (cppr.SubscriptionReference.ReferenceParameters->__anyAttribute != NULL) {
                            LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): ReferenceParameter->__anyAttribute %s",
                                                                  cppr.SubscriptionReference.ReferenceParameters->__anyAttribute);
                        }
                        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): SubscriptionReference.ReferenceParameters->__size %d",
                                                              cppr.SubscriptionReference.ReferenceParameters->__size);
                        for (int i = 0; i < cppr.SubscriptionReference.ReferenceParameters->__size; i++) {
                            LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): ReferenceParameter %s",
                                                                  cppr.SubscriptionReference.ReferenceParameters->__any[i]);
                        }
                    }

                    // Set up proxy to read from subscription reference
                    m_pPullPointProxy = new PullPointSubscriptionBindingProxy(cppr.SubscriptionReference.Address);
                    soap_wsse_add_Security(m_pPullPointProxy);
                } else {
                    m_pEventsProxy->soap_stream_fault(std::cerr);
                    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Error creating pull point subscription");
                }

                if (m_pPullPointProxy != NULL)
                    Start();

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
    Stop();
    pthread_join(m_OnvifThread, NULL); // wait for it to end
	if (m_pDeviceProxy != NULL)
		delete m_pDeviceProxy;
	if (m_pPTZProxy != NULL)
		delete m_pPTZProxy;
    if (m_pMediaProxy != NULL)
        delete m_pMediaProxy;
    if (m_pEventsProxy != NULL)
        delete m_pEventsProxy;
    if (m_pPullPointProxy != NULL)
        delete m_pPullPointProxy;
}

void *
OnvifThread (void *param)
{
    OnvifDevice* pOnvifDevice = (OnvifDevice*)param;
    pOnvifDevice->PullPointThread();
    pthread_exit (NULL);
}

void OnvifDevice::Start()
{
    m_bRunning = true;
    pthread_create (&m_OnvifThread, NULL, OnvifThread, (void*)this);
}

void OnvifDevice::Stop()
{
    m_bRunning = false;
}

void OnvifDevice::PullPointThread() {
   /* char* str = new char[m_sSubscriptionAddress.length()+1];
    strcpy(str, m_sSubscriptionAddress.c_str());
    while (m_bRunning) {
        LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): PullPointThread: pulling messages:");
        soap_wsse_add_UsernameTokenDigest(m_pPullPointProxy, NULL,  m_pAIPC->DATA_Get_AuthUser().c_str(), m_pAIPC->DATA_Get_AuthPassword().c_str());
        _tev__PullMessages pm;
        pm.Timeout = 60000; // 60 sec
        pm.MessageLimit = 0;
        const SOAP_ENV__Header* header = m_pPullPointProxy->soap_header();
        m_pPullPointProxy->soap_header(header->wsse__Security, header->wsa5__MessageID, header->wsa5__RelatesTo,
                                       header->wsa5__From, header->wsa5__ReplyTo, header->wsa5__FaultTo,
                                       str, header->wsa5__Action, header->chan__ChannelInstance);
        _tev__PullMessagesResponse pmr;
#if GSOAP_VERSION >= 20822
        if (m_pPullPointProxy->PullMessages(&pm, pmr) == SOAP_OK) {
#else
        if (m_pPullPointProxy->PullMessages(&pm, &pmr) == SOAP_OK) {
#endif
            LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): current time : %d", pmr.CurrentTime);
            LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): messages : %d", pmr.wsnt__NotificationMessage.size());
            //                    for (size_t i = 0; i < pmr.wsnt__NotificationMessage.size(); i++) {
            //                      LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): ", pmr.wsnt__NotificationMessage[i].);
            //                }
        } else {
            m_pPullPointProxy->soap_stream_fault(std::cerr);
            LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Error pulling messages");
        }
    }
    delete[] str;*/
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

string OnvifDevice::GetStreamURI(string sMediaToken)
{
    LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): GetStreamURI %s", sMediaToken.c_str());
    _trt__GetStreamUri gsu;
    gsu.ProfileToken = sMediaToken;
    tt__StreamSetup ss;
    ss.Stream = tt__StreamType__RTP_Unicast;
    tt__Transport transport;
    transport.Protocol = tt__TransportProtocol__UDP;
    ss.Transport = &transport;
    gsu.StreamSetup = &ss;
    _trt__GetStreamUriResponse gsur;
    soap_wsse_add_Security(m_pMediaProxy);
    soap_wsse_add_UsernameTokenDigest(m_pMediaProxy, NULL, m_sUser.c_str(), m_sPasswd.c_str());
#if GSOAP_VERSION >= 20822
    if (m_pMediaProxy->GetStreamUri(&gsu, gsur) == SOAP_OK) {
#else
    if (m_pMediaProxy->GetStreamUri(&gsu, &gsur) == SOAP_OK) {
#endif
        if (gsur.MediaUri) {
            LoggerWrapper::GetInstance ()->Write (LV_WARNING, "OnvifDevice(): Stream URI: %s",
                                                  gsur.MediaUri->Uri.c_str());
            return gsur.MediaUri->Uri;
        }
    } else {
        m_pMediaProxy->soap_stream_fault(std::cerr);
        LoggerWrapper::GetInstance ()->Write (LV_CRITICAL, "OnvifDevice(): Error getting Stream URI");
    }
    return "";
}
