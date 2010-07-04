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
#include "Asterisk.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include <stack>
#include <map>

#ifndef WIN32
	#include <dirent.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
#endif

#include "asteriskmanager.h"

#include "DCE/DCEConfig.h"
#include "PlutoUtils/DBHelper.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DeviceTemplate.h"

#include "utils.h"
using namespace ASTERISK;

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Asterisk::Asterisk(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Asterisk_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}
//<-dceag-getconfig-b->
bool Asterisk::GetConfig()
{
	if( !Asterisk_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	string sServerIP = DATA_Get_Server_IP();
	AsteriskManager::getInstance()->Set_Server_IP(sServerIP.empty() ? "127.0.0.1" : sServerIP);

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Server IP: %s", AsteriskManager::getInstance()->Get_Server_IP().c_str());

	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Asterisk::~Asterisk()
//<-dceag-dest-e->
{
	AsteriskManager::getInstance()->Close();
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Asterisk::Register()
//<-dceag-reg-e->
{
    return Connect(PK_DeviceTemplate_get());
}

void Asterisk::PostConnect()
{
	// Searching for an AppServer device
       	DeviceData_Base *pDevice_AppServer = m_pData->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_App_Server_CONST,this);
        if (! pDevice_AppServer)
        {
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Asterisk::PostConnect App_Server device not found in installation. Bailing out.");
                return;
        }
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "App_Server started");


	if ( ! IsAsteriskServerRunning() ) {
		// Starting the asterisk daemon
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Starting asterisk.");
		DCE::CMD_Spawn_Application spawnApplication(
				m_dwPK_Device, pDevice_AppServer->m_dwPK_Device,
				"/usr/sbin/amportal",  	    	// launch this
				"start-asterisk",	// reference it with this name
				"start", 		// params
				"",                     // execute this serialized message on exit with failure
				"",                     // execute this serialized message on exit with success
				false,false,false,true);
		SendCommand(spawnApplication);
	}
}

bool Asterisk::Connect(int iPK_DeviceTemplate) {
    if(!Asterisk_Command::Connect(iPK_DeviceTemplate)) {
        return false;
    }

    /*
     *         instantiate &
     *                 initizalize manager
     *                     */
    AsteriskManager *manager = AsteriskManager::getInstance();
    manager->Initialize(this);
    return true;
}


/*
    When you receive commands that are destined to one of your children,
    then if that child implements DCE then there will already be a separate class
    created for the child that will get the message.  If the child does not, then you will
    get all    commands for your children in ReceivedCommandForChild, where
    pDeviceData_Base is the child device.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Asterisk::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Asterisk::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void Asterisk::SomeFunction()
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

/*

    COMMANDS TO IMPLEMENT

*/

//<-dceag-c233-b->

	/** @brief COMMAND: #233 - PBX_Originate */
	/** Originate a call */
		/** @param #75 PhoneNumber */
			/** Phone id from which to place the call */
		/** @param #82 PhoneType */
			/** Phone type from which to place the call */
		/** @param #83 PhoneExtension */
			/** Extension to dial */
		/** @param #84 PhoneCallerID */
			/** Caller id */

void Asterisk::CMD_PBX_Originate(string sPhoneNumber,string sPhoneType,string sPhoneExtension,string sPhoneCallerID,string &sCMD_Result,Message *pMessage)
//<-dceag-c233-e->
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Command Originate called with param: "
		"Extension: %s, PhoneNumber: %s, PhoneType: %s, PhoneCallerID: %s",
		sPhoneExtension.c_str(), sPhoneNumber.c_str(), sPhoneType.c_str(), sPhoneCallerID.c_str());

    AsteriskManager::getInstance()->Originate(sPhoneExtension,sPhoneNumber,sPhoneType, sPhoneCallerID);
}//<-dceag-c235-b->

	/** @brief COMMAND: #235 - PBX_Transfer */
	/** Transfer a call to other phone */
		/** @param #83 PhoneExtension */
			/** Phone extension to redirect to */
		/** @param #265 Channel 1 */
			/** Channel 1 to transfer */
		/** @param #266 Channel 2 */
			/** Channel 2 to transfer (can be empty) */

