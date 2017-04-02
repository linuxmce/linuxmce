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
#include "LIRC_DCE.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "PlutoUtils/ProcessUtils.h"
#include "Message.h"
#include "DCERouter.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"

#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <sys/fcntl.h>
#endif

void * StartLeeching(void * Arg)
{
	LIRC_DCE * pLIRC_DCE = (LIRC_DCE *) Arg;

	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	pLIRC_DCE->lirc_leech(pLIRC_DCE->m_DeviceID);
	return NULL;
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
LIRC_DCE::LIRC_DCE(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: LIRC_DCE_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, IRReceiverBase(this)
{
	m_LeechingThread=0;
	pRoute = pRouter;
}

//<-dceag-getconfig-b->
bool LIRC_DCE::GetConfig()
{
	if( !LIRC_DCE_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	if( !m_Virtual_Device_Translator.GetConfig(m_pData) )
		return false;
	
	IRBase::setCommandImpl(this);
	IRBase::setAllDevices(&(GetData()->m_AllDevices));
	IRReceiverBase::GetConfig(m_pData);

	system("killall -9 lircd");

	system("mkdir -p /var/run/lirc");

	// Always do this
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Running modprobe lirc_dev");
	system("modprobe lirc_dev");

	string sSystemDevice = DATA_Get_System_Device();
	string::size_type pos=0;
	while( pos<sSystemDevice.size() && pos!=string::npos )
	{
		string sModProbe = StringUtils::Tokenize(sSystemDevice,",",pos);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Running modprobe %s",sModProbe.c_str());
//		if (sModProbe == "lirc_mceusb2" || sModProbe == "lirc_mceusb")
//			sModProbe = "mceusb";
		system(("modprobe " + sModProbe).c_str());
	}

	FILE *fp;
	string sCOM1 = "1";
	string sCOM2 = "2";
	string sLIRCDriver = DATA_Get_Device();
	string sSerialPort = DATA_Get_Serial_Port();
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Making hardware config");
	if(sSerialPort == sCOM1) {
		sSerialPort = "/dev/ttyS0";
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "->Found Serial 1 in database");
	} else if(sSerialPort == sCOM2) {
		sSerialPort = "/dev/ttyS1";
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "->Found Serial 2 in database");
	} else if(sSerialPort != "") {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "->Found Serial %s in database",sSerialPort.c_str());
	} else {
		struct stat buf;
#ifndef WIN32		
		if (stat("/dev/lirc", &buf) != -1 && S_ISCHR(buf.st_mode))
		{
			sSerialPort = "/dev/lirc";
		}
		else if (stat("/dev/lirc/0", &buf) != -1 && S_ISCHR(buf.st_mode))
		{
			sSerialPort = "/dev/lirc/0";
		}
		else if (stat("/dev/lirc0", &buf) != -1 && S_ISCHR(buf.st_mode))
		{
			sSerialPort = "/dev/lirc0";
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No SerialPort selected and no default entry found. Exiting.");
			return false;
		}
#endif
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "No SerialPort selected, selecting default %s", sSerialPort.c_str());
	}
	
	if(sLIRCDriver == "") {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "No Driver selected, using 'default' as default");
		sLIRCDriver = "default";
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "->Using Driver %s in database",sLIRCDriver.c_str());

	remove("/etc/lirc/lircd.conf");
	fp = fopen("/etc/lirc/lircd.conf", "wt");
	int iNumRemotes=0;
	// Find all our sibblings that are remote controls 
	for(Map_DeviceData_Base::iterator itD=m_pData->m_AllDevices.m_mapDeviceData_Base.begin();
		itD!=m_pData->m_AllDevices.m_mapDeviceData_Base.end();++itD)
	{
		DeviceData_Base *pDevice = itD->second;
		if( pDevice->m_dwPK_Device_ControlledVia==m_pData->m_dwPK_Device_ControlledVia &&
			pDevice->m_dwPK_DeviceCategory==DEVICECATEGORY_LIRC_Remote_Controls_CONST )
		{
			string sType;
			DCE::CMD_Get_Device_Data_Cat CMD_Get_Device_Data_Cat2(m_dwPK_Device,DEVICECATEGORY_General_Info_Plugins_CONST,true,BL_SameHouse,
				pDevice->m_dwPK_Device,DEVICEDATA_Remote_Layout_CONST,true,&sType);
			SendCommand(CMD_Get_Device_Data_Cat2);

			string sConfiguration;
			DCE::CMD_Get_Device_Data_Cat CMD_Get_Device_Data_Cat(m_dwPK_Device,DEVICECATEGORY_General_Info_Plugins_CONST,true,BL_SameHouse,
				pDevice->m_dwPK_Device,DEVICEDATA_Configuration_CONST,true,&sConfiguration);
	
			if( SendCommand(CMD_Get_Device_Data_Cat) && sConfiguration.size() )
			{
				const char *pConfig = sConfiguration.c_str();
				fwrite(pConfig,1,sConfiguration.size(),fp);
				string sUpper = StringUtils::ToUpper(sConfiguration);
				string::size_type pos_name=0;
				while( (pos_name=sUpper.find("NAME",pos_name))!=string::npos )
				{
					// Be sure there's nothing but white space before this
					string::size_type pos_space=pos_name-1;
					while( pos_space>0 && (pConfig[pos_space]==' ' || pConfig[pos_space]=='\t') )
						pos_space--;

					if( pos_space>0 && pConfig[pos_space]!='\r' && pConfig[pos_space]!='\n' )
					{
						pos_name++;
						continue; // It's a name embedded somewhere in the file, not the name we want
					}

					pos_name += 4; // skip the name
					while( pConfig[pos_name] && (pConfig[pos_name]==' ' || pConfig[pos_name]=='\t') )
						pos_name++;

					pos_space = pos_name +1;
					while( pConfig[pos_space] && pConfig[pos_space]!='\n' && pConfig[pos_space]!='\r' && pConfig[pos_space]!='\t' )
						pos_space++;
					m_mapNameToDevice[ sConfiguration.substr(pos_name,pos_space-pos_name) ] = pDevice->m_dwPK_Device;
					char cRemoteLayout = sType.size() ? sType[0] : 'W';
					m_mapRemoteLayout[pDevice->m_dwPK_Device]= cRemoteLayout;
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Added remote %s device %d layout %c",sConfiguration.substr(pos_name,pos_space-pos_name).c_str(),pDevice->m_dwPK_Device,cRemoteLayout);
					iNumRemotes++;
					break;
				}
			}
		}
	}
	fclose(fp);

	if( iNumRemotes==0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"There are no remote controls -- LIRC will be dormant");
		return true;
	}

	// If default is used, then wipe the kernel ir to keycodes mapping
	// as this will interfere with normal LIRC operation (TSCHAK)
	
	if (sLIRCDriver == "default")
	  {
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"default driver selected. Wiping kernel key translation table.");
	    system("ir-keytable -c"); // seeya.
	  }

	system((string("lircd") + " -H " + sLIRCDriver + " -d " + sSerialPort + " /etc/lirc/lircd.conf").c_str());
