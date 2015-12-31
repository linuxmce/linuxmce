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
#include "IRTrans.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "PlutoUtils/LinuxSerialUSB.h"
#include "PlutoUtils/DatabaseUtils.h"

#ifdef WIN32
typedef int WSAEVENT;
#endif

#ifdef LINUX
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <netdb.h>
#include <stdint.h>

typedef int DWORD;
typedef int WSAEVENT;


extern int hCom;

SOCKET local_socket;

#define LIRCD                   "/dev/lircd"
#define PERMISSIONS             0666


#endif

extern "C"
{
#include "remote.h"
#include "dbstruct.h"
#include "network.h"
#include "errcode.h"
#include "fileio.h"
#include "lowlevel.h"
#include "server.h"
#include "global.h"
#include "webserver.h"
#include "flash.h"
#include "xap.h"
#include "ccf.h"
	
	int libmain (int argc,char *argv[]);
	void DoExecuteNetCommand (int client,NETWORKCOMMAND *com,STATUSBUFFER *stat);
	void LCDBrightness (int val);
}
extern void (*CallBackFn)(const char *pRemote,const char *pCommand);
IRTrans *g_pIRTrans=NULL;

void *DoStartIRServer(void *pVoid)
{
	IRTrans *pIRTrans = (IRTrans *) pVoid;
	pIRTrans->StartIRServer();
	return NULL;
}

void DoGotIRCommand(const char *pRemote,const char *pCommand)
{
	g_pIRTrans->GotIRCommand(pRemote,pCommand);
}

//<-dceag-const-b->!
// The primary constructor when the class is created as a stand-alone device
IRTrans::IRTrans(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
        : IRTrans_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
        , IRReceiverBase(this), VFD_LCD_Base(40,2,20)
//<-dceag-const-e->
{
	m_bIRServerRunning=false;
	g_pIRTrans=this;
}

//<-dceag-getconfig-b->
bool IRTrans::GetConfig()
{
	if( !IRTrans_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	if( m_pData->m_dwPK_DeviceTemplate!=DEVICETEMPLATE_IRTrans_Dign_VFD_CONST )
	{
		m_bQuit_VL=true;
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"This IRTrans doesn't have a display");
	}

	if( m_dwPK_Device!=DEVICEID_MESSAGESEND )
	{
		if( !m_Virtual_Device_Translator.GetConfig(m_pData) )
			return false;

		IRReceiverBase::GetConfig(m_pData);
		IRBase::setCommandImpl(this);
		IRBase::setAllDevices(&(GetData()->m_AllDevices));
	}

	FileUtils::DelDir("remotes");
	system("mkdir remotes");

	string sResult;
	DCE::CMD_Get_Sibling_Remotes_Cat CMD_Get_Sibling_Remotes_Cat(m_dwPK_Device,DEVICECATEGORY_Infrared_Plugins_CONST,true,BL_AllHouses, DEVICECATEGORY_IRTrans_Remote_Controls_CONST, &sResult);
	SendCommand(CMD_Get_Sibling_Remotes_Cat);
	vector<string> vectRemotes;

	StringUtils::Tokenize(sResult, "`", vectRemotes); 
	int i;
	for(i=0;i<vectRemotes.size();i++)
	{
		vector<string> vectRemoteConfigs;
		StringUtils::Tokenize(vectRemotes[i], "~", vectRemoteConfigs);
		if (vectRemoteConfigs.size() == 3)
		{
			int PK_DeviceRemote = atoi(vectRemoteConfigs[0].c_str());
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Adding remote ID %d, layout %s\r\n", PK_DeviceRemote, vectRemoteConfigs[1].c_str());
			char cRemoteLayout = 'W';
			if( vectRemoteConfigs[1].size() )
				cRemoteLayout = vectRemoteConfigs[1][0];
			
			m_mapRemoteLayout[PK_DeviceRemote]=cRemoteLayout;

			string sConfiguration = vectRemoteConfigs[2];
			const char *pConfig = sConfiguration.c_str();
			FILE *fp = fopen(("remotes/" + StringUtils::itos(PK_DeviceRemote) + ".rem").c_str(),"wb");
			fwrite(pConfig,1,vectRemoteConfigs[2].size(),fp);
			fclose(fp);
			string sUpper = StringUtils::ToUpper(sConfiguration);
			string::size_type pos_name=0;
			while( (pos_name=sUpper.find("[NAME]",pos_name))!=string::npos )
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

				pos_name += 6; // skip the name
				while( pConfig[pos_name] && (pConfig[pos_name]==' ' || pConfig[pos_name]=='\t') )
					pos_name++;

				pos_space = pos_name +1;
				while( pConfig[pos_space] && pConfig[pos_space]!='\n' && pConfig[pos_space]!='\r' && pConfig[pos_space]!='\t' )
					pos_space++;
				m_mapNameToDevice[ sConfiguration.substr(pos_name,pos_space-pos_name) ] = PK_DeviceRemote;
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Added remote %s device %d layout %c",sConfiguration.substr(pos_name,pos_space-pos_name).c_str(),PK_DeviceRemote,cRemoteLayout);
				break;
			}
		}
	}

	pthread_t pthread_id; 
	if(pthread_create( &pthread_id, NULL, DoStartIRServer, (void*) this) )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot start IRServer thread");
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
IRTrans::~IRTrans()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool IRTrans::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
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
void IRTrans::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	if( m_sAltPort.size() && m_bIRServerRunning )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"IRTrans setting port to %s",m_sAltPort.c_str());
		SetDeviceDataInDB(m_dwPK_Device,DEVICEDATA_COM_Port_on_PC_CONST,m_sAltPort);
		m_sAltPort="";
	}

	// Let the IR Base class try to handle the message
	if ( IRBase::ProcessMessage(pMessage) )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Message %d processed by IRBase class",pMessage->m_dwID);
		sCMD_Result = "OK";
		return;
	}

	sCMD_Result = "UNHANDLED CHILD";
}

