/*
 Main
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 www.plutohome.com
 
 Phone: +1 (877) 758-8648
 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 */
#include "SIP_Thread.h"
#include <lpconfig.h>
#include <linphonecore.h>
#include "DCE/Logger.h"
#include "DCE/DCEConfig.h"
using namespace DCE;

SimplePhoneConf SimplePhoneConf::m_Instance;

static inline void func_enter(const char * func)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "%s - called", func);
}
static inline void func_exit(const char * func)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "%s - exited", func);
}
/* Callbacks for linphone core */
static void stub() {}
#ifdef LINPHONE_3_6
static void call_state_changed(LinphoneCore *linphoneCore, LinphoneCall *call, LinphoneCallState cstate, const char *msg);
#else
static void call_received(LinphoneCore *linphoneCore, const char *from);
static void bye_received(LinphoneCore *linphoneCore, const char *from);
#endif
static void auth_requested(LinphoneCore *linphoneCore, const char *realm, const char *username);
static void display_something(LinphoneCore * linphoneCore, LINPHONE_CONST char *something);
static void display_url(LinphoneCore * linphoneCore, LINPHONE_CONST char *something, LINPHONE_CONST char *url);
static void display_warning (LinphoneCore * linphoneCore, LINPHONE_CONST char *something);
	
/* C Object, private members */
static LinphoneCore *LS_LinphoneCore;
static LinphoneCoreVTable LS_LinphoneCoreVTable;
static LinphoneProxyConfig * LS_pLinphoneProxyConfig = NULL;
static LinphoneAuthInfo * LS_pLinphoneAuthInfo = NULL;
static SimplePhone * LS_pSimplePhone;
pluto_pthread_mutex_t LS_linphone_mutex("linphone_simplephone"); // this mutex protects the access to the linphone library
static string LS_Auth_Username;
static string LS_Auth_Realm;
static int LS_Auth_Received = 0;
/* C Object, public members */
bool LS_bQuit = false;
/* C Object, private methods - declaration */
static void LS_RegisterWithAsterisk();
static void LS_Main_Loop();
static void LS_UnregisterWithAsterisk();
static void LS_InitVTable();
static void LS_InitProxy();
static void LS_SetupAuth();
static void LS_SignalHandler(int ExitStatus);
static bool LS_AcceptCall_nolock();
static bool LS_DropCall_nolock();
static void LS_ProcessEvents();
/* C Object, private methods - implementation */
/** Register with Asterisk */
static void LS_RegisterWithAsterisk()
{
	PLUTO_SAFETY_LOCK(sl, LS_linphone_mutex);
	func_enter("LS_RegisterWithAsterisk");
	
	linphone_core_enable_logs(stdout);
	//linphone_core_disable_logs();
#ifdef BUILD_FOR_1004
	LS_LinphoneCore = linphone_core_new(&LS_LinphoneCoreVTable, "/etc/pluto/simplephone.conf", NULL);
#else
	LS_LinphoneCore = linphone_core_new(&LS_LinphoneCoreVTable, "/etc/pluto/simplephone.conf", NULL, NULL);
#endif
	const char** soundcards=linphone_core_get_sound_devices(LS_LinphoneCore);
	if (!LS_pSimplePhone->m_sSoundCardNumber.empty())
	  {
	    int iSoundCardNum=atoi(LS_pSimplePhone->m_sSoundCardNumber.c_str());
	    linphone_core_set_capture_device(LS_LinphoneCore,soundcards[iSoundCardNum+1]);
	    linphone_core_set_playback_device(LS_LinphoneCore,soundcards[iSoundCardNum+1]);
	    linphone_core_set_ringer_device(LS_LinphoneCore,soundcards[iSoundCardNum+1]);
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"LS_RegisterWithAsterisk: Sound card set to %s",soundcards[iSoundCardNum+1]);
	  }
	
	if (LS_pLinphoneProxyConfig != NULL)
	{
		linphone_core_add_proxy_config(LS_LinphoneCore, LS_pLinphoneProxyConfig);
		linphone_core_set_default_proxy(LS_LinphoneCore, LS_pLinphoneProxyConfig);
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "LS_RegisterWithAsterisk: Proxy wasn't registered");
	}

	func_exit("LS_RegisterWithAsterisk");
}
/** Thread main loop */
static void LS_Main_Loop()
{
	func_enter("LS_Main_Loop");
	while (!LS_bQuit)
	{
		LS_ProcessEvents();
		Sleep(20);
		if (LS_Auth_Received == 1)
			LS_SetupAuth();
	}
	func_exit("LS_Main_Loop");
}
/** Unregister with Asterisk */
static void LS_UnregisterWithAsterisk()
{
	PLUTO_SAFETY_LOCK(sl, LS_linphone_mutex);
	func_enter("LS_UnregisterWithAsterisk");
	linphone_core_terminate_call(LS_LinphoneCore, NULL);
	linphone_core_destroy(LS_LinphoneCore);
	func_exit("LS_UnregisterWithAsterisk");
}
/** Initialize LinphoneCoreVTable variable */
static void LS_InitVTable()
{
	LS_LinphoneCoreVTable.show = (ShowInterfaceCb) stub;
#ifdef LINPHONE_3_6
	LS_LinphoneCoreVTable.call_state_changed = call_state_changed;
#else
	LS_LinphoneCoreVTable.inv_recv = call_received;
	LS_LinphoneCoreVTable.bye_recv = bye_received;
#endif
	LS_LinphoneCoreVTable.auth_info_requested = auth_requested;
	LS_LinphoneCoreVTable.display_status = (DisplayStatusCb) display_something;
	LS_LinphoneCoreVTable.display_message = (DisplayMessageCb) display_something;
	LS_LinphoneCoreVTable.display_warning = (DisplayMessageCb) display_warning;
	LS_LinphoneCoreVTable.display_url = (DisplayUrlCb) display_url;
#ifndef LINPHONE_3_6
	LS_LinphoneCoreVTable.display_question = (DisplayQuestionCb) stub;
#endif
}
/** Initialize LinphoneProxyConfig variable */
static void LS_InitProxy()
{
	func_enter("LS_InitProxy");
	DCEConfig dceconf;
	
	string sProxy = SimplePhoneConf::Instance().Get_Server_IP();
	string sExtension = LS_pSimplePhone->GetExtension();
	string sIdentity = "sip:" + sExtension + "@" + sProxy;
	sProxy = "sip:" + sProxy;
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS,
	"LS_InitProxy -- Proxy: %s; Extension: %s; Identity: %s",
	sProxy.c_str(), sExtension.c_str(), sIdentity.c_str());

