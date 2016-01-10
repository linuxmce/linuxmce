/*
 Bluetooth_Dongle
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to 
 the terms of the Pluto Public License, available at: 
 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, 
 but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. 
 
 See the Pluto Public License for more details.
 
 */

/**
 *
 * @file Bluetooth_Dongle.cpp
 * @brief source file for the BD_Orbiter and Bluetooth_Dongle classes
 * @author
 * @todo notcommented
 *
 */


 
//<-dceag-d-b->
#include "Bluetooth_Dongle.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "../DCE/DeviceData_Base.h"

#include "../pluto_main/Define_Command.h"
#include "../pluto_main/Define_DeviceCategory.h"
#include "../pluto_main/Define_DeviceData.h"
#include "../pluto_main/Define_Text.h"

#include "../Orbiter/Orbiter.h"
#include "OrbiterBluetoothCreator.h"

#include "../VIPShared/BD_CP_SendMeKeystrokes.h"
#include "../VIPShared/BD_CP_SendFile.h"
#include "../VIPShared/BD_PC_Disconnect.h"
#include "../VIPShared/BDCommandProcessor_BluetoothDongle.h"

#include "BD/PhoneDevice.h"
#include "BD/BDCommandProcessor.h"

#include "PlutoUtils/PlutoDefs.h"

#include "../Orbiter/Simulator.h"

#define DEFAULT_SCREEN_WIDTH 176
#define DEFAULT_SCREEN_HEIGHT 209

extern Command_Impl *g_pCommand_Impl;

using namespace DCE;

/** @test
#ifdef BT_SOCKET

#else
	#ifndef WIN32
	#else
#include "BD/BDCommandProcessor.h"
	#endif
#endif
*/
//-----------------------------------------------------------------------------------------------------

#ifdef WIN32
#include "CRTDBG.h"
#endif

//-----------------------------------------------------------------------------------------------------

void *HandleBDCommandProcessorThread( void *p )
{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"HandleBDCommandProcessorThread started");

	BD_Orbiter_Plus_DongleHandle *pBD_Orbiter_Plus_DongleHandle = (BD_Orbiter_Plus_DongleHandle *)p;

    Bluetooth_Dongle *pBluetooth_Dongle = pBD_Orbiter_Plus_DongleHandle->m_pBluetooth_Dongle;
    PLUTO_SAFETY_LOCK( bm, pBluetooth_Dongle->m_BTMutex );

	BD_Orbiter *pBD_Orbiter = pBD_Orbiter_Plus_DongleHandle->m_pBD_Orbiter; // converting it to an BD_Orbiter object
	string sMacAddress = pBD_Orbiter_Plus_DongleHandle->m_sMacAddress;
	u_int64_t iMacAddress = pBD_Orbiter_Plus_DongleHandle->m_iMacAddress;
    string sVMC_File = pBD_Orbiter_Plus_DongleHandle->m_sVMCFile;
    string sConfig_File = pBD_Orbiter_Plus_DongleHandle->m_sConfigFile;
	
	if( !pBD_Orbiter || !pBD_Orbiter->m_pBDCommandProcessor || pBD_Orbiter->m_pBDCommandProcessor->m_bDead ) // check if everything is OK first
	{
		if( pBD_Orbiter && pBD_Orbiter->m_pBDCommandProcessor ) // pBD_Orbiter->m_pBDCommandProcessor->m_bDead is true
		{
			delete pBD_Orbiter->m_pBDCommandProcessor;
			pBD_Orbiter->m_pBDCommandProcessor=NULL;
		}
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Command processor is invalid. Aborting" );
		return NULL;
	}

	if( NULL != pBD_Orbiter->m_pPhoneDevice )
		pBD_Orbiter->m_pPhoneDevice->m_bIsConnected = true;

	BD_CP_SendMeKeystrokes *pBD_CP_SendMeKeystrokes = new BD_CP_SendMeKeystrokes( 1 );
	pBD_Orbiter->m_pBDCommandProcessor->AddCommand( pBD_CP_SendMeKeystrokes );

    //sending plutomo.vmc file
    if(sVMC_File != "")
    {
        string sDestinationVMCFileName = "PlutoMO.vmc";

        size_t iSize = 0;
        char *pData = FileUtils::ReadFileIntoBuffer(sVMC_File, iSize);

        if(pData)
        {
            LoggerWrapper::GetInstance()->Write(LV_WARNING, "Sending %s file to PlutoMO, size %d", sVMC_File.c_str(), iSize);

            BD_CP_SendFile *pBD_CP_SendFile = new BD_CP_SendFile(
                const_cast<char *>(sDestinationVMCFileName.c_str()), 
                pData, 
                (unsigned long)sDestinationVMCFileName.length(), 
                (unsigned long)iSize
            );
            pBD_Orbiter->m_pBDCommandProcessor->AddCommand( pBD_CP_SendFile );            
            PLUTO_SAFE_DELETE(pData);
        }
    }

    //sending config file
    if(FileUtils::FileExists(sConfig_File))
    {
        string sDestinationCfgFileName = "PlutoMO.cfg";

        size_t iSize = 0;
        char *pData = FileUtils::ReadFileIntoBuffer(sConfig_File, iSize);

        if(pData)
        {
            LoggerWrapper::GetInstance()->Write(LV_WARNING, "Sending %s file to PlutoMO, size %d", sConfig_File.c_str(), iSize);

            BD_CP_SendFile *pBD_CP_SendFile = new BD_CP_SendFile(
                const_cast<char *>(sDestinationCfgFileName.c_str()), 
                pData, 
                (unsigned long)sDestinationCfgFileName.length(), 
                (unsigned long)iSize
                );
            pBD_Orbiter->m_pBDCommandProcessor->AddCommand( pBD_CP_SendFile );            
            PLUTO_SAFE_DELETE(pData);
        }
    }

	bm.Release();
	
    while( 
        !pBluetooth_Dongle->m_bQuit_get()&&
        NULL != pBD_Orbiter->m_pBDCommandProcessor && 
        !pBD_Orbiter->m_pBDCommandProcessor->m_bDead && 
        pBD_Orbiter->m_pBDCommandProcessor->ReceiveCommand( 0, 0, NULL )
    ); // loop for receiving commands

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Exiting command processor...");

    if(pBluetooth_Dongle->m_bQuit_get())
    {
        LoggerWrapper::GetInstance()->Write( LV_STATUS, "Going to quit...");

        LoggerWrapper::GetInstance()->Write( LV_STATUS, "Deleting command processor...");
        PLUTO_SAFE_DELETE(pBD_Orbiter->m_pBDCommandProcessor);

        LoggerWrapper::GetInstance()->Write( LV_STATUS, "Deleting orbiter... %p", pBD_Orbiter->m_pOrbiter);
        PLUTO_SAFE_DELETE(pBD_Orbiter->m_pOrbiter);

        LoggerWrapper::GetInstance()->Write( LV_STATUS, "HandleBDCommandProcessorThread existed for %s", sMacAddress.c_str());
        return NULL;
    }

	bm.Relock();

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Sending 'Mobile_orbiter_lost' to Orbiter_Plugin...");
	pBluetooth_Dongle->GetEvents()->Mobile_orbiter_lost( sMacAddress, true );
	
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Removing phone from the detection list...");
	pBluetooth_Dongle->RemoveDeviceFromDetectionList( iMacAddress );
	
	if( NULL != pBD_Orbiter->m_pPhoneDevice )
	{
	    PLUTO_SAFETY_LOCK(mm,pBluetooth_Dongle->m_MapMutex);
		map<u_int64_t,class PhoneDevice *>::iterator it = pBluetooth_Dongle->m_mapPhoneDevice_Detected.find(iMacAddress);
		PhoneDevice *pDevice = it == pBluetooth_Dongle->m_mapPhoneDevice_Detected.end() ? NULL : (*it).second;

		if( pDevice )
		    pBluetooth_Dongle->m_mapPhoneDevice_Detected[iMacAddress]->m_bIsConnected = false;
		
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Setting PhoneDevice IsConnected flag to false...");
	}

    LoggerWrapper::GetInstance()->Write( LV_STATUS, "Deleting command processor...");
    PLUTO_SAFE_DELETE(pBD_Orbiter->m_pBDCommandProcessor);

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Deleting orbiter... %p", pBD_Orbiter->m_pOrbiter);
	PLUTO_SAFE_DELETE(pBD_Orbiter->m_pOrbiter);
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Orbiter deleted. %p", pBD_Orbiter->m_pOrbiter);

    //don't need it anymore
    bm.Release();

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Deleting parameter...");
	PLUTO_SAFE_DELETE(pBD_Orbiter_Plus_DongleHandle);

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Exiting HandleBDCommandProcessorThread...");
	return NULL;
}

