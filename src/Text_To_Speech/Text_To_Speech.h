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
#ifndef Text_To_Speech_h
#define Text_To_Speech_h

//	DCE Implemenation for #57 Text To Speech

#include "Gen_Devices/Text_To_SpeechBase.h"
//<-dceag-d-e->

#define FILE_PATH "/home/public/data/tts"

//<-dceag-decl-b->
namespace DCE
{
	class Text_To_Speech : public Text_To_Speech_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Text_To_Speech(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Text_To_Speech();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		pluto_pthread_mutex_t m_TTSMutex; //this will protect needredraw vectors
		int m_dwPK_Device_MediaPlugin;
		DeviceData_Base* m_pDevice_pbx;
		int m_nDevice_pbx;
		int m_dwID;  // A numeric ID for the request
		map<time_t,string> m_mapOutstandingFiles; // So we can delete files more than 5 minutes old

private:
        char *CreateWAV(string sText,string sVoice,int &Size);

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_BranchNo();
	string DATA_Get_default_voice();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #253 - Send Audio To Device */
	/** Will convert the text to an audio file, and send it to the device with the "Play Media" Command. */
		/** @param #9 Text */
			/** What to say */
		/** @param #75 PhoneNumber */
			/** A comma delimited list of phone extensions to send it to, prepend a 9 for outside lines */
		/** @param #103 List PK Device */
			/** A comma delimited list of the devices to send it to */
		/** @param #254 Bypass Event */
			/** Will be passed in MH Play Media command */
		/** @param #276 Dont Setup AV */
			/** Dont Setup AV */
		/** @param #278 Voice */
			/** Installed voice to use (blank for default voice) */

	virtual void CMD_Send_Audio_To_Device(string sText,string sPhoneNumber,string sList_PK_Device,bool bBypass_Event,bool bDont_Setup_AV,string sVoice) { string sCMD_Result; CMD_Send_Audio_To_Device(sText.c_str(),sPhoneNumber.c_str(),sList_PK_Device.c_str(),bBypass_Event,bDont_Setup_AV,sVoice.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Send_Audio_To_Device(string sText,string sPhoneNumber,string sList_PK_Device,bool bBypass_Event,bool bDont_Setup_AV,string sVoice,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #256 - Text To Wave */
	/** This will take a text and convert it into wave file */
		/** @param #9 Text */
			/** The text to say */
		/** @param #19 Data */
			/** This is the wave file */
		/** @param #278 Voice */
			/** Installed voice to use (blank for default voice) */

	virtual void CMD_Text_To_Wave(string sText,string sVoice,char **pData,int *iData_Size) { string sCMD_Result; CMD_Text_To_Wave(sText.c_str(),sVoice.c_str(),pData,iData_Size,sCMD_Result,NULL);};
	virtual void CMD_Text_To_Wave(string sText,string sVoice,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
