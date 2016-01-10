/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "Generic_VFDLCD.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "VFD_LCD/LCDLogic/MenuLoader.h"
#include "VFD_LCD/LCDLogic/MenuHolder.h"
#include "VFD_LCD/LCDLogic/ActionProcessor.h"
#include "VFD_LCD/LCDLogic/LCDManager.h"
#include "VFD_LCD/Renderers/LCDRenderer.h"
#include "VFD_LCD/InputProviders/SocketStatusInputProvider.h"

#include <memory>
#include "pluto_main/Table_Device.h"

void *WorkerThread(void *);

//<-dceag-const-b->!
// The primary constructor when the class is created as a stand-alone device
Generic_VFDLCD::Generic_VFDLCD(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Generic_VFDLCD_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter),
//<-dceag-const-e->
	m_LCDMessageMutex("lcd messages"), m_WorkerThreadID(0), m_MessageProcessingMutex("message processing")
{
	pthread_cond_init(&m_LCDMessageCond, NULL);
	m_LCDMessageMutex.Init(NULL,&m_LCDMessageCond);

	pthread_cond_init(&m_MessageProcessingCond, NULL);
	m_MessageProcessingMutex.Init(NULL,&m_MessageProcessingCond);

	m_spMenu_Holder.reset(NULL);
	m_spLCDManager.reset(NULL);
	m_spLCDRenderer.reset(NULL);
	m_spSocketStatusInputProvider.reset(NULL);

#ifdef WIN32
	string sHost = m_sIPAddress;
#else
    const char *pMySqlHost = getenv("MySqlHost");
	string sHost = pMySqlHost && strlen(pMySqlHost) ? pMySqlHost : m_sIPAddress;
#endif

    m_pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());
    if(!m_pDatabase_pluto_main->Connect(sHost, "root", "", "pluto_main", 3306))
	{
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot connect to database!");
		delete m_pDatabase_pluto_main;
		m_pDatabase_pluto_main = NULL;
	}
}

//<-dceag-const2-b->!
// The constructor when the class is created as an embedded instance within another stand-alone device
Generic_VFDLCD::Generic_VFDLCD(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: Generic_VFDLCD_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter),
//<-dceag-const2-e->
	m_LCDMessageMutex("lcd messages"), m_WorkerThreadID(0), m_MessageProcessingMutex("message processing")
{
	m_pDatabase_pluto_main = NULL;
}

//<-dceag-dest-b->
Generic_VFDLCD::~Generic_VFDLCD()
//<-dceag-dest-e->
{
	m_bQuit_set(true);
	pthread_cond_broadcast(&m_LCDMessageCond);

	if(m_WorkerThreadID)
	{
		pthread_join(m_WorkerThreadID, NULL);
		m_WorkerThreadID = 0;
	}

	delete m_pDatabase_pluto_main;
	m_pDatabase_pluto_main = NULL;

	pthread_mutex_destroy(&m_LCDMessageMutex.mutex);
	pthread_mutex_destroy(&m_MessageProcessingMutex.mutex);
}

//<-dceag-getconfig-b->
bool Generic_VFDLCD::GetConfig()
{
	if( !Generic_VFDLCD_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Generic_VFDLCD::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Generic_VFDLCD_Command *Create_Generic_VFDLCD(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new Generic_VFDLCD(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Generic_VFDLCD::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Generic_VFDLCD::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

bool Generic_VFDLCD::Setup(string sXMLMenuFilename, string sLCDSerialPort, int nSocketServerPort)
{
	//setup menu loader
	auto_ptr<MenuLoader> spMenuLoader(new MenuLoader());

	//setup menu holder
	m_spMenu_Holder.reset(spMenuLoader->Load(sXMLMenuFilename));

	if(NULL == m_spMenu_Holder.get())
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot parse %s", sXMLMenuFilename.c_str());
		return false;
	}

	m_spMenu_Holder->Setup(new ActionProcessor(this));

	//setup LCD manager
	m_spLCDManager.reset(new LCDManager(m_spMenu_Holder.get(), m_pDatabase_pluto_main));

	//setup LCD renderer
	m_spLCDRenderer.reset(new LCDRenderer(m_spLCDManager.get(), sLCDSerialPort)); 
	m_spLCDManager->AddRenderer(m_spLCDRenderer.get());

	//setup socket status input provider
	m_spSocketStatusInputProvider.reset(new SocketStatusInputProvider(m_spLCDManager.get(), nSocketServerPort));

	//start worker thread
	if(pthread_create(&m_WorkerThreadID, NULL, WorkerThread, (void*)this))
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to create worker thread!");
		return false;
	}

	//test
	CMD_Display_Message("this is a test", "2", "test", "5", "");

	//all ok
	return true;
}

void *WorkerThread(void *p)
{
	Generic_VFDLCD *pGeneric_VFDLCD = reinterpret_cast<Generic_VFDLCD *>(p);
	ILCDMessageProvider *pLCDMessageProvider = dynamic_cast<ILCDMessageProvider *>(pGeneric_VFDLCD);
	ILCDMessageProcessor *pLCDMessageProcessor = dynamic_cast<ILCDMessageProcessor *>(pGeneric_VFDLCD);

	if(NULL == pLCDMessageProvider)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "WorkerThread: Got no LCDMessageProvider. Quitting...");
		return NULL;
	}

	if(NULL == pLCDMessageProcessor)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "WorkerThread: Got no pLCDMessageProcessor. Quitting...");
		return NULL;
	}

	while(!pLCDMessageProvider->IsQuitting())
	{
		//process any messages
		bool bGotNewMessages = false;
		LCDMessage *pLCDMessage = NULL;

		do
		{
			pLCDMessage = pLCDMessageProvider->GetNextMessage();
			if(NULL != pLCDMessage)
			{
				pLCDMessageProcessor->ProcessMessage(pLCDMessage);
				delete pLCDMessage;
				pLCDMessage = NULL;

				bGotNewMessages = true;
			}
		}
		while(bGotNewMessages);

		//wait until we've got new messages or we have to quit
		pLCDMessageProvider->WaitForAnyMessage();
	}

	return NULL;
}

