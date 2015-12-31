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
#ifndef Media_Live_LCDButtons_h
#define Media_Live_LCDButtons_h

//	DCE Implemenation for #1860 Media Live LCD/Buttons

#include "Gen_Devices/Media_Live_LCDButtonsBase.h"
//<-dceag-d-e->

#include "../VFD_LCD/VFD_LCD_Base.h"

//<-dceag-decl-b->!
namespace DCE
{
	class Media_Live_LCDButtons : public Media_Live_LCDButtons_Command, public VFD_LCD_Base
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
		pthread_t m_KeyboardLoopThread_Id;
		int m_VfdHandle,m_iPlayBackIcon_Last,m_iSourceIcon_Last;
		string m_sLine1Last,m_sLine2Last; // Cache this so we don't have to re-update if the text hasn't changed since that disturbs the scrolling
		int m_iVfdScrollRegionLast,m_iVfdVolumeLevel_Last; // Also cache this
		bool m_bSpeakerIcon_Last,m_bVolumeIcon_Last;
		char *m_pKeyboardDevice;
public:
		// Public member variables
		DeviceData_Base *m_pDevice_Orbiter;

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Media_Live_LCDButtons(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Media_Live_LCDButtons();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		void DoUpdateDisplay(vector<string> *vectString);  // Put this message on the VFD Display
		void ButtonCallBack(int PK_Button);
		
//<-dceag-const2-b->!

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Block_Device();
	bool DATA_Get_Only_One_Per_PC();
	bool DATA_Get_Autoassign_to_parents_room();
	bool DATA_Get_PNP_Create_Without_Prompting();
	bool DATA_Get_Immediate_Reload_Isnt_Necessar();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


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

	virtual void CMD_Display_Message(string sText,string sType,string sName,string sTime,string sList_PK_Device) { string sCMD_Result; CMD_Display_Message(sText.c_str(),sType.c_str(),sName.c_str(),sTime.c_str(),sList_PK_Device.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Display_Message(string sText,string sType,string sName,string sTime,string sList_PK_Device,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #837 - Show Media Playback State */
	/** Show the current state of the media playback */
		/** @param #5 Value To Assign */
			/** Empty = no media playing, otherwise a speed, 0=pause, 1000=normal forward, -4000 = 4x reverse, etc. */
		/** @param #29 PK_MediaType */
			/** The type of media */
		/** @param #76 Level */
			/** The level of the volume, from 0-100.  empty means it's not known, or "MUTE" */

	virtual void CMD_Show_Media_Playback_State(string sValue_To_Assign,int iPK_MediaType,string sLevel) { string sCMD_Result; CMD_Show_Media_Playback_State(sValue_To_Assign.c_str(),iPK_MediaType,sLevel.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Show_Media_Playback_State(string sValue_To_Assign,int iPK_MediaType,string sLevel,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