void Asterisk::CMD_PBX_Transfer(string sPhoneExtension,string sChannel_1,string sChannel_2,string &sCMD_Result,Message *pMessage)
//<-dceag-c235-e->
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS, 
		"Command Transfer called with param: Channel1 : %s, Channel2: %s, PhoneExtension %s ",
        sChannel_1.c_str(), sChannel_2.c_str(), sPhoneExtension.c_str());

	AsteriskManager::getInstance()->Transfer(sChannel_1,sChannel_2,sPhoneExtension);
}
//<-dceag-c237-b->

	/** @brief COMMAND: #237 - PBX_Hangup */
	/** Hangs up a call */
		/** @param #264 Channel */
			/** The channel id to hang up */

void Asterisk::CMD_PBX_Hangup(string sChannel,string &sCMD_Result,Message *pMessage)
//<-dceag-c237-e->
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Command Hangup called with param: "
		"sChannel: %s", sChannel.c_str());

    AsteriskManager::getInstance()->Hangup(sChannel);
}
//<-dceag-createinst-b->!
//<-dceag-createinst-e->!

#define VOICEMAIL_LOCATION "/var/lib/asterisk/sounds/voicemail/default/"
#define CHECK_PERIOD 60
std::map <string,int> ext2user;

void * startVoiceMailThread(void * Arg)
{
    Asterisk *asterisk = (Asterisk *) Arg;
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Started VoiceMail Thread");
    std::map <std::string,std::string> users2vm;

#ifndef WIN32

	char buffer[1024];
	int seconds = 0;

    while(!asterisk->GetMBQuit())
    {
        if(seconds > CHECK_PERIOD)
        {
            DIR * dir1=opendir(VOICEMAIL_LOCATION);
            if(dir1)
            {
                struct dirent *dirent1 = NULL;
                while((dirent1 = readdir(dir1)))
                {
                    struct stat statbuf;
                    strcpy(buffer,VOICEMAIL_LOCATION);
                    strcat(buffer,dirent1->d_name);
                    stat(buffer,&statbuf);
                    if((S_ISDIR(statbuf.st_mode)) && (dirent1->d_name[0] != '.'))
                    {
                        int vm_new_count=0;
                        int vm_old_count=0;						
                        string exten= dirent1->d_name;
                        strcat(buffer,"/INBOX/");
                        DIR * dir2=opendir(buffer);
                        if(dir2)
                        {
                            struct dirent *dirent2 = NULL;
                            while((dirent2 = readdir(dir2)))
                            {
                                if(strstr(dirent2->d_name,".wav"))
                                {
                                    vm_new_count++;
                                }
                            }
                            closedir(dir2);
                        }
                        strcat(buffer,"Old/");
                        dir2=opendir(buffer);
                        if(dir2)
                        {
                            struct dirent *dirent2 = NULL;
                            while((dirent2 = readdir(dir2)))
                            {
                                if(strstr(dirent2->d_name,".wav"))
                                {
                                    vm_old_count++;
                                }
                            }
                            closedir(dir2);
                        }
                        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Found voicemail for user %d (ext %s) new:%d, old:%d",ext2user[exten],exten.c_str(),vm_new_count,vm_old_count);
						std::string joined_values=StringUtils::itos(vm_new_count)+string(" ")+StringUtils::itos(vm_old_count);
                        if((joined_values != users2vm[exten]) && (ext2user[exten] != 0))
                        {
                            LoggerWrapper::GetInstance()->Write(LV_STATUS, "Will send an event that user voicemail count has changed");
                            Message *msg=new Message(asterisk->GetData()->m_dwPK_Device, DEVICETEMPLATE_VirtDev_Orbiter_Plugin_CONST, PRIORITY_NORMAL, MESSAGETYPE_EVENT, EVENT_Voice_Mail_Changed_CONST,0);
                            msg->m_mapParameters[EVENTPARAMETER_PK_Users_CONST] = ext2user[exten];
                            msg->m_mapParameters[EVENTPARAMETER_Value_CONST] = joined_values;
                            asterisk->GetEvents()->SendMessage(msg);
                            users2vm[exten]=joined_values;
                        }
                    }
                }
                closedir(dir1);
            }
            seconds = 0;
        }
        seconds++;
        Sleep(1000);
    }

#endif

    return NULL;
}