void Generic_VFDLCD::ProvideMessage(LCDMessage *pLCDMessage)
{
	PLUTO_SAFETY_LOCK(mm, m_LCDMessageMutex);
	m_listLCDMessages.push_back(pLCDMessage);
	mm.Release();

	pthread_cond_broadcast(&m_LCDMessageCond);
}

LCDMessage *Generic_VFDLCD::GetNextMessage()
{
	PLUTO_SAFETY_LOCK(mm, m_LCDMessageMutex);
	if(!m_listLCDMessages.empty())
	{
		LCDMessage *pLCDMessage = m_listLCDMessages.front();
		m_listLCDMessages.pop_front();
		return pLCDMessage;
	}

	return NULL;
}

void Generic_VFDLCD::WaitForAnyMessage()
{
	PLUTO_SAFETY_LOCK(mm, m_LCDMessageMutex);
	if(!m_bQuit_get() && m_listLCDMessages.empty())
		mm.CondWait();
}

bool Generic_VFDLCD::IsQuitting()
{
	return m_bQuit_get();
}

void Generic_VFDLCD::ProcessMessage(LCDMessage *pLCDMessage)
{
	m_spLCDManager->ShowStatus(pLCDMessage->text, pLCDMessage->type);

	bool bInterrupted = false;

	if(!m_bQuit_get())
	{
		//wait to be interrupted 
		PLUTO_SAFETY_LOCK(mm, m_MessageProcessingMutex);
		if(pLCDMessage->time != -1)
		{
			if(!mm.TimedCondWait(pLCDMessage->time, 0))
				bInterrupted = true;
		}
		else
		{
			mm.CondWait();
			bInterrupted = true;
		}
	}

	m_spLCDManager->RestoreState();

	if(bInterrupted)
	{
		//What to do ?
	}
}

void Generic_VFDLCD::Interrupt()
{
	pthread_cond_broadcast(&m_MessageProcessingCond);
}

//<-dceag-c406-b->

	/** @brief COMMAND: #406 - Display Message */
	/** Display a message on the lcd/vfd display */
		/** @param #9 Text */
			/** The message to display */
		/** @param #14 Type */
			/** For devices implementing VFD_LCD_Base, this is the message type defined in the header */
		/** @param #50 Name */
			/** you can give the message a name, such as "status", "error", etc */
		/** @param #102 Time */
			/** Number of seconds to display the message for */
		/** @param #103 List PK Device */
			/** If going to a plugin that wil relay messages to other devices (ie orbiter_plugin and orbiter), A comma delimited list of devices to display this message on.  If going to a display device directly (like vfd/lcd) this is ignored. */

void Generic_VFDLCD::CMD_Display_Message(string sText,string sType,string sName,string sTime,string sList_PK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c406-e->
{
	ProvideMessage(new LCDMessage(sText, atoi(sType.c_str()), sName, atoi(sTime.c_str())));
}

//<-dceag-c837-b->

	/** @brief COMMAND: #837 - Show Media Playback State */
	/** Show the current state of the media playback */
		/** @param #5 Value To Assign */
			/** Empty = no media playing, otherwise a speed, 0=pause, 1000=normal forward, -4000 = 4x reverse, etc. */
		/** @param #29 PK_MediaType */
			/** The type of media */
		/** @param #76 Level */
			/** The level of the volume, from 0-100.  empty means it's not known, or "MUTE" */

void Generic_VFDLCD::CMD_Show_Media_Playback_State(string sValue_To_Assign,int iPK_MediaType,string sLevel,string &sCMD_Result,Message *pMessage)
//<-dceag-c837-e->
{
	if(sValue_To_Assign.empty())
	{
		ProvideMessage(new LCDMessage("No media playing", 0, "No media", -1));
	}
	else
	{
		string sStatus = "Media playing: speed " + sValue_To_Assign + " volum " + sLevel;
		ProvideMessage(new LCDMessage(sStatus, 0, "Media playing", -1));
	}
}