// Must override so we can call IRBase::Start() after creating children
void IRTrans::CreateChildren()
{
	IRTrans_Command::CreateChildren();
	Start();
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void IRTrans::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/



//<-dceag-c687-b->

	/** @brief COMMAND: #687 - Set Screen Type */
	/** Sent by Orbiter when the screen changes to tells the i/r receiver what type of screen is displayed so it can adjust mappings if necessary. */
		/** @param #48 Value */
			/** a character: M=Main Menu, m=other menu, R=Pluto Remote, r=Non-pluto remote, N=navigable OSD on media dev, f=full screen media app, F=File Listing, c=computing list, C=Computing full screen */

void IRTrans::CMD_Set_Screen_Type(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c687-e->
{
//	DCE::CMD_Goto_Screen CMD_Goto_Screen(m_dwPK_Device,169,"1",1);  // What was this for???
//	SendCommand(CMD_Goto_Screen);
	m_cCurrentScreen=(char) iValue;
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Screen type now %c",m_cCurrentScreen);
}

void IRTrans::StartIRServer()
{
	string sComPortOnPC = m_sPort.size() ? m_sPort : DATA_Get_COM_Port_on_PC();
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"In start IR Server %s",sComPortOnPC.c_str());

#ifndef WIN32
	CallBackFn=&DoGotIRCommand;
#endif
	m_bIRServerRunning=true;

	char TTYPort[255];
	TTYPort[0]=0;
	if( sComPortOnPC.size() && sComPortOnPC.size()<255 )
#ifndef WIN32
		strcpy(TTYPort,TranslateSerialUSB(sComPortOnPC).c_str());
#else
		strcpy(TTYPort,sComPortOnPC.c_str());
#endif

	bool bLoaded = false;
	char *argv[]={"IRTrans","-loglevel","4","-debug_code","-no_lirc", "-no_web",TTYPort};

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Trying to open IRTrans on %s",TTYPort);
#ifndef WIN32
	if( TTYPort[0]==0 || libmain(7,argv)!=0 )
#else
	if( TTYPort[0]==0 || true )
#endif
	{
		DBHelper mySqlHelper(m_sIPAddress,"root","","pluto_main");
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"IRTrans not found on default port.  Will scan for it %d",(int) mySqlHelper.m_bConnected);
		if( mySqlHelper.m_bConnected )
		{
			vector<string> vectPorts;
			DatabaseUtils::GetUnusedPortsOnPC(&mySqlHelper,m_dwPK_Device,vectPorts);
			size_t s;
			for(s=0;s<vectPorts.size();++s)
			{
				string sPort = vectPorts[s];
#ifndef WIN32
				string sPortTranslated = TranslateSerialUSB(sPort);
#else
				string sPortTranslated = sPort;
#endif
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Looking on %s (%s)",sPort.c_str(),sPortTranslated.c_str());
#ifndef WIN32
				m_sAltPort=sPort;
				strcpy(TTYPort,sPortTranslated.c_str());
				if( libmain(7,argv)!=0 )
#else
				if( true )
#endif
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"IRTrans not found on %s",sPort.c_str());
				else
					break;  // we don't get here anyway
			}
			if( s==vectPorts.size() )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"No IRTrans found on any port.  Clearing the database entry");
				SetDeviceDataInDB(m_dwPK_Device,DEVICEDATA_COM_Port_on_PC_CONST,"");
			}
		}
	}
	else bLoaded=true;

	// We won't get here until it exits