//TODO: Check if it started

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Creating Leeching Thread for %d remotes",iNumRemotes);
	if (pthread_create(&m_LeechingThread, NULL, StartLeeching, (void *) this))
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to create Leeching Thread");
		m_bQuit_set(true);
		return false;
	}
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
LIRC_DCE::~LIRC_DCE()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool LIRC_DCE::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

// Must override so we can call IRBase::Start() after creating children
void LIRC_DCE::CreateChildren()
{
        LIRC_DCE_Command::CreateChildren();
        Start();
}
//<-dceag-createinst-b->!

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void LIRC_DCE::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
        if (pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND) 
	{
	        if (pMessage->m_dwID == COMMAND_Send_Code_CONST)
		        CMD_Send_Code(pMessage->m_mapParameters[COMMANDPARAMETER_Text_CONST], sCMD_Result, pMessage);
		else
		{
		  if (IRBase::ProcessMessage(pMessage)) 
		  {
		    sCMD_Result = "OK";
		  }
		}
        } else
		sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void LIRC_DCE::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

int LIRC_DCE::lirc_leech(int DeviceID) {
#ifndef WIN32
	int sock;
	if((sock = lirc_init("irexec", 1)) == -1)
		return 1;

	char *code;
	struct timeval last_time = {0, 0}, this_time, time_diff;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Leeching");
	while(true)
	{
		fd_set fdset;
		struct timeval tv;

		do
		{
			FD_ZERO(&fdset);
			FD_SET(sock, &fdset);

			tv.tv_sec = 1;
			tv.tv_usec = 0;

			if (m_bQuit_get())
				return 0;
		} while (select(sock + 1, &fdset, NULL, NULL, &tv) == 0);

		if (errno == EINTR)
		{
			errno = 0;
			continue;
		}

		if (lirc_nextcode(&code) != 0)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Lost connection with lircd");
			return 1;
		}
		
		gettimeofday(&this_time, NULL);
		time_diff = this_time - last_time;

		string sCode = string(code);
		vector<string> vectTrimmed;
		
		StringUtils::Tokenize(sCode," ",vectTrimmed);
		int iRepeat = atoi(vectTrimmed[1].c_str());
		string sToken = vectTrimmed[2];
		string sName = vectTrimmed[3];
		if( sName[ sName.size()-1 ] == '\r' || sName[ sName.size()-1 ] == '\n' )
			sName = sName.substr(0,sName.size()-1);

		int PK_Device = m_mapNameToDevice[sName];

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Key Pressed. %s (%s), repeat %d, time diff %lds%ldus name %s device %d",
				sToken.c_str(), sCode.c_str(), iRepeat, time_diff.tv_sec, time_diff.tv_usec,sName.c_str(),PK_Device);
		
		// ignore repeats and keys received in less than 100ms since the previous (processed) key
		if (iRepeat != 0 || (time_diff.tv_sec == 0 && time_diff.tv_usec < 100000))
			continue;
		last_time = this_time;
		
		ReceivedCode(PK_Device,sToken.c_str());

		free(code);
	}
	lirc_deinit();
