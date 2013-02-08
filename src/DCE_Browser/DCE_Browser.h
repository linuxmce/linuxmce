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
#ifndef DCE_Browser_h
#define DCE_Browser_h

//	DCE Implemenation for #2185 DCE Browser

#include "Gen_Devices/DCE_BrowserBase.h"
//<-dceag-d-e->
class browserWindow;
//<-dceag-decl-b->
namespace DCE
{
	class DCE_Browser : public DCE_Browser_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
public:
            browserWindow *qWebBrowser;
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		DCE_Browser(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~DCE_Browser();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		DCE_Browser(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Username();

			*****EVENT***** accessors inherited from base class
	void EVENT_Watching_Media(int iPK_Room);
	void EVENT_Stopped_Watching_Media(int iPK_Room);
	void EVENT_Url_Loaded();

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #81 - Navigate Next */
	/** Navigate to the next page if there is one. */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

	virtual void CMD_Navigate_Next(int iStreamID) { string sCMD_Result; CMD_Navigate_Next(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Navigate_Next(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #82 - Navigate Prev */
	/** Navigate to the previous web page if there is one */
		/** @param #41 StreamID */
			/** The stream on which to do the navigation. */

	virtual void CMD_Navigate_Prev(int iStreamID) { string sCMD_Result; CMD_Navigate_Prev(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Navigate_Prev(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #190 - Enter/Go */
	/** Enter was hit */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_EnterGo(int iStreamID) { string sCMD_Result; CMD_EnterGo(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_EnterGo(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #200 - Move Up */
	/** Up */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Up(int iStreamID) { string sCMD_Result; CMD_Move_Up(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #201 - Move Down */
	/** Down */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Down(int iStreamID) { string sCMD_Result; CMD_Move_Down(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #202 - Move Left */
	/** Left */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Left(int iStreamID) { string sCMD_Result; CMD_Move_Left(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #203 - Move Right */
	/** Right */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Right(int iStreamID) { string sCMD_Result; CMD_Move_Right(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #204 - 0 */
	/** 0 */

	virtual void CMD_0() { string sCMD_Result; CMD_0(sCMD_Result,NULL);};
	virtual void CMD_0(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #205 - 1 */
	/** 1 */

	virtual void CMD_1() { string sCMD_Result; CMD_1(sCMD_Result,NULL);};
	virtual void CMD_1(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #206 - 2 */
	/** 2 */

	virtual void CMD_2() { string sCMD_Result; CMD_2(sCMD_Result,NULL);};
	virtual void CMD_2(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #207 - 3 */
	/** 3 */

	virtual void CMD_3() { string sCMD_Result; CMD_3(sCMD_Result,NULL);};
	virtual void CMD_3(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #208 - 4 */
	/** 4 */

	virtual void CMD_4() { string sCMD_Result; CMD_4(sCMD_Result,NULL);};
	virtual void CMD_4(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #209 - 5 */
	/** 5 */

	virtual void CMD_5() { string sCMD_Result; CMD_5(sCMD_Result,NULL);};
	virtual void CMD_5(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #210 - 6 */
	/** 6 */

	virtual void CMD_6() { string sCMD_Result; CMD_6(sCMD_Result,NULL);};
	virtual void CMD_6(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #211 - 7 */
	/** 7 */

	virtual void CMD_7() { string sCMD_Result; CMD_7(sCMD_Result,NULL);};
	virtual void CMD_7(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #212 - 8 */
	/** 8 */

	virtual void CMD_8() { string sCMD_Result; CMD_8(sCMD_Result,NULL);};
	virtual void CMD_8(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #213 - 9 */
	/** 9 */

	virtual void CMD_9() { string sCMD_Result; CMD_9(sCMD_Result,NULL);};
	virtual void CMD_9(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** Navigate back .. ( Escape ) */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Back_Prior_Menu(int iStreamID) { string sCMD_Result; CMD_Back_Prior_Menu(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Back_Prior_Menu(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #409 - Save Bookmark */
	/** Bookmark the current page. */
		/** @param #17 PK_Users */
			/** The user to save this under, or 0 for public */
		/** @param #19 Data */
			/** The picture to save as the thumbnail, in jpg format.  If not specified the plugin will try to grab a frame from the media player */
		/** @param #29 PK_MediaType */
			/** The media type, if not specified it will get it from the stream */
		/** @param #45 PK_EntertainArea */
			/** The entertainment area with the media */
		/** @param #163 Description */
			/** The description for this bookmark. */
		/** @param #188 Position */
			/** The position to save as.  For TV, CHAN: or PROG: or SERIES:.  Leave blank and the plugin will grab the current value */
		/** @param #225 Always */
			/** If true, then this is the start position */

	virtual void CMD_Save_Bookmark(int iPK_Users,char *pData,int iData_Size,int iPK_MediaType,string sPK_EntertainArea,string sDescription,string sPosition,bool bAlways) { string sCMD_Result; CMD_Save_Bookmark(iPK_Users,pData,iData_Size,iPK_MediaType,sPK_EntertainArea.c_str(),sDescription.c_str(),sPosition.c_str(),bAlways,sCMD_Result,NULL);};
	virtual void CMD_Save_Bookmark(int iPK_Users,char *pData,int iData_Size,int iPK_MediaType,string sPK_EntertainArea,string sDescription,string sPosition,bool bAlways,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #606 - Reload */
	/** Reload Web Page */

	virtual void CMD_Reload() { string sCMD_Result; CMD_Reload(sCMD_Result,NULL);};
	virtual void CMD_Reload(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1099 - Load Url */
	/** the url you want loaded.  */

	virtual void CMD_Load_Url() { string sCMD_Result; CMD_Load_Url(sCMD_Result,NULL);};
	virtual void CMD_Load_Url(string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
