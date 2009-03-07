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
#include <fstream>
//<-dceag-d-b->
#include "Text_To_Speech.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->
//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Text_To_Speech::Text_To_Speech(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Text_To_Speech_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_TTSMutex( "tts" )
{
	m_TTSMutex.Init(NULL);
	m_dwID=0;
	m_dwPK_Device_MediaPlugin=0;
}

//<-dceag-getconfig-b->
bool Text_To_Speech::GetConfig()
{
	if( !Text_To_Speech_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Be sure there's no temporary files in the directory
	FileUtils::DelDir(FILE_PATH);
	FileUtils::MakeDir(FILE_PATH);
	for(Map_DeviceData_Base::iterator it=m_pData->m_AllDevices.m_mapDeviceData_Base.begin();it!=m_pData->m_AllDevices.m_mapDeviceData_Base.end();++it)
	{
		DeviceData_Base *pDeviceData_Base = it->second;
		if( pDeviceData_Base->m_dwPK_DeviceCategory==DEVICECATEGORY_Media_Plugins_CONST )
		{
			m_dwPK_Device_MediaPlugin=pDeviceData_Base->m_dwPK_Device;
			break;
		}
	}
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Text_To_Speech::~Text_To_Speech()
//<-dceag-dest-e->
{
	
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Text_To_Speech::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
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
void Text_To_Speech::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Text_To_Speech::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

char *Text_To_Speech::CreateWAV(string sText,string sVoice,int &Size)
{
   	string sCmd;
	string sTextFile = "sample.txt";
	string sFile = "sample.wav";
#ifdef WIN32
	sCmd = "iisc /ttw \"" + sText + "\" " + sFile;
	system(sCmd.c_str());
#else
	
	ofstream out(sTextFile.c_str());
	if(!out){
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Could not create temporary text file for sample generation.");	
	}
	out << sText;
	out.close();

	sCmd = "text2wave " + sTextFile + " -o " + sFile;

	//Use custom voice
	if(sVoice!="") {
		sCmd += " -eval \"("+sVoice+")\"";
	} else {
		sCmd += " -eval \"("+DATA_Get_default_voice()+")\"";
	}
	system(sCmd.c_str());
#endif

    size_t iSize;
    char *pData = FileUtils::ReadFileIntoBuffer(sFile, iSize);
    Size = iSize;

#ifdef WIN32
	sCmd = "del " + sFile;
	system(sCmd.c_str());
	sCmd = "del " + sTextFile;
	system(sCmd.c_str());
#else
	sCmd = "rm " + sFile;
	system(sCmd.c_str());
	sCmd = "rm " + sTextFile;
	system(sCmd.c_str());
#endif

	return pData;
}

//<-dceag-c253-b->

	/** @brief COMMAND: #253 - Send Audio To Device */
	/** Will convert the text to an audio file, and send it to the device with the "Play Media" Command. */
		/** @param #9 Text */
			/** What to say */
		/** @param #103 List PK Device */
			/** A comma delimited list of the devices to send it to */
		/** @param #254 Bypass Event */
			/** Will be passed in MH Play Media command */
		/** @param #276 Dont Setup AV */
			/** Dont Setup AV */
		/** @param #278 Voice */
			/** Installed voice to use (blank for default voice) */

void Text_To_Speech::CMD_Send_Audio_To_Device(string sText,string sList_PK_Device,bool bBypass_Event,bool bDont_Setup_AV,string sVoice,string &sCMD_Result,Message *pMessage)
//<-dceag-c253-e->
{
	PLUTO_SAFETY_LOCK(tm,m_TTSMutex);

	string sFile = "/home/public/data/tts/" + StringUtils::itos(m_dwID++) + ".wav";
	string sTextFile = "/home/public/data/tts/" + StringUtils::itos(m_dwID++) + ".txt";
	
	//text2wave will not take a text string directly, so lets put the text into a temporary file
	ofstream out(sTextFile.c_str());
	if(!out){
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Could not create temporary text file");	
	}
	out << sText;
	out.close();

	string sCmd = "text2wave " + sTextFile + " -o " + sFile;

	//Use custom voice
	if(sVoice!="") {
		sCmd += " -eval \"("+sVoice+")\"";
	} else {
		sCmd += " -eval \"("+DATA_Get_default_voice()+")\"";
	}
	system(sCmd.c_str());
	if( FileUtils::FileExists(sFile) )
	{
		string::size_type pos=0;
		while( pos<sList_PK_Device.size() && pos!=string::npos )
		{
			int PK_Device = atoi(StringUtils::Tokenize(sList_PK_Device,",",pos).c_str());
			if( PK_Device )
			{
				DCE::CMD_MH_Play_Media CMD_MH_Play_Media(m_dwPK_Device,m_dwPK_Device_MediaPlugin,PK_Device,sFile,0,0,"",true,0,0 /* bQueue */,0 /* bBypass_Event */, 0 /* bDont_Setup_AV */);
				SendCommand(CMD_MH_Play_Media);
			}
		}
		m_mapOutstandingFiles[time(NULL)]=sFile;
		m_mapOutstandingFiles[time(NULL)]=sTextFile;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Processed file: %s outstanding: %d",sCmd.c_str(),(int) m_mapOutstandingFiles.size());
	for(map<time_t,string>::iterator it=m_mapOutstandingFiles.begin();it!=m_mapOutstandingFiles.end();)
	{
		if( it->first + 300 < time(NULL) )
		{
			FileUtils::DelFile(it->second);
			m_mapOutstandingFiles.erase(it++);
		}
		else
			it++;
	}
}

//<-dceag-c256-b->

	/** @brief COMMAND: #256 - Text To Wave */
	/** This will take a text and convert it into wave file */
		/** @param #9 Text */
			/** The text to say */
		/** @param #19 Data */
			/** This is the wave file */
		/** @param #278 Voice */
			/** Installed voice to use (blank for default voice) */

void Text_To_Speech::CMD_Text_To_Wave(string sText,string sVoice,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c256-e->
{
	*pData = CreateWAV(sText,sVoice,*iData_Size);
}

//<-dceag-createinst-b->!
