/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
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
#include "StartOrbiter.h"
#include "PlutoUtils/CommonIncludes.h"	
#include "Simulator.h"
#include "OrbiterRenderer.h"
#include "Orbiter.h"
#include "DCE/Logger.h"
#include "ServerLogger.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Orbiter_Win32.h"
#include "MainDialog.h"
using namespace DCE;

#include <string>
using namespace std;
//-----------------------------------------------------------------------------------------------------
extern void (*g_pDeadlockHandler)(PlutoLock *pPlutoLock);
extern void (*g_pSocketCrashHandler)(Socket *pSocket);
extern Command_Impl *g_pCommand_Impl;
//-----------------------------------------------------------------------------------------------------
void DeadlockHandler(PlutoLock *pPlutoLock)
{
	// This isn't graceful, but for the moment in the event of a deadlock we'll just kill everything and force a reload
	if( g_pCommand_Impl )
	{   
        ::MessageBox(NULL, TEXT("Deadlock problem.  Going to reload and quit"), TEXT("Orbiter"), MB_OK);

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Win32/StartOrbiter Deadlock problem.  %d  Going to reload and quit",g_pCommand_Impl->m_dwPK_Device);
		g_pCommand_Impl->OnReload();
	}
}
//-----------------------------------------------------------------------------------------------------
void SocketCrashHandler(Socket *pSocket)
{
	// This isn't graceful, but for the moment in the event of a socket crash we'll just kill everything and force a reload
	if( g_pCommand_Impl )
	{
        ::MessageBox(NULL, TEXT("Socket problem.  Going to reload and quit"), TEXT("Orbiter"), MB_OK);

		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Win32/StartOrbiter Socket problem. %d  Going to reload and quit",g_pCommand_Impl->m_dwPK_Device);
		g_pCommand_Impl->OnReload();
	}
}
//-----------------------------------------------------------------------------------------------------
enum OrbiterStages
{
	osConnect,
	osErrorReconnect,
	osRun,
	osQuit
};
//-----------------------------------------------------------------------------------------------------
bool EventLoop(Orbiter* pOrbiter);
//-----------------------------------------------------------------------------------------------------

Orbiter *Connect(int &PK_Device,int PK_DeviceTemplate, string sRouter_IP,string sLocalDirectory,bool bLocalMode, 
	int Width,int Height, bool bFullScreen, bool &bMustQuit)
{
    bMustQuit = false;

	if(!bLocalMode)
		WriteStatusOutput("Connecting to DCERouter...");

	Orbiter *pOrbiter = NULL;

	try
	{
		if(NULL != Orbiter::Instance())
			Orbiter::DestroyInstance();

		WriteStatusOutput("Building a new orbiter");
		pOrbiter = new Orbiter_Win32(PK_Device, PK_DeviceTemplate, sRouter_IP, sLocalDirectory, bLocalMode, 
			Width, Height, bFullScreen); //the builder method
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "New orbiter created!");
	}
	catch(string s)
	{
        WriteStatusOutput(("ERROR: " + s).c_str());
		LoggerWrapper::GetInstance()->Write(LV_STATUS,s.c_str());
		return NULL;
	}
	catch(const char *s)
	{
        WriteStatusOutput((string("ERROR: ") + s).c_str());
		LoggerWrapper::GetInstance()->Write(LV_STATUS, s);
		return NULL;
	}

	if(NULL != pOrbiter && !bLocalMode)
	{
		WriteStatusOutput("Getting config and connecting...");
		bool bConnected = pOrbiter->GetConfig() && pOrbiter->Connect(0);  // Don't validate the device template, since the same binary is used for lots of devices
		PK_Device = pOrbiter->m_dwPK_Device;
        if(!bConnected)
        {
            if(pOrbiter->m_bQuit_get())
                bMustQuit = true;
            else
            {
				Orbiter::DestroyInstance();
                return NULL;
            }
        }
	}

	return pOrbiter;
}
//-----------------------------------------------------------------------------------------------------
bool Run(Orbiter* pOrbiter, bool bLocalMode)
{
	if(pOrbiter->m_bQuit_get())
		return false;

	WriteStatusOutput("Parsing configuration data...");

#ifndef POCKETFROG
    pOrbiter->Initialize(gtSDLGraphic);

	if(pOrbiter->m_bQuit_get())
		return false;

	if(pOrbiter->m_bReload)
		return false;

    if (!bLocalMode)
        pOrbiter->CreateChildren();

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Starting processing events");

	WriteStatusOutput("Starting processing events...");
#endif

	return EventLoop(pOrbiter);
}
//-----------------------------------------------------------------------------------------------------
bool EventLoop(Orbiter* pOrbiter)
{
	if(Simulator::GetInstance()->m_bEnableGenerator)
		Simulator::GetInstance()->StartRandomEventGenerator();

	pOrbiter->Renderer()->EventLoop();

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "About to quit EventLoop. Reload %d, ConnectionLost %d, Quit %d", 
		(int) pOrbiter->m_bReload);

	return !pOrbiter->m_bReload;
}
//-----------------------------------------------------------------------------------------------------
void StartOrbiter(int PK_Device,int PK_DeviceTemplate,string sRouter_IP,string sLocalDirectory,bool bLocalMode,
					int Width,int Height, bool bFullScreen)
{
	Orbiter *pOrbiter = NULL;
	OrbiterStages stage = osConnect;

	g_pDeadlockHandler=DeadlockHandler;
	g_pSocketCrashHandler=SocketCrashHandler; 

	while(stage != osQuit)
	{
		switch(stage)
		{
			case osConnect:
                {
					WriteStatusOutput("Stage connect");
				    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Stage connect");
                    bool bMustQuit = false;
				    pOrbiter = Connect(PK_Device, PK_DeviceTemplate, sRouter_IP, sLocalDirectory, bLocalMode, 
						Width, Height, bFullScreen, bMustQuit);

				    if(pOrbiter != NULL && !bMustQuit)
				    {
			            LoggerWrapper::GetInstance()->Write(LV_STATUS, "Connect OK");
					    stage = osRun;

					    LoggerWrapper::GetInstance()->Write( LV_STATUS, "Orbiter SelfUpdate: starting" );
					    if(pOrbiter->SelfUpdate())
					    {
						    LoggerWrapper::GetInstance()->Write( LV_STATUS, "Orbiter SelfUpdate: need to close orbiter" );
						    stage = osQuit;
					    }
				    }
				    else if(!bMustQuit)
					    stage = osErrorReconnect;
                    else
                        stage = osQuit;
                }
				break;

			case osRun:
 				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Stage run");
				if(Run(pOrbiter, bLocalMode))
					stage = osQuit; 
				else
					stage = osErrorReconnect;
				break;

			case osErrorReconnect:
//				ShowPopup("Pluto Orbiter", "Failed to connect to DCERouter. Trying to reconnect...");

				WriteStatusOutput("Failed to connect to DCERouter. I'll try again in 2 seconds...");
				Sleep(2000);
				stage = osConnect;
				break; 
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Stage is now osQuit.");	
    
	pOrbiter = NULL;
}
//-----------------------------------------------------------------------------------------------------