//-----------------------------------------------------------------------------------------------------

void *ReconnectToBluetoothDongleThread(void *p)
{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"ReconnectToBluetoothDongleThread started");

    BD_ReconnectInfo *pReconnectInfo = (BD_ReconnectInfo *)p;

    if(!pReconnectInfo)
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "BD_ReconnectInfo is invalid!");
        return NULL;
    }

    Bluetooth_Dongle *pBluetooth_Dongle = pReconnectInfo->m_pBluetooth_Dongle;
    string sVMC_File = pReconnectInfo->m_sVMCFile;
    string sConfig_File = pReconnectInfo->m_sConfig_File;
    string sPhoneMacAddress = pReconnectInfo->m_sPhoneMacAddress;
    int iDeviceToLink = pReconnectInfo->m_iDeviceToLink;

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Waiting HandleBDCommandProcessor for %s to exit...", sPhoneMacAddress.c_str());
    while(1)
    {
        PLUTO_SAFETY_LOCK(bm, pBluetooth_Dongle->m_BTMutex);
		map<string, class BD_Orbiter *>::iterator it = pBluetooth_Dongle->m_mapOrbiterSockets.find( sPhoneMacAddress );
		BD_Orbiter *pBD_Orbiter = it == pBluetooth_Dongle->m_mapOrbiterSockets.end() ? NULL : ( *it ).second;

        //still connected ?
        if(!pBD_Orbiter)
            break;

        if(NULL == pBD_Orbiter->m_pBDCommandProcessor && NULL == pBD_Orbiter->m_pOrbiter)
            break;
        
        bm.Release();

        LoggerWrapper::GetInstance()->Write(LV_WARNING, "HandleBDCommandProcessor for %s still running. I'll try again in 250 ms... (cp %p, orb %p)", sPhoneMacAddress.c_str(), pBD_Orbiter->m_pBDCommandProcessor, pBD_Orbiter->m_pOrbiter);
        Sleep(250);
    }

    //BDCommandProcessor is disconnected now, let's give mobile a little time to disconnect
    //LoggerWrapper::GetInstance()->Write(LV_WARNING, "Sleeping 2 sec. to give mobile a little time to disconnect...");
    //Sleep(2000); 

    LoggerWrapper::GetInstance()->Write(LV_WARNING, "HandleBDCommandProcessor for %s BDCommandProcessor is disconnected now, we can connect to new dongle with id %d", sPhoneMacAddress.c_str(), iDeviceToLink);
    DCE::CMD_Link_with_mobile_orbiter CMD_Link_with_mobile_orbiter(iDeviceToLink, iDeviceToLink, sPhoneMacAddress, sVMC_File, sConfig_File);
    pBluetooth_Dongle->SendCommand(CMD_Link_with_mobile_orbiter);

    delete pReconnectInfo;

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "ReconnectToBluetoothDongleThread exiting...");
    return NULL;
}

