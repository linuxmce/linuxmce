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
#include "Tira.h"
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
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#ifndef WIN32
#include "TiraAPI.h"
#endif

int ConvertToCCF(char* CCFOut, int CCFStringSize, const unsigned char* tta);
Tira *g_pTira=NULL;

void *Learning_Thread(void *p)
{
	Tira *pTira = (Tira *) p;
	pTira->m_bLearningIR=true;
	pTira->LearningThread();
	pTira->m_bLearningIR=false;
	return NULL;
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Tira::Tira(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Tira_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, IRReceiverBase(this), m_TiraMutex("tira")
{
	g_pTira=this; // Used for the callback
	m_tsLastButton.tv_sec=0;
	m_bMustConvertRC5_6=true;
	m_bLearningIR=m_bAbortLearning=false;
	pthread_cond_init(&m_TiraCond, NULL);
	m_TiraMutex.Init(NULL,&m_TiraCond);
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Tira::~Tira()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool Tira::GetConfig()
{
	if( !Tira_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	if( !m_Virtual_Device_Translator.GetConfig(m_pData) )
		return false;
	
	IRBase::setCommandImpl(this);
	IRBase::setAllDevices(&(GetData()->m_AllDevices));
	IRReceiverBase::GetConfig(m_pData);
	DeviceData_Base *pDevice = m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Infrared_Plugins_CONST);
	if( pDevice )
		m_dwPK_Device_IRPlugin = pDevice->m_dwPK_Device;
	else
		m_dwPK_Device_IRPlugin = 0;

	m_iRepeat=DATA_Get_Repeat();

	if( pDevice )
	{
		string sResult;
		DCE::CMD_Get_Sibling_Remotes CMD_Get_Sibling_Remotes(m_dwPK_Device,pDevice->m_dwPK_Device, DEVICECATEGORY_Tira_Remote_Controls_CONST, &sResult);
		getCommandImpl()->SendCommand(CMD_Get_Sibling_Remotes);
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
				
				m_mapRemoteLayout[pDevice->m_dwPK_Device]=cRemoteLayout;

				string sConfiguration = vectRemoteConfigs[2];
				vector<string> vectCodes;
				StringUtils::Tokenize(sConfiguration,"\r\n",vectCodes);
				for(size_t s=0;s<vectCodes.size();++s)
				{
					string::size_type pos=0;
					string sButton = StringUtils::Tokenize(vectCodes[s]," ",pos);
					while(pos<vectCodes[s].size())
					{
						string sCode = StringUtils::Tokenize(vectCodes[s]," ",pos);
						m_mapCodesToButtons[sCode] = make_pair<string,int> (sButton,pDevice->m_dwPK_Device);
						LoggerWrapper::GetInstance()->Write(LV_STATUS,"Code: %s will fire button %s",sCode.c_str(),sButton.c_str());
					}
				}
			}
		}
	}

#ifndef WIN32
	int res;
	/*
	res = LoadTiraDLL();
	if ( res != 0 ) 
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Unable to load Tira DLL");
		return true;  // Not much to do, return true so we don't try to reload over and over
	}
	*/
	tira_init();

	int port;
	for(port=0;port<9;++port)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Trying to start Tira on port %d",port);
		res = tira_start(port);
		if ( res == 0 ) 
			break;
	}
	
	if( res!=0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Unable to start Tira on any port");
		return true;  // Not much to do, return true so we don't try to reload over and over
	}

    	res = tira_set_handler(OurCalback);
	if( res!=0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Unable to register our callback");
		return true;  // Not much to do, return true so we don't try to reload over and over
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Tira running on port %d",port);
#endif

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Tira::Register()
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
void Tira::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	// Let the IR Base class try to handle the message
	if (IRBase::ProcessMessage(pMessage))
	{
		printf("Message processed by IRBase class\n");
		sCMD_Result = "OK";
		return;
	}
	
	if( pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND && pMessage->m_dwID==COMMAND_Learn_IR_CONST )
	{
		if( pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST]=="1" )
			StartLearning(pMessage->m_dwPK_Device_To,atoi(pMessage->m_mapParameters[COMMANDPARAMETER_PK_Command_CONST].c_str()),
				pMessage->m_dwPK_Device_From,
				atoi(pMessage->m_mapParameters[COMMANDPARAMETER_PK_Text_CONST].c_str()));
		else
			StopLearning();
	}

	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Tira::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!
/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c191-b->

	/** @brief COMMAND: #191 - Send Code */
	/** The I/R code -- usually in Pronto format */
		/** @param #9 Text */
			/** The I/R code -- usually in Pronto format */

void Tira::CMD_Send_Code(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c191-e->
{
	if( sText.size() && (sText[0]=='5' || sText[0]=='6') )
	{
		string sTextNew = ConvertRC5_6(sText);
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Converted %s to %s",sText.c_str(),sTextNew.c_str());
sText=sTextNew;
	}
	SendIR("",sText,1);
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

void Tira::CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c245-e->
{
	if( sOnOff=="1" )
		StartLearning(iPK_Device,iPK_Command,pMessage->m_dwPK_Device_From,iPK_Text);
	else
		StopLearning();
}

//<-dceag-c687-b->

	/** @brief COMMAND: #687 - Set Screen Type */
	/** Sent by Orbiter when the screen changes to tells the i/r receiver what type of screen is displayed so it can adjust mappings if necessary. */
		/** @param #48 Value */
			/** a character: M=Main Menu, m=other menu, R=Pluto Remote, r=Non-pluto remote, N=navigable OSD on media dev, f=full screen media app, F=File Listing, c=computing list, C=Computing full screen */

void Tira::CMD_Set_Screen_Type(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c687-e->
{
	m_cCurrentScreen=(char) iValue;
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Screen type now %c",m_cCurrentScreen);
}


void Tira::SendIR(string Port, string IRCode,int iRepeat)
{
	if( m_bLearningIR )
		StopLearning();

	const char *pBuffer = IRCode.c_str();
	size_t size = IRCode.size();
	bool bDeleteBuffer=false;
	if( pBuffer[0]=='/' )
	{
		bDeleteBuffer=true;
		pBuffer = FileUtils::ReadFileIntoBuffer(pBuffer,size);
	}
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Tira Sending: %s",pBuffer);

#ifndef WIN32
    int res = tira_transmit(iRepeat, /* the docs say to repeat more than once, but I found with pronto codes that means the code is seen more than once */
                            -1, /* Use embedded frequency value*/
                            (const unsigned char *) pBuffer,
                            size);

    if ( res != 0 ) 
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Tira failed Sending(%d): %s",res,IRCode.c_str());
#endif
	if( bDeleteBuffer )
		delete pBuffer;
}

// Must override so we can call IRBase::Start() after creating children
void Tira::CreateChildren()
{
	Tira_Command::CreateChildren();
	Start();
}
void Tira::OurCallback(const char *szButton)
{
	timespec ts_now;
	gettimeofday(&ts_now,NULL);

	if( m_sLastButton==szButton )
	{
		timespec ts_diff = ts_now-m_tsLastButton;
		if( ts_diff.tv_sec==0 && ts_diff.tv_nsec<500000000 )
			return;
	}
	else
	{
		m_sLastButton=szButton;
		m_tsLastButton=ts_now;
	}

	map<string,pair<string,int> >::iterator it=m_mapCodesToButtons.find(szButton);
	if( it==m_mapCodesToButtons.end() )
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Cannot find anything for IR %s",szButton);
	else
		ReceivedCode(it->second.second,it->second.first.c_str());
}

int __stdcall OurCalback(const char * eventstring) {
   g_pTira->OurCallback(eventstring);
   printf("IR Data %s\n", eventstring);
   return 0;
};

void Tira::StartLearning(int PK_Device,int PK_Command,int PK_Orbiter,int PK_Text)
{
	if( !PK_Device || !PK_Command || !m_dwPK_Device_IRPlugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot learn without a device, command & IR Plugin %d %d %d",
			PK_Device, PK_Command, m_dwPK_Device_IRPlugin);
		return;
	}
	m_iPK_Device_Learning=PK_Device; m_iPK_Command_Learning=PK_Command;
	m_iPK_Orbiter=PK_Orbiter; m_iPK_Text=PK_Text;

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Start learning Command %d Device %d",
		PK_Command,PK_Device);

	PLUTO_SAFETY_LOCK(tm,m_TiraMutex);
	pthread_t t;
	pthread_create(&t, NULL, Learning_Thread, (void*)this);
}

void Tira::StopLearning()
{
	PLUTO_SAFETY_LOCK(tm,m_TiraMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Stop learning");
	m_bAbortLearning=true;

	// Wait up to 1 second for the learning thread to die
	for(int i=0;i<10 && m_bLearningIR;++i)
	{
		tm.Release();
		Sleep(100);
		tm.Relock();
	}
	if( m_bLearningIR )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not stop the learning thread");
}
int iCounter=0;

void Tira::LearningThread()
{
	PLUTO_SAFETY_LOCK(tm,m_TiraMutex);
	m_tLearningStarted=time(NULL);
	tira_start_capture();
	while( !m_bAbortLearning )
	{
		unsigned char *Data=NULL;
		int Size=0;
		tira_get_captured_data( (const unsigned char **) &Data, &Size );
		if( Data && Size )
		{
			FileUtils::WriteBufferIntoFile( ("/tira.buf"+StringUtils::itos(iCounter)).c_str(),(const char *) Data,Size);
			char CCF[4096];
			if( ConvertToCCF(CCF, 4096, Data)==0 )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Learned code device %d command %d orbiter %d text %d code %s",
						m_iPK_Device_Learning,m_iPK_Command_Learning,m_iPK_Orbiter,m_iPK_Text,CCF);
				DCE::CMD_Store_Infrared_Code CMD_Store_Infrared_Code(m_dwPK_Device,m_dwPK_Device_IRPlugin,
					m_iPK_Device_Learning,CCF,m_iPK_Command_Learning);
				SendCommand(CMD_Store_Infrared_Code);
				getCodeMap()[longPair(m_iPK_Device_Learning, m_iPK_Command_Learning)] = CCF;
				if( m_iPK_Orbiter && m_iPK_Text )
				{
					DCE::CMD_Set_Text CMD_Set_Text(m_dwPK_Device,m_iPK_Orbiter,"",
						"Learned ok",m_iPK_Text);
					SendCommand(CMD_Set_Text);
				}
FileUtils::WriteBufferIntoFile( ("/tira_pronto.buf"+StringUtils::itos(iCounter)).c_str(),(const char *) CCF,4096);
iCounter++;
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Got %d",getCodeMap().size());

			}
			else
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Couldn't convert %d %s to CCF",Size,Data);
			tira_cancel_capture();
			return;
		}

		tm.TimedCondWait(0,100000000);
		// Only timeout if we have an orbiter and text object.  Otherwise we have no way to report this 
		// to the user
		if( m_iPK_Orbiter && m_iPK_Text && m_tLearningStarted + LEARNING_TIMEOUT < time(NULL) )
		{
			DCE::CMD_Set_Text CMD_Set_Text(m_dwPK_Device,m_iPK_Orbiter,"",
				"Learning failed",m_iPK_Text);
			SendCommand(CMD_Set_Text);
			tira_cancel_capture();
			return;
		}
	}
}