/* Later releases of linphone contain sipsetup.h - upto intrepid they do not */
#ifdef sipsetup_h
	LS_pLinphoneProxyConfig = linphone_proxy_config_new();
#else
	LS_pLinphoneProxyConfig = linphone_proxy_config_new(sProxy.c_str());
#endif
	if (! LS_pLinphoneProxyConfig)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "LS_InitProxy: Failed to initialize proxy");
		LS_bQuit = true;
		return;
	}
	
	linphone_proxy_config_set_server_addr(LS_pLinphoneProxyConfig, sProxy.c_str());
	linphone_proxy_config_set_identity(LS_pLinphoneProxyConfig, sIdentity.c_str());
	linphone_proxy_config_enableregister(LS_pLinphoneProxyConfig, TRUE);
	linphone_proxy_config_expires(LS_pLinphoneProxyConfig, 600);
	func_exit("LS_InitProxy");
}
/** Setup SIP authentication */
static void LS_SetupAuth()
{
	PLUTO_SAFETY_LOCK(sl, LS_linphone_mutex);
	string sPassword = LS_pSimplePhone->GetPassword();
	linphone_auth_info_set_passwd(LS_pLinphoneAuthInfo, sPassword.c_str());
	linphone_core_add_auth_info(LS_LinphoneCore, LS_pLinphoneAuthInfo);
	LS_Auth_Received = 0;
}
static void LS_SignalHandler(int ExitStatus)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SimplePhone: terminated by signal");
	LS_bQuit = true;
}
/* C Object, public methods - implementation */
/** Linphone SIP Thread */
void * LS_Thread(void * arg)
{
	LS_pSimplePhone = (SimplePhone *) arg;
	
	signal(SIGTERM, LS_SignalHandler);
	signal(SIGINT, LS_SignalHandler);
	
	LS_linphone_mutex.Init(NULL);
	LS_InitVTable();
	LS_InitProxy();
	
	/* start doing stuff */
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Started SIP/Linphone thread");
	LS_RegisterWithAsterisk();
	LS_Main_Loop();
	LS_UnregisterWithAsterisk();
	signal(SIGTERM, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	return NULL;
}
/** Returns true of a call is in progress, false otherwise */
bool LS_ActiveCall()
{
	return linphone_core_in_call(LS_LinphoneCore);
}
/** Send a DTMF tone though and active call */
void LS_SentDTMF(char cDTMF)
{
	PLUTO_SAFETY_LOCK(sl, LS_linphone_mutex);
	linphone_core_send_dtmf(LS_LinphoneCore, cDTMF);
}
/** Initiate a call */
void LS_InitiateCall(string sNumber)
{
	PLUTO_SAFETY_LOCK(sl, LS_linphone_mutex);
	linphone_core_invite(LS_LinphoneCore, sNumber.c_str());
}
/** Drop the active call */
bool LS_DropCall()
{
	PLUTO_SAFETY_LOCK(sl, LS_linphone_mutex);
	return LS_DropCall_nolock();
}
/** Drop the active call - version without mutex */
bool LS_DropCall_nolock()
{
	func_enter("LS_DropCall");
	bool retval = linphone_core_terminate_call(LS_LinphoneCore, NULL) != -1;
	func_exit("LS_DropCall");
	return retval;
}
/** Process linphone events */
void LS_ProcessEvents()
{
	PLUTO_SAFETY_LOCK_ERRORSONLY(sl, LS_linphone_mutex);
	linphone_core_iterate(LS_LinphoneCore);
}
/** Accept the incoming call */
bool LS_AcceptCall()
{
	PLUTO_SAFETY_LOCK(sl, LS_linphone_mutex);
	return LS_AcceptCall_nolock();
}
/** Accept the incoming call - version without mutex */
bool LS_AcceptCall_nolock()
{
	bool retval = linphone_core_accept_call(LS_LinphoneCore, NULL) != -1;
	return retval;
}
/* Linphone callbacks - implementation */
/* Note: the lock is already taken in LS_ProcessEvents; these functions are called from linphone_core_iterate */
#ifdef LINPHONE_3_6
static void call_state_changed(LinphoneCore *linphoneCore, LinphoneCall *call, LinphoneCallState cstate, const char *msg){
	char *from;
	from = linphone_call_get_remote_address_as_string(call);
        switch(cstate){
		case LinphoneCallIncomingReceived:
			//printf("New Incoming Call.\n");
			if (strstr(from, "\"plutosecurity\""))
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "SimplePhone: It's a 'speak in the house' call");
				LS_AcceptCall_nolock();
			}
			free(from);
		break;
		case LinphoneCallEnd:
			//printf("Call is terminated.\n");
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "SimplePhone: Received bye, from '%s'", from);
			LS_DropCall_nolock(); // lock is already taken in LS_Main_Loop
		break;
                default:
                        printf("Unhandled notification %i\n",cstate);
        }
}
#else
static void call_received(LinphoneCore *linphoneCore, const char *from)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SimplePhone: Received call, from '%s'", from);
	if (strstr(from, "\"plutosecurity\"") == from)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "SimplePhone: It's a 'speak in the house' call");
		LS_AcceptCall_nolock();
	}
}
static void bye_received(LinphoneCore *linphoneCore, const char *from)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SimplePhone: Received bye, from '%s'", from);
	LS_DropCall_nolock(); // lock is already taken in LS_Main_Loop
}
#endif
static void auth_requested(LinphoneCore *linphoneCore, const char *realm, const char *username)
{
	func_enter("callback: auth_requested");
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SimplePhone: Authorisation requested, realm: '%s', username '%s'", realm, username);
	if (LS_pLinphoneAuthInfo != NULL)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "SimplePhone: Authorisation requested one too many times");
		func_exit("callback: auth_requested");
		return;
	}
	
	LS_Auth_Username = username;
	LS_Auth_Realm = realm;
	LS_Auth_Received = 1;
	
	LS_pLinphoneAuthInfo = linphone_auth_info_new(username, NULL, LS_pSimplePhone->GetPassword().c_str(), NULL, realm);
	func_exit("callback: auth_requested");
}
static void display_something(LinphoneCore * linphoneCore, LINPHONE_CONST char *something)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SimplePhone: lib message: '%s'", something);
}
static void display_url(LinphoneCore * linphoneCore, LINPHONE_CONST char *something, LINPHONE_CONST char *url)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SimplePhone: lib url: '%s' '%s'", something, url);
}
static void display_warning (LinphoneCore * linphoneCore, LINPHONE_CONST char *something)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "SimplePhone: lib warning: '%s'", something);
}
