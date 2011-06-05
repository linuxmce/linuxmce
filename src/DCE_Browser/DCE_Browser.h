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
#include "browserwindow.h"
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


	/** @brief COMMAND: #606 - Reload */
	/** Reload Web Page */

	virtual void CMD_Reload() { string sCMD_Result; CMD_Reload(sCMD_Result,NULL);};
	virtual void CMD_Reload(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1090 - Goto Url */
	/** goto provided url */
		/** @param #193 URL */
			/** the url to go to */

	virtual void CMD_Goto_Url(string sURL) { string sCMD_Result; CMD_Goto_Url(sURL.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Goto_Url(string sURL,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1091 - Navigate Forward */
	/** Navigate Forward */

	virtual void CMD_Navigate_Forward() { string sCMD_Result; CMD_Navigate_Forward(sCMD_Result,NULL);};
	virtual void CMD_Navigate_Forward(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1092 - Navigate Back */
	/** Navigate Back */

	virtual void CMD_Navigate_Back() { string sCMD_Result; CMD_Navigate_Back(sCMD_Result,NULL);};
	virtual void CMD_Navigate_Back(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1093 - Bookmark Website */
	/** Creates bookmark for user */

	virtual void CMD_Bookmark_Website() { string sCMD_Result; CMD_Bookmark_Website(sCMD_Result,NULL);};
	virtual void CMD_Bookmark_Website(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1094 - Increase DPI */
	/** Increase font size */

	virtual void CMD_Increase_DPI() { string sCMD_Result; CMD_Increase_DPI(sCMD_Result,NULL);};
	virtual void CMD_Increase_DPI(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1095 - Reduce DPI */
	/** Decrease Font Size */

	virtual void CMD_Reduce_DPI() { string sCMD_Result; CMD_Reduce_DPI(sCMD_Result,NULL);};
	virtual void CMD_Reduce_DPI(string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