#ifndef WIN32
	LCDBrightness(5);
#endif

	m_bIRServerRunning=false;
}

void IRTrans::GotIRCommand(const char *pRemote,const char *pCommand)
{
	if( m_dwPK_Device==DEVICEID_MESSAGESEND )
	{
		ForceKeystroke(pCommand, m_sAVWHost, m_iAVWPort);
		return;
	}

	if( m_sAltPort.size() && m_bIRServerRunning )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"IRTrans setting port to %s",m_sAltPort.c_str());
		SetDeviceDataInDB(m_dwPK_Device,DEVICEDATA_COM_Port_on_PC_CONST,m_sAltPort);
		m_sAltPort="";
	}

	int PK_Device = m_mapNameToDevice[pRemote];
	if( !PK_Device )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Got command %s from unknown remote %s",pCommand,pRemote);
	else
		ReceivedCode(PK_Device,pCommand);
}

void IRTrans::DoUpdateDisplay(vector<string> *vectString)
{
	if( m_sAltPort.size() && m_bIRServerRunning )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"IRTrans setting port to %s",m_sAltPort.c_str());
		SetDeviceDataInDB(m_dwPK_Device,DEVICEDATA_COM_Port_on_PC_CONST,m_sAltPort);
		m_sAltPort="";
	}

	if( !vectString || vectString->size()==0 )
		return;

	LCDCOMMAND lcdCommand;
	STATUSBUFFER statusBuffer;

	lcdCommand.netcommand=COMMAND_LCD;
	lcdCommand.mode=3;
	lcdCommand.lcdcommand=3;
	lcdCommand.timeout=0;
	lcdCommand.adress=76;
	lcdCommand.protocol_version=200;
	lcdCommand.wid=40;
	lcdCommand.hgt=4;
	memset(lcdCommand.framebuffer,32,200);
	lcdCommand.framebuffer[81]=0;
	
	strncpy((char *) lcdCommand.framebuffer,(*vectString)[0].c_str(),min((size_t)40,(*vectString)[0].size()));
	if( vectString->size()>1 )
		strncpy((char *) (lcdCommand.framebuffer+40),(*vectString)[1].c_str(),min((size_t)40,(*vectString)[1].size()));

	for(int i=0;i<=80;++i)
		if( lcdCommand.framebuffer[i]<' ' || lcdCommand.framebuffer[i]>'~' )
			lcdCommand.framebuffer[i]=' ';
/*
	string s1=(*vectString)[0];
string s2=vectString->size()>1 ? (*vectString)[1] : "";
int i1=s1.size();
int i2=s2.size();
LoggerWrapper::GetInstance()->Write(LV_WARNING,"\x1b[2J;%s\n%s",s1.c_str(),s2.c_str());
*/

	LoggerWrapper::GetInstance()->Write(LV_WARNING,"setting display to: \n%s",(char *) lcdCommand.framebuffer);
#ifdef LINUX
	DoExecuteNetCommand (0,(NETWORKCOMMAND *)&lcdCommand,&statusBuffer);
#endif
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

void IRTrans::CMD_Display_Message(string sText,string sType,string sName,string sTime,string sList_PK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c406-e->
{
	NewMessage(atoi(sType.c_str()),sName,sText,atoi(sTime.c_str()));
}

void IRTrans::SendIR(string Port, string IRCode,int iRepeat)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"IRTrans Sending: %s",IRCode.c_str());
#ifdef LINUX
	int res;
	IRDATA ird;
		
	res = DecodeCCF ((char *) IRCode.c_str(),&ird,START);
	if (res <= 0) {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Illegal Pronto command %s",IRCode.c_str());
		return;
	}

	ird.address = 0;
	ird.target_mask = 0xffff;
	DoSendIR (&ird,0,0);
#endif
}
//<-dceag-c191-b->

	/** @brief COMMAND: #191 - Send Code */
	/** Sends an I/R code to a device. */
		/** @param #9 Text */
			/** The I/R code -- usually in Pronto format */

void IRTrans::CMD_Send_Code(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c191-e->
{
	SendIR("",sText,1);
}

//<-dceag-c194-b->

	/** @brief COMMAND: #194 - Toggle Power */
	/** Set relay state (0 or 1) */
		/** @param #8 On/Off */
			/** Depending on each device On/Off can be interpreted differently, but in genereal On/Off has a value of 1 for on and 0 for Off */

void IRTrans::CMD_Toggle_Power(string sOnOff,string &sCMD_Result,Message *pMessage)
//<-dceag-c194-e->
{
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

void IRTrans::CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c245-e->
{
}
