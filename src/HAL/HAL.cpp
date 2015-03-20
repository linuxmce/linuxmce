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
//<-dceag-d-b->
#include "HAL.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DCE/DCERouter.h"
#include "DCE/DCEConfig.h"

#include "main.h"
#include "lmce-udevd.h"
#include "seriald.h"

class HAL::UdevPrivate
{
	public:

		UdevPrivate(HAL * parent);

		~UdevPrivate();

		/** udevd thread */
		pthread_t udevd_thread;

		/** seriald connection thread */
		pthread_t serial_thread;

		/** udevd thread flag */
		bool running;

		/** serial connection thread flag */
		bool serial_running;

	private:

		HAL * parent_;
};

HAL::UdevPrivate::UdevPrivate(HAL * parent)
	: running(false),
	  serial_running(false),
	  parent_(parent)
{
}

HAL::UdevPrivate::~UdevPrivate()
{
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
HAL::HAL(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: HAL_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_sSignature = StringUtils::itos(DeviceID) + ",HAL"; // A unique signature for all the devices we detect

	m_pUdevPrivate = new UdevPrivate(this);
	if( m_pUdevPrivate == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "HAL(): Not enough memory!");
	}
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
HAL::HAL(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: HAL_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
	m_pUdevPrivate = new UdevPrivate(this);
	if( m_pUdevPrivate == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "HAL(): Not enough memory!");
	}
}

//<-dceag-dest-b->
HAL::~HAL()
//<-dceag-dest-e->
{
	// wait for thread finish
	if( m_pUdevPrivate->running )
	{
		if( LmceUdevD::shutDown()==false )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "HAL::~HAL failed to stop pluto thread.  Doing a sigterm which will kill everything");
			// This will kill everything and exit with an error code incrementing the reload count
			if( 0 != pthread_kill( m_pUdevPrivate->udevd_thread, SIGTERM) )
			{
				//try KILL
				pthread_kill( m_pUdevPrivate->udevd_thread, SIGKILL);
			}
		}
		if( 0 != pthread_join( m_pUdevPrivate->udevd_thread, NULL ) )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "pthread_join hald failed");
		}
		
		m_pUdevPrivate->running = false;
	}
	
	if( m_pUdevPrivate->serial_running )
	{
		if( SerialD::shutDown()==false )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "HAL::~HAL failed to stop serial thread.  Doing a sigterm which will kill everything");
			if( 0 != pthread_kill( m_pUdevPrivate->serial_thread, SIGTERM) )
			{
				//try KILL
				pthread_kill( m_pUdevPrivate->serial_thread, SIGKILL);
			}
		}
			
		if( 0 != pthread_join( m_pUdevPrivate->serial_thread, NULL ) )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "pthread_join seriald failed");
		}
		
		m_pUdevPrivate->serial_running = false;
	}
	
	delete m_pUdevPrivate;
	m_pUdevPrivate = NULL;
}

//<-dceag-getconfig-b->
bool HAL::GetConfig()
{
	if( !HAL_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	int ret = pthread_create( &m_pUdevPrivate->udevd_thread, NULL, LmceUdevD::startUp, (void *) this );
	if( 0 != ret )
	{
		m_pUdevPrivate->running = false;
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Couldn't create hald thread. return = %d", ret);
		return false;
	}
	m_pUdevPrivate->running = true;
	
	ret = pthread_create( &m_pUdevPrivate->serial_thread, NULL, SerialD::startUp, (void *) this );
	if( 0 != ret )
	{
		m_pUdevPrivate->serial_running = false;
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Couldn't create seriald thread. return = %d", ret);
		return false;
	}
	m_pUdevPrivate->serial_running = true;
	
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool HAL::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
HAL_Command *Create_HAL(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new HAL(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void HAL::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void HAL::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void HAL::SomeFunction()
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

bool HAL::sendMessage(Message * pMsg, std::string &returnValue)
{
	returnValue = "";
	
	if(m_pEvent != NULL)
	{			
		if(pMsg != NULL)
		{
			string response;
			bool bRet = m_pEvent->SendMessage( pMsg, response);
			pMsg = NULL;
			if(bRet && (!response.empty() || response.find("Error") == string::npos) )
			{
				returnValue = response;
				return true;
			}
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "pMsg object is NULL");
		}
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "m_pEvent == NULL");
	
	return false;
}

bool HAL::sendMessage(std::string params, std::string &returnValue)
{
	returnValue = "";
	
	if(m_pEvent != NULL)
	{			
		Message *pMsg = new Message(params);
		if(pMsg != NULL)
		{
			string response;
			bool bRet = m_pEvent->SendMessage( pMsg, response);
			pMsg = NULL;
			if(bRet && (!response.empty() || response.find("Error") == string::npos) )
			{
				returnValue = response;
				return true;
			}
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "cannot allocate pMsg object");
		}
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "m_pEvent == NULL");
	
	return false;
}

