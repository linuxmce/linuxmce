/*
     Copyright (C) 2013 LinuxMCE 

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef External_Media_Identifier_h
#define External_Media_Identifier_h

//	DCE Implemenation for #2311 Media Identifier

#include "Gen_Devices/External_Media_IdentifierBase.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class External_Media_Identifier : public External_Media_Identifier_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		External_Media_Identifier(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~External_Media_Identifier();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		External_Media_Identifier(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_Priority();
	bool DATA_Get_Only_One_Per_PC();
	bool DATA_Get_Autoassign_to_parents_room();

			*****EVENT***** accessors inherited from base class
	void EVENT_Media_Identified(string sMRL,string sID,int iPK_Device,int iValue,string sFormat,char *pImage,int iImage_Size);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #314 - Identify Media */
	/** New media has been inserted and needs to be identified.  Any third party Media Identifier device can provide a description of the media. */
		/** @param #2 PK_Device */
			/** The disk drive that has the media */
		/** @param #10 ID */
			/** The ID of the disk */
		/** @param #13 Filename */
			/** The media that needs to be identified, such as /dev/cdrom under Linux, or E: under Windows */
		/** @param #201 PK_Device_Related */
			/** If specified, the device to send the resulting 'media identified' command to.  Otherwise it's the from device */

	virtual void CMD_Identify_Media(int iPK_Device,string sID,string sFilename,int iPK_Device_Related) { string sCMD_Result; CMD_Identify_Media(iPK_Device,sID.c_str(),sFilename.c_str(),iPK_Device_Related,sCMD_Result,NULL);};
	virtual void CMD_Identify_Media(int iPK_Device,string sID,string sFilename,int iPK_Device_Related,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