//-----------------------------------------------------------------------------------------------------

//<-dceag-const-b->! custom
Bluetooth_Dongle::Bluetooth_Dongle( int iDeviceID, string sServerAddress, bool bConnectEventHandler, 
                                   bool bLocalMode, class Router *pRouter )
	: Bluetooth_Dongle_Command( iDeviceID, sServerAddress, bConnectEventHandler, bLocalMode, pRouter ), 
    m_BTMutex( "Bluetooth socket mutex" ), m_ScreenMutex("orbiters rendering")
//<-dceag-const-e->
{
#ifndef WIN32
    setenv("SDL_VIDEODRIVER", "dummy", 1);
#endif

    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_BTMutex.Init( &m_MutexAttr );
    m_ScreenMutex.Init( &m_MutexAttr );
}

//<-dceag-getconfig-b->
bool Bluetooth_Dongle::GetConfig()
{
	if( !Bluetooth_Dongle_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	PLUTO_SAFETY_LOCK( bm, m_BTMutex );

	Map_DeviceData_Base::iterator it;
	for( it=GetData()->m_AllDevices.m_mapDeviceData_Base.begin(); it!=GetData()->m_AllDevices.m_mapDeviceData_Base.end(); ++it )
	{
		DeviceData_Base *pDeviceData_Base = ( *it ).second;
		if( pDeviceData_Base->WithinCategory( DEVICECATEGORY_Mobile_Orbiter_CONST ) )
		{
			if( pDeviceData_Base->m_sMacAddress.length() ) // it has a associated mac address
			{
				m_mapOrbiterSockets[pDeviceData_Base->m_sMacAddress] = new BD_Orbiter;
				// We're definitely not connected to this right now.
				GetEvents()->Mobile_orbiter_lost( pDeviceData_Base->m_sMacAddress, false );
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "Accepting bluetooth connections from %s", pDeviceData_Base->m_sMacAddress.c_str() );
			}
			else
				LoggerWrapper::GetInstance()->Write( LV_WARNING, "Mobile orbiter: %d found, with no mac address", pDeviceData_Base->m_dwPK_Device );
		}
	}
	bm.Release();

	StartScanning(); // start the scanning loop
	return true;
}
//<-dceag-const2-b->!

//-----------------------------------------------------------------------------------------------------

//<-dceag-dest-b->
Bluetooth_Dongle::~Bluetooth_Dongle()
//<-dceag-dest-e->
{
    //just to be sure
    m_bQuit_set(true);

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Starting Bluetooth_Dongle destructor... ");

	PLUTO_SAFETY_LOCK( bm, m_BTMutex );

    map<string, BD_Orbiter*>::iterator it;
    for(it = m_mapOrbiterSockets.begin(); it != m_mapOrbiterSockets.end(); it++)
    {
        BD_Orbiter* pBD_Orbiter = (*it).second;
        string sMacAddress = (*it).first;

        if(!pBD_Orbiter)
        {
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "Skipping device %s... ", sMacAddress.c_str());
            continue;
        }

        if(pBD_Orbiter->m_pBDCommandProcessor)
        {
            LoggerWrapper::GetInstance()->Write(LV_WARNING, "Marking BDCommandProcessor as dead for %s mac... ", sMacAddress.c_str());
            pBD_Orbiter->m_pBDCommandProcessor->m_bDead = true;
        }
    }
    bm.Release();

    LoggerWrapper::GetInstance()->Write(LV_WARNING, "Waiting all HandleBDCommandProcessor to exit...");
    bool bThreadsRunning = true;
    int nRetries = 5;
    while(bThreadsRunning && nRetries > 0)
    {
        nRetries--;
        bThreadsRunning = false;
        bm.Relock();

        map<string, BD_Orbiter*>::iterator it;
        for(it = m_mapOrbiterSockets.begin(); it != m_mapOrbiterSockets.end(); it++)
        {
            BD_Orbiter* pBD_Orbiter = (*it).second;
            string sMacAddress = (*it).first;

            if(pBD_Orbiter && (pBD_Orbiter->m_pBDCommandProcessor || pBD_Orbiter->m_pOrbiter))
            {
                LoggerWrapper::GetInstance()->Write(LV_WARNING, "HandleBDCommandProcessor for %s device is still running...", sMacAddress.c_str());
                bThreadsRunning = true;
                break;
            }
        }

        bm.Release();
        Sleep(500);
    }

	//TODO : pthread_join scanning loop thread

	pthread_mutexattr_destroy(&m_MutexAttr);
	pthread_mutex_destroy(&m_BTMutex.mutex);
    pthread_mutex_destroy(&m_ScreenMutex.mutex);

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Exiting Bluetooth_Dongle destructor... ");
}

