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
#include "SMPTE_Fountain.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "SMPTEGen.h"

#ifdef WIN32
	long smpte_cur, smpte_stop, smpte_pause;
	bool alsa_soundout_shutdown;
	const char *device;
	void *OutputThread(void* param) { return NULL; }
#else
	extern void *OutputThread(void* param);
	extern const char *device;
	extern int smpte_pause;
#endif

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
SMPTE_Fountain::SMPTE_Fountain(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: SMPTE_Fountain_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pStartMediaInfo=NULL;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
SMPTE_Fountain::~SMPTE_Fountain()
//<-dceag-dest-e->
{
	alsa_soundout_shutdown = true;	
}


void *StartSynchronizationThread(void *p)
{
	SMPTE_Fountain *pF = (SMPTE_Fountain*)p;
	pF->SynchronizationThread();
	return NULL;
}

void *StartAskXineThread(void *p)
{
	SMPTE_Fountain *pF = (SMPTE_Fountain*)p;
	pF->AskXineThread();
	return NULL;
}


//<-dceag-getconfig-b->
bool SMPTE_Fountain::GetConfig()
{
	if( !SMPTE_Fountain_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	m_bIsActive = false;
	m_pDevice_Xine = m_pData->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_Xine_Player_CONST);
	if( !m_pDevice_Xine )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"SMPTE_Fountain::GetConfig  cannot find xine");
		return false;
	}

	m_smpteDefaultPreDelay=SMPTEGen::FromTimecode("00:00:10"); // Default of 10 second pre-delay
	m_smpteXineStartupOffset=SMPTEGen::FromTimecode("00:00:00:20"); // Half-second Xine startup allowance
	m_smpteAdjustmentThreshold=SMPTEGen::FromTimecode("00:00:02"); // Allow generated SMPTE and Xine to drift by a maximum of 1 second	
	m_smpteXineSongStop=-1;

	device = DATA_Get_Alsa_Output_Device().c_str();
	string sConfiguration = DATA_Get_Configuration();
	string::size_type pos = 0;
	while( true )
	{
		string sToken = StringUtils::Tokenize(sConfiguration,"\r\n",pos);
		if( sToken.empty() )
		{
			if( pos>=sConfiguration.size() )
				break;
			else
				continue;
		}
		string::size_type tab = 0;
		string sFile = StringUtils::Tokenize(sToken,"\t",tab);
		string sTimeCode = StringUtils::Tokenize(sToken,"\t",tab);
		if( sFile.empty() || sTimeCode.empty() )
			continue;
		long TimeCode = SMPTEGen::FromTimecode(sTimeCode.c_str());
		string sPrepend = StringUtils::Tokenize(sToken,"\t",tab);
		long Prepend;
		if( sPrepend.empty() )
			Prepend=m_smpteDefaultPreDelay;
		else
			Prepend = SMPTEGen::FromTimecode(sPrepend.c_str());
		long Append = SMPTEGen::FromTimecode(StringUtils::Tokenize(sToken,"\t",tab).c_str());
		m_mapFilesTimeCode[sFile] = new TimeCodeInfo(Prepend,Append,TimeCode);
	}
	PurgeInterceptors();  // Since this is not a plugin, be sure to purge so we're not registered more than once
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &SMPTE_Fountain::MediaPlaying ), 0, m_pDevice_Xine->m_dwPK_Device, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Play_Media_CONST );
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &SMPTE_Fountain::MediaStopped ), m_pDevice_Xine->m_dwPK_Device, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Playback_Completed_CONST );
	
	pthread_t threadid;
	pthread_create(	&threadid, NULL, OutputThread, NULL); 
	pthread_create(	&threadid, NULL, StartAskXineThread, (void *)this); 
	pthread_create(	&threadid, NULL, StartSynchronizationThread, (void *)this); 

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool SMPTE_Fountain::Register()
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
void SMPTE_Fountain::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void SMPTE_Fountain::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c192-b->

	/** @brief COMMAND: #192 - On */
	/** Turn on the device */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
		/** @param #98 PK_Device_Pipes */
			/** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

void SMPTE_Fountain::CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage)
//<-dceag-c192-e->
{
	m_bIsActive = true;
}

//<-dceag-c193-b->

	/** @brief COMMAND: #193 - Off */
	/** Turn off the device */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

void SMPTE_Fountain::CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage)
//<-dceag-c193-e->
{
	m_bIsActive = false;
}

bool SMPTE_Fountain::MediaStopped( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	if( false && !m_bIsActive )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Ignoring -- we're not active");
		return false;
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Stopping SMPTE output");
	smpte_stop = -1;
	smpte_cur = 0;
	m_smpteXineReportedTime=0;

	return false;
}