#endif

	LoggerWrapper::GetInstance()->Write(0, "Over and out"); 
	return 0;
}


//<-dceag-c687-b->

	/** @brief COMMAND: #687 - Set Screen Type */
	/** Sent by Orbiter when the screen changes to tells the i/r receiver what type of screen is displayed so it can adjust mappings if necessary. */
		/** @param #48 Value */
			/** a character: M=Main Menu, m=other menu, R=Pluto Remote, r=Non-pluto remote, N=navigable OSD on media dev, f=full screen media app, F=File Listing, c=computing list, C=Computing full screen */

void LIRC_DCE::CMD_Set_Screen_Type(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c687-e->
{
	m_cCurrentScreen=(char) iValue;
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Screen type now %c",m_cCurrentScreen);
}

void LIRC_DCE::SendIR(string Port, string sIRCode,int iRepeat)
{
 	string sTextWoSpaces;
	size_t sTextSize = sIRCode.size();
	for (size_t i = 0; i < sTextSize; ++i)
	{
		if (sIRCode[i] != ' ')
			sTextWoSpaces += sIRCode[i];
	}
	cout << "Sending code: " << sIRCode << endl;
	cout << "Without spaces: " << sTextWoSpaces << endl;
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending code: %s", sIRCode.c_str());
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Without spaces: %s", sTextWoSpaces.c_str());
	string sCount = "--count=" + StringUtils::itos(iRepeat);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Repeats: %s", sCount.c_str());
	char * args[] = {"/usr/bin/irsend", (char*) sCount.c_str(), "SEND_PRONTO", (char*) sTextWoSpaces.c_str(), "", NULL};
	ProcessUtils::SpawnDaemon(args[0], args, false);
}

//<-dceag-c191-b->

	/** @brief COMMAND: #191 - Send Code */
	/** The I/R code -- usually in Pronto format */
		/** @param #9 Text */
			/** The I/R code -- usually in Pronto format */

void LIRC_DCE::CMD_Send_Code(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c191-e->
{
        SendIR("0", sText, 1);
	sCMD_Result = "OK";
}

//<-dceag-c245-b->

	/** @brief COMMAND: #245 - Learn IR */
	/** The next IR code received is to be learned in Pronto format and fire a Store IR Code command to the I/R Plugin when done */
		/** @param #2 PK_Device */
			/** You can specify the device to learn for here, or you can send the command to the device itself and leave this blank */
		/** @param #8 On/Off */
			/** Turn IR Learning mode on or off
0, 1 */
		/** @param #25 PK_Text */
			/** If specified, the text object  which should contain the result of the learn command */
		/** @param #154 PK_Command */
			/** Command ID for which the learning is done for */

void LIRC_DCE::CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c245-e->
{
}