//-----------------------------------------------------------------------------------------------------

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Bluetooth_Dongle::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

bool Bluetooth_Dongle::Connect(int iPK_DeviceTemplate) 
{
	if(!Bluetooth_Dongle_Command::Connect(iPK_DeviceTemplate)) {
		return false;
	}

	string sPK_EntertainArea;
	DCE::CMD_Get_EntAreas_For_Device_Cat CMD_Get_EntAreas_For_Device_Cat(m_dwPK_Device,DEVICECATEGORY_Media_Plugins_CONST,false,BL_SameHouse,m_dwPK_Device,&sPK_EntertainArea);
	SendCommand(CMD_Get_EntAreas_For_Device_Cat);
	m_dwPK_EntertainArea = atoi(sPK_EntertainArea.c_str());
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Set EA to %d",m_dwPK_EntertainArea);

	return true;
}
/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Bluetooth_Dongle::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Bluetooth_Dongle::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}


void Bluetooth_Dongle::BugReport() 
{
#ifndef WIN32
	if ( !fork() ) 
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "In child process.");
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Creating bug report in /var/log/BluetoothDongle_BugReport.log ...");
		execl ("/usr/pluto/bin/BluetoothDongle_BugReport.sh",NULL);
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Could not launch BluetoothDongle_BugReport.sh !");

		_exit(1);
	}
#endif
}

//-----------------------------------------------------------------------------------------------------

bool Bluetooth_Dongle::ScanningLoop()
{
    if(m_bQuit_get())
        return false;
    
    // First, scan the map for connections that have died.
	map<string, BD_Orbiter *>::iterator iMos;
	{
		PLUTO_SAFETY_LOCK( bm, m_BTMutex );

        if(m_bQuit_get())
            return false;

		for( iMos = m_mapOrbiterSockets.begin(); 
			iMos != m_mapOrbiterSockets.end(); ++iMos )
		{
			BD_Orbiter *p_BD_Orbiter = ( *iMos ).second;
			if ( NULL != p_BD_Orbiter->m_pBDCommandProcessor && p_BD_Orbiter->m_pBDCommandProcessor->m_bDead )
			{
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending orbiter lost from ScanningLoop");
				GetEvents()->Mobile_orbiter_lost( ( *iMos ).first.c_str(), false );
				BDCommandProcessor *pSock = p_BD_Orbiter->m_pBDCommandProcessor;
				p_BD_Orbiter->m_pBDCommandProcessor = NULL;
				pSock->m_bExit = true; // Cause the scan loop to terminate
			}		
		}
	}

#ifdef SIMULATE_DETECTION
    bool bResult = PhoneDetection_Simulate::ScanningLoop();
#else
    #ifdef WIN32
        bool bResult = PhoneDetection_Bluetooth_Windows::ScanningLoop();
    #else
        bool bResult = PhoneDetection_Bluetooth_Linux::ScanningLoop();
    #endif
#endif

    if(!bResult) //something wrong happened
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "The detection loop is broken.");
//        extern Command_Impl *g_pCommand_Impl;

	BugReport();
	
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Going to reload.");
		if(NULL != g_pCommand_Impl)
			g_pCommand_Impl->OnReload();
    }

    return bResult;
}

//-----------------------------------------------------------------------------------------------------

// temporary hacks
/* TODO - FIND A SOLUTION

THE FRAMEWORK DETECTS A PHONE ONE SCAN, CONNECTS, BUT THEN DOESN'T DETECT IT AGAIN.  SEEMS TO BE A BUG
IN THE BLUETOOTH LIBRARIES.  SO, IF THE FRAMEWORK THINKS A DEVICE IS LOST, WE'LL INTERCEPT IT HERE AND
CHECK TO SEE IF WE HAVE A CONNECTION.  IF SO WE'LL IGNORE THE LOST DEVICE.  SIMILARLY WE'LL IGNOER
THE FOUND DEVICE IF WE'RE ALREADY CONNECTED.  WE'LL JUST PUT IT IN THE MAP AND DO NOTHING */

// This will also add the phone to the map
void Bluetooth_Dongle::Intern_NewDeviceDetected(class PhoneDevice *pDevice)
{
	if(m_bQuit_get())
		return;

	BD_Orbiter *pBD_Orbiter = m_mapOrbiterSockets_Find( pDevice->m_sMacAddress );
	if ( pBD_Orbiter==NULL || pBD_Orbiter->m_pOrbiter==NULL || pBD_Orbiter->m_pBDCommandProcessor==NULL || pBD_Orbiter->m_pBDCommandProcessor->m_bDead )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Bluetooth dongle intercepted new device.  We're not connected.  Proceeding like normal.");
		PhoneDetectionEngine::Intern_NewDeviceDetected(pDevice);
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Bluetooth dongle intercepted new device.  We're connected.  Ignoring it %p",pBD_Orbiter);

		if( pBD_Orbiter )
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Status: %p %p %p %d %d",pBD_Orbiter->m_pOrbiter,pBD_Orbiter->m_pBDCommandProcessor,
					pBD_Orbiter->m_pPhoneDevice, pBD_Orbiter->m_pPhoneDevice->m_bIsConnected,
					pBD_Orbiter->m_pBDCommandProcessor->m_bDead ? 1 : 0);

		/*
		PhoneDevice *pDExisting = m_mapPhoneDevice_Detected_Find(pDevice->m_iMacAddress);

		if(pDExisting)
			delete pDExisting;

		PLUTO_SAFETY_LOCK(mm,m_MapMutex);
		m_mapPhoneDevice_Detected[pDevice->m_iMacAddress] = pDevice;
		*/

		//don't replace the phonedevice`
		if(pDevice)
			delete pDevice;
	}
}

