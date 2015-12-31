/*
     Copyright (C) 2014 LinuxMCE 

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef CEC_Adaptor_h
#define CEC_Adaptor_h

//	DCE Implemenation for #2284 CEC Adaptor

#include "Gen_Devices/CEC_AdaptorBase.h"
//<-dceag-d-e->

#include <cec.h>
#include "IRBase/IRBase.h"
#include "../LIRC_DCE/IRReceiverBase.h"
#include "CECDevice.h"

//<-dceag-decl-b->
namespace DCE
{
  class CEC_Adaptor : public CEC_Adaptor_Command, public IRReceiverBase, IRBase
	{
//<-dceag-decl-e->
		// Private member variables
	  CEC::cec_logical_addresses m_CECAddresses;
	  CEC::cec_logical_addresses m_myCECAddresses;
	  std::map<uint16_t,uint8_t> m_mapPhysicalAddress_to_LA;
	  std::map<int,uint16_t> m_mapPK_Device_to_PA;

		// Private methods
public:
		// Public member variables
	  string m_sPort;   // COM Port on PC
	  CEC::ICECAdapter *m_pParser;
	  string m_sAVWHost;
	  int m_iAVWPort;
	  int m_dwPK_Device_IRPlugin;
	  virtual void PrepareToDelete();
	  map<string,pair<string,int> > m_mapCodesToButtons;
          map<uint64_t,cec_user_control_code> m_mapLMCEtoCECcodes;
	  std::vector<CECDevice> m_vectCecDevices;


//<-dceag-const-b->
public:
		// Constructors/Destructor
		CEC_Adaptor(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~CEC_Adaptor();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		virtual void CreateChildren();
		virtual void SendIR(string Port, string IRCode, int iRepeat); // Required from IRBase
		


//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		CEC_Adaptor(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_COM_Port_on_PC();
	bool DATA_Get_Ignore();
	bool DATA_Get_Only_One_Per_PC();
	bool DATA_Get_Autoassign_to_parents_room();
	bool DATA_Get_PNP_Create_Without_Prompting();
	bool DATA_Get_Immediate_Reload_Isnt_Necessar();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #191 - Send Code */
	/** The I/R code -- usually in Pronto format */
		/** @param #9 Text */
			/** The I/R code -- usually in Pronto format */

	virtual void CMD_Send_Code(string sText) { string sCMD_Result; CMD_Send_Code(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Send_Code(string sText,string &sCMD_Result,Message *pMessage);


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
