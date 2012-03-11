/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
   Langston Ball  golgoj4@gmail.com
    QOrbiter is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QOrbiter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QOrbiter.  If not, see <http://www.gnu.org/licenses/>.
*/

//<-dceag-d-b->

#include "qOrbiter.h"
#include "datamodels/gridItem.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "datamodels/listModel.h"
#include <datamodels/avdevice.h>
#include <contextobjects/existingorbiter.h>
#include <contextobjects/modelpage.h>

#include "QBuffer"
#include "QApplication"
#ifndef ANDROID
#include <iostream>
#endif
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
qOrbiter::qOrbiter( int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter, QObject *parent)
    : qOrbiter_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
    //<-dceag-const-e->
{
    iPK_Device_DatagridPlugIn =  long(6);
    iPK_Device_OrbiterPlugin = long(9);
    iPK_Device_GeneralInfoPlugin = long(4);
    iPK_Device_SecurityPlugin = long(13);
    iPK_Device_LightingPlugin = long(8);
    m_dwIDataGridRequestCounter = 0;
    iOrbiterPluginID = 9;
    iMediaPluginID = 10;
    iPK_Device_eventPlugin = 12;
    m_pOrbiterCat = 5;

    internal_streamID = 0;

    videoDefaultSort = "13";
    audioDefaultSort = "2";
    photoDefaultSort = "13";
    gamesDefaultSort = "49";

    backwards = false;
    i_current_mediaType = 0;

    media_totalPages=0;
    media_currentPage=0;
    media_pos=0;
    media_pageSeperator = 25;

    b_mediaPlaying = false;
    m_dwPK_Device_NowPlaying = 0;
    m_dwPK_Device_NowPlaying_Video = 0;
    m_dwPK_Device_NowPlaying_Audio = 0;
    m_dwPK_Device_CaptureCard = 0;
    m_dwMaxRetries = 3;

    initializeGrid();




}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
qOrbiter::qOrbiter(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter, QObject *parent )
    : qOrbiter_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
    //<-dceag-const2-e->
{
}

//<-dceag-dest-b->
qOrbiter::~qOrbiter()
//<-dceag-dest-e->
{

    /*  char *pData; int iSize;
    pData= "NULL"; iSize = 0;

    CMD_Orbiter_Registered CMD_Orbiter_unRegistered(m_dwPK_Device, iOrbiterPluginID, "0" ,iPK_User, StringUtils::itos(1), iFK_Room, &pData, &iSize);
    SendMessage (CMD_Orbiter_unRegistered);
*/
}

//<-dceag-getconfig-b->
bool qOrbiter::GetConfig()
{
    if( !qOrbiter_Command::GetConfig() )
        return false;
    //<-dceag-getconfig-e->

    // Put your code here to initialize the data in this class
    // The configuration parameters DATA_ are now populated

    if( !coreDevices.GetConfig(m_pData) )
    {
        RouterNeedsReload();
        return false;
    }

    return true;
}