bool SMPTE_Fountain::MediaPlaying( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	if( false && !m_bIsActive )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Ignoring -- we're not active");
		return false;
	}

	if( pMessage->m_mapParameters.find(COMMANDPARAMETER_OriginatorNumber_CONST)!=pMessage->m_mapParameters.end() && 
		atoi(pMessage->m_mapParameters[COMMANDPARAMETER_OriginatorNumber_CONST].c_str()) == m_dwPK_Device )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Ignoring -- it came from us");
		return false;
	}


	string sFileName = pMessage->m_mapParameters[COMMANDPARAMETER_Filename_CONST];
	if( m_mapFilesTimeCode.find(sFileName)==m_mapFilesTimeCode.end() )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Ignoring unknown file %s",sFileName.c_str());
		return false;
	}

	if( m_pStartMediaInfo )
		delete m_pStartMediaInfo;

	TimeCodeInfo *pTimeCodeInfo = m_mapFilesTimeCode[sFileName];

	m_pStartMediaInfo = new StartMediaInfo();
	m_pStartMediaInfo->m_sFilename = sFileName;
	m_pStartMediaInfo->m_iPK_MediaType = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_PK_MediaType_CONST].c_str()); 
	m_pStartMediaInfo->m_iStreamID = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_StreamID_CONST].c_str()); 
	m_pStartMediaInfo->m_sMediaPosition = pMessage->m_mapParameters[COMMANDPARAMETER_MediaPosition_CONST]; 

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Setting Time Offset to: %d", pTimeCodeInfo->m_PadBefore);
	smpte_stop = m_smpteSongOffset = pTimeCodeInfo->m_StartTime;
	smpte_cur = smpte_stop - pTimeCodeInfo->m_PadBefore;
	m_smpteStartXineTime = smpte_stop - m_smpteXineStartupOffset;
	m_smpteXineSongStop = -1;

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"SMPTE start: %d, stop %d, xine-start %d", smpte_cur, smpte_stop, m_smpteStartXineTime);
	
	if( pTimeCodeInfo->m_PadBefore )
	{
		m_pStartMediaInfo->m_bManuallyStart = true;
		return true;
	}
	else
	{
		m_pStartMediaInfo->m_bManuallyStart = false;
		return false;
	}
}

int SMPTE_Fountain::GetLengthOfActiveXineStream()
{
	string sText,sMediaPosition;
	DCE::CMD_Report_Playback_Position CMD_Report_Playback_Position(m_dwPK_Device,m_pDevice_Xine->m_dwPK_Device,0,&sText,&sMediaPosition);
	SendCommand(CMD_Report_Playback_Position);
	string::size_type pos = sMediaPosition.find("TOTAL:");
	if( pos==string::npos )
		return -1; // Couldn't get it
	
	return atoi( sMediaPosition.substr(pos+6).c_str() ) * 25 / 1000;
}


void SMPTE_Fountain::SynchronizationThread()
{
	while(alsa_soundout_shutdown == false)
	{
		if (m_smpteStartXineTime)
		{
			if (smpte_cur > m_smpteStartXineTime)
			{
				m_smpteXineReportedTime = 0;
				m_smpteXineSongStop = -1;
				m_smpteStartXineTime = 0;
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending Xine start command");
				DCE::CMD_Play_Media CMD_Play_Media(m_dwPK_Device,m_pDevice_Xine->m_dwPK_Device,
					m_pStartMediaInfo->m_iPK_MediaType,m_pStartMediaInfo->m_iStreamID,m_pStartMediaInfo->m_sMediaPosition,m_pStartMediaInfo->m_sFilename);
				CMD_Play_Media.m_pMessage->m_mapParameters[COMMANDPARAMETER_OriginatorNumber_CONST] = StringUtils::itos(m_dwPK_Device);
				SendCommand(CMD_Play_Media);
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Finished sending Xine start command");
			}
		}
		else
		{
			if (m_smpteXineReportedTime)
			{
				int AdjustedXineTime = m_smpteXineReportedTime + m_smpteSongOffset;
				if ((smpte_cur < AdjustedXineTime - m_smpteAdjustmentThreshold) ||
					(smpte_cur > AdjustedXineTime + m_smpteAdjustmentThreshold))
				{
					cout << "Adjusting smpte time to match xine time (" << SMPTEGen::FromSMPTECode(smpte_cur);
					cout << "to " << SMPTEGen::FromSMPTECode(AdjustedXineTime) << " diff: " << smpte_cur - AdjustedXineTime << "thresh: " << m_smpteAdjustmentThreshold << endl;
					smpte_cur = AdjustedXineTime;
				}
			}
		}
		Sleep(100);
	}
}

void SMPTE_Fountain::AskXineThread()
{
	while(alsa_soundout_shutdown == false)
	{
		string sConnectInfo = "127.0.0.1:12000";
		string sName = "ask-xine-socket";
		AskXine_Socket *pS = new AskXine_Socket(sConnectInfo, sName);
		
		if (!pS->Connect())
		{
			cout << "Failed to connect to socket!" << endl;
			Sleep(1000);
		}
		else
		{	
			string sLine;
			while (pS->ReceiveString(sLine, 30) && alsa_soundout_shutdown==false)
			{
			//	cout << "Read next line:" << sLine << endl; 
				int Sp, H, M, S, Ms;
				sscanf(sLine.c_str(), "%i,%02i:%02i:%02i.%03i", &Sp, &H, &M, &S, &Ms);
				m_smpteXineReportedTime = SMPTEGen::FromTimecode(StringUtils::Format("%d:%d:%d:%d", H, M, S, (Ms*25 / 1000)).c_str());		
				if (Sp == 1000)
				{
					smpte_pause = 0;
				}
				else
				{
					if (m_smpteXineReportedTime > 0)
						smpte_pause = 1;
				}
				if (m_smpteXineSongStop<0 && m_smpteXineReportedTime > 0)
				{
					m_smpteXineSongStop = GetLengthOfActiveXineStream();
					cout << "\nLength of stream " << m_smpteXineSongStop << "\n" << endl;
					smpte_stop = m_smpteSongOffset+m_smpteXineSongStop;
				}
			} 
			pS->Disconnect();
		}
		
		delete pS;
		pS = NULL;
	}
}