//-----------------------------------------------------------------------------------------------------

void Bluetooth_Dongle::Intern_LostDevice(class PhoneDevice *pDevice)
{
	if(m_bQuit_get())
		return;

	BD_Orbiter *pBD_Orbiter = m_mapOrbiterSockets_Find( pDevice->m_sMacAddress );
	if ( pBD_Orbiter==NULL || pBD_Orbiter->m_pOrbiter==NULL || pBD_Orbiter->m_pBDCommandProcessor==NULL || pBD_Orbiter->m_pBDCommandProcessor->m_bDead )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Bluetooth dongle intercepted lost device.  We're not connected.  Proceeding like normal");
		PhoneDetectionEngine::Intern_LostDevice(pDevice);
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Bluetooth dongle intercepted lost device.  We are connected.  Ignoring it%p", pBD_Orbiter);
		if( pBD_Orbiter )
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Status: %p %p %d",pBD_Orbiter->m_pOrbiter,pBD_Orbiter->m_pBDCommandProcessor,pBD_Orbiter->m_pBDCommandProcessor->m_bDead ? 1 : 0);
	}
}

//-----------------------------------------------------------------------------------------------------

void Bluetooth_Dongle::NewDeviceDetected( class PhoneDevice *pDevice )
{
	if(m_bQuit_get())
		return;

	// We'll just handle this the same way
	SignalStrengthChanged( pDevice );
}

//-----------------------------------------------------------------------------------------------------

void Bluetooth_Dongle::LostDevice( class PhoneDevice *pDevice )
{
	if(m_bQuit_get())
		return;

	PLUTO_SAFETY_LOCK( bm, m_BTMutex );
	map<string, class BD_Orbiter *>::iterator it = m_mapOrbiterSockets.find( pDevice->m_sMacAddress );
	BD_Orbiter *pBD_Orbiter = it == m_mapOrbiterSockets.end() ? NULL : ( *it ).second;

	bool bConnectionFailed = 
		NULL != pBD_Orbiter 
		&& NULL != pBD_Orbiter->m_pPhoneDevice
		&& !pBD_Orbiter->m_pPhoneDevice->m_bIsConnected;

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending orbiter lost from LostDevice...");
	GetEvents()->Mobile_orbiter_lost( pDevice->m_sMacAddress.c_str(), bConnectionFailed );

	if ( NULL != pBD_Orbiter && NULL != pBD_Orbiter->m_pOrbiter )
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Lost %s device!", pDevice->m_sMacAddress.c_str() );
		
		if( NULL != pBD_Orbiter->m_pPhoneDevice )
			pBD_Orbiter->m_pPhoneDevice->m_bIsConnected = false;

		delete pBD_Orbiter->m_pOrbiter;
		pBD_Orbiter->m_pOrbiter = NULL;
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Orbiter deleted for %s device!", pDevice->m_sMacAddress.c_str() );
			
		if( NULL != pBD_Orbiter->m_pBDCommandProcessor )
		{
			pBD_Orbiter->m_pBDCommandProcessor->m_bDead = true;
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Setting m_bDead = true for BDCommandProcessor, %s device!", pDevice->m_sMacAddress.c_str() );
		}
	}
}

//-----------------------------------------------------------------------------------------------------

void Bluetooth_Dongle::SignalStrengthChanged( class PhoneDevice *pDevice )
{
	if(m_bQuit_get())
		return;

	BD_Orbiter *pBD_Orbiter = m_mapOrbiterSockets_Find( pDevice->m_sMacAddress );

	if( NULL != pBD_Orbiter )
		pBD_Orbiter->m_pPhoneDevice = pDevice;

 	if ( !pDevice->m_bIsConnected )
	{
		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Detected device mac: %s link quality: %d", 
			pDevice->m_sMacAddress.c_str(), pDevice->m_iLinkQuality );

		GetEvents()->Mobile_orbiter_detected( pDevice->m_sMacAddress, pDevice->m_iLinkQuality, pDevice->m_sID );
	}	
}

//-----------------------------------------------------------------------------------------------------
/*

	COMMANDS TO IMPLEMENT

*/
//-----------------------------------------------------------------------------------------------------

//<-dceag-c60-b->

	/** @brief COMMAND: #60 - Link with mobile orbiter */
	/** After the dongle has detected the device, then the orbiter plugin will send this command to tell the dongle to link to the phone. */
		/** @param #47 Mac address */
			/** The mac address of the phone */
		/** @param #118 VMC File */
			/** If VMC File is not empty, BluetoothDongle will have to send the file to PlutoMO */
		/** @param #130 Config File */
			/** Path to Config File the be sent to PlutoMO */

