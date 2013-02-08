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
#ifndef IRTrans_h
#define IRTrans_h

//	DCE Implemenation for #1715 IRTrans IRT-USB Transm/Receive

#include "Gen_Devices/IRTransBase.h"
//<-dceag-d-e->

/*
	We will set a call back function for IRTrans CallBackFn=&DoGotIRCommand; so GotIRCommand is called when
	keys are pressed, and will receive the keyname and the remote.  We pass this back to IRReceiver Base with
	ReceivedCode(PK_Device,pCommand);, which handles processing the code.

	if run with device id -1003 (DEVICEID_MESSAGESEND) that means orbiter isn't running, we're just booting up,
	and want to use the i/r remote to simulate the u/d/l/r/enter and +/- keys for A/V Wizard
*/

#include "../LIRC_DCE/IRReceiverBase.h"
#include "../VFD_LCD/VFD_LCD_Base.h"
#include "IRBase/IRBase.h"

//<-dceag-decl-b->!
namespace DCE
{
	class IRTrans : public IRTrans_Command, public IRReceiverBase, public VFD_LCD_Base, public IRBase
	{
//<-dceag-decl-e->
		// Private member variables
		bool m_bIRServerRunning;
		map<string,int> m_mapNameToDevice;
		string m_sAltPort; // We will set this if we were found to be running on an alt port

		// Private methods
public:
		// Public member variables
		string m_sPort; // Only used when running local mode
		string m_sAVWHost;
		int m_iAVWPort;
		
		void StartIRServer();
		void GotIRCommand(const char *pRemote,const char *pCommand);
		void DoUpdateDisplay(vector<string> *vectString);  // Put this message on the VFD Display
		virtual void SendIR(string Port, string IRCode,int iRepeat); // Required from IRBase
		virtual void CreateChildren(); // Must override so we can call IRBase::Start() after creating children

//<-dceag-const-b->
public:
		// Constructors/Destructor
		IRTrans(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~IRTrans();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		virtual void OnQuit() { LoggerWrapper::GetInstance()->Write(LV_STATUS,"IRTrans will quit"); m_bQuit_VL = true; m_bQuit_set(true); Command_Impl::OnQuit(); };
		virtual void OnReload() { LoggerWrapper::GetInstance()->Write(LV_STATUS,"IRTrans will reload"); m_bQuit_VL = true; m_bQuit_set(true); Command_Impl::OnQuit(); }

//<-dceag-const2-b->!

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_COM_Port_on_PC();
	void DATA_Set_COM_Port_on_PC(string Value);
	bool DATA_Get_Only_One_Per_PC();
	bool DATA_Get_Autoassign_to_parents_room();
	bool DATA_Get_PNP_Create_Without_Prompting();
	bool DATA_Get_Immediate_Reload_Isnt_Necessar();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #191 - Send Code */
	/** Sends an I/R code to a device. */
		/** @param #9 Text */
			/** The I/R code -- usually in Pronto format */

	virtual void CMD_Send_Code(string sText) { string sCMD_Result; CMD_Send_Code(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Send_Code(string sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #194 - Toggle Power */
	/** Set relay state (0 or 1) */
		/** @param #8 On/Off */
			/** Depending on each device On/Off can be interpreted differently, but in genereal On/Off has a value of 1 for on and 0 for Off */

	virtual void CMD_Toggle_Power(string sOnOff) { string sCMD_Result; CMD_Toggle_Power(sOnOff.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Toggle_Power(string sOnOff,string &sCMD_Result,Message *pMessage);


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

	virtual void CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command) { string sCMD_Result; CMD_Learn_IR(iPK_Device,sOnOff.c_str(),iPK_Text,iPK_Command,sCMD_Result,NULL);};
	virtual void CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command,string &sCMD_Result,Message *pMessage);


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


	/** @brief COMMAND: #687 - Set Screen Type */
	/** Sent by Orbiter when the screen changes to tells the i/r receiver what type of screen is displayed so it can adjust mappings if necessary. */
		/** @param #48 Value */
			/** a character: M=Main Menu, m=other menu, R=Pluto Remote, r=Non-pluto remote, N=navigable OSD on media dev, f=full screen media app, F=File Listing, c=computing list, C=Computing full screen */

	virtual void CMD_Set_Screen_Type(int iValue) { string sCMD_Result; CMD_Set_Screen_Type(iValue,sCMD_Result,NULL);};
	virtual void CMD_Set_Screen_Type(int iValue,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