//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool qOrbiter::Register()
//<-dceag-reg-e->
{
    return Connect(PK_DeviceTemplate_get());
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
 cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You
 can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
qOrbiter_Command *Create_qOrbiter(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
    return new qOrbiter(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

/*
 When you receive commands that are destined to one of your children,
 then if that child implements DCE then there will already be a separate class
 created for the child that will get the message.  If the child does not, then you will
 get all	commands for your children in ReceivedCommandForChild, where
 pDeviceData_Base is the child device.  If you handle the message, you
 should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void qOrbiter::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void qOrbiter::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void qOrbiter::SomeFunction()
{
 // If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
 // to do all your registration, such as creating message interceptors

 // If you use an IDE with auto-complete, after you type DCE:: it should give you a list of all
 // commands and requests, including the parameters.  See "AllCommandsRequests.h"

 // Examples:

 // Send a specific the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.
 DCE::CMD_Simulate_Mouse_Click CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);
 SendCommand(CMD_Simulate_Mouse_Click);

 // Send the message to orbiters 32898 and 27283 (ie a device list, hence the _DL)
 // And we want a response, which will be "OK" if the command was successfull
 string sResponse;
 DCE::CMD_Simulate_Mouse_Click_DL CMD_Simulate_Mouse_Click_DL(m_dwPK_Device,"32898,27283",55,77)
 SendCommand(CMD_Simulate_Mouse_Click_DL,&sResponse);

 // Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
 // Note the _Cat for category
 DCE::CMD_Simulate_Mouse_Click_Cat CMD_Simulate_Mouse_Click_Cat(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77)
    SendCommand(CMD_Simulate_Mouse_Click_Cat);

 // Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
 // Note the _DT.
 DCE::CMD_Simulate_Mouse_Click_DT CMD_Simulate_Mouse_Click_DT(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);
 SendCommand(CMD_Simulate_Mouse_Click_DT);

 // This command has a normal string parameter, but also an int as an out parameter
 int iValue;
 DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength(m_dwDeviceID, DestDevice, sMac_address,&iValue);
 // This send command will wait for the destination device to respond since there is
 // an out parameter
 SendCommand(CMD_Get_Signal_Strength);

 // This time we don't care about the out parameter.  We just want the command to
 // get through, and don't want to wait for the round trip.  The out parameter, iValue,
 // will not get set
 SendCommandNoResponse(CMD_Get_Signal_Strength);

 // This command has an out parameter of a data block.  Any parameter that is a binary
 // data block is a pair of int and char *
 // We'll also want to see the response, so we'll pass a string for that too

 int iFileSize;
 char *pFileContents
 string sResponse;
 DCE::CMD_Request_File CMD_Request_File(m_dwDeviceID, DestDevice, "filename",&pFileContents,&iFileSize,&sResponse);
 SendCommand(CMD_Request_File);

 // If the device processed the command (in this case retrieved the file),
 // sResponse will be "OK", and iFileSize will be the size of the file
 // and pFileContents will be the file contents.  **NOTE**  We are responsible
 // free deleting pFileContents.


 // To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

 // Get our IP address from our data
 string sIP = DATA_Get_IP_Address();

 // Set our data "Filename" to "myfile"
 DATA_Set_Filename("myfile");

 // Fire the "Finished with file" event, which takes no parameters
 EVENT_Finished_with_file();
 // Fire the "Touch or click" which takes an X and Y parameter
 EVENT_Touch_or_click(10,150);
}
*/
//<-dceag-sample-e->

/*

 COMMANDS TO IMPLEMENT

*/

//<-dceag-c1-b->

/** @brief COMMAND: #1 - Capture Keyboard To Variable */
/** As the user types, using either the keyboard or simulate keypress commands, what he types will be stored in a variable and/or put into a text object. */
/** @param #3 PK_DesignObj */
/** The Design Object which contains the text Object */
/** @param #4 PK_Variable */
/** The variable in which to store the input */
/** @param #8 On/Off */
/** If 0, this stops capturing */
/** @param #14 Type */
/** 1=normal, 2=pin, 3=phone number */
/** @param #24 Reset */
/** if true, the next keypress will clear the variable and start new */
/** @param #25 PK_Text */
/** The text object in which to store the current input */
/** @param #55 DataGrid */
/** If 1, we'll scroll the data grid too when typing keys. */

void qOrbiter::CMD_Capture_Keyboard_To_Variable(string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid,string &sCMD_Result,Message *pMessage)
//<-dceag-c1-e->
{
    cout << "Need to implement command #1 - Capture Keyboard To Variable" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #4 - PK_Variable=" << iPK_Variable << endl;
    cout << "Parm #8 - OnOff=" << sOnOff << endl;
    cout << "Parm #14 - Type=" << sType << endl;
    cout << "Parm #24 - Reset=" << sReset << endl;
    cout << "Parm #25 - PK_Text=" << iPK_Text << endl;
    cout << "Parm #55 - DataGrid=" << bDataGrid << endl;
}

//<-dceag-c2-b->

/** @brief COMMAND: #2 - Orbiter Beep */
/** Make the orbiter beep */

void qOrbiter::CMD_Orbiter_Beep(string &sCMD_Result,Message *pMessage)
//<-dceag-c2-e->
{
    cout << "Need to implement command #2 - Orbiter Beep" << endl;
}

//<-dceag-c3-b->

/** @brief COMMAND: #3 - Display On/Off */
/** Turn the display on or off */
/** @param #8 On/Off */
/** 0=Off, 1=On */
/** @param #125 Already processed */
/** Normally Orbiter will forward the on/off through DCE so the other devices can turn on/off.  If this is true, it won't. */

void qOrbiter::CMD_Display_OnOff(string sOnOff,bool bAlready_processed,string &sCMD_Result,Message *pMessage)
//<-dceag-c3-e->
{
    cout << "Need to implement command #3 - Display On/Off" << endl;
    cout << "Parm #8 - OnOff=" << sOnOff << endl;
    cout << "Parm #125 - Already_processed=" << bAlready_processed << endl;
}

//<-dceag-c4-b->

/** @brief COMMAND: #4 - Go back */
/** Make the orbiter go back to the prior screen, like the back button in a web browser */
/** @param #16 PK_DesignObj_CurrentScreen */
/** If this is specified, the orbiter will ignore the command unless this is the current screen */
/** @param #21 Force */
/** Screens can be flagged, "Cant go back", meaning the go back will skip over them.  If Force=1, the Orbiter returns to the prior screen regardless */

void qOrbiter::CMD_Go_back(string sPK_DesignObj_CurrentScreen,string sForce,string &sCMD_Result,Message *pMessage)
//<-dceag-c4-e->
{
    cout << "Need to implement command #4 - Go back" << endl;
    cout << "Parm #16 - PK_DesignObj_CurrentScreen=" << sPK_DesignObj_CurrentScreen << endl;
    cout << "Parm #21 - Force=" << sForce << endl;
}

//<-dceag-c5-b->

/** @brief COMMAND: #5 - Goto DesignObj */
/** Goto a specific design obj */
/** @param #2 PK_Device */
/** For this screen only, override the normal "control device" stored in variable #1, and treat this device as the control screen.  When the screen changes, it will be reset */
/** @param #3 PK_DesignObj */
/** The screen to go to.  Can be be fully qualified (x.y.z), or just contain the screen # */
/** @param #10 ID */
/** Assigns an optional ID to this particular "viewing" of the screen, used with Kill Screen.  There can be lots of instances of the same screen in the history queue (such as call in progress).  This allows a program to pop a particular one out of the queue. */
/** @param #16 PK_DesignObj_CurrentScreen */
/** If this is specified, the orbiter will ignore the command unless this is the current screen.  If this is -1, that will match a main menu or screen saver (ie the Orbiter is not in use). */
/** @param #22 Store Variables */
/** If 1, the Orbiter will store the current variable values, and restore them if a 'go back' causes it to return to this screen */
/** @param #114 Cant Go Back */
/** If true, then when this screen goes away the user won't be able to return to it -- it will be skipped over, unless Go Back with Force=1 is used.  This prevents layers of popup screens. */

void qOrbiter::CMD_Goto_DesignObj(int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables,bool bCant_Go_Back,string &sCMD_Result,Message *pMessage)
//<-dceag-c5-e->
{
    cout << "Need to implement command #5 - Goto DesignObj" << endl;
    cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #10 - ID=" << sID << endl;
    cout << "Parm #16 - PK_DesignObj_CurrentScreen=" << sPK_DesignObj_CurrentScreen << endl;
    cout << "Parm #22 - Store_Variables=" << bStore_Variables << endl;
    cout << "Parm #114 - Cant_Go_Back=" << bCant_Go_Back << endl;
}

//<-dceag-c6-b->

/** @brief COMMAND: #6 - Show Object */
/** Change an objects visible state. */
/** @param #3 PK_DesignObj */
/** The object to show or hide */
/** @param #4 PK_Variable */
/** The variable to use in the comparisson.  See Comparisson Value. */
/** @param #6 Comparisson Operator */
/** A type of comparisson: =  <  <>  !=  > */
/** @param #7 Comparisson Value */
/** If a Variable, Comparisson Type, and Comparisson Value are specified, the command will be ignored if the comparisson is not true */
/** @param #8 On/Off */
/** 1=show object, 0=hide object */

void qOrbiter::CMD_Show_Object(string sPK_DesignObj,int iPK_Variable,string sComparisson_Operator,string sComparisson_Value,string sOnOff,string &sCMD_Result,Message *pMessage)
//<-dceag-c6-e->
{
    cout << "Need to implement command #6 - Show Object" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #4 - PK_Variable=" << iPK_Variable << endl;
    cout << "Parm #6 - Comparisson_Operator=" << sComparisson_Operator << endl;
    cout << "Parm #7 - Comparisson_Value=" << sComparisson_Value << endl;
    cout << "Parm #8 - OnOff=" << sOnOff << endl;
}

//<-dceag-c7-b->

/** @brief COMMAND: #7 - Terminate Orbiter */
/** Causes the Orbiter application to exit */

void qOrbiter::CMD_Terminate_Orbiter(string &sCMD_Result,Message *pMessage)
//<-dceag-c7-e->
{
    cout << "Need to implement command #7 - Terminate Orbiter" << endl;
}

//<-dceag-c8-b->

/** @brief COMMAND: #8 - Remove Screen From History */
/** The orbiter keeps a history of visible screens, allowing the user to go back.  See Go_Back.  This removes screens from the queue that should not available anymore.  An example is when a call comes in, the controllers are sent to an incoming call screen. */
/** @param #10 ID */
/** If specified, only screens that match this ID will be removed */
/** @param #159 PK_Screen */
/** The screen to remove */

void qOrbiter::CMD_Remove_Screen_From_History(string sID,int iPK_Screen,string &sCMD_Result,Message *pMessage)
//<-dceag-c8-e->
{
    cout << "Need to implement command #8 - Remove Screen From History" << endl;
    cout << "Parm #10 - ID=" << sID << endl;
    cout << "Parm #159 - PK_Screen=" << iPK_Screen << endl;
}

//<-dceag-c9-b->

/** @brief COMMAND: #9 - Scroll Grid */
/** Scroll a datagrid */
/** @param #1 Relative Level */
/** The grid will scroll this many lines.  If prefaced with a P, it will scroll this many pages.  If not specified, it will scroll 1 page. */
/** @param #3 PK_DesignObj */
/** The grid to scroll.  If not specified, any currently visible grids will scroll */
/** @param #30 PK_Direction */
/** The direction to scroll the grid */

void qOrbiter::CMD_Scroll_Grid(string sRelative_Level,string sPK_DesignObj,int iPK_Direction,string &sCMD_Result,Message *pMessage)
//<-dceag-c9-e->
{
    cout << "Need to implement command #9 - Scroll Grid" << endl;
    cout << "Parm #1 - Relative_Level=" << sRelative_Level << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #30 - PK_Direction=" << iPK_Direction << endl;
}

//<-dceag-c10-b->

/** @brief COMMAND: #10 - Move Highlight */
/** Move the current highlight pointer */
/** @param #1 Relative Level */
/** The grid will scroll this many lines.  If prefaced with a P, it will scroll this many pages.  If not specified, it will scroll 1 page. */
/** @param #3 PK_DesignObj */
/** The grid to scroll.  If not specified, any currently visible grids will scroll */
/** @param #30 PK_Direction */
/** The direction to move the highlight */

void qOrbiter::CMD_Move_Highlight(string sRelative_Level,string sPK_DesignObj,int iPK_Direction,string &sCMD_Result,Message *pMessage)
//<-dceag-c10-e->
{
    cout << "Need to implement command #10 - Move Highlight" << endl;
    cout << "Parm #1 - Relative_Level=" << sRelative_Level << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #30 - PK_Direction=" << iPK_Direction << endl;
}

//<-dceag-c13-b->

/** @brief COMMAND: #13 - Play Sound */
/** Plays a sound file on the orbiter */
/** @param #19 Data */
/** A pointer to a block of memory representing the sound file to play */
/** @param #20 Format */
/** Indicates what type of data is in the memory block.  1=wav, 2=mp3 */

void qOrbiter::CMD_Play_Sound(char *pData,int iData_Size,string sFormat,string &sCMD_Result,Message *pMessage)
//<-dceag-c13-e->
{
    cout << "Need to implement command #13 - Play Sound" << endl;
    cout << "Parm #19 - Data  (data value)" << endl;
    cout << "Parm #20 - Format=" << sFormat << endl;
}

//<-dceag-c14-b->

/** @brief COMMAND: #14 - Refresh */
/** Invalidates and redraws the current screen, optionally re-requesting data from a datagrid.  The OnLoad commands are not fired.  See Regen Screen. */
/** @param #15 DataGrid ID */
/** Normally refresh does not cause the orbiter to re-request data.  But if a specific grid ID is specified, that grid will be refreshed.  Specify * to re-request all grids on the current screen */

void qOrbiter::CMD_Refresh(string sDataGrid_ID,string &sCMD_Result,Message *pMessage)
//<-dceag-c14-e->
{
    cout << "Need to implement command #14 - Refresh" << endl;
    cout << "Parm #15 - DataGrid_ID=" << sDataGrid_ID << endl;
}

//<-dceag-c15-b->

/** @brief COMMAND: #15 - Regen Screen */
/** The screen is reloaded like the user was going to it for the first time.  The OnUnload and OnLoad commands are fired. */

void qOrbiter::CMD_Regen_Screen(string &sCMD_Result,Message *pMessage)
//<-dceag-c15-e->
{
    cout << "Need to implement command #15 - Regen Screen" << endl;
}

//<-dceag-c16-b->

/** @brief COMMAND: #16 - Requires Special Handling */
/** When a button needs to do something too sophisticated for a normal command, attach this command.  When the controller sees it, it will pass execution to a local handler that must be added to the Orbiter's code. */

void qOrbiter::CMD_Requires_Special_Handling(string &sCMD_Result,Message *pMessage)
//<-dceag-c16-e->
{
    cout << "Need to implement command #16 - Requires Special Handling" << endl;
}

//<-dceag-c17-b->

/** @brief COMMAND: #17 - Seek Data Grid */
/** Causes a datagrid to seek to a particular position. */
/** @param #9 Text */
/** If specified, the orbiter will jump to the first row which has a cell that starts with this text.  Specify Position X to use a column other than the first one. */
/** @param #11 Position X */
/** The column to jump to.  If Text is not blank, the column to search. */
/** @param #12 Position Y */
/** The row to jump to.  Ignored if Text is not blank */
/** @param #15 DataGrid ID */
/** The datagrid to scroll.  If not specified, the first visible one will be used */

void qOrbiter::CMD_Seek_Data_Grid(string sText,int iPosition_X,int iPosition_Y,string sDataGrid_ID,string &sCMD_Result,Message *pMessage)
//<-dceag-c17-e->
{
    cout << "Need to implement command #17 - Seek Data Grid" << endl;
    cout << "Parm #9 - Text=" << sText << endl;
    cout << "Parm #11 - Position_X=" << iPosition_X << endl;
    cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
    cout << "Parm #15 - DataGrid_ID=" << sDataGrid_ID << endl;
}

//<-dceag-c18-b->

/** @brief COMMAND: #18 - Set Graphic To Display */
/** All objects on screen can be either in "Normal" mode, "Selected mode", "Highlighted mode", or any number of "Alternate modes".  These are like "views".  A Selected mode may appear depressed, for example.  All children of this object will also be set. */
/** @param #3 PK_DesignObj */
/** The object to set */
/** @param #10 ID */
/** 0=standard mode, -1=selected -2=highlight a positive number is one of the alternates */

void qOrbiter::CMD_Set_Graphic_To_Display(string sPK_DesignObj,string sID,string &sCMD_Result,Message *pMessage)
//<-dceag-c18-e->
{
    cout << "Need to implement command #18 - Set Graphic To Display" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #10 - ID=" << sID << endl;
}

//<-dceag-c20-b->

/** @brief COMMAND: #20 - Set Object Parameter */
/** changes one of the object's DesignObjParameters */
/** @param #3 PK_DesignObj */
/** The object to change */
/** @param #5 Value To Assign */
/** The value to assign */
/** @param #27 PK_DesignObjParameter */
/** The parameter */

void qOrbiter::CMD_Set_Object_Parameter(string sPK_DesignObj,string sValue_To_Assign,int iPK_DesignObjParameter,string &sCMD_Result,Message *pMessage)
//<-dceag-c20-e->
{
    cout << "Need to implement command #20 - Set Object Parameter" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
    cout << "Parm #27 - PK_DesignObjParameter=" << iPK_DesignObjParameter << endl;
}

//<-dceag-c21-b->

/** @brief COMMAND: #21 - Set Object Position */
/** Change an objects's position on the screen */
/** @param #3 PK_DesignObj */
/** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
/** @param #11 Position X */
/**  */
/** @param #12 Position Y */
/**  */

void qOrbiter::CMD_Set_Object_Position(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
//<-dceag-c21-e->
{
    cout << "Need to implement command #21 - Set Object Position" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #11 - Position_X=" << iPosition_X << endl;
    cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
}

//<-dceag-c22-b->

/** @brief COMMAND: #22 - Set Object Size */
/** Change an object's size */
/** @param #3 PK_DesignObj */
/** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
/** @param #11 Position X */
/**  */
/** @param #12 Position Y */
/**  */

void qOrbiter::CMD_Set_Object_Size(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
//<-dceag-c22-e->
{
    cout << "Need to implement command #22 - Set Object Size" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #11 - Position_X=" << iPosition_X << endl;
    cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
}

//<-dceag-c23-b->

/** @brief COMMAND: #23 - Set Pos Rel To Parent */
/** Like Set Object Position, but the X and Y coordinates are assumed to be relative to the parent rather than absolute */
/** @param #3 PK_DesignObj */
/** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
/** @param #11 Position X */
/**  */
/** @param #12 Position Y */
/**  */

void qOrbiter::CMD_Set_Pos_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
//<-dceag-c23-e->
{
    cout << "Need to implement command #23 - Set Pos Rel To Parent" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #11 - Position_X=" << iPosition_X << endl;
    cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
}

//<-dceag-c24-b->

/** @brief COMMAND: #24 - Set Size Rel To Parent */
/** Change an object's size, relative to it's parent object */
/** @param #3 PK_DesignObj */
/** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
/** @param #11 Position X */
/** The percentage of the parent object's width.  100=the parent's full width. */
/** @param #12 Position Y */
/** The percentage of the parent object's height.  100=the parent's full height. */

void qOrbiter::CMD_Set_Size_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
//<-dceag-c24-e->
{
    cout << "Need to implement command #24 - Set Size Rel To Parent" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #11 - Position_X=" << iPosition_X << endl;
    cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
}

//<-dceag-c25-b->

/** @brief COMMAND: #25 - Set Text */
/** Change the text within a text object on the fly */
/** @param #3 PK_DesignObj */
/** The Design Object which contains the text object.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
/** @param #9 Text */
/** The text to assign */
/** @param #25 PK_Text */
/** The text object in which to store the current input */

void qOrbiter::CMD_Set_Text(string sPK_DesignObj,string sText,int iPK_Text,string &sCMD_Result,Message *pMessage)
//<-dceag-c25-e->
{
    //   cout << "Need to implement command #25 - Set Text" << endl;
    //   cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    //  cout << "Parm #9 - Text=" << sText << endl;
    //  cout << "Parm #25 - PK_Text=" << iPK_Text << endl;
    emit statusMessage(QString::fromStdString(sText));
}

//<-dceag-c26-b->

/** @brief COMMAND: #26 - Set Bound Icon */
/** Sets an icon that is bound to status.  "Bind Icon" is put in the object's on load commands, and then this command sets the status at runtime. */
/** @param #5 Value To Assign */
/** The value corresponding to an alt graphic. */
/** @param #9 Text */
/** Text for the binding */
/** @param #14 Type */
/** The type of bound icon. */

void qOrbiter::CMD_Set_Bound_Icon(string sValue_To_Assign,string sText,string sType,string &sCMD_Result,Message *pMessage)
//<-dceag-c26-e->
{
    /*
    cout << "Need to implement command #26 - Set Bound Icon" << endl;
    cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
    cout << "Parm #9 - Text=" << sText << endl;
    cout << "Parm #14 - Type=" << sType << endl;
    */

}

//<-dceag-c27-b->

/** @brief COMMAND: #27 - Set Variable */
/** Change the value of a variable */
/** @param #4 PK_Variable */
/** The variable to change */
/** @param #5 Value To Assign */
/** The value to assign */

void qOrbiter::CMD_Set_Variable(int iPK_Variable,string sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c27-e->
{
    cout << "Need to implement command #27 - Set Variable" << endl;
    cout << "Parm #4 - PK_Variable=" << iPK_Variable << endl;
    cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
    //emit statusMessage(QString::fromStdString(sValue_To_Assign));
}

//<-dceag-c28-b->

/** @brief COMMAND: #28 - Simulate Keypress */
/** Simulates that a key has been touched.  Touchable keys on screen can use this command to allow for simultaneous operation with keyboard or mouse.  Also works with the "Capture Keyboard to Variable" command. */
/** @param #26 PK_Button */
/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
/** @param #41 StreamID */
/** ID of stream to apply */
/** @param #50 Name */
/** The application to send the keypress to. If not specified, it goes to the DCE device. */

void qOrbiter::CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
    cout << "Need to implement command #28 - Simulate Keypress" << endl;
    cout << "Parm #26 - PK_Button=" << sPK_Button << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
    cout << "Parm #50 - Name=" << sName << endl;
}

//<-dceag-c29-b->

/** @brief COMMAND: #29 - Simulate Mouse Click */
/** Simulates a mouse click or touch on the indicated x & y coordinates */
/** @param #11 Position X */
/** position X */
/** @param #12 Position Y */
/** position Y */
/** @param #41 StreamID */
/** ID of stream to apply */

void qOrbiter::CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c29-e->
{
    cout << "Need to implement command #29 - Simulate Mouse Click" << endl;
    cout << "Parm #11 - Position_X=" << iPosition_X << endl;
    cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c30-b->

/** @brief COMMAND: #30 - Stop Sound */
/** If a sound file is being played, it will be stopped. */

void qOrbiter::CMD_Stop_Sound(string &sCMD_Result,Message *pMessage)
//<-dceag-c30-e->
{
    cout << "Need to implement command #30 - Stop Sound" << endl;
}

//<-dceag-c31-b->

/** @brief COMMAND: #31 - Store Variables */
/** The orbiter will store a snapshot of the variables at this moment, and if the user returns to this screen with a go back, it will restore the variables to this value. */

void qOrbiter::CMD_Store_Variables(string &sCMD_Result,Message *pMessage)
//<-dceag-c31-e->
{
    cout << "Need to implement command #31 - Store Variables" << endl;
}

//<-dceag-c32-b->

/** @brief COMMAND: #32 - Update Object Image */
/** Changes the background image within an object */
/** @param #3 PK_DesignObj */
/** The object in which to put the bitmap */
/** @param #14 Type */
/** 1=bmp, 2=jpg, 3=png */
/** @param #19 Data */
/** The contents of the bitmap, like reading from the file into a memory buffer */
/** @param #23 Disable Aspect Lock */
/** If 1, the image will be stretched to fit the object */

void qOrbiter::CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage)
//<-dceag-c32-e->
{
    /*
    cout << "Need to implement command #32 - Update Object Image" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #14 - Type=" << sType << endl;
    cout << "Parm #19 - Data  (data value)" << endl;
    cout << "Parm #23 - Disable_Aspect_Lock=" << sDisable_Aspect_Lock << endl;
*/

    // emit statusMessage("Incoming" + QString::fromStdString(sType));
    // emit statusMessage("Update Image detected. Size:" + QString::number(iData_Size));
    const uchar *data = (uchar*)pData;
    emit objectUpdate(data, iData_Size);

}

//<-dceag-c58-b->

/** @brief COMMAND: #58 - Set Current User */
/** Sets what user is currently using the orbiter. */
/** @param #17 PK_Users */
/** The user currently using the orbiter. */

void qOrbiter::CMD_Set_Current_User(int iPK_Users,string &sCMD_Result,Message *pMessage)
//<-dceag-c58-e->
{
    cout << "Need to implement command #58 - Set Current User" << endl;
    cout << "Parm #17 - PK_Users=" << iPK_Users << endl;
}

//<-dceag-c59-b->

/** @brief COMMAND: #59 - Set Entertainment Area */
/** If you don't know the location, you can also set just the entertainment area */
/** @param #45 PK_EntertainArea */
/** The current entertainment area where the orbiter is. */

void qOrbiter::CMD_Set_Entertainment_Area(string sPK_EntertainArea,string &sCMD_Result,Message *pMessage)
//<-dceag-c59-e->
{
    cout << "Need to implement command #59 - Set Entertainment Area" << endl;
    cout << "Parm #45 - PK_EntertainArea=" << sPK_EntertainArea << endl;
}

//<-dceag-c66-b->

/** @brief COMMAND: #66 - Select Object */
/** The same as clicking on an object. */
/** @param #3 PK_DesignObj */
/** The object to select. */
/** @param #16 PK_DesignObj_CurrentScreen */
/** Will only happen if this is the current screen. */
/** @param #102 Time */
/** If specified, rather than happening immediately it will happen in x seconds. */

void qOrbiter::CMD_Select_Object(string sPK_DesignObj,string sPK_DesignObj_CurrentScreen,string sTime,string &sCMD_Result,Message *pMessage)
//<-dceag-c66-e->
{
    cout << "Need to implement command #66 - Select Object" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #16 - PK_DesignObj_CurrentScreen=" << sPK_DesignObj_CurrentScreen << endl;
    cout << "Parm #102 - Time=" << sTime << endl;
}

//<-dceag-c67-b->

/** @brief COMMAND: #67 - Spawn Application */
/** Spawn the given application.  Mainly used for windows orbiters. */
/** @param #13 Filename */
/** The name of the executable file to spawn */
/** @param #50 Name */
/** A name that we'll remember the application by for future kill commands */
/** @param #51 Arguments */
/** Command arguments, tab delimited */
/** @param #94 SendOnFailure */
/** Send this messages if the process exited with failure error code. */
/** @param #95 SendOnSuccess */
/** Send this messages if the process exited with success error code. */
/** @param #115 Show logo */
/** If this is set then we will first select the logo  before spawning the application. */
/** @param #120 Retransmit */
/** If false, and if Exclusive is true and another instance is killed, the 'send messages on termination' will not be sent. */
/** @param #126 Exclusive */
/** If true, then kill other apps with this same name */
/** @param #241 Detach */
/** Detach application after spawning / Don't kill this app on reload. */

void qOrbiter::CMD_Spawn_Application(string sFilename,string sName,string sArguments,string sSendOnFailure,string sSendOnSuccess,bool bShow_logo,bool bRetransmit,bool bExclusive,bool bDetach,string &sCMD_Result,Message *pMessage)
//<-dceag-c67-e->
{
    cout << "Need to implement command #67 - Spawn Application" << endl;
    cout << "Parm #13 - Filename=" << sFilename << endl;
    cout << "Parm #50 - Name=" << sName << endl;
    cout << "Parm #51 - Arguments=" << sArguments << endl;
    cout << "Parm #94 - SendOnFailure=" << sSendOnFailure << endl;
    cout << "Parm #95 - SendOnSuccess=" << sSendOnSuccess << endl;
    cout << "Parm #115 - Show_logo=" << bShow_logo << endl;
    cout << "Parm #120 - Retransmit=" << bRetransmit << endl;
    cout << "Parm #126 - Exclusive=" << bExclusive << endl;
    cout << "Parm #241 - Detach=" << bDetach << endl;
}

//<-dceag-c72-b->

/** @brief COMMAND: #72 - Surrender to OS */
/** Let the O/S take over.  This is useful with the Orbiter running on the media director's desktop as a full screen app, and media is inserted, or the user starts a computer application on the mobile phone.  The orbiter will then let the other application ta */
/** @param #8 On/Off */
/** 1=Hide and let the OS take over.  0=The orbiter comes up again. */
/** @param #54 Fully release keyboard */
/** Only applies if on/off is 1.  If this is false, the orbiter will still filter keystrokes, looking for macros to implement, and only pass on keys that it doesn't catch.  If true, it will pass all keys.  True also releases the mouse. */
/** @param #225 Always */
/** If true, the mouse will always be ignored */

void qOrbiter::CMD_Surrender_to_OS(string sOnOff,bool bFully_release_keyboard,bool bAlways,string &sCMD_Result,Message *pMessage)
//<-dceag-c72-e->
{
    cout << "Need to implement command #72 - Surrender to OS" << endl;
    cout << "Parm #8 - OnOff=" << sOnOff << endl;
    cout << "Parm #54 - Fully_release_keyboard=" << bFully_release_keyboard << endl;
    cout << "Parm #225 - Always=" << bAlways << endl;
}

//<-dceag-c77-b->

/** @brief COMMAND: #77 - Set Current Room */
/** If you don't know the location, you can also set just the room */
/** @param #57 PK_Room */
/** The room */

void qOrbiter::CMD_Set_Current_Room(int iPK_Room,string &sCMD_Result,Message *pMessage)
//<-dceag-c77-e->
{
    cout << "Need to implement command #77 - Set Current Room" << endl;
    cout << "Parm #57 - PK_Room=" << iPK_Room << endl;

    // CMD_Set_Current_Room_DL setRoom((long)m_dwPK_Device, "9", iPK_Room);
    //  SendCommand(setRoom);

}

//<-dceag-c85-b->

/** @brief COMMAND: #85 - Reset Highlight */
/** Resets the currently highlighted object.  Do this when you hide or unhide blocks that have tab stops. */
/** @param #3 PK_DesignObj */
/** If specified, this object will be highlighted.  Otherwise the first detected one. */

void qOrbiter::CMD_Reset_Highlight(string sPK_DesignObj,string &sCMD_Result,Message *pMessage)
//<-dceag-c85-e->
{
    cout << "Need to implement command #85 - Reset Highlight" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
}

//<-dceag-c88-b->

/** @brief COMMAND: #88 - Set Current Location */
/** Sets the location the orbiter is in.  The location is a combination of room and entertainment area. */
/** @param #65 LocationID */
/** The location ID is a sequential number created by OrbiterGen which defines a combination of room and entertainment area. */

void qOrbiter::CMD_Set_Current_Location(int iLocationID,string &sCMD_Result,Message *pMessage)
//<-dceag-c88-e->
{
    cout << "Need to implement command #88 - Set Current Location" << endl;
    cout << "Parm #65 - LocationID=" << iLocationID << endl;
}

//<-dceag-c192-b->

/** @brief COMMAND: #192 - On */
/** Turn the device on */
/** @param #97 PK_Pipe */
/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
/** @param #98 PK_Device_Pipes */
/** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

void qOrbiter::CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage)
//<-dceag-c192-e->
{
    cout << "Need to implement command #192 - On" << endl;
    cout << "Parm #97 - PK_Pipe=" << iPK_Pipe << endl;
    cout << "Parm #98 - PK_Device_Pipes=" << sPK_Device_Pipes << endl;
}

//<-dceag-c193-b->

/** @brief COMMAND: #193 - Off */
/** Turn the device off */
/** @param #97 PK_Pipe */
/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

void qOrbiter::CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage)
//<-dceag-c193-e->
{
    cout << "Need to implement command #193 - Off" << endl;
    cout << "Parm #97 - PK_Pipe=" << iPK_Pipe << endl;
}

//<-dceag-c238-b->

/** @brief COMMAND: #238 - Continuous Refresh */
/** Continuously refresh the current page.  Used when the page contains constantly changing data. */
/** @param #102 Time */
/** The interval time in seconds */

void qOrbiter::CMD_Continuous_Refresh(string sTime,string &sCMD_Result,Message *pMessage)
//<-dceag-c238-e->
{
    cout << "Need to implement command #238 - Continuous Refresh" << endl;
    cout << "Parm #102 - Time=" << sTime << endl;
}

//<-dceag-c242-b->

/** @brief COMMAND: #242 - Set Now Playing */
/** Used by the media engine to set the "now playing" text on an orbiter.  If the orbiter is bound to the remote for an entertainment area it will get more updates than just media,  like cover art, but this is the basic information that is visible on screens */
/** @param #3 PK_DesignObj */
/** 4 comma delimited objects: normal remote, popup remote, file list remote, popup file list remote, guide */
/** @param #5 Value To Assign */
/** The description of the media */
/** @param #9 Text */
/** The description of the current section (ie chapter in a dvd, etc.) */
/** @param #29 PK_MediaType */
/** The type of media playing */
/** @param #41 StreamID */
/** The ID of the current stream */
/** @param #48 Value */
/** The track number or position in the playlist */
/** @param #50 Name */
/** The name of the window for the application to remain in the foreground */
/** @param #103 List PK Device */
/** (comma-delimited list): The current source device, video device, the current audio device, 1/0 if audio device supports discrete volume */
/** @param #120 Retransmit */
/** If true, it will re-request the plist (current playlist) grid */

void qOrbiter::CMD_Set_Now_Playing(string sPK_DesignObj,string sValue_To_Assign,string sText,int iPK_MediaType,int iStreamID,int iValue,string sName,string sList_PK_Device,bool bRetransmit,string &sCMD_Result,Message *pMessage)
//<-dceag-c242-e->
{

    cout << "Need to implement command #242 - Set Now Playing" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
    cout << "Parm #9 - Text=" << sText << endl;
    cout << "Parm #29 - PK_MediaType=" << iPK_MediaType << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
    cout << "Parm #48 - Value=" << iValue << endl;
    cout << "Parm #50 - Name=" << sName << endl;
    cout << "Parm #103 - List_PK_Device=" << sList_PK_Device << endl;
    cout << "Parm #120 - Retransmit=" << bRetransmit << endl;

    i_current_mediaType = iPK_MediaType;
    string::size_type pos=0;
    m_dwPK_Device_NowPlaying = atoi(StringUtils::Tokenize(sList_PK_Device,",",pos).c_str());
    m_dwPK_Device_NowPlaying_Video = atoi(StringUtils::Tokenize(sList_PK_Device,",",pos).c_str());
    m_dwPK_Device_NowPlaying_Audio = atoi(StringUtils::Tokenize(sList_PK_Device,",",pos).c_str());
    m_dwPK_Device_CaptureCard = atoi(StringUtils::Tokenize(sList_PK_Device,",",pos).c_str());

    QString scrn = sPK_DesignObj.c_str();
    int pos1 = scrn.indexOf(",");
    scrn.remove(pos1, scrn.length());
    // qDebug() << sValue_To_Assign.c_str();



    if (iPK_MediaType == 0)
    {
        emit resetNowPlaying();
        emit setNowPlaying(false);
        emit gotoQml("Screen_1.qml");
        b_mediaPlaying = false;
        BindMediaRemote(false);
        emit currentScreenChanged("Screen_1.qml");
        currentScreen = "Screen_1.qml";
        internal_streamID = iStreamID;
        emit streamIdChanged(iStreamID);
        emit mediaTypeChanged(iPK_MediaType);
    }
    else
    {

        emit setNowPlaying(true);
        emit currentScreenChanged("Screen_"+scrn+".qml");
        currentScreen = "Screen_"+scrn+".qml";
        emit subtitleChanged((QString::fromStdString(sText)));
        emit streamIdChanged(iStreamID);
        emit subtitleChanged(QString::fromStdString(sText));
        internal_streamID = iStreamID;
        emit mediaTypeChanged(iPK_MediaType);
        emit np_playlistIndexChanged(iValue);
        QString md1 = QString::fromStdString(sValue_To_Assign);
        QStringList mdlist;
        mdlist= md1.split(QRegExp("\\n"), QString::SkipEmptyParts);
        QString port = QString::fromStdString(GetCurrentDeviceData(m_dwPK_Device_NowPlaying, 171));
        emit newTCport(port.toInt());

        if (mdlist.count() == 2)
        {
            emit np_title1Changed(mdlist.at(0));
            emit np_title2Changed(mdlist.at(1));
            //nowPlayingButton->setTitle(mdlist.at(0));
            // nowPlayingButton->setTitle2(mdlist.at(1));
        }
        else
        {
            emit np_title1Changed(QString::fromStdString(sValue_To_Assign));
            emit np_title2Changed("");
        }

    }

    if(iPK_MediaType ==1)
    {
        //channel ID will be sent to the playlist parser to find our position of the current data
        emit np_channelID(QString::number(iValue));
        emit np_network(QString::fromStdString(sValue_To_Assign));
        //the remaining two signals are sent to the now playing class immediatly
        emit np_program(QString::fromStdString(sText));
        emit mythTvUpdate("i"+QString::number(iValue));

        if(bRetransmit == 0 )
        {
            emit epgDone();
        }
        b_mediaPlaying = true;
    }
    else if(iPK_MediaType== 11)
    {
        emit np_channelID(QString::number(iValue));
        emit np_network(QString::fromStdString(sValue_To_Assign));
        if(bRetransmit == 0 )
        {
            livetvDone();
        }
        b_mediaPlaying = true;

    }
    else if(iPK_MediaType == 5)
    {
        if(bRetransmit == 1)
        {
            emit clearPlaylist();
        }
        emit np_title1Changed(QString::fromStdString(sValue_To_Assign ));
        emit np_title2Changed(QString::fromStdString(sText));
        b_mediaPlaying = true;
        emit playlistPositionChanged(iValue);
        GetNowPlayingAttributes();
    }
    else
    {
        GetNowPlayingAttributes();
        emit playlistPositionChanged(iValue);
        if(bRetransmit == 1)
        {
            emit clearPlaylist();
        }

    }

}

//<-dceag-c254-b->

/** @brief COMMAND: #254 - Bind Icon */
/** Used to make a button have an icon that reflects a current state, such as the user's status, the house mode, etc.  This is accomplished by creating an object with multiple alternate versions, and then executing a "Set  Bound Icon" to select the right one. */
/** @param #3 PK_DesignObj */
/** The object which contains the icon, or whose child objects contain the icon. */
/** @param #14 Type */
/** The type of binding, like "housemode", "userstatus_39288", etc. */
/** @param #104 Child */
/** If true, it will set the property for the child object(s), rather than the designated object. */

void qOrbiter::CMD_Bind_Icon(string sPK_DesignObj,string sType,bool bChild,string &sCMD_Result,Message *pMessage)
//<-dceag-c254-e->
{
    cout << "Need to implement command #254 - Bind Icon" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #14 - Type=" << sType << endl;
    cout << "Parm #104 - Child=" << bChild << endl;
}

//<-dceag-c258-b->

/** @brief COMMAND: #258 - Clear Selected Devices */
/** Floorplans, in particular, rely on a vector of selected devices, allowing the user to select more than one.  This command clears that list, removing any selected devices.  It can optionally cause the Object passed in as a parameter to be refreshed. */
/** @param #3 PK_DesignObj */
/** If specified, the object referenced here will be invalidated and redrawn. */

void qOrbiter::CMD_Clear_Selected_Devices(string sPK_DesignObj,string &sCMD_Result,Message *pMessage)
//<-dceag-c258-e->
{
    cout << "Need to implement command #258 - Clear Selected Devices" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
}

//<-dceag-c260-b->

/** @brief COMMAND: #260 - Set Main Menu */
/** Sets the Main Menu to 'Normal', 'Sleeping' or 'Screen Saver', optionally assigning new screens to those values. */
/** @param #9 Text */
/** Can be N, S, or V to set to the Normal, Sleeping, or Screen Saver, optionally followed by an = to assign a new screen to that menu.  e.g. N or N=1872.0.0 */

void qOrbiter::CMD_Set_Main_Menu(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c260-e->
{
    cout << "Need to implement command #260 - Set Main Menu" << endl;
    cout << "Parm #9 - Text=" << sText << endl;
}

//<-dceag-c265-b->

/** @brief COMMAND: #265 - Quit */
/** Exits the orbiter application */

void qOrbiter::CMD_Quit(string &sCMD_Result,Message *pMessage)
//<-dceag-c265-e->
{
    cout << "Need to implement command #265 - Quit" << endl;
}

//<-dceag-c324-b->

/** @brief COMMAND: #324 - Set Timeout */
/** Specifies when a given screen will timeout, executing the timeout actions.  This will also reset a pending timeout */
/** @param #3 PK_DesignObj */
/** The screen to set the timeout on.  If blank the current screen. */
/** @param #102 Time */
/** The timeout in seconds.  0 or blank means no timeout. */

void qOrbiter::CMD_Set_Timeout(string sPK_DesignObj,string sTime,string &sCMD_Result,Message *pMessage)
//<-dceag-c324-e->
{
    cout << "Need to implement command #324 - Set Timeout" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #102 - Time=" << sTime << endl;
}

//<-dceag-c325-b->

/** @brief COMMAND: #325 - Keep Screen On */
/** Allow or don't allow the screen to blank with the screen saver. */
/** @param #8 On/Off */
/** If other than "0", the screen saver will be disabled. */

void qOrbiter::CMD_Keep_Screen_On(string sOnOff,string &sCMD_Result,Message *pMessage)
//<-dceag-c325-e->
{
    cout << "Need to implement command #325 - Keep Screen On" << endl;
    cout << "Parm #8 - OnOff=" << sOnOff << endl;
}

//<-dceag-c330-b->

/** @brief COMMAND: #330 - Set Mouse Pointer Over Object */
/** Positions the on-screen mouse pointer centered over a certain object */
/** @param #3 PK_DesignObj */
/** The object to center the mouse over. */

void qOrbiter::CMD_Set_Mouse_Pointer_Over_Object(string sPK_DesignObj,string &sCMD_Result,Message *pMessage)
//<-dceag-c330-e->
{
    cout << "Need to implement command #330 - Set Mouse Pointer Over Object" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
}

//<-dceag-c354-b->

/** @brief COMMAND: #354 - Show Mouse Pointer */
/** Shows or hides the mouse pointer. */
/** @param #8 On/Off */
/** 1=show it, 0=hide it. */

void qOrbiter::CMD_Show_Mouse_Pointer(string sOnOff,string &sCMD_Result,Message *pMessage)
//<-dceag-c354-e->
{
    cout << "Need to implement command #354 - Show Mouse Pointer" << endl;
    cout << "Parm #8 - OnOff=" << sOnOff << endl;
}

//<-dceag-c366-b->

/** @brief COMMAND: #366 - Activate Window */
/** ActivateApplication - Used by Linux On Screeen Orbiters only */
/** @param #50 Name */
/** Name as known by ratpoison. */

void qOrbiter::CMD_Activate_Window(string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c366-e->
{
    cout << "Need to implement command #366 - Activate Window" << endl;
    cout << "Parm #50 - Name=" << sName << endl;
}

//<-dceag-c389-b->

/** @brief COMMAND: #389 - Send Message */
/** Sends a message stored in a parameter as a text object. */
/** @param #9 Text */
/** The message in command line-style format */
/** @param #144 Go Back */
/** Go back after sending the command if it does not contain another goto screen or go back */

void qOrbiter::CMD_Send_Message(string sText,bool bGo_Back,string &sCMD_Result,Message *pMessage)
//<-dceag-c389-e->
{
    cout << "Need to implement command #389 - Send Message" << endl;
    cout << "Parm #9 - Text=" << sText << endl;
    cout << "Parm #144 - Go_Back=" << bGo_Back << endl;
}

//<-dceag-c397-b->

/** @brief COMMAND: #397 - Show Popup */
/** Shows a screen as a popup, at position x, y */
/** @param #3 PK_DesignObj */
/** The ID of the screen */
/** @param #11 Position X */
/** X position */
/** @param #12 Position Y */
/** Y position */
/** @param #16 PK_DesignObj_CurrentScreen */
/** If specified the popup will be local to this screen, otherwise it will be global.  Global and local popups are treated separately */
/** @param #50 Name */
/** The popup name */
/** @param #126 Exclusive */
/** Hide any other popups that are also visible, unless don't hide is set. */
/** @param #127 Dont Auto Hide */
/** If true, this popup will not be automatically hidden when the screen changes or another exclusive is shown.  It must be explicitly hidden. */

void qOrbiter::CMD_Show_Popup(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string sName,bool bExclusive,bool bDont_Auto_Hide,string &sCMD_Result,Message *pMessage)
//<-dceag-c397-e->
{
    cout << "Need to implement command #397 - Show Popup" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #11 - Position_X=" << iPosition_X << endl;
    cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
    cout << "Parm #16 - PK_DesignObj_CurrentScreen=" << sPK_DesignObj_CurrentScreen << endl;
    cout << "Parm #50 - Name=" << sName << endl;
    cout << "Parm #126 - Exclusive=" << bExclusive << endl;
    cout << "Parm #127 - Dont_Auto_Hide=" << bDont_Auto_Hide << endl;

}

//<-dceag-c398-b->

/** @brief COMMAND: #398 - Remove Popup */
/** Hides a popup. */
/** @param #16 PK_DesignObj_CurrentScreen */
/** (optional).  The screen on which it's a local popup */
/** @param #50 Name */
/** The name of the popup.  If not specified all popups will be removed */

void qOrbiter::CMD_Remove_Popup(string sPK_DesignObj_CurrentScreen,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c398-e->
{
    cout << "Need to implement command #398 - Remove Popup" << endl;
    cout << "Parm #16 - PK_DesignObj_CurrentScreen=" << sPK_DesignObj_CurrentScreen << endl;
    cout << "Parm #50 - Name=" << sName << endl;
}

//<-dceag-c399-b->

/** @brief COMMAND: #399 - Show Shortcuts */
/** Shows keyboard shortcuts for 10 seconds or until the screen changes. */

void qOrbiter::CMD_Show_Shortcuts(string &sCMD_Result,Message *pMessage)
//<-dceag-c399-e->
{
    cout << "Need to implement command #399 - Show Shortcuts" << endl;
}

//<-dceag-c401-b->

/** @brief COMMAND: #401 - Show File List */
/** Shows the file list */
/** @param #29 PK_MediaType */
/** The type of media the user wants to browse. */

void qOrbiter::CMD_Show_File_List(int iPK_MediaType,string &sCMD_Result,Message *pMessage)
//<-dceag-c401-e->
{

    if (iPK_MediaType != q_mediaType.toInt())
    {
        initializeGrid();
        i_currentMediaModelRow = 0;
        i_mediaModelRows = 0;

    }
    q_mediaType = QString::number(iPK_MediaType);


    emit gotoQml("Screen_47.qml");
    currentScreen= "Screen_47.qml";
    emit statusMessage("Show File List Complete, Calling request Media Grid");

    setGridStatus(true);
    prepareFileList(iPK_MediaType);


}

//<-dceag-c402-b->

/** @brief COMMAND: #402 - Use Popup Remote Controls */
/** If this command is executed the remote controls will be displayed as popups. */
/** @param #11 Position X */
/** The location of the popup */
/** @param #12 Position Y */
/** The location of the popup */
/** @param #16 PK_DesignObj_CurrentScreen */
/** The screen on which to put the popup */

void qOrbiter::CMD_Use_Popup_Remote_Controls(int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string &sCMD_Result,Message *pMessage)
//<-dceag-c402-e->
{
    cout << "Need to implement command #402 - Use Popup Remote Controls" << endl;
    cout << "Parm #11 - Position_X=" << iPosition_X << endl;
    cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
    cout << "Parm #16 - PK_DesignObj_CurrentScreen=" << sPK_DesignObj_CurrentScreen << endl;
}

//<-dceag-c403-b->

/** @brief COMMAND: #403 - Use Popup File List */
/** If this command is executed the file lists will be displayed as popups. */
/** @param #11 Position X */
/** The location of the popup */
/** @param #12 Position Y */
/** The location of the popup */
/** @param #16 PK_DesignObj_CurrentScreen */
/** The screen to put the popup on */

void qOrbiter::CMD_Use_Popup_File_List(int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string &sCMD_Result,Message *pMessage)
//<-dceag-c403-e->
{
    cout << "Need to implement command #403 - Use Popup File List" << endl;
    cout << "Parm #11 - Position_X=" << iPosition_X << endl;
    cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
    cout << "Parm #16 - PK_DesignObj_CurrentScreen=" << sPK_DesignObj_CurrentScreen << endl;
}

//<-dceag-c405-b->

/** @brief COMMAND: #405 - Scale this object */
/** If you add this command to the startup list of an object it will cause Orbiter Gen to scale this object and all it's children. */
/** @param #48 Value */
/** The value to scale to.  100=full size, 50=half size */

void qOrbiter::CMD_Scale_this_object(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c405-e->
{
    cout << "Need to implement command #405 - Scale this object" << endl;
    cout << "Parm #48 - Value=" << iValue << endl;
}

//<-dceag-c407-b->

/** @brief COMMAND: #407 - Set Floorplan */
/** Sets the object to use for one of the following types:
light, media, climate, security, telecom */
/** @param #3 PK_DesignObj */
/** The screen to use for this floorplan */
/** @param #14 Type */
/** One of the following:
light, climate, media, security, telecom */
/** @param #119 True/False */
/** True if this is a popup.  False if it's full screen */

void qOrbiter::CMD_Set_Floorplan(string sPK_DesignObj,string sType,bool bTrueFalse,string &sCMD_Result,Message *pMessage)
//<-dceag-c407-e->
{
    cout << "Need to implement command #407 - Set Floorplan" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #14 - Type=" << sType << endl;
    cout << "Parm #119 - TrueFalse=" << bTrueFalse << endl;
}

//<-dceag-c408-b->

/** @brief COMMAND: #408 - Show Floorplan */
/** Shows the floorplan */
/** @param #11 Position X */
/** If the floorplan is not full screen, the location where it should be displayed */
/** @param #12 Position Y */
/** If the floorplan is not full screen, the location where it should be displayed */
/** @param #14 Type */
/** The type of floorplan */

void qOrbiter::CMD_Show_Floorplan(int iPosition_X,int iPosition_Y,string sType,string &sCMD_Result,Message *pMessage)
//<-dceag-c408-e->
{
    cout << "Need to implement command #408 - Show Floorplan" << endl;
    cout << "Parm #11 - Position_X=" << iPosition_X << endl;
    cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
    cout << "Parm #14 - Type=" << sType << endl;
}

//<-dceag-c413-b->

/** @brief COMMAND: #413 - Forward local k/b to OSD */
/** Means this orbiter's keyboard should be controlling the application running on the media director. */
/** @param #119 True/False */
/** If 1, do it.  It 0, stop */

void qOrbiter::CMD_Forward_local_kb_to_OSD(bool bTrueFalse,string &sCMD_Result,Message *pMessage)
//<-dceag-c413-e->
{
    cout << "Need to implement command #413 - Forward local k/b to OSD" << endl;
    cout << "Parm #119 - TrueFalse=" << bTrueFalse << endl;
}

//<-dceag-c415-b->

/** @brief COMMAND: #415 - Set Mouse Position Relative */
/** Move the mouse relative to its current position */
/** @param #11 Position X */
/** The X Position to move */
/** @param #12 Position Y */
/** The Y Position to move */

void qOrbiter::CMD_Set_Mouse_Position_Relative(int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage)
//<-dceag-c415-e->
{
    cout << "Need to implement command #415 - Set Mouse Position Relative" << endl;
    cout << "Parm #11 - Position_X=" << iPosition_X << endl;
    cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
}

//<-dceag-c416-b->

/** @brief COMMAND: #416 - Simulate Mouse Click At Present Pos */
/** Simulates clicking the mouse button */
/** @param #14 Type */
/** If L or empty, the left button.  If R the right button. */

void qOrbiter::CMD_Simulate_Mouse_Click_At_Present_Pos(string sType,string &sCMD_Result,Message *pMessage)
//<-dceag-c416-e->
{
    cout << "Need to implement command #416 - Simulate Mouse Click At Present Pos" << endl;
    cout << "Parm #14 - Type=" << sType << endl;
}

//<-dceag-c689-b->

/** @brief COMMAND: #689 - Update Time Code */
/** Update the time code of the current media */
/** @param #41 StreamID */
/** The Stream to update */
/** @param #102 Time */
/** The current time.  If there is both a section time and total time, they should be \t delimited, like 1:03\t60:30 */
/** @param #132 Total */
/** If there is both a section time and total time, they should be \t delimited, like 1:03\t60:30 */
/** @param #133 Speed */
/** The current speed */
/** @param #134 Title */
/** For DVD's, the title */
/** @param #135 Section */
/** For DVD's, the section */

void qOrbiter::CMD_Update_Time_Code(int iStreamID,string sTime,string sTotal,string sSpeed,string sTitle,string sSection,string &sCMD_Result,Message *pMessage)
//<-dceag-c689-e->
{
    cout << "Need to implement command #689 - Update Time Code" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
    cout << "Parm #102 - Time=" << sTime << endl;
    cout << "Parm #132 - Total=" << sTotal << endl;
    cout << "Parm #133 - Speed=" << sSpeed << endl;
    cout << "Parm #134 - Title=" << sTitle << endl;
    cout << "Parm #135 - Section=" << sSection << endl;
}

//<-dceag-c741-b->

/** @brief COMMAND: #741 - Goto Screen */
/** Goto a specific screen. */
/** @param #10 ID */
/** Assigns an optional ID to this particular "viewing" of the screen, used with Kill Screen.  There can be lots of instances of the same screen in the history queue (such as call in progress).  This allows a program to pop a particular one out of the queue. */
/** @param #159 PK_Screen */
/** The screen id. */
/** @param #251 Interruption */
/** Indicates at what times to ignore the screen change if it would interrupt the user.  A value in: enum eInterruption */
/** @param #252 Turn On */
/** If true, turn the display on if it's off. */
/** @param #253 Queue */
/** If true, then if the screen change was ignored so as not to interrpt the user, queue it for when the user is done */

void qOrbiter::CMD_Goto_Screen(string sID,int iPK_Screen,int iInterruption,bool bTurn_On,bool bQueue,string &sCMD_Result,Message *pMessage)
//<-dceag-c741-e->
{

    cout << "Need to implement command #741 - Goto Screen" << endl;
    cout << "Parm #10 - ID=" << sID << endl;
    cout << "Parm #159 - PK_Screen=" << iPK_Screen << endl;
    cout << "Parm #251 - Interruption=" << iInterruption << endl;
    cout << "Parm #252 - Turn_On=" << bTurn_On << endl;
    cout << "Parm #253 - Queue=" << bQueue << endl;
    cout << "scmdresult" << sCMD_Result << endl;

    //qDebug() << "Vect msg count" << pMessage->m_vectExtraMessages.size();

    map<long, string >::const_iterator end = pMessage->m_mapParameters.end();
    for (map<long, string >::const_iterator it = pMessage->m_mapParameters.begin(); it != end; ++it)
    {
        long dparam = it->first;
        string dparam2 = it->second;
        // ScreenParameters->addParam( QString::fromStdString(dparam2), dparam );
        emit addScreenParam(QString::fromStdString(dparam2), dparam );
    }


    map<long, char* >::const_iterator end2 = pMessage->m_mapData_Parameters.end();
    for (map<long, char* >::const_iterator it2 = pMessage->m_mapData_Parameters.begin(); it2 != end2; ++it2)
    {
        long dparam = it2->first;
        string dparam2 = it2->second;
    }

    QString params = pMessage->ToString(true).c_str();
    QStringList paramList = params.split(" ");
    QString str = QString::number(iPK_Screen);
    emit gotoQml(QString("Screen_"+str+".qml"));
}

//<-dceag-c795-b->

/** @brief COMMAND: #795 - Set Mouse Behavior */
/** Indicates if the mouse should be locked to horizontal or vertical movements, how to handle range of motion, etc. */
/** @param #3 PK_DesignObj */
/** An object to lock the movement to */
/** @param #39 Options */
/** The following letter(s): [r/a]ramp/absolute */
/** @param #126 Exclusive */
/** If true, all existing mouse behavior will be removed */
/** @param #211 Direction */
/** a letter: [h]orizontal, [v]ertical, [b]oth */

void qOrbiter::CMD_Set_Mouse_Behavior(string sPK_DesignObj,string sOptions,bool bExclusive,string sDirection,string &sCMD_Result,Message *pMessage)
//<-dceag-c795-e->
{
    cout << "Need to implement command #795 - Set Mouse Behavior" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #39 - Options=" << sOptions << endl;
    cout << "Parm #126 - Exclusive=" << bExclusive << endl;
    cout << "Parm #211 - Direction=" << sDirection << endl;
}

//<-dceag-c801-b->

/** @brief COMMAND: #801 - Set Mouse Sensitivity */
/** 1=Sensitive, 3=not sensitive */
/** @param #48 Value */
/** 1=sensitive, 3=least sensitive */

void qOrbiter::CMD_Set_Mouse_Sensitivity(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c801-e->
{
    cout << "Need to implement command #801 - Set Mouse Sensitivity" << endl;
    cout << "Parm #48 - Value=" << iValue << endl;
}

//<-dceag-c809-b->

/** @brief COMMAND: #809 - Display Alert */
/** Displays a short alert that appears on the orbiter as a discrete popup message that goes away automatically. */
/** @param #9 Text */
/** The text in the alert */
/** @param #70 Tokens */
/** File this alert with this token, and if another alert comes in before timeout with the same token, replace it. */
/** @param #182 Timeout */
/** Make the alert go away after this many seconds */
/** @param #251 Interruption */
/** How to interrupt the user if something is happening */

void qOrbiter::CMD_Display_Alert(string sText,string sTokens,string sTimeout,int iInterruption,string &sCMD_Result,Message *pMessage)
//<-dceag-c809-e->
{
    cout << "Need to implement command #809 - Display Alert" << endl;
    cout << "Parm #9 - Text=" << sText << endl;
    cout << "Parm #70 - Tokens=" << sTokens << endl;
    cout << "Parm #182 - Timeout=" << sTimeout << endl;
    cout << "Parm #251 - Interruption=" << iInterruption << endl;


}

//<-dceag-c810-b->

/** @brief COMMAND: #810 - Set Active Application */
/** Tells an On screen orbiter what application is currently active */
/** @param #50 Name */
/** A description of the app */
/** @param #159 PK_Screen */
/** The Screen for the OSD */
/** @param #216 Identifier */
/** The window identifier */
/** @param #226 PK_Screen_GoTo */
/** The screen for the orbiter remote */

void qOrbiter::CMD_Set_Active_Application(string sName,int iPK_Screen,string sIdentifier,int iPK_Screen_GoTo,string &sCMD_Result,Message *pMessage)
//<-dceag-c810-e->
{
    cout << "Need to implement command #810 - Set Active Application" << endl;
    cout << "Parm #50 - Name=" << sName << endl;
    cout << "Parm #159 - PK_Screen=" << iPK_Screen << endl;
    cout << "Parm #216 - Identifier=" << sIdentifier << endl;
    cout << "Parm #226 - PK_Screen_GoTo=" << iPK_Screen_GoTo << endl;
}

//<-dceag-c811-b->

/** @brief COMMAND: #811 - Get Active Application */
/** Return the currently active application */
/** @param #50 Name */
/** A description of the app */
/** @param #159 PK_Screen */
/** The Screen for the OSD */
/** @param #216 Identifier */
/** The window identifier */
/** @param #226 PK_Screen_GoTo */
/** The Screen for the orbiter remote */

void qOrbiter::CMD_Get_Active_Application(string *sName,int *iPK_Screen,string *sIdentifier,int *iPK_Screen_GoTo,string &sCMD_Result,Message *pMessage)
//<-dceag-c811-e->
{
    cout << "Need to implement command #811 - Get Active Application" << endl;
    cout << "Parm #50 - Name=" << sName << endl;
    cout << "Parm #159 - PK_Screen=" << iPK_Screen << endl;
    cout << "Parm #216 - Identifier=" << sIdentifier << endl;
    cout << "Parm #226 - PK_Screen_GoTo=" << iPK_Screen_GoTo << endl;
}

//<-dceag-c834-b->

/** @brief COMMAND: #834 - Execute Shortcut */
/** Execute the shortcut associated with a key.  Called when a key is held down. */
/** @param #48 Value */
/** The ascii value of the key (ie 65='A').  Valid are 0-9,*,#,A-Z */

void qOrbiter::CMD_Execute_Shortcut(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c834-e->
{
    cout << "Need to implement command #834 - Execute Shortcut" << endl;
    cout << "Parm #48 - Value=" << iValue << endl;
}

//<-dceag-c838-b->

/** @brief COMMAND: #838 - Bind to Wireless Keyboard */
/** If the USB RF dongle is attached it causes it to do a bind request and add a remote. */

void qOrbiter::CMD_Bind_to_Wireless_Keyboard(string &sCMD_Result,Message *pMessage)
//<-dceag-c838-e->
{
    cout << "Need to implement command #838 - Bind to Wireless Keyboard" << endl;
}


//<-dceag-c912-b->

/** @brief COMMAND: #912 - Activate PC Desktop */
/** Activate or de-activate the PC-desktop */
/** @param #119 True/False */
/** If true, switch to the last PC desktop.  If false, switch to Orbiter's desktop */

void qOrbiter::CMD_Activate_PC_Desktop(bool bTrueFalse,string &sCMD_Result,Message *pMessage)
//<-dceag-c912-e->
{
    cout << "Need to implement command #912 - Activate PC Desktop" << endl;
    cout << "Parm #119 - TrueFalse=" << bTrueFalse << endl;
}

//<-dceag-c923-b->

/** @brief COMMAND: #923 - Assisted Make Call */
/** Send make call command back to Orbiter and let it decide if we are going to make a direct call or a transfer/conference */
/** @param #17 PK_Users */
/** The called user. Only one is supported now. */
/** @param #83 PhoneExtension */
/** The phone number to be called. */
/** @param #184 PK_Device_From */
/** The device which starts the call. */
/** @param #263 PK_Device_To */
/** The called device. */

void qOrbiter::CMD_Assisted_Make_Call(int iPK_Users,string sPhoneExtension,string sPK_Device_From,int iPK_Device_To,string &sCMD_Result,Message *pMessage)
//<-dceag-c923-e->
{
    cout << "Need to implement command #923 - Assisted Make Call" << endl;
    cout << "Parm #17 - PK_Users=" << iPK_Users << endl;
    cout << "Parm #83 - PhoneExtension=" << sPhoneExtension << endl;
    cout << "Parm #184 - PK_Device_From=" << sPK_Device_From << endl;
    cout << "Parm #263 - PK_Device_To=" << iPK_Device_To << endl;
}

bool DCE::qOrbiter::initialize()
{

    if ( GetConfig() && Connect(PK_DeviceTemplate_get()) )
    {
        //   qDebug("Starting Manager");
        emit startManager(QString::number(m_dwPK_Device), QString::fromStdString(m_sHostName) );
#ifdef ANDROID
        DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Orbiter Connected, requesting configuration for device %d", m_dwPK_Device);
#endif

        char *oData;
        oData = NULL;
        int iData_Size;
        iData_Size = 0;
        string *p_sResponse;
        p_sResponse = NULL;
        string filePath = "/var/tmp/"+QString::number(m_dwPK_Device).toStdString()+"conf.xml";
        CMD_Request_File configFileRequest((long)m_dwPK_Device, (long)4 , (string)filePath, &oData, &iData_Size);

        if (SendCommand(configFileRequest) == false)
        {
            // setConnectedState(false);
            // processError("Cant request config!");
            return false;
        }
        else
        {
            emit statusMessage("Requesting remote configuration..");
        }

        //setDceResponse("Idata recieved: " +QString::number(iData_Size)) ; // size of xml file
        QByteArray configData;              //config file put into qbytearray for processing
        configData = oData;
        if (configData.size() == 0)
        {
            emit statusMessage("Invalid config for device: " + QString::number(m_dwPK_Device));
            emit statusMessage("Please run http://dcerouter/lmce-admin/qOrbiterGenerator.php?d=" + QString::number(m_dwPK_Device)) ;

            return false;
        }
        emit configReady(configData);
        return true;
    }
    else
    {


        //  qDebug("Checking Connection Error Type:");
        QApplication::processEvents(QEventLoop::AllEvents);
        if(  m_pEvent->m_pClientSocket->m_eLastError==ClientSocket::cs_err_CannotConnect )
        {

            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No Router");
            emit statusMessage("No Connection to Router");
            emit routerInvalid();
            QApplication::processEvents(QEventLoop::AllEvents);

        }
        else if( m_pEvent->m_pClientSocket->m_eLastError==ClientSocket::cs_err_BadDevice )
        {

            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Bad Device");
            emit statusMessage("Bad Device");
            DeviceIdInvalid();
            QApplication::processEvents(QEventLoop::AllEvents);

        }

        else if(  m_pEvent->m_pClientSocket->m_eLastError==ClientSocket::cs_err_NeedReload )
        {

            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Router Needs Reload");
            emit statusMessage("Needs Reload");
            QApplication::processEvents(QEventLoop::AllEvents);

        }
        else
        {

            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Connect() Failed");
            emit statusMessage("QOrbiter Connect() Failed :(");
            QApplication::processEvents(QEventLoop::AllEvents);
        }

        Disconnect();
        return false;
        QApplication::processEvents(QEventLoop::AllEvents);
    }


}

bool DCE::qOrbiter::deinitialize()
{
    char *pData;
    int iSize;
    pData = "NULL";
    iSize = 0;
    BindMediaRemote(false);
    DCE::CMD_Orbiter_Registered CMD_Orbiter_Registered(m_dwPK_Device, iOrbiterPluginID, StringUtils::itos(m_dwPK_Device) ,i_user, StringUtils::itos(i_ea), i_room, &pData, &iSize);
    SendCommand(CMD_Orbiter_Registered);
    this->~qOrbiter();

}




bool qOrbiter::getConfiguration()
{

}

void qOrbiter::registerDevice(int user, QString ea, int room)
{
    //  emit statusMessage("registering");

    char *pData;
    int iSize;
    i_user = user;
    i_ea = ea.toInt();
    i_room = room;


    pData = "NULL";
    iSize = 0;
    string pResponse ="";
    //on-off  PkUsers -int entArea -string  int room
    DCE::CMD_Orbiter_Registered CMD_Orbiter_Registered(m_dwPK_Device, iOrbiterPluginID,  "1" ,      i_user,         StringUtils::itos(i_ea),          i_room,           &pData, &iSize);
    if (SendCommand(CMD_Orbiter_Registered, &pResponse) && pResponse=="OK")
    {

        emit statusMessage("DCERouter Responded to Register with " + QString::fromStdString(pResponse));
        setLocation(room, ea.toInt());

        GetScreenSaverImages();
    }
    else
    {

    }




}

void qOrbiter::qmlSetup(QString device, QString address)
{
    m_dwPK_Device = device.toLong();
    m_sHostName = address.toStdString();
    if(initialize())
    {



    }
    else
    {
        connectionError();
    }

}

void qOrbiter::setCurrentScreen(QString s)
{
    //currentScreen = s;
}

void qOrbiter::connectionError()
{


}

void qOrbiter::getFloorplanDeviceCommand(int device)
{
    long current_device = (long)device;

    int cellsToRender= 0;

    int gHeight = 1;
    int gWidth = 6;
    string dgName ="fpdev_"+StringUtils::itos(m_dwPK_Device);
    int iData_Size=0;
    int GridCurRow = 0;
    int GridCurCol= 0;
    char *pData;
    pData = "NULL";
    string m_sSeek = "";
    m_dwIDataGridRequestCounter++;
    string option = StringUtils::itos(current_device);

    int offset = 0;
    int pkVar  = 0;
    int iOffset= 0;
    string valassign = "0";
    bool isSuccessfull;

    CMD_Populate_Datagrid cmd_populate_device_grid(m_dwPK_Device, iPK_Device_DatagridPlugIn, StringUtils::itos( m_dwIDataGridRequestCounter ), string(dgName), 94, option, 0, &pkVar, &valassign,  &isSuccessfull, &gHeight, &gWidth );

    if (SendCommand(cmd_populate_device_grid))
    {
        /*
              initial request to populate the text only grid as denoted by the lack of a leading "_" as in _MediaFile_43
              this way, we can safely check empty grids and error gracefully in the case of no matching media
              */


        //CMD_Request_Datagrid_Contents(long DeviceIDFrom, long DeviceIDTo,                   string sID,                                              string sDataGrid_ID,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset,    char **pData,int *iData_Size,int *iRow,int *iColumn
        DCE::CMD_Request_Datagrid_Contents req_device_grid( long(device), long(iPK_Device_DatagridPlugIn), StringUtils::itos( m_dwIDataGridRequestCounter ), string(dgName),    int(gWidth), int(gHeight),           false, false,        true,   string(m_sSeek),    int(iOffset),  &pData,         &iData_Size, &GridCurRow, &GridCurCol );
        if(SendCommand(req_device_grid))
        {
            DataGridTable *pDataGridTable = new DataGridTable(iData_Size,pData,false);
            cellsToRender= pDataGridTable->GetRows();
#ifdef ANDROID
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Floorplan Device Grid Dimensions: Height %i, Width %i", gHeight, gWidth);
#endif
            QString cellTitle;
            QString fk_file;
            QString filePath;
            int index;

            for(MemoryDataTable::iterator it=pDataGridTable->m_MemoryDataTable.begin();it!=pDataGridTable->m_MemoryDataTable.end();++it)
            {
                DataGridCell *pCell = it->second;
                const char *pPath = pCell->GetImagePath();
                filePath = QString::fromUtf8(pPath);
                fk_file = pCell->GetValue();
                cellTitle = QString::fromUtf8(pCell->m_Text);
                index = pDataGridTable->CovertColRowType(it->first).first;
                /*
             //   model->appendRow(new gridItem(fk_file, cellTitle, filePath, index, cellImg,  model));
    */
            }
        }
    }

}

void qOrbiter::setRecievingStatus(bool b)
{
    finished = b;
    emit recievingStatusChanged();

}

bool qOrbiter::getRecievingStatus()
{
    return finished;
}

void qOrbiter::setGridStatus(bool b)
{

    requestMore = b;
    //qDebug() << "Request set to " << requestMore;
    //checkLoadingStatus();
}

bool qOrbiter::getGridStatus()
{
    return requestMore;
}

void qOrbiter::setCurrentRow(int row)
{
    i_currentMediaModelRow = row;
#ifdef for_desktop
    populateAdditionalMedia();
#endif
}

int qOrbiter::getCurrentRow()
{
    return i_currentMediaModelRow;
}

void qOrbiter::initializeGrid()
{
    goBack.clear();
    QString datagridVariableString ;
    //datagrid option variables
    //  QString q_mediaType;           //1
    q_subType="";             //2
    q_fileFormat="";          //3
    q_attribute_genres="";    //4
    q_mediaSources ="1,2";         //5 need comma delineation
    q_usersPrivate = "0";        //6
    q_attributetype_sort="";  //7
    q_pk_users="0";             //8
    q_last_viewed="2";        //9
    q_pk_attribute="";        //10
    qs_seek ="";

    datagridVariableString.append(q_mediaType).append("|").append(q_subType).append("|").append(q_fileFormat).append("|").append(q_attribute_genres).append("|").append(q_mediaSources).append("|").append(q_usersPrivate).append("|").append(q_attributetype_sort).append("|").append(q_pk_users).append("|").append(q_last_viewed).append("|").append(q_pk_attribute);

    // goBack.append(datagridVariableString);
    emit statusMessage("Dg Variables Reset");
}

void qOrbiter::setStringParam(int paramType, QString param)
{
    /*
    QString q_mediaType;           //1 passed in inital dg request
    QString q_subType;             //2
    QString q_fileFormat;          //3
    QString q_attribute_genres;    //4
    QString q_mediaSources;         //5 needs comma seperator

    QString q_usersPrivate;        //6
    QString q_attributetype_sort;  //7
    QString q_pk_users;             //8
    QString q_last_viewed;        //9
    QString q_pk_attribute;        //10
    QString *datagridVariableString;
    */

    QStringList longassstring;
    QString datagridVariableString;

    backwards = false;
    setGridStatus(false);
    switch (paramType)
    {
    case 1:
        q_subType = param;
        longassstring << q_mediaType+ "|" + q_subType + "|" + q_fileFormat + "|" + q_attribute_genres + "|" + q_mediaSources << "|" + q_usersPrivate +"|" + q_attributetype_sort +"|" + q_pk_users + "|" + q_last_viewed +"|" + q_pk_attribute;
        datagridVariableString = longassstring.join("|");
        emit clearAndContinue(q_mediaType.toInt());;
        break;

    case 2:
        q_fileFormat = param;

        longassstring << q_mediaType+ "|" + q_subType + "|" + q_fileFormat + "|" + q_attribute_genres + "|" + q_mediaSources << "|" + q_usersPrivate +"|" + q_attributetype_sort +"|" + q_pk_users + "|" + q_last_viewed +"|" + q_pk_attribute;
        datagridVariableString = longassstring.join("|");
        emit clearAndContinue(q_mediaType.toInt());;
        break;

    case 3:
        q_attribute_genres = param;

        longassstring << q_mediaType+ "|" + q_subType + "|" + q_fileFormat + "|" + q_attribute_genres + "|" + q_mediaSources << "|" + q_usersPrivate +"|" + q_attributetype_sort +"|" + q_pk_users + "|" + q_last_viewed +"|" + q_pk_attribute;
        datagridVariableString = longassstring.join("|");
        emit clearAndContinue(q_mediaType.toInt());;
        break;

    case 4:
        if (q_mediaSources != param.remove("!D"))
        {
            if(param.contains("!F"))
            {
                emit showFileInfo(true);
                emit setFocusFile(param);
                GetFileInfoForQml(param);
                break;
            }
            else if (param.contains("!P"))
            {
                emit showFileInfo(true);
                emit setFocusFile(param);
                GetFileInfoForQml(param);
                break;
            }
            else
            {
                q_pk_attribute = param.remove("!A");
                longassstring << q_mediaType+ "|" + q_subType + "|" + q_fileFormat + "|" + q_attribute_genres + "|" + q_mediaSources << "|" + q_usersPrivate +"|" + q_attributetype_sort +"|" + q_pk_users + "|" + q_last_viewed +"|" + q_pk_attribute;
                datagridVariableString = longassstring.join("|");
                emit clearAndContinue(q_mediaType.toInt());;
            }
        }
        break;

    case 5:
        q_usersPrivate = param;

        longassstring << q_mediaType+ "|" + q_subType + "|" + q_fileFormat + "|" + q_attribute_genres + "|" + q_mediaSources << "|" + q_usersPrivate +"|" + q_attributetype_sort +"|" + q_pk_users + "|" + q_last_viewed +"|" + q_pk_attribute;
        datagridVariableString = longassstring.join("|");
        emit clearAndContinue(q_mediaType.toInt());;
        break;

    case 6:
        if (param.contains("!P"))
        {
            emit showFileInfo(true);
            emit setFocusFile(param);
            break;
        }
        else
        {
            if(backwards ==false)
            {
                q_attributetype_sort = param;
            }
            longassstring << q_mediaType+ "|" + q_subType + "|" + q_fileFormat + "|" + q_attribute_genres + "|" + q_mediaSources << "|" + q_usersPrivate +"|" + q_attributetype_sort +"|" + q_pk_users + "|" + q_last_viewed +"|" + q_pk_attribute;
            datagridVariableString = longassstring.join("|");
            emit clearAndContinue(q_mediaType.toInt());;
            break;
        }

    case 7:
        q_pk_users = param;

        longassstring << q_mediaType+ "|" + q_subType + "|" + q_fileFormat + "|" + q_attribute_genres + "|" + q_mediaSources << "|" + q_usersPrivate +"|" + q_attributetype_sort +"|" + q_pk_users + "|" + q_last_viewed +"|" + q_pk_attribute;
        datagridVariableString = longassstring.join("|");
        emit clearAndContinue(q_mediaType.toInt());;
        break;
    case 8:
        q_last_viewed = param;
        longassstring << q_mediaType+ "|" + q_subType + "|" + q_fileFormat + "|" + q_attribute_genres + "|" + q_mediaSources << "|" + q_usersPrivate +"|" + q_attributetype_sort +"|" + q_pk_users + "|" + q_last_viewed +"|" + q_pk_attribute;
        datagridVariableString = longassstring.join("|");
        emit clearAndContinue(q_mediaType.toInt());;

        break;
    case 9:
        if(param.contains("!F"))
        {

            emit showFileInfo(true);
            emit setFocusFile(param);
            break;
        }
        else if (param.contains("!P"))
        {
            emit showFileInfo(true);
            emit setFocusFile(param);
            break;
        }

        else
        {
            q_pk_attribute = param.remove("!A");
            longassstring << q_mediaType+ "|" + q_subType + "|" + q_fileFormat + "|" + q_attribute_genres + "|" + q_mediaSources << "|" + q_usersPrivate +"|" + q_attributetype_sort +"|" + q_pk_users + "|" + q_last_viewed +"|" + q_pk_attribute;
            datagridVariableString = longassstring.join("|");
            emit clearAndContinue(q_mediaType.toInt());;
            break;
        }

    default:
        emit clearAndContinue(q_mediaType.toInt());;

    }
}

void qOrbiter::goBackGrid()
{

    backwards= true;

    if(goBack.isEmpty())
    {
        initializeSorting();
        emit clearAndContinue(i_current_mediaType);
    }
    else
    {
        goBack.removeLast();
        int back = goBack.count() - 1;

        QStringList reverseParams = goBack.at(back).split("|", QString::KeepEmptyParts);
        q_mediaType = reverseParams.first();
        q_subType = reverseParams.at(1);
        q_fileFormat = reverseParams.at(2);
        q_attribute_genres = reverseParams.at(3);
        q_mediaSources = reverseParams.at(4);
        q_usersPrivate = reverseParams.at(5);
        q_attributetype_sort = reverseParams.at(6);
        q_pk_users = reverseParams.at(7);
        q_last_viewed = reverseParams.at(8);
        q_pk_attribute = reverseParams.at(9);

        emit clearAndContinue(q_mediaType.toInt());

    }
}

void qOrbiter::requestPage(int page)
{

    media_pos = page * media_pageSeperator;
    media_currentPage = page;
    setGridStatus(true);

    //qDebug () << "Navigating to page " << media_currentPage;
    //qDebug() << "Cell Range::" << media_pos << "to" << media_pos + media_pageSeperator;
#ifndef for_desktop
    emit clearPageGrid();
#endif

}


void qOrbiter::jumpMobileGrid(int page)
{

}

void DCE::qOrbiter::executeCommandGroup(int cmdGrp)
{
#ifndef ANDROID
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Executing Command Group %d", cmdGrp);
#endif
    string *pResponse;
    CMD_Execute_Command_Group execCommandGroup((long)m_dwPK_Device, (long)2, cmdGrp);

    SendCommand(execCommandGroup);
}

void qOrbiter::displayToggle(int i)
{
    string state;

    if (i == 0)
    {
        state = "off";
        DCE::CMD_Off display(m_dwPK_Device, iMediaPluginID, 0);
        SendCommand(display);
        emit statusMessage("Attempting to toggle the MD display Off");
    }
    else
    {
        state ="on";
        DCE::CMD_On display(m_dwPK_Device, iMediaPluginID, 1, "");
        SendCommand(display);
        emit statusMessage("Attempting to toggle the MD display on");

    }


}

void qOrbiter::setMediaSpeed(int s)
{
    internal_playback_speed = s;
}

void qOrbiter::getGridView(bool direction)
{

}

void qOrbiter::shutdownMD()
{

}

QImage DCE::qOrbiter::getfileForDG(string filePath)
{
    char *picData = NULL;
    int picData_Size;
    picData_Size = 0;

    CMD_Request_File reqFile((long)m_dwPK_Device, (long)4 , (string)filePath, &picData, &picData_Size);
    string p_sResponse="";

    //SendCommand(reqFile, p_sResponse);

    if(SendCommand(reqFile, &p_sResponse) && p_sResponse=="OK")
    {
        QByteArray tempData;
        tempData.setRawData(picData, picData_Size);

        if (!returnImage.loadFromData(tempData))
        {
            emit statusMessage("Couldnt Load From Data, setting default icon image");
            returnImage.load(":/icons/icon.png");
            return returnImage;
        }
        return returnImage;

    }
    else
    {
        emit statusMessage("Couldnt Load From Router, setting default icon image");
        returnImage.load(":/icons/icon.png");
        return returnImage;
    }

}

void DCE::qOrbiter::GetFileInfoForQml(QString qs_file_reference)
{
    string s_value_assignment;

    CMD_Get_Attributes_For_Media cmd_file_info(m_dwPK_Device, iMediaPluginID , qs_file_reference.toStdString(), "", &s_value_assignment);

    if (!SendCommand(cmd_file_info))
    {
        cout << "No Router";
    }
    else
    {
        GetMediaAttributeGrid(qs_file_reference);
    }

}

void DCE::qOrbiter::GetMediaAttributeGrid(QString  qs_fk_fileno)
{
    qDebug("Getting file info");
    int gHeight = 1;
    int gWidth = 1;
    int pkVar = 0;
    string valassign ="";
    bool isSuccessfull;// = "false";

    string m_sGridID ="mattrfile_"+StringUtils::itos(m_dwPK_Device);

    int iRow_count=5;
    int iColumn_count = 5;
    bool m_bKeep_Row_Header = false;
    bool m_bKeepColHeader = false;
    bool m_bAdd_UpDown_Arrows = true;

    string m_sSeek;
    string m_sDataGrid_ID = "1";
    int iOffset;

    int iColumn = 1;
    int iRowCount= 5;
    int iRowColums = 5;
    int m_iSeekColumn = 0;
    int iData_Size=0;
    int GridCurRow = 0;
    int GridCurCol= 0;
    string pResponse="";
    char *pData;
    pData = "NULL";
    int iRow;

    QString temp;
    m_dwIDataGridRequestCounter++;

    string s_val;
    CMD_Get_Attributes_For_Media attribute_detail_get(m_dwPK_Device, iMediaPluginID,  qs_fk_fileno.toStdString(), " ",&s_val );
    SendCommand(attribute_detail_get);
    QString breaker = s_val.c_str();


    QStringList details = breaker.split(QRegExp("\\t"));
    int placeholder;

    placeholder = details.indexOf("TITLE");
    if(placeholder != -1)
    {   temp = details.at(placeholder+1);
        emit fd_titleChanged(temp);
    }

    placeholder = details.indexOf("SYNOPSIS");
    if(placeholder != -1)
    {
        temp = details.at(placeholder+1);
        emit fd_synopChanged(temp);
    }

    placeholder = details.indexOf("PICTURE");
    if(placeholder != -1)
    {
        // filedetailsclass->setScreenshot(details.at(placeholder+1));
        // filedetailsclass->setScreenshotimage(getfileForDG(details.at(placeholder+1).toStdString()));
        QImage temp =  getfileForDG(details.at(placeholder+1).toStdString());
        emit fd_titleImageChanged(temp);
    }

    placeholder = details.indexOf("PATH");
    if(placeholder != -1)
    {
        emit fd_pathChanged(details.at(placeholder+1));
    }

    placeholder = details.indexOf("FILENAME");
    if(placeholder != -1)
    {
        emit fd_fileChanged(qs_fk_fileno);
    }

    CMD_Populate_Datagrid cmd_populate_attribute_grid(m_dwPK_Device, iPK_Device_DatagridPlugIn, StringUtils::itos( m_dwIDataGridRequestCounter ), string(m_sGridID), 83, qs_fk_fileno.toStdString(), DEVICETEMPLATE_Datagrid_Plugin_CONST, &pkVar, &valassign,  &isSuccessfull, &gHeight, &gWidth );

    if (SendCommand(cmd_populate_attribute_grid, &pResponse))
    {
        /*
          initial request to populate the text only grid as denoted by the lack of a leading "_" as in _MediaFile_43
          this way, we can safely check empty grids and error gracefully in the case of no matching media
          */

        string imgDG = m_sGridID; //standard text grid with no images. this will not crash the router if requested and its empty, picture grids will

        //CMD_Request_Datagrid_Contents(long DeviceIDFrom, long DeviceIDTo,                   string sID,                                              string sDataGrid_ID,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset,    char **pData,int *iData_Size,int *iRow,int *iColumn
        DCE::CMD_Request_Datagrid_Contents req_data_grid( long(m_dwPK_Device), long(iPK_Device_DatagridPlugIn), StringUtils::itos( m_dwIDataGridRequestCounter ), string(m_sGridID),    int(gWidth), int(gHeight),           false, false,        true,   string(m_sSeek),    int(iOffset),  &pData,         &iData_Size, &GridCurRow, &GridCurCol );
        if(SendCommand(req_data_grid))
        {

            bool barrows = false; //not sure what its for
            //creating a dg table to check for cells. If 0, then we error out and provide a single "error cell"
            DataGridTable *pDataGridTable = new DataGridTable(iData_Size,pData,false);
            // int cellsToRender= pDataGridTable->GetRows();
#ifndef ANDROID
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Attribute Datagrid Dimensions: Height %i, Width %i", gHeight, gWidth);
#endif

            int index;
            QString cellfk;
            DataGridCell *pCell;
            for(MemoryDataTable::iterator it=pDataGridTable->m_MemoryDataTable.begin();it!=pDataGridTable->m_MemoryDataTable.end();++it)
            {
                pCell = it->second;
                // fk.remove("!F");
                const char *pPath = pCell->GetImagePath();
                index = pDataGridTable->CovertColRowType(it->first).first;
                QString attributeType = pCell->m_mapAttributes_Find("Title").c_str();
                QString attribute  = pCell->m_mapAttributes_Find("Name").c_str();
                cellfk = pCell->GetValue();

                if(attributeType == "Program")
                {
                    emit fd_programChanged(attribute);
                    ;
                }
                else if(attributeType == "Title")
                {
                    emit fd_mediaTitleChanged(attribute);
                }
                else if(attributeType == "Channel")
                {
                    emit fd_chanIdChanged(attribute);
                }
                else if(attributeType == "Episode")
                {
                    emit fd_episodeChanged(attribute);
                }
                else if(attributeType == "Performer")
                {
                    emit fd_performersChanged(attribute);
                }
                else if(attributeType == "Composer/Writer")
                {
                    emit fd_composersChanged(attribute);
                }
                else if(attributeType == "Director")
                {
                    emit fd_directorChanged(attribute);
                }
                else if(attributeType == "Genre")
                {
                    emit fd_genreChanged(attribute);
                }
                else if(attributeType == "Album")
                {
                    emit fd_albumChanged(attribute);
                }
                else if(attributeType == "Studio")
                {
                    emit fd_studioChanged(attribute);
                }
                else if(attributeType == "Track")
                {
                    emit fd_trackChanged(attribute);
                }
                else if(attributeType == "Rating")
                {
                    //filedetailsclass->setRating(attribute);
                }
                else
                {

                }
            }

        }

    }

}

void DCE::qOrbiter::GetSecurityCam(int i_inc_pkdevice)
{

}

void DCE::qOrbiter::playMedia(QString inc_FKFile)
{

    //changed to remove media type as that is decided on by the media plugin and passed back

    CMD_MH_Play_Media playMedia(m_dwPK_Device, iMediaPluginID, 0 , inc_FKFile.toStdString(), 0, 0, StringUtils::itos(i_ea), false, false, false, false, false);
    cout << "Playing file: " << inc_FKFile.toStdString() << endl;
    string pos = "";
    int streamID = NULL;
    // CMD_Play_Media playMedia(m_dwPK_Device, iMediaPluginID, i_current_mediaType, 1001 , pos, inc_FKFile.toStdString());
    SendCommand(playMedia);

}

void DCE::qOrbiter::StopMedia()
{

    CMD_MH_Stop_Media endMedia(m_dwPK_Device, iMediaPluginID,0,i_current_mediaType ,0,StringUtils::itos(i_ea),false);
    SendCommand(endMedia);

}

void DCE::qOrbiter::RwMedia()
{
    CMD_Change_Playback_Speed rewind_media(m_dwPK_Device, iMediaPluginID, internal_streamID , -2, true);
    SendCommand(rewind_media);
}

void DCE::qOrbiter::FfMedia()
{

    CMD_Change_Playback_Speed forward_media(m_dwPK_Device, iMediaPluginID, internal_streamID, +2, true);
    SendCommand(forward_media);
}

void DCE::qOrbiter::PauseMedia()
{
    CMD_Pause_Media pause_media(m_dwPK_Device, iMediaPluginID,internal_streamID);
    SendCommand(pause_media);
    //  nowPlayingButton->setMediaSpeed(0);
}

void DCE::qOrbiter::requestMediaPlaylist()
{

    int gHeight = 0;
    int gWidth = 0;
    int pkVar = 0;
    string valassign ="";
    bool isSuccessfull;// = "false";
    string m_sGridID ="plist_"+StringUtils::itos(m_dwPK_Device);
    string m_sSeek;
    int iOffset;
    int iData_Size=0;
    int GridCurRow = 0;
    int GridCurCol= 0;
    char *pData;
    pData = "NULL";
    m_dwIDataGridRequestCounter++;

    string sPopulateResp ="";
    CMD_Populate_Datagrid cmd_populate_nowplaying_grid(m_dwPK_Device, iPK_Device_DatagridPlugIn, StringUtils::itos( m_dwIDataGridRequestCounter ), string(m_sGridID), 18, "38", DEVICETEMPLATE_Datagrid_Plugin_CONST, &pkVar, &valassign,  &isSuccessfull, &gHeight, &gWidth );

    if (SendCommand(cmd_populate_nowplaying_grid))
    {
        //CMD_Request_Datagrid_Contents(long DeviceIDFrom, long DeviceIDTo,                   string sID,                                              string sDataGrid_ID,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset,    char **pData,int *iData_Size,int *iRow,int *iColumn
        sPopulateResp = "";
        DCE::CMD_Request_Datagrid_Contents req_data_grid( long(m_dwPK_Device), long(iPK_Device_DatagridPlugIn), StringUtils::itos( m_dwIDataGridRequestCounter ), string(m_sGridID),    int(gWidth), int(gHeight),           false, false,        false,   string(m_sSeek),    int(iOffset),  &pData,         &iData_Size, &GridCurRow, &GridCurCol );
        if(SendCommand(req_data_grid))
        {
            DataGridTable *pDataGridTable = new DataGridTable(iData_Size,pData,false);
#ifndef ANDROID
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Attribute Datagrid Dimensions: Height %i, Width %i", gHeight, gWidth);
#endif
            QString cellTitle;
            QString qs_plsIndex;
            QString fk_file;
            int index;

            for(MemoryDataTable::iterator it=pDataGridTable->m_MemoryDataTable.begin();it!=pDataGridTable->m_MemoryDataTable.end();++it)
            {
                DataGridCell *pCell;
                pCell = it->second;
                cellTitle = pCell->GetText();
                qs_plsIndex = pCell->GetValue();
                index = qs_plsIndex.toInt();
                fk_file = pCell->GetValue();
                emit playlistItemAdded(new PlaylistItemClass(cellTitle,  fk_file,index));

                QApplication::processEvents(QEventLoop::AllEvents);
            }

        }
        emit playlistDone();
    }
    else
    {
        emit statusMessage("Could not populate playlist!");
    }
}

void qOrbiter::checkTimeCode()
{
    emit setMyIp(QString::fromStdString(m_sIPAddress));
}

void qOrbiter::showTimeCode()
{

}

void qOrbiter::changedPlaylistPosition(QString pos)
{

    if(!pos.contains(QRegExp("TITLE:")))
    {
        qDebug() << pos;
        jumpToPlaylistPosition(pos.toInt());
    }
    else
    {
        //      pqOrbiter->setPosition(pos);
    }
}

void DCE::qOrbiter::ShowFloorPlan(int floorplantype)
{

    string sval = "";
    string id = "1";

    CMD_Get_Current_Floorplan getFloorPlan(m_dwPK_Device, iOrbiterPluginID, id, floorplantype , &sval);
    SendCommand(getFloorPlan);
    QString Screen = QString("Screen_").append(StringUtils::itos(floorplantype).c_str()).append(".qml");
    //qDebug() << "SVAL::" << sval.c_str();
    //   CMD_Get_Floorplan_Layout getLayout(m_dwPK_Device, iMediaPluginID, &sval);
    // SendCommand(getLayout);
    //qDebug() << "SVAL::" << sval.c_str();
    //gotoQScreen(Screen);
}

void DCE::qOrbiter::GetScreenSaverImages() // unused at this time
{
    string sFilename;
    CMD_Get_Screen_Saver_Files screen_saver_files(m_dwPK_Device, iPK_Device_OrbiterPlugin,m_dwPK_Device, &sFilename);
    SendCommand(screen_saver_files);
    QStringList tempList = QString::fromStdString(sFilename).split("\n");
    emit screenSaverImages(tempList);

}

void DCE::qOrbiter::BindMediaRemote(bool onoff)
{
    string status;
    if (onoff == true)
    {
        status = "1";
    }
    else
    {
        status = "0";
    }
    string designobj = "2355";
    CMD_Bind_to_Media_Remote bind_remote(m_dwPK_Device, iMediaPluginID, m_dwPK_Device,string("4962") ,status, string(""), StringUtils::itos(i_ea), 0, 0);
    SendCommand(bind_remote);
}

void DCE::qOrbiter::jumpToPlaylistPosition(int pos)
{
    qDebug("jumping to playlist item");
    CMD_Jump_Position_In_Playlist jump_playlist(m_dwPK_Device, iMediaPluginID, StringUtils::itos(pos), internal_streamID);
    SendCommand(jump_playlist);
}

void DCE::qOrbiter::setNowPlayingDetails()
{
    if(i_current_mediaType != 11)
    {

        qDebug("Requesting playlist");

    }
    else{


    }

}

void DCE::qOrbiter::SetSecurityStatus(string pin, string mode, int user, string special)
{
    CMD_Set_House_Mode set_security_mode(m_dwPK_Device, iPK_Device_SecurityPlugin, mode, user, pin,0, special);
    SendCommand(set_security_mode);
}

void DCE::qOrbiter::GetSingleSecurityCam(int cam_device, int iHeight, int iWidth) //shows security camera, needs to be threaded as it blocks the ui
{
    char *sData;
    int sData_size= 0;
    string imgtype;


    CMD_Get_Video_Frame singleVideoFrame(m_dwPK_Device,long(cam_device), string("1"), 0, iWidth, iHeight, &sData, &sData_size, &imgtype);
    SendCommand(singleVideoFrame);

    QImage returnedFrame;
    returnedFrame.loadFromData(QByteArray(sData, sData_size));
    // SecurityVideo->currentFrame = returnedFrame;

}

void DCE::qOrbiter::GetMultipleSecurityCams(QStringList cams) // not implemented yet
{
}

void DCE::qOrbiter::GetNowPlayingAttributes()
{
    /*
      this functions purpose is to return the attributes for the currently playing media this in this
      orbiter's EA it is a candidate for being extended along with the file details metadata function
      to expand our ui capabilities. it is essentially a clone of the many datagrid requests
      that the orbiter makes in addition to parsing of other information passed for a given object.
      */

    int p = 0;
    string s_val;
    string pResponse="";
    CMD_Get_Attributes_For_Media attribute_detail_get(m_dwPK_Device, iMediaPluginID, "" , StringUtils::itos(i_ea),&s_val );
    if (SendCommand(attribute_detail_get, &pResponse) && pResponse == "OK")
    {
        emit statusMessage("Getting Media Attributes");
        QString breaker = s_val.c_str();

        QStringList details = breaker.split(QRegExp("\\t"));
        int placeholder;
        QString filepath;
        placeholder = details.indexOf("TITLE");
        if(placeholder != -1)
        {

        }

        placeholder = details.indexOf("SYNOPSIS");
        if(placeholder != -1)
        {
            //nowPlayingButton->setSynop(details.at(placeholder+1));
        }

        placeholder = details.indexOf("PICTURE");
        if(placeholder != -1)
        {

        }

        placeholder = details.indexOf("PATH");
        if(placeholder != -1)
        {
            filepath = details.at(placeholder+1) + "/";
        }

        placeholder = details.indexOf("FILENAME");
        if(placeholder != -1)
        {
            filepath.append(details.at(placeholder+1));
        }

        emit np_filename(filepath);
        qDebug() <<filepath;

        int gHeight = 0;
        int gWidth = 0;
        int pkVar = 0;
        string valassign ="";
        bool isSuccessfull;// = "false";

        string m_sGridID ="mac_"+StringUtils::itos(m_dwPK_Device); // the string identifier on the type of grid
        string m_sSeek;
        int iData_Size=0;
        int GridCurRow = 0;
        int GridCurCol= 0;

        char *pData;
        pData = "NULL";

        m_dwIDataGridRequestCounter++;

        CMD_Populate_Datagrid cmd_populate_attribute_grid(m_dwPK_Device, iPK_Device_DatagridPlugIn, StringUtils::itos( m_dwIDataGridRequestCounter ), string(m_sGridID), 31, StringUtils::itos(i_ea), 0, &pkVar, &valassign,  &isSuccessfull, &gHeight, &gWidth );

        if (SendCommand(cmd_populate_attribute_grid))
        {
            /*
                  initial request to populate the text only grid as denoted by the lack of a leading "_" as in _MediaFile_43
                  this way, we can safely check empty grids and error gracefully in the case of no matching media
                  */

            //standard text grid with no images. this will not crash the router if requested and its empty, picture grids will

            //CMD_Request_Datagrid_Contents(long DeviceIDFrom, long DeviceIDTo,                   string sID,                                              string sDataGrid_ID,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset,    char **pData,int *iData_Size,int *iRow,int *iColumn
            DCE::CMD_Request_Datagrid_Contents req_data_grid( long(m_dwPK_Device), long(iPK_Device_DatagridPlugIn), StringUtils::itos( m_dwIDataGridRequestCounter ), string(m_sGridID),    int(gWidth), int(gHeight),           false, false,        true,   string(m_sSeek),    0,  &pData,         &iData_Size, &GridCurRow, &GridCurCol );
            if(SendCommand(req_data_grid))
            {


                //creating a dg table to check for cells. If 0, then we error out and provide a single "error cell"
                DataGridTable *pDataGridTable = new DataGridTable(iData_Size,pData,false);
                //int cellsToRender= pDataGridTable->GetRows();
#ifndef ANDROID
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Attribute Datagrid Dimensions: Height %i, Width %i", gHeight, gWidth);
#endif
                QString cellTitle;
                QString cellAttribute;
                int index;
                QString cellfk;
                DataGridCell *pCell;
                for(MemoryDataTable::iterator it=pDataGridTable->m_MemoryDataTable.begin();it!=pDataGridTable->m_MemoryDataTable.end();++it)
                {

                    pCell = it->second;
                    index = pDataGridTable->CovertColRowType(it->first).first;
                    cellTitle = pCell->GetText();
                    cellAttribute = pCell->GetValue();
                    cellfk = pCell->GetValue();
                    QStringList parser = cellTitle.split(QRegExp("(\\n|:\\s)"), QString::KeepEmptyParts);
                    QString attributeType = parser.at(0);
                    QString attribute;
                    if(parser.length() < 2)
                    {
                        attribute = "";
                    }
                    attribute = parser.at(1);
                    if(attributeType == "Program")
                    {

                        emit np_program(attribute);

                    }
                    else if(attributeType == "Title")
                    {

                        emit np_mediaTitleChanged(attribute);

                    }
                    else if(attributeType == "Channel")
                    {

                        emit np_channel(attribute);
                    }
                    else if(attributeType == "Episode")
                    {
                        emit np_episode(attribute);

                    }
                    else if(attributeType == "Performer")
                    {
                        emit np_performer(attribute);

                    }
                    else if(attributeType == "Director")
                    {
                        emit np_director(attribute);

                    }
                    else if(attributeType == "Genre")
                    {
                        emit np_genre(attribute);

                    }
                    else if(attributeType == "Album")
                    {
                        emit np_album(attribute);

                    }
                    else if(attributeType == "Track")
                    {
                        emit np_track(attribute);

                    }
                    else if(attributeType == "Synopsis")
                    {
                        emit np_synopsisChanged(attribute);

                    }
                    else if(attributeType == "Release Date")
                    {
                        emit np_releaseDate(attribute);

                    }
                }
            }
        }
    }


}

void DCE::qOrbiter::requestLiveTvPlaylist()
{

    /*
      this functions purpose is to return the attributes for the currently playing media this in this
      orbiter's EA it is a candidate for being extended along with the file details metadata function
      to expand our ui capabilities. it is essentially a clone of the many datagrid requests
      that the orbiter makes.
      */

    qDebug("Getting current Epg");

    int gHeight = 1;
    int gWidth = 1;
    int pkVar = 0;
    string valassign ="";
    bool isSuccessfull;// = "false";
    string m_sGridID ="tvchan_"+StringUtils::itos(m_dwPK_Device); // the string identifier on the type of grid
    string m_sSeek;
    string m_EA = QString::number(i_ea).toStdString();
    string m_UserID = QString::number(i_user).toStdString();
    int iOffset;
    int iData_Size=0;
    int GridCurRow = 0;
    int GridCurCol= 0;
    char *pData;
    pData = "NULL";
    m_dwIDataGridRequestCounter++;
#ifdef ANDROID
    CMD_Populate_Datagrid cmd_populate_livetv_grid(m_dwPK_Device, iPK_Device_DatagridPlugIn, StringUtils::itos( m_dwIDataGridRequestCounter ), string(m_sGridID), 11, m_UserID + "," + m_EA, 0, &pkVar, &valassign,  &isSuccessfull, &gHeight, &gWidth );
#elif for_android
    CMD_Populate_Datagrid cmd_populate_livetv_grid(m_dwPK_Device, iPK_Device_DatagridPlugIn, StringUtils::itos( m_dwIDataGridRequestCounter ), string(m_sGridID),11, m_UserID + "," + m_EA, 0, &pkVar, &valassign,  &isSuccessfull, &gHeight, &gWidth );
#else
    CMD_Populate_Datagrid cmd_populate_livetv_grid(m_dwPK_Device, iPK_Device_DatagridPlugIn, StringUtils::itos( m_dwIDataGridRequestCounter ), string(m_sGridID), 11, m_UserID + "," + m_EA, 0, &pkVar, &valassign,  &isSuccessfull, &gHeight, &gWidth );
#endif
    if (SendCommand(cmd_populate_livetv_grid))
    {
        /*
              initial request to populate the text only grid as denoted by the lack of a leading "_" as in _MediaFile_43
              this way, we can safely check empty grids and error gracefully in the case of no matching media
              */

        //standard text grid with no images. this will not crash the router if requested and its empty, picture grids will

        //CMD_Request_Datagrid_Contents(long DeviceIDFrom, long DeviceIDTo,                   string sID,                                              string sDataGrid_ID,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset,    char **pData,int *iData_Size,int *iRow,int *iColumn
        DCE::CMD_Request_Datagrid_Contents req_data_grid( long(m_dwPK_Device), long(iPK_Device_DatagridPlugIn), StringUtils::itos( m_dwIDataGridRequestCounter ), string(m_sGridID),    int(gWidth), int(gHeight),           false, false,        true,   string(m_sSeek),    int(iOffset),  &pData,         &iData_Size, &GridCurRow, &GridCurCol );
        if(SendCommand(req_data_grid))
        {
            //creating a dg table to check for cells. If 0, then we error out and provide a single "error cell"
            DataGridTable *pDataGridTable = new DataGridTable(iData_Size,pData,false);

            //LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Attribute Datagrid Dimensions: Height %i, Width %i", gHeight, gWidth);
            QString channelName;
            QString channelIndex;
            QString program;
            int index = 0;
            int channelNumber;
            QImage channelimage;
            DataGridCell *pCell;

            for(MemoryDataTable::iterator it=pDataGridTable->m_MemoryDataTable.begin();it!=pDataGridTable->m_MemoryDataTable.end();++it)
            {
                QApplication::processEvents(QEventLoop::AllEvents);
                pCell = it->second;
                QStringList breaker = QString::fromStdString(pCell->m_mapAttributes_Find("Name").c_str()).split(" ");
                channelName = breaker.at(1);
                channelNumber = breaker.at(0).toInt();
                channelIndex = pCell->GetValue();
                program = QString::fromStdString(pCell->GetText());
                program.remove(channelName);
                program.remove(breaker.at(0));
                EPGItemClass *t = new EPGItemClass(channelName, channelNumber, channelIndex, program, index, channelimage, channelimage);
                emit addChannel(t);
                index++;
                QApplication::processEvents(QEventLoop::AllEvents);
            }
        }
    }
    else
    {
        emit statusMessage("Could Not Populate");
    }

    if(i_current_mediaType == 1)
    {
        emit epgDone();
    }
    else
    {
        emit livetvDone();
    }
}


void DCE::qOrbiter::TuneToChannel(QString channel, QString chanid) //tunes to channel based on input, need some reworking for myth
{

    if(i_current_mediaType = 11)
    {
        setMediaResponse("Setting Channel! " + channel);
        CMD_Tune_to_channel changeChannel(m_dwPK_Device, iMediaPluginID, chanid.toStdString(), chanid.toStdString());
        SendCommand(changeChannel);
        emit np_channel(chanid);
        emit liveTvUpdate(chanid);
        emit livetvDone();
    }
    else
    {

        if(channel.contains("i"))
        {
            CMD_Tune_to_channel mythChannel(m_dwPK_Device, iMediaPluginID, channel.toStdString(), channel.toStdString());
            SendCommand(mythChannel);
        }
        else
        {

        }



    }

}


void DCE::qOrbiter::mute()
{
    CMD_Mute muteAudio(m_dwPK_Device, iMediaPluginID);
    SendCommand(muteAudio);
}


void DCE::qOrbiter::changedTrack(QString direction)
{
    string pResponse="";
    if (i_current_mediaType == 1)
    {
        if(direction == "+1")
        {
            CMD_Channel_up tvChanUp(m_dwPK_Device, iMediaPluginID );
            if(SendCommand(tvChanUp, &pResponse) && pResponse =="OK")
            {
                emit mediaMessage("Channel Up sent");
            }
        }
        else
        {
            CMD_Channel_up tvChanDwn(m_dwPK_Device, iMediaPluginID );
            if(SendCommand(tvChanDwn, &pResponse) && pResponse =="OK")
            {
                emit mediaMessage("Channel down sent");
            }
        }


    }
    else
    {
        CMD_Jump_Position_In_Playlist jump_playlist(m_dwPK_Device, iMediaPluginID, direction.toStdString(), internal_streamID );
        string sResponse = "";
        if(SendCommand(jump_playlist, &sResponse) && sResponse=="OK")
        {
            emit mediaMessage("Jumped to" + direction);
        }
    }
}

void DCE::qOrbiter::populateAdditionalMedia() //additional media grid that populates after the initial request to break out the threading and allow for a checkpoint across threads
{

    if(checkLoadingStatus() == true)
    {
        emit statusMessage("requesting additional media");

#ifdef for_desktop
        int gHeight = 1;
        int gWidth = 1;
        int offset = 0;
        int GridCurRow =i_currentMediaModelRow;
        int GridCurCol= 0;
        string m_sSeek = "" ;
        int iOffset=0;
#else
        int gHeight = 1;
        int gWidth = media_pageSeperator;
        int offset = 0;
        int GridCurRow =media_currentPage ;
        int GridCurCol= 0 ;
        int iOffset=0;
        //qDebug() << "our target is row:" << GridCurCol;
        string m_sSeek = "" ;

#endif

        string imgDG ="_MediaFile_"+QString::number(m_dwPK_Device).toStdString();

        int iData_Size=0;
        char *pData;
        pData = "NULL";
        //CMD_Request_Datagrid_Contents(                              long DeviceIDFrom,                long DeviceIDTo,                   string sID,                                string sDataGrid_ID, int iRow_count,int iColumn_count,        bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,       int iOffset,    char **pData,int *iData_Size,int *iRow,int *iColumn
#ifdef for_desktop
        DCE::CMD_Request_Datagrid_Contents req_data_grid_pics( long(m_dwPK_Device), long(iPK_Device_DatagridPlugIn), StringUtils::itos( m_dwIDataGridRequestCounter ), string(imgDG),    1, 1,                     false,                 false,                                 true, string(m_sSeek),   iOffset,  &pData,         &iData_Size, &GridCurRow, &GridCurCol );
#else
        DCE::CMD_Request_Datagrid_Contents req_data_grid_pics( long(m_dwPK_Device), long(iPK_Device_DatagridPlugIn), StringUtils::itos( m_dwIDataGridRequestCounter ), string(imgDG),    1, media_pageSeperator,                     false,                 false,                                 true, string(m_sSeek),   iOffset,  &pData,         &iData_Size, &GridCurRow, &GridCurCol );
#endif
        std::string pResponse ="";
        if(SendCommand(req_data_grid_pics, &pResponse) && pResponse == "OK")
        {
            DataGridTable *pDataGridTable = new DataGridTable(iData_Size,pData,false);
            setMediaResponse("grid request ok");

            // LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Pic Datagrid Dimensions: Height %i, Width %i", gHeight, gWidth);
            DataGridCell *pCell;
            QString cellTitle;
            QString fk_file;
            QString filePath;
            int index;
            QImage cellImg;

            for(MemoryDataTable::iterator it=pDataGridTable->m_MemoryDataTable.begin();it!=pDataGridTable->m_MemoryDataTable.end();++it)
            {
                pCell= it->second;
                const char *pPath = pCell->GetImagePath();
                filePath = QString::fromUtf8(pPath);
                fk_file = pCell->GetValue();
                cellTitle = QString::fromUtf8(pCell->m_Text);

                index = pDataGridTable->CovertColRowType(it->first).first;
                if (pPath )
                {
#ifndef ANDROID
                    cellImg = getfileForDG(pPath);
#else
                    cellImg = getfileForDG(pPath);
#endif
                }
                else
                {
                    cellImg.load(":/icons/icon.png");
                }

                emit addItem(new gridItem(fk_file, cellTitle, filePath, index, cellImg));
            }

            delete pDataGridTable;
        }
    }
    else
    {
        //qDebug("Grid set to stop, will not load more.");
    }

}


void DCE::qOrbiter::SetSecurityMode(int pin, int mode)
{
    CMD_Set_House_Mode change_house_modes(m_dwPK_Device, iPK_Device_SecurityPlugin, StringUtils::itos(mode),i_user, StringUtils::itos(pin), 0, "");
    SendCommand(change_house_modes);
}

void DCE::qOrbiter::setLocation(int location, int ea) // sets the ea and room
{
    i_room = location;
    i_ea = ea;

    CMD_Set_Entertainment_Area_DL set_entertain_area(m_dwPK_Device, StringUtils::itos(iOrbiterPluginID), StringUtils::itos(ea));
    SendCommand(set_entertain_area);

    CMD_Set_Current_Room_DL set_current_room(m_dwPK_Device, StringUtils::itos(iOrbiterPluginID), location);
    SendCommand(set_current_room);

}

void DCE::qOrbiter::setUser(int user)
{
    CMD_Set_Current_User_DL set_user(m_dwPK_Device, StringUtils::itos(iPK_Device_GeneralInfoPlugin), user);
    SendCommand(set_user);
}

void DCE::qOrbiter::QuickReload() //experimental function. checkConnection is going to be our watchdog at some point, now its just um. there to restart things.
{
    Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, m_sHostName);
    event_Impl.m_pClientSocket->SendString( "RELOAD" );


}

void DCE::qOrbiter::powerOn(QString devicetype)
{

    if (devicetype == "DISPLAY")
    {

    }

}

void DCE::qOrbiter::getMediaTimeCode()
{

}

void DCE::qOrbiter::GetAdvancedMediaOptions() // prepping for advanced media options
{
    int cellsToRender= 0;

    int gHeight = 1;
    int gWidth = 8;
    string imgDG ="resetav_"+StringUtils::itos(m_dwPK_Device);
    int iData_Size=0;
    int GridCurRow = 0;
    int GridCurCol= 0;
    char *pData;
    pData = "NULL";
    string m_sSeek = "";
    m_dwIDataGridRequestCounter++;
    int offset = 0;
    //CMD_Request_Datagrid_Contents(                              long DeviceIDFrom,                long DeviceIDTo,                   string sID,                                string sDataGrid_ID, int iRow_count,int iColumn_count,        bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,       int iOffset,    char **pData,int *iData_Size,int *iRow,int *iColumn
    DCE::CMD_Request_Datagrid_Contents advanced_av_grid( long(m_dwPK_Device), long(iPK_Device_DatagridPlugIn), StringUtils::itos( m_dwIDataGridRequestCounter ), string(imgDG),    100,    int(gHeight),                      false,                 false,                                 true, string(m_sSeek), 0   ,  &pData,         &iData_Size, &GridCurRow, &GridCurCol );

    if(SendCommand(advanced_av_grid))
    {
        DataGridTable *pDataGridTable = new DataGridTable(iData_Size,pData,false);
        cellsToRender= pDataGridTable->GetRows();

#ifndef ANDROID
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Advanced AV  Grid Dimensions: Height %i, Width %i", gHeight, gWidth);
#endif
        QString cellTitle;
        QString fk_file;
        QString filePath;
        int index;
        for(MemoryDataTable::iterator it=pDataGridTable->m_MemoryDataTable.begin();it!=pDataGridTable->m_MemoryDataTable.end();++it)
        {
            DataGridCell *pCell = it->second;
            const char *pPath = pCell->GetImagePath();
            filePath = QString::fromUtf8(pPath);
            fk_file = pCell->GetValue();
            cellTitle = QString::fromUtf8(pCell->m_Text);
            index = pDataGridTable->CovertColRowType(it->first).first;
        }
    }

}

/*
  problem function that gets dg of alarms, but i cant get traversal right and end up with one!
  its supposed to select the alarms and present them in a dg. Dg selection works currently, parsing does not.
  */

void DCE::qOrbiter::GetAlarms(bool toggle, int grp)
{
    bool state;
    string *sResponse;
    if (toggle == true)
    {

        CMD_Toggle_Event_Handler toggleAlarm(m_dwPK_Device, iPK_Device_eventPlugin, grp);
        SendCommand(toggleAlarm);
    }

    emit statusMessage("Getting alarm state");
    int cellsToRender= 0;
    int gHeight = 4;
    int gWidth = 3;
    string dgName ="Sleeping_Alarms_"+StringUtils::itos(m_dwPK_Device);
    int iData_Size=0;
    int GridCurRow = 0;
    int GridCurCol= 0;
    char *pData;
    pData = "NULL";
    string m_sSeek = "";
    m_dwIDataGridRequestCounter++;
    string option =StringUtils::itos(i_room);

    int offset = 0;
    int pkVar  = 0;
    int iOffset= 0;
    string valassign = "0";
    bool isSuccessfull;

    CMD_Populate_Datagrid sleepingAlarms(m_dwPK_Device, iPK_Device_DatagridPlugIn, StringUtils::itos( m_dwIDataGridRequestCounter ), string(dgName), 29, option, 0, &pkVar, &valassign,  &isSuccessfull, &gHeight, &gWidth );

    if (SendCommand(sleepingAlarms))
    {
        /*
              initial request to populate the text only grid as denoted by the lack of a leading "_" as in _MediaFile_43
              this way, we can safely check empty grids and error gracefully in the case of no matching media
              */

        //CMD_Request_Datagrid_Contents(long DeviceIDFrom, long DeviceIDTo,                   string sID,                                              string sDataGrid_ID,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset,    char **pData,int *iData_Size,int *iRow,int *iColumn
        DCE::CMD_Request_Datagrid_Contents sleeping_alarms( long(m_dwPK_Device), long(iPK_Device_DatagridPlugIn), StringUtils::itos( m_dwIDataGridRequestCounter ), string(dgName),    int(gWidth), int(gHeight),           false, false,        true,   string(m_sSeek),    int(iOffset),  &pData,         &iData_Size, &GridCurRow, &GridCurCol );
        if(SendCommand(sleeping_alarms))
        {
            DataGridTable *pDataGridTable = new DataGridTable(iData_Size,pData,false);
            cellsToRender= pDataGridTable->GetRows();
#ifndef ANDROID
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "sleeping menu alarms Grid Dimensions: Height %i, Width %i", gHeight, gWidth);
#endif
            QString name;
            QString days;
            QString timeleft;
            QString alarmtime;
            int eventgrp;
            int counter = 0;
            int col = 0;
            int row = 0;
            for (int counter = 0; counter < cellsToRender; counter++)
            {
                DataGridCell *pCell = pDataGridTable->GetData(row,col);

                if (row == 0)
                {
                    QString test = pCell->GetText();
                    if (test == "ON")
                    {
                        state = true;

                    }
                    else
                    {
                        state= false;
                    }

                    row++;
                }

                else if (row == 1)
                {
                    eventgrp = atoi(pCell->GetValue());
                    QString data = pCell->GetText();
                    QStringList breakerbreaker = data.split(QRegExp("\n"), QString::KeepEmptyParts );
                    name = breakerbreaker.at(0);
                    days=breakerbreaker.at(2);
                    days.remove("Day of Week");
                    timeleft=breakerbreaker.at(1);
                    alarmtime=".";
                    // sleeping_alarms.append(new SleepingAlarm( eventgrp, name, alarmtime, state, timeleft, days));
                    row = -1;
                    col++;
                    row++;
                }
            }
        }
    }

}

//zoom level for current media player
void DCE::qOrbiter::setZoom(QString zoomLevel)
{
    string sResponse;
    CMD_Set_Zoom setMediaZoom(m_dwPK_Device, iMediaPluginID, internal_streamID, string(zoomLevel.toAscii()));
    if (SendCommand(setMediaZoom))
    {

    }
    else
    {
        emit statusMessage(QString("Error sending command!"));
    }
}


void DCE::qOrbiter::setAspect(QString ratio) //set aspect ratio for current media player
{
    string sResponse;
    CMD_Set_Aspect_Ratio setMediaAspect(m_dwPK_Device, iMediaPluginID, internal_streamID, ratio.toStdString());
    if (SendCommand(setMediaAspect))
    {

    }
    else
    {
        emit statusMessage("Error sending command!");
    }
}

void DCE::qOrbiter::GetText(int textno)
{

}

//used for resume to pass complex things like chapters and positions in playlists.
void DCE::qOrbiter::setPosition(QString position)
{
    /*
    if(currentScreen.contains( "Screen_49.qml"))

    {
        CMD_Set_Media_Position setPosition(m_dwPK_Device, m_dwPK_Device_NowPlaying, internal_streamID, position.toStdString());
        //qDebug("DVD pls change") ;
        if(!SendCommand(setPosition))
        {

        }
    }
    else
    {
        CMD_Set_Media_Position setPosition(m_dwPK_Device, ScreenParameters->getParam(186).toInt(), ScreenParameters->getParam(187).toInt(), position.toStdString());

        if(!SendCommand(setPosition))
        {

        }
    }
*/
}

void DCE::qOrbiter::showMenu() //show the dvd menu
{
    CMD_Goto_Media_Menu showDVDmenu(m_dwPK_Device, iMediaPluginID, internal_streamID, 0 );
    if(!SendCommand(showDVDmenu))
    {

    }

}

void DCE::qOrbiter::moveDirection(QString direction) //connects ui buttons to dce commands
{
    if(direction.contains("up"))
    {
        CMD_Move_Up moveUp(m_dwPK_Device, iMediaPluginID, internal_streamID);
        if(!SendCommand(moveUp))
        {

        }
    }
    else if(direction.contains("down"))
    {
        CMD_Move_Down moveDown(m_dwPK_Device, iMediaPluginID, internal_streamID);
        if(!SendCommand(moveDown))
        {

        }
    }
    else if(direction.contains("right"))
    {
        CMD_Move_Right move(m_dwPK_Device, iMediaPluginID, internal_streamID);
        if(!SendCommand(move))
        {

        }
    }
    else  if(direction.contains("left"))
    {
        CMD_Move_Left move(m_dwPK_Device, iMediaPluginID, internal_streamID);
        if(!SendCommand(move))
        {

        }
    }
    else if(direction.contains("enter"))
    {
        CMD_EnterGo enter(m_dwPK_Device, iMediaPluginID, internal_streamID);
        if(!SendCommand(enter))
        {

        }
    }
}

void DCE::qOrbiter::JogStream(QString jump) //jumps position in stream for jog
{
    CMD_Jump_to_Position_in_Stream jog(m_dwPK_Device, iMediaPluginID, jump.toStdString(), m_dwPK_Device_NowPlaying  );
    if(!SendCommand(jog))
    {

    }
}


/*
  Show advanced buttons is for the ir or other controls used to send specific commands to a device. It comes in the form of a data grid initially that lists
  the different devices. This can be cross - referenced with the now playing devices to show only relevant devices for the ui
  */
void DCE::qOrbiter::showAdvancedButtons()
{
    int cellsToRender= 0;

    int gHeight = 1;
    int gWidth = 8;
    string imgDG ="resetav_"+StringUtils::itos(m_dwPK_Device);
    int iData_Size=0;
    int GridCurRow = 0;
    int GridCurCol= 0;
    char *pData;
    pData = "NULL";
    string m_sSeek = "";
    m_dwIDataGridRequestCounter++;
    int iOffset = 0;
    string option ="";
    bool isSuccessfull;
    int pkVar = 0;
    string valassign = "";
    CMD_Populate_Datagrid cmd_populate_device_grid(m_dwPK_Device, iPK_Device_DatagridPlugIn, StringUtils::itos( m_dwIDataGridRequestCounter ), string(imgDG), 91, option, 0, &pkVar, &valassign,  &isSuccessfull, &gHeight, &gWidth );

    if (SendCommand(cmd_populate_device_grid))
    {

        //CMD_Request_Datagrid_Contents(long DeviceIDFrom, long DeviceIDTo,                   string sID,                                              string sDataGrid_ID,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset,    char **pData,int *iData_Size,int *iRow,int *iColumn
        DCE::CMD_Request_Datagrid_Contents req_device_grid( long(m_dwPK_Device), long(iPK_Device_DatagridPlugIn), StringUtils::itos( m_dwIDataGridRequestCounter ), string(imgDG),    int(gWidth), int(gHeight),           false, false,        true,   string(m_sSeek),    int(iOffset),  &pData,         &iData_Size, &GridCurRow, &GridCurCol );
        if(SendCommand(req_device_grid))
        {
            DataGridTable *pDataGridTable = new DataGridTable(iData_Size,pData,false);
            cellsToRender= pDataGridTable->GetRows();

#ifndef ANDROID
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Advanced AV  Grid Dimensions: Height %i, Width %i", gHeight, gWidth);
#endif

            QString cellTitle;
            QString fk_file;
            QString filePath;
            int index;
            for(MemoryDataTable::iterator it=pDataGridTable->m_MemoryDataTable.begin();it!=pDataGridTable->m_MemoryDataTable.end();++it)
            {

                DataGridCell *pCell = it->second;
                const char *pPath = pCell->GetImagePath();
                filePath = QString::fromUtf8(pPath);
                fk_file = pCell->GetValue();
                cellTitle = QString::fromUtf8(pCell->m_Text);
                index = pDataGridTable->CovertColRowType(it->first).first;
                emit statusMessage(cellTitle + "::" + fk_file);
                //buttonList.append(new AvDevice(fk_file.toInt(), cellTitle, "core"));
            }
            //  qorbiterUIwin->rootContext()->setContextProperty("buttonList" ,QVariant::fromValue(buttonList));
        }
    }
}

void DCE::qOrbiter::movePlaylistEntry(bool pos, int num)
{
    if (pos==true)
    {
        CMD_Move_Playlist_entry_Up move_entry_up(m_dwPK_Device, iOrbiterPluginID, num);
        SendCommand(move_entry_up);
    }
    else
    {
        CMD_Move_Playlist_entry_Down move_entry_down(m_dwPK_Device, iOrbiterPluginID, num);
        SendCommand(move_entry_down);
    }
}

void DCE::qOrbiter::addToPlaylist(bool now, std::string playlist)
{
    /*
      found out this command doesnt exist and need to be created. ack.
      */
}

void DCE::qOrbiter::grabScreenshot(QString fileWithPath)
{

    char *screenieData;         //screenshot data using the char** data structure for passing of data
    int screenieDataSize=0;       //var for size of data
    string s_format ="jpg";   //the format we want returned
    int imageH;                 //image height of desired screenshot
    int imageW;                 //width of desired screenshot
    string sDisableAspectLock = "0";


    qDebug() << fileWithPath;
    int gHeight = 0;
    int gWidth = 0;
    int pkVar = 0;
    string valassign ="";
    bool isSuccessfull;// = "false";
    string m_sGridID ="thumbs_"+StringUtils::itos(m_dwPK_Device); // the string identifier on the type of grid
    string m_sSeek;
    int iData_Size=0;
    int GridCurRow = 0;
    int GridCurCol= 0;

    char *pData;
    pData = "NULL";
    m_dwIDataGridRequestCounter++;

    CMD_Populate_Datagrid cmd_populate_attribute_grid(m_dwPK_Device, iPK_Device_DatagridPlugIn, StringUtils::itos( m_dwIDataGridRequestCounter ), string(m_sGridID), 31, QString::number(i_ea).toStdString(), 0, &pkVar, &valassign,  &isSuccessfull, &gHeight, &gWidth );

    if (SendCommand(cmd_populate_attribute_grid))
    {
        /*
              initial request to populate the text only grid as denoted by the lack of a leading "_" as in _MediaFile_43
              this way, we can safely check empty grids and error gracefully in the case of no matching media
              */

        string imgDG = m_sGridID; //standard text grid with no images. this will not crash the router if requested and its empty, picture grids will

        //CMD_Request_Datagrid_Contents(long DeviceIDFrom, long DeviceIDTo,                   string sID,                                              string sDataGrid_ID,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset,    char **pData,int *iData_Size,int *iRow,int *iColumn
        DCE::CMD_Request_Datagrid_Contents req_data_grid( long(m_dwPK_Device), long(iPK_Device_DatagridPlugIn), StringUtils::itos( m_dwIDataGridRequestCounter ), string(m_sGridID),    int(gWidth), int(gHeight),           false, false,        true,   string(m_sSeek),    0,  &pData,         &iData_Size, &GridCurRow, &GridCurCol );
        if(SendCommand(req_data_grid))
        {

            //not sure what its for
            //creating a dg table to check for cells. If 0, then we error out and provide a single "error cell"
            DataGridTable *pDataGridTable = new DataGridTable(iData_Size,pData,false);


#ifndef ANDROID
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Attribute Datagrid Dimensions: Height %i, Width %i", gHeight, gWidth);
#endif

            QString cellTitle;
            QString cellAttribute;
            int index;
            QString cellfk;
            DataGridCell *pCell;
            for(MemoryDataTable::iterator it=pDataGridTable->m_MemoryDataTable.begin();it!=pDataGridTable->m_MemoryDataTable.end();++it)
            {

                pCell = it->second;

                index = pDataGridTable->CovertColRowType(it->first).first;
                cellTitle = pCell->GetText();
                cellAttribute = pCell->GetValue();
                cellfk = pCell->GetValue();
                QStringList parser = cellTitle.split(QRegExp("(\\n|:\\s)"), QString::KeepEmptyParts);
                screenshotVars.append(new screenshotAttributes( cellfk, cellTitle, cellAttribute.prepend("!A") ));
            }

        }



         int iEK_File;
        CMD_Get_ID_from_Filename getFileID(m_dwPK_Device, iMediaPluginID, fileWithPath.toStdString(), &iEK_File);
        string fResp = "";
        if(SendCommand(getFileID, &fResp) && fResp == "OK")
        {
            QString fk;
            fk.append("!F"+ QString::fromStdString(StringUtils::itos(iEK_File)));
            emit statusMessage(fk);
            // string *fk = StringUtils::itos(&iEK_File);
   screenshotVars.prepend(new screenshotAttributes(fk, QString("Filename"),fk ));
        }


    }
    emit screenshotVariablesReady(screenshotVars);
    screenshotVars.clear();

    CMD_Get_Video_Frame grabMediaScreenshot(long(m_dwPK_Device), long(m_dwPK_Device_NowPlaying), sDisableAspectLock,internal_streamID, 800 , 800, &screenieData, &screenieDataSize, &s_format);
    string mpResp= "";

    if(SendCommand(grabMediaScreenshot, &mpResp) && mpResp =="OK")
    {
        QByteArray screenShotData;              //config file put into qbytearray for processing
        screenShotData.setRawData(screenieData, screenieDataSize);
        emit screenShotReady(screenShotData);
    }
}

/*
   Default color buttons for the sat settop boxes and VDR remotes.
*/
void DCE::qOrbiter::redButton()
{
    CMD_Red pressRed(m_dwPK_Device, iMediaPluginID);
    SendCommand(pressRed);
}

void DCE::qOrbiter::greenButton()
{
    CMD_Green pressGreen(m_dwPK_Device, iMediaPluginID);
    SendCommand(pressGreen);
}

void DCE::qOrbiter::yellowButton()
{
    CMD_Yellow pressYellow(m_dwPK_Device, iMediaPluginID);
    SendCommand(pressYellow);
}

void DCE::qOrbiter::blueButton()
{
    CMD_Blue pressBlue(m_dwPK_Device, iMediaPluginID);
    SendCommand(pressBlue);
}

void DCE::qOrbiter::powerOff(QString deviceType)
{
}

void DCE::qOrbiter::CopyDisc()
{
}

void DCE::qOrbiter::ShowBookMarks()
{
}

void qOrbiter::getImage()
{
}

void DCE::qOrbiter::processScreenShot(char picData, int picDataSize, std::string fileFormat)
{
}

void DCE::qOrbiter::adjustVolume(int vol)
{
    if (vol > 0)
    {
        CMD_Vol_Up raiseVol(m_dwPK_Device, iMediaPluginID, vol);
        SendCommand(raiseVol);
    }
    else
    {
        CMD_Vol_Down lowerVolume(m_dwPK_Device, iMediaPluginID, vol);
        SendCommand(lowerVolume);
    }
}

void DCE::qOrbiter::OnDisconnect()
{
    emit routerDisconnect();
}

void DCE::qOrbiter::OnReload()
{

    emit routerReloading("Router Reload");
    DisconnectAndWait();


}

void qOrbiter::OnReplaceHandler()
{
    emit replaceDevice();
    this->deinitialize();
    emit closeOrbiter();
}

void DCE::qOrbiter::extraButtons(QString button)
{
    if(button.toLower() == "livetv")
    {
        CMD_Live_AV_Path selectLiveTvPath(m_dwPK_Device, iMediaPluginID, StringUtils::itos(i_ea), true);
        SendCommand(selectLiveTvPath);
    }

    if(button.toLower() == "schedule")
    {

    }

    if(button.toLower() == "recordings")
    {
        CMD_Recorded_TV_Menu showRecorded(m_dwPK_Device, m_dwPK_Device_NowPlaying);
        SendCommand(showRecorded);
    }

    if(button.toLower() == "music")
    {
        CMD_Music showMusic(m_dwPK_Device, m_dwPK_Device_NowPlaying );
        SendCommand(showMusic);
    }

    if(button.toLower() == "guide")
    {
        CMD_Guide showGuide(m_dwPK_Device, m_dwPK_Device_NowPlaying );
        SendCommand(showGuide);
    }

    if(button.toLower() == "menu")
    {
        CMD_Menu_Show_Menu showMenu(m_dwPK_Device, m_dwPK_Device_NowPlaying );
        SendCommand(showMenu);
    }

    if(button.toLower() == "info")
    {
        CMD_Info showInfo(m_dwPK_Device, m_dwPK_Device_NowPlaying, "" );
        SendCommand(showInfo);
    }

    if(button.toLower() == "favorites")
    {
        CMD_Favorites showFav(m_dwPK_Device, m_dwPK_Device_NowPlaying );
        SendCommand(showFav);
    }

    if(button.toLower() == "record")
    {
        CMD_Record record(m_dwPK_Device, m_dwPK_Device_NowPlaying );
        SendCommand(record);
    }

    if(button.toLower() == "help")
    {
        CMD_Help showHelp(m_dwPK_Device, m_dwPK_Device_NowPlaying );
        SendCommand(showHelp);
    }

    if(button.toLower() == "channelbookmark")
    {

    }

    if(button.toLower() == "exit")
    {
        CMD_Exit exitAVscreen(m_dwPK_Device, m_dwPK_Device_NowPlaying);
        SendCommand(exitAVscreen);
    }

}

void DCE::qOrbiter::saveScreenAttribute(QString attribute, QByteArray data)
{

    string sAttribute = attribute.toStdString();
    char *pData = (char*) data.constData();
    int pDataSize = data.size();
    CMD_Make_Thumbnail thumb(m_dwPK_Device, iMediaPluginID, sAttribute, pData, pDataSize );
    string cResp = "";
    if(SendCommand(thumb))
    {
        emit statusMessage("Created Thumbnail");
    }
    else
    {

    }
}

void DCE::qOrbiter::newOrbiter()
{
    //CMD_New_Orbiter newOrbiterDevice(int devicefrom, int deviceto, string sType, int iPK_Users, int iPK_devicetemplate, string sMacAddress, int pk_room, int width, int height, int skin, int lang, int size, int pkdevice);
    //SendCommand(newOrbiterDevice);
}

/*
  re-implementing from original orbiter as part of the orbiter setup. i dont know how it all works, but i have an idea
  PromptFor looks for the specific item it needs a list of, like users or rooms and then PromptUser will display the information
  on-screen so the user can choose and then a map of choices is broken down and submitted with the new orbiter command.
  line 146 in orbiter renderer.cpp for reference.
  */
int DCE::qOrbiter::PromptFor(std::string sToken)
{

}

int DCE::qOrbiter::PromptUser(std::string sPrompt, int iTimeoutSeconds, map<int, std::string> *p_mapPrompts)
{

}

void DCE::qOrbiter::adjustLighting(int level)
{
    if (level == 10)
    {
        CMD_Set_Level up(m_dwPK_Device, iPK_Device_LightingPlugin, StringUtils::itos(level));
        SendCommand(up);
    }
    else
    {
        CMD_Set_Level dn(m_dwPK_Device, iPK_Device_LightingPlugin, StringUtils::itos(level));
        SendCommand(dn);

    }
}

int DCE::qOrbiter::DeviceIdInvalid()
{


    emit statusMessage("Device ID is invalid. Finding Existing Orbiters of this type");
    QApplication::processEvents(QEventLoop::AllEvents);
    QList<QObject*>  temp_orbiter_list ;

    map<int,string> mapDevices;
    GetDevicesByTemplate(PK_DeviceTemplate_get(),&mapDevices);

    if( mapDevices.size()==0 )
    {
        emit statusMessage("No orbiters of this type found. Would you like to setup a new one?");
        QApplication::processEvents(QEventLoop::AllEvents);
        return 0;
    }

    for(map<int,string>::iterator it=mapDevices.begin();it!=mapDevices.end();++it)
    {

        temp_orbiter_list.append(new ExistingOrbiter((int)it->first, QString::fromStdString(it->second)));
        cout << it->first << " " << it->second << endl;
    }
    emit deviceInvalid(temp_orbiter_list);
    return 0;


}

void DCE::qOrbiter::prepareFileList(int iPK_MediaType)
{
    q_mediaType = QString::number(iPK_MediaType);
    requestMore = true;
    i_currentMediaModelRow = 0;

    setMediaResponse("Initial media request");
    //emit cleanupGrid();
#ifdef for_desktop
    int gHeight = 1;            //how many rows we want
    int gWidth = 0;             //how many columns we want. in this case, just the one
    int pkVar = 0;              // ??
    int iOffset = 0;            // ??
    int GridCurRow = 0;         //the row to start from
    int GridCurCol= 0;           //column to start from
#else
    int gHeight = media_pageSeperator;
    int gWidth = 0;
    int pkVar = 0;
    int iOffset = 0;
    int GridCurRow = 0;
    int GridCurCol= 0;
#endif

    string valassign ="";              // ??
    bool isSuccessfull;// = "false";   // holdover from the old orbiter?

    string m_sGridID ="MediaFile_"+QString::number(m_dwPK_Device).toStdString(); //identfier that specifies the orbiter device it goes to
    string m_sSeek=""; //the theoretical seek

    int iData_Size=0;
    char *pData;
    pData = "NULL";

    m_dwIDataGridRequestCounter++;  //request counter dont know what its used for.

    QString params;                 // param string for the datagrid options as specified in initializeSort()
    QString s;

    if(backwards == true)                               //this deals with screen handlers
    {
        emit statusMessage("Going back a level");

    }
    else
    {
        //video
        if (iPK_MediaType == 5)
        {
            if (q_attributetype_sort == "")
            {
                q_attributetype_sort = videoDefaultSort;
            }
            else if(q_attributetype_sort == "12" && q_pk_attribute !="") //catches program
            {
                q_attributetype_sort= "52";
            }
            else if(q_attributetype_sort == "1" && q_pk_attribute !="") //catches director
            {
                q_attributetype_sort = "13";
            }
            else if (q_attributetype_sort =="2" )
            {
                q_attributetype_sort = "13";
            }
            else if (q_attributetype_sort =="8")
            {
                q_attributetype_sort = "13";
            }
            else if(q_attributetype_sort =="52")
            {
                q_attributetype_sort = "13";
            }

        }

        //audio
        if (iPK_MediaType == 4)
        {
            if (q_attributetype_sort == "" )
            {
                q_attributetype_sort = audioDefaultSort;
            }
            else if(q_attributetype_sort == "2" && q_pk_attribute !="") //catches performer
            {
                q_attributetype_sort= "3";
            }
            else if (q_attributetype_sort =="8"  && q_pk_attribute !="")
            {
                q_attributetype_sort = "3";
            }
            else if(q_attributetype_sort =="3")
            {
                q_attributetype_sort = "13";
            }

        }
        //photos


        //playlists


        //games

    }


    params = "|"+q_subType +"|"+q_fileFormat+"|"+q_attribute_genres+"|"+q_mediaSources+"||"+q_attributetype_sort+"||2|"+q_pk_attribute+"";
    s = q_mediaType + params;
    if(backwards == false)
    {
        q_mediaType = QString::number(iPK_MediaType);
        goBack<< s;
    }
    //qDebug() << s;
    CMD_Populate_Datagrid populateDataGrid(m_dwPK_Device, iPK_Device_DatagridPlugIn, StringUtils::itos( m_dwIDataGridRequestCounter ), string(m_sGridID), 63, s.toStdString(), DEVICETEMPLATE_Datagrid_Plugin_CONST, &pkVar, &valassign,  &isSuccessfull, &gHeight, &gWidth );

    if (SendCommand(populateDataGrid))
    {

        /*
              initial request to populate the text only grid as denoted by the lack of a leading "_" as in _MediaFile_43
              this way, we can safely check empty grids and error gracefully in the case of no matching media
              */

        //standard text grid with no images. this will not crash the router if requested and its empty, picture grids will

        //CMD_Request_Datagrid_Contents(long DeviceIDFrom, long DeviceIDTo,                   string sID,                                              string sDataGrid_ID,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset,    char **pData,int *iData_Size,int *iRow,int *iColumn

        DCE::CMD_Request_Datagrid_Contents req_data_grid( long(m_dwPK_Device), long(iPK_Device_DatagridPlugIn), StringUtils::itos( m_dwIDataGridRequestCounter ), string(m_sGridID),    gHeight, gWidth,           false, false,        true,   string(m_sSeek),    iOffset,  &pData,         &iData_Size, &GridCurRow, &GridCurCol );
        if(SendCommand(req_data_grid))
        {
            //not sure what its for
            //creating a dg table to check for cells. If 0, then we error out and provide a single "error cell"
            DataGridTable *pDataGridTable = new DataGridTable(iData_Size,pData,false);
            int cellsToRender= pDataGridTable->GetRows();
#ifndef ANDROID
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Datagrid Dimensions: Height %i, Width %i", gHeight, gWidth);
#endif
            if (cellsToRender == 0)
            {
                //performing checks
                delete pDataGridTable;
                // exit ; //exit the loop because there is no grid? - eventually provide "no media" feedback
            }
            else
            {
                emit gridModelSizeChange(pDataGridTable->GetRows());
                i_mediaModelRows = cellsToRender;
                QList <QObject*> modelPages;
                int iPages = cellsToRender / media_pageSeperator; //15 being the items per page.
                for (int i=0; i < (iPages+1); i++)
                {
                    modelPage * item = new modelPage(i, QString::number(i));
                    modelPages.append(item);
                }

                emit modelPageCount(modelPages);
                //qDebug() << cellsToRender << " Cells converted to " << modelPages.count() ;


                string imgDG = "_"+m_sGridID; //correcting the grid id string for images

                //CMD_Request_Datagrid_Contents(                              long DeviceIDFrom,                long DeviceIDTo,                   string sID,                                string sDataGrid_ID, int iRow_count,int iColumn_count,        bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,       int iOffset,    char **pData,int *iData_Size,int *iRow,int *iColumn
#ifdef for_desktop
                DCE::CMD_Request_Datagrid_Contents req_data_grid_pics( long(m_dwPK_Device), long(iPK_Device_DatagridPlugIn), StringUtils::itos( m_dwIDataGridRequestCounter ), string(imgDG),    1, 1,                     false,                 false,                                 true, string(m_sSeek),   iOffset,  &pData,         &iData_Size, &GridCurRow, &GridCurCol );
#else
                DCE::CMD_Request_Datagrid_Contents req_data_grid_pics( long(m_dwPK_Device), long(iPK_Device_DatagridPlugIn), StringUtils::itos( m_dwIDataGridRequestCounter ), string(imgDG),    1, media_pageSeperator,                     false,                 false,                                 true, string(m_sSeek),   iOffset,  &pData,         &iData_Size, &GridCurRow, &GridCurCol );
#endif
                if(SendCommand(req_data_grid_pics))
                {
                    DataGridTable *pDataGridTable = new DataGridTable(iData_Size,pData,false);

                    emit statusMessage("sent image dg request");
#ifndef ANDROID
                    LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Pic Datagrid Dimensions: Height %i, Width %i", gHeight, gWidth);
#endif
                    QString cellTitle;
                    QString fk_file;
                    QString filePath;
                    int index;
                    QImage cellImg;
                    DataGridCell *pCell;

                    for(MemoryDataTable::iterator it=pDataGridTable->m_MemoryDataTable.begin();it!=pDataGridTable->m_MemoryDataTable.end();++it)
                    {

                        pCell = it->second;
                        const char *pPath = pCell->GetImagePath();
                        filePath = QString::fromUtf8(pPath);
                        fk_file = pCell->GetValue();
                        cellTitle = QString::fromUtf8(pCell->m_Text);

                        index = pDataGridTable->CovertColRowType(it->first).first;
                        if (pPath )
                        {
#ifndef ANDROID
                            cellImg = getfileForDG(pPath);
#else
                            cellImg = getfileForDG(pPath);
#endif

                        }
                        else
                        {
                            cellImg.load(":/icons/icon.png");
                        }
                        emit addItem(new gridItem(fk_file, cellTitle, filePath, index, cellImg));

                    }


                }
            }
            delete pDataGridTable;
        }
    }



    /*
          Datagrid params
          # 4 PK_Variable (int) (Out)  The populate grid can optionally return a variable number
                                        to assign a value into. For example, the current path in
                                        the file grid.

          # 5 Value To Assign (string) (Out) The value to assign into the variable.

          # 10 ID (string)              For debugging purposes if problems arise with a request
                                        not being filled, or a grid not populated when it should be.
                                        If the Orbiter specified an ID when requesting the grid or populating it,
                                         the Datagrid plug-in will log the ID and status so the develope

          # 15 DataGrid ID (string)     A unique ID for this instance of the grid that will be passed with the
                                        Request Datagrid Contents command.

          # 38 PK_DataGrid (int)        Which grid should be populated

          # 39 Options (string)         The options are specific the type of grid (PK_Datagrid). These are not
                                        pre-defined. The grid generator and orbiter must both pass the options
                                        in the correct format for the type of grid.

         # 40 IsSuccessful (bool) (Out) Returns false if the grid could not be populated. Perhaps there was no
                                        registered datagrid generator.

         # 44 PK_DeviceTemplate (int)   If more than 1 plugin registered to handle this grid, this parameter
                                         will be used to match teh right one

         # 60 Width (int) (Out)         The width of the grid, in columns, if the width is determined at
                                        populate time, such as a file grid. If the whole size of the grid is unknown,
                                        such as the EPG grid, this should be 0.

        # 61 Height (int) (Out)         The height of the grid, in rows, if the heightis determined at populate time,
                                        such as a file grid. If the whole size of the grid is unknown, such as the EPG grid,
                                         this should be 0.
    */

}

void DCE::qOrbiter::cleanupGrid()
{
    emit statusMessage("Clearing Grid");
    //emit clearPageGrid();

}

bool qOrbiter::checkLoadingStatus()
{
    if(i_currentMediaModelRow < i_mediaModelRows && requestMore == true )
    {

        //emit statusMessage("Count" + QString::number(i_currentMediaModelRow)+ "/" + QString::number(i_mediaModelRows));
        return true;
    }
    else
    {
        //emit statusMessage("Loading Ended");


        return false;

    }
}

void qOrbiter::setupTimeCode()
{

    //initializing threading for timecode to prevent blocking

}