void Bluetooth_Dongle::CMD_Link_with_mobile_orbiter(string sMac_address,string sVMC_File,string sConfig_File,string &sCMD_Result,Message *pMessage)
//<-dceag-c60-e->
{
	{
		PLUTO_SAFETY_LOCK( bm, m_BTMutex );
		map<string, BD_Orbiter *>::iterator iMos = m_mapOrbiterSockets.find( sMac_address );
		if ( iMos == m_mapOrbiterSockets.end() )
		{
			 //#define NOSQL
			#ifdef NOSQL
				// hack it in so it works without the database
				m_mapOrbiterSockets[sMac_address] = new BD_Orbiter;
				iMos = m_mapOrbiterSockets.find( sMac_address );
			#else
				LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Got link with MO for nonexistent ID %s!", sMac_address.c_str() );
				return;
			#endif
		}

		BD_Orbiter *pBD_Orbiter = ( *iMos ).second;

		if ( pBD_Orbiter->m_pBDCommandProcessor != NULL )
		{
			if( pBD_Orbiter->m_pBDCommandProcessor->m_bDead )
			{
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "Try to reconnect to MO %s. The connection with the app seems to be dead.", sMac_address.c_str() );

				delete pBD_Orbiter->m_pBDCommandProcessor;
				pBD_Orbiter->m_pBDCommandProcessor = NULL;
			}
			else
			{
				LoggerWrapper::GetInstance()->Write( LV_STATUS, "Got link with MO %s that's already linked!", sMac_address.c_str() );
				return;
			}
		}
	}

	// Do this just to get the id
	PhoneDevice pdTemp( "", sMac_address, 0 );

    PLUTO_SAFETY_LOCK(mm,m_MapMutex);
	map<u_int64_t,class PhoneDevice *>::iterator it = m_mapPhoneDevice_Detected.find(pdTemp.m_iMacAddress);
	PhoneDevice *pD = it==m_mapPhoneDevice_Detected.end() ? NULL : (*it).second;
	if( !pD )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot find device %s anymore to link to, map has %d entries", sMac_address.c_str(), (int) m_mapPhoneDevice_Detected.size());
		return;
	}
    mm.Release();

    LoggerWrapper::GetInstance()->Write( LV_WARNING, "About to connect to PlutoMO. We'll suspend scanning..." );
    SuspendScanning();

#ifdef BT_SOCKET
	// We must be using the DetectionSimulator
	// We'll borrow the establishment socket
	EstablishmentSocket *pSocket = new EstablishmentSocket( 1, "localhost:3461", "foo" );
	if( !pSocket->Connect() )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot connect to phone on Socket sServerAddress" );
		return;
	}

	BDCommandProcessor *pProcessor = 
		new BDCommandProcessor_BluetoothDongle( this, sMac_address, pSocket );
#else
	BDCommandProcessor *pProcessor = 
		new BDCommandProcessor_BluetoothDongle( this, sMac_address, pD );
#endif
	PLUTO_SAFETY_LOCK( bm, m_BTMutex );
	map<string, class BD_Orbiter *>::iterator its = m_mapOrbiterSockets.find( sMac_address );
	BD_Orbiter *pBD_Orbiter = its == m_mapOrbiterSockets.end() ? NULL : ( *its ).second;
	pBD_Orbiter->m_pBDCommandProcessor = pProcessor;
	
	if( NULL != pProcessor && !pProcessor->m_bDead )
	{
		BD_Orbiter_Plus_DongleHandle *pBD_Orbiter_Plus_DongleHandle = 
			new BD_Orbiter_Plus_DongleHandle(
				pBD_Orbiter, this, sMac_address, 
				pBD_Orbiter->m_pPhoneDevice->m_iMacAddress,
                sVMC_File,
                sConfig_File
			);

        LoggerWrapper::GetInstance()->Write( LV_WARNING, "Connected to PlutoMO." );
	    int ret = pthread_create( &pProcessor->m_BDSockThreadID, NULL, HandleBDCommandProcessorThread, ( void* )pBD_Orbiter_Plus_DongleHandle );
		pthread_detach(pProcessor->m_BDSockThreadID);
LoggerWrapper::GetInstance()->Write(LV_STATUS,"pthread_create returned with %d", ret);
	}
    else
    {
        if(pBD_Orbiter->m_pBDCommandProcessor)
        {
            delete pBD_Orbiter->m_pBDCommandProcessor;
            pBD_Orbiter->m_pBDCommandProcessor = NULL;
        }

        LoggerWrapper::GetInstance()->Write( LV_WARNING, "Failed to connect to PlutoMO. We'll resume scanning" );
        ResumeScanning();
    }

}

//-----------------------------------------------------------------------------------------------------

//<-dceag-c61-b->

	/** @brief COMMAND: #61 - Get Signal Strength */
	/** Returns the actual signal strength of the phone for this dongle. */
		/** @param #47 Mac address */
			/** The mac address of the phone */
		/** @param #48 Value */
			/** The strength of the connection
(255 is the maximum strength) */