void Asterisk::CreateChildren()
{
    Asterisk_Command::CreateChildren();
    DCEConfig dceconf;
    DBHelper mySqlHelper(dceconf.m_sDBHost, dceconf.m_sDBUser, dceconf.m_sDBPassword, dceconf.m_sDBName,dceconf.m_iDBPort);
    PlutoSqlResult result_set;
    DB_ROW row=NULL;
    const char *sql_buff1="SELECT PK_Users,Extension FROM Users";
    if( (result_set.r=mySqlHelper.db_wrapper_query_result(sql_buff1)) == NULL )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "SQL FAILED : %s",sql_buff1);
        return;
    }
    while((row = db_wrapper_fetch_row(result_set.r)))
    {
        if(row[1])
        {
            ext2user[row[1]] = atoi(row[0]);
        }
    }
    const char *sql_buff2="SELECT FK_Device,IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData=31";
    if( (result_set.r=mySqlHelper.db_wrapper_query_result(sql_buff2)) == NULL )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "SQL FAILED : %s",sql_buff2);
        return;
    }
    while((row = db_wrapper_fetch_row(result_set.r)))
    {
        if(row[1])
        {
			dev2ext[atoi(row[0])] = NULL != row[1] ? row[1] : "";
        }
    }

    if (pthread_create(&voicemailThread, NULL, startVoiceMailThread, (void *) this))
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to create VoiceMail Thread");
        m_bQuit_set(true);
        exit(1);
    }
    pthread_detach(voicemailThread);
}

bool Asterisk::IsAsteriskServerRunning() {

	struct sockaddr_in serverSocket;
	struct hostent *host;
	int socketAsterisk;

	int asteriskPort=4596;
	string asteriskHost="127.0.0.1";

	socketAsterisk = (int)socket(PF_INET, SOCK_STREAM, 0);
	if (socketAsterisk == -1) 
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Could create client test socket for asterisk daemon. This should not happend here. Returning empty mrl.");
		return false;
	}

	if( (host = gethostbyname(asteriskHost.c_str())) == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Could not resolve IP address. Not connecting.");
		return false;
	}

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Trying to connect to Asterisk daemon to see if is running.");

	serverSocket.sin_family = AF_INET;
	memcpy((char *) &serverSocket.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
	serverSocket.sin_port = htons(asteriskPort);

	if ( connect(socketAsterisk, (struct sockaddr*)&serverSocket, sizeof(serverSocket)) == 0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "We have made a successful connection to asterisk on %s:%d. Life is good.", asteriskHost.c_str(), asteriskPort);
		close(socketAsterisk);
		return true;
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "We can't connect yet to the server.");
		return false;
	}
}
//<-dceag-c922-b->

	/** @brief COMMAND: #922 - Send Asterisk Status */
	/** Ask for current extensions and calls status. */

void Asterisk::CMD_Send_Asterisk_Status(string &sCMD_Result,Message *pMessage)
//<-dceag-c922-e->
{
	AsteriskManager::getInstance()->GetExtensionsStatus();
	AsteriskManager::getInstance()->GetCallsStatus();
}
//<-dceag-c1084-b->

	/** @brief COMMAND: #1084 - PBX_Application */
	/** Link an extension to a dialplan application.  (Ex. call an extension and playback a sound file using the playback application) */
		/** @param #83 PhoneExtension */
			/** Extension to dial */
		/** @param #109 Data String */
			/** Application data */
		/** @param #285 Application */
			/** Application to connect with */

void Asterisk::CMD_PBX_Application(string sPhoneExtension,string sData_String,string sApplication,string &sCMD_Result,Message *pMessage)
//<-dceag-c1084-e->
{
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Command Application called with param: "
                "PhoneNumber: %s, Application: %s, Application Data: %s",
                sPhoneExtension.c_str(), sApplication.c_str(), sData_String.c_str());

    AsteriskManager::getInstance()->Application(sPhoneExtension,sApplication,sData_String);
}