void Bluetooth_Dongle::CMD_Get_Signal_Strength(string sMac_address,int *iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c61-e->
{
	cout << "Need to implement command #61 - Get Signal Strength" << endl;
	cout << "Parm #47 - Mac_address=" << sMac_address << endl;
	cout << "Parm #48 - Value_int_=" << *iValue << endl;

	int iLinkQuality = GetLinkQuality(sMac_address.c_str());
	LoggerWrapper::GetInstance()->Write( LV_WARNING, "Link quality for %s device is %d", sMac_address.c_str(), iLinkQuality );

	*iValue = iLinkQuality;
} 

//-----------------------------------------------------------------------------------------------------

//<-dceag-c62-b->

	/** @brief COMMAND: #62 - Create Mobile Orbiter */
	/** The bluetooth dongle spawns an internal mobile orbiter which communicates with the phone. */
		/** @param #2 PK_Device */
			/** The ID of the controller to spawn. */
		/** @param #45 PK_EntertainArea */
			/** If not empty, the Orbiter will be created in this entertainarea rather than the one for the dongle.  This will take precendence over PK_Room */
		/** @param #47 Mac address */
			/** The Mac Address of the phone. */
		/** @param #57 PK_Room */
			/** If not 0, the device will be created in this room instead of the default room for the dongle. */

void Bluetooth_Dongle::CMD_Create_Mobile_Orbiter(int iPK_Device,string sPK_EntertainArea,string sMac_address,int iPK_Room,string &sCMD_Result,Message *pMessage)
//<-dceag-c62-e->
{
	PLUTO_SAFETY_LOCK( bm, m_BTMutex );
	
	// Lookup the mobile orbiter entry
	map<string, class BD_Orbiter *>::iterator it = m_mapOrbiterSockets.find( sMac_address );
	BD_Orbiter *pBD_Orbiter = it == m_mapOrbiterSockets.end() ? NULL : ( *it ).second;

	LoggerWrapper::GetInstance()->Write( LV_WARNING, "$$$ CMD_Create_Mobile_Orbiter received for %s device $$$", sMac_address.c_str() );
	
	if( NULL != pBD_Orbiter->m_pBDCommandProcessor && !pBD_Orbiter->m_pBDCommandProcessor->m_bDead )
	{
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "Ready to cleanup orbiter. %p", pBD_Orbiter->m_pOrbiter);
		
		if( NULL != pBD_Orbiter->m_pOrbiter )
		{
			delete pBD_Orbiter->m_pOrbiter;
			pBD_Orbiter->m_pOrbiter = NULL;
		}

		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Orbiter created for %s device", sMac_address.c_str() );

		class OrbiterBluetooth *pOrbiter;
		if( atoi(sPK_EntertainArea.c_str()) || iPK_Room )
        {
            pOrbiter =  CreateOrbiterBluetooth( pBD_Orbiter->m_pBDCommandProcessor, iPK_Device, 0, m_sIPAddress, 
                "", false, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, &m_ScreenMutex, iPK_Room, atoi(sPK_EntertainArea.c_str()));
        }
		else
		{
			pOrbiter = CreateOrbiterBluetooth( pBD_Orbiter->m_pBDCommandProcessor, iPK_Device, 0, m_sIPAddress, 
                "", false, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, &m_ScreenMutex, 0, m_dwPK_EntertainArea);
		}

        if(!pOrbiter) //failed to start orbiter
        {
            if( NULL != pBD_Orbiter->m_pBDCommandProcessor )
            {
                pBD_Orbiter->m_pBDCommandProcessor->m_bDead = true;
                LoggerWrapper::GetInstance()->Write( LV_WARNING, "Setting m_bDead = true for BDCommandProcessor, %s device!", sMac_address.c_str() );
            }
        }

		if(NULL != pOrbiter)
		{
			((Orbiter *)pOrbiter)->FireEntAreaRoomCommands();
			Simulator::GetInstance()->m_pOrbiter = (Orbiter *)pOrbiter;	
#ifdef WIN32			
			Simulator::GetInstance()->LoadConfigurationFile("BluetoothDongle.conf");
#else //LINUX
			Simulator::GetInstance()->LoadConfigurationFile("/etc/BluetoothDongle.conf");
#endif			
			if(Simulator::GetInstance()->m_bEnableGenerator)
				Simulator::GetInstance()->StartRandomEventGenerator();
		}	

		pBD_Orbiter->m_pOrbiter = ( Orbiter * )pOrbiter;
	}
    else
	{
		if( NULL == pBD_Orbiter->m_pBDCommandProcessor )
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Cannot create orbiter for %s device: the CommandProcessor is null!", sMac_address.c_str() );
		else
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Cannot create orbiter for %s device: the CommandProcessor is dead!", sMac_address.c_str() );
	}

    ResumeScanning();
}

//<-dceag-c332-b->

	/** @brief COMMAND: #332 - Ignore MAC Address */
	/** Causes the dongle to ignore any detections of this MAC Address. */
		/** @param #47 Mac address */
			/** The Mac Address */

void Bluetooth_Dongle::CMD_Ignore_MAC_Address(string sMac_address,string &sCMD_Result,Message *pMessage)
//<-dceag-c332-e->
{
	PhoneDevice p("",sMac_address,0); // Just need a numeric mac
	m_mapIgnoreMacs[p.m_iMacAddress]=true;
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Ignoring MAC: %s",sMac_address.c_str());
}
//<-dceag-c333-b->

	/** @brief COMMAND: #333 - Disconnect From Mobile Orbiter */
	/** Disconnects Mobile Orbiter from this BluetoothDongle. */
		/** @param #47 Mac address */
			/** The mac address of the phone */
		/** @param #118 VMC File */
			/** Path the VMC file to send */
		/** @param #124 DeviceToLink */
			/** Send CMD_Link_with_mobile_orbiter command to DeviceToLink */
		/** @param #130 Config File */
			/** Path to Config File to send */

void Bluetooth_Dongle::CMD_Disconnect_From_Mobile_Orbiter(string sMac_address,string sVMC_File,int iDeviceToLink,string sConfig_File,string &sCMD_Result,Message *pMessage)
//<-dceag-c333-e->
{
	PLUTO_SAFETY_LOCK( bm, m_BTMutex );
	LoggerWrapper::GetInstance()->Write( LV_WARNING, "Have to disconnect from MO with mac %s", sMac_address.c_str() );
	
	// Lookup the mobile orbiter entry
	map<string, class BD_Orbiter *>::iterator it = m_mapOrbiterSockets.find( sMac_address );
	BD_Orbiter *pBD_Orbiter = it == m_mapOrbiterSockets.end() ? NULL : ( *it ).second;
	
	if ( NULL != pBD_Orbiter && NULL != pBD_Orbiter->m_pOrbiter )
	{
		if( NULL != pBD_Orbiter->m_pPhoneDevice )
			pBD_Orbiter->m_pPhoneDevice->m_bIsConnected = false;

		delete pBD_Orbiter->m_pOrbiter;
		pBD_Orbiter->m_pOrbiter = NULL;

		LoggerWrapper::GetInstance()->Write( LV_WARNING, "Orbiter deleted for %s device!", sMac_address.c_str() );

		if( NULL != pBD_Orbiter->m_pPhoneDevice )
		{
			PLUTO_SAFETY_LOCK(mm,m_MapMutex);
			map<u_int64_t,class PhoneDevice *>::iterator it = m_mapPhoneDevice_Detected.find(pBD_Orbiter->m_pPhoneDevice->m_iMacAddress);
			PhoneDevice *pDevice = it == m_mapPhoneDevice_Detected.end() ? NULL : (*it).second;

			if( pDevice )
				m_mapPhoneDevice_Detected[pBD_Orbiter->m_pPhoneDevice->m_iMacAddress]->m_bIsConnected = false;
		}

		if( NULL != pBD_Orbiter->m_pBDCommandProcessor )
		{
			pBD_Orbiter->m_pBDCommandProcessor->m_bDead = true;
			LoggerWrapper::GetInstance()->Write( LV_WARNING, "Setting m_bDead = true for BDCommandProcessor, %s device!", sMac_address.c_str() );
		}
	}

	bm.Release();

    BD_ReconnectInfo *pReconnectInfo = new BD_ReconnectInfo(this, sMac_address, iDeviceToLink, sVMC_File, sConfig_File);

    pthread_t pthread_id;
    int ret = pthread_create( &pthread_id, NULL, ReconnectToBluetoothDongleThread, (void*)pReconnectInfo );
	pthread_detach(pthread_id);
LoggerWrapper::GetInstance()->Write(LV_STATUS,"pthread_create returned with %d", ret);
}
//<-dceag-createinst-b->! I don't need it.
//<-dceag-createinst-e->
//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void Bluetooth_Dongle::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DCE:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Examples:
	
	// Send a specific the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.
	DCE::CMD_Simulate_Mouse_Click CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);
	SendCommand(CMD_Simulate_Mouse_Click);

	// Send the message to orbiters 32898 and 27283 (ie a device list, hence the _DL)
	// And we want a response, which will be "OK" if the command was successfull
	string sResponse;
	DCE::CMD_Simulate_Mouse_Click_DL CMD_Simulate_Mouse_Click_DL(m_dwPK_Device,"32898,27283",55,77)
	SendCommand(CMD_Simulate_Mouse_Click_DL,&sResponse);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	// Note the _Cat for category
	DCE::CMD_Simulate_Mouse_Click_Cat CMD_Simulate_Mouse_Click_Cat(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77)
    SendCommand(CMD_Simulate_Mouse_Click_Cat);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	// Note the _DT.
	DCE::CMD_Simulate_Mouse_Click_DT CMD_Simulate_Mouse_Click_DT(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);
	SendCommand(CMD_Simulate_Mouse_Click_DT);

	// This command has a normal string parameter, but also an int as an out parameter
	int iValue;
	DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength(m_dwDeviceID, DestDevice, sMac_address,&iValue);
	// This send command will wait for the destination device to respond since there is
	// an out parameter
	SendCommand(CMD_Get_Signal_Strength);  

	// This time we don't care about the out parameter.  We just want the command to 
	// get through, and don't want to wait for the round trip.  The out parameter, iValue,
	// will not get set
	SendCommandNoResponse(CMD_Get_Signal_Strength);  

	// This command has an out parameter of a data block.  Any parameter that is a binary
	// data block is a pair of int and char *
	// We'll also want to see the response, so we'll pass a string for that too

	int iFileSize;
	char *pFileContents
	string sResponse;
	DCE::CMD_Request_File CMD_Request_File(m_dwDeviceID, DestDevice, "filename",&pFileContents,&iFileSize,&sResponse);
	SendCommand(CMD_Request_File);

	// If the device processed the command (in this case retrieved the file),
	// sResponse will be "OK", and iFileSize will be the size of the file
	// and pFileContents will be the file contents.  **NOTE**  We are responsible
	// free deleting pFileContents.


	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();
	// Fire the "Touch or click" which takes an X and Y parameter
	EVENT_Touch_or_click(10,150);
}
*/
//<-dceag-sample-e->
