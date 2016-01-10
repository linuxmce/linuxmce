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
#ifndef Tira_h
#define Tira_h

//	DCE Implemenation for #1717 Tira (Pluto)

#include "Gen_Devices/TiraBase.h"
//<-dceag-d-e->

#include "../LIRC_DCE/IRReceiverBase.h"
#include "IRBase/IRBase.h"
#include "TiraAPI.h"

#define LEARNING_TIMEOUT 10 // Stop learning after this many seconds

//<-dceag-decl-b->!
namespace DCE
{
	class Tira : public Tira_Command, public IRReceiverBase, public IRBase
	{
//<-dceag-decl-e->
		// Private member variables
		int m_iPK_Device_Learning,m_iPK_Command_Learning,m_dwPK_Device_IRPlugin; // The device and command we are learning, and the IR Plugin
		int m_iPK_Orbiter,m_iPK_Text;
		map<string,pair<string,int> > m_mapCodesToButtons;
		time_t m_tLearningStarted;
		string m_sLastButton;
		timespec m_tsLastButton;
		pluto_pthread_mutex_t m_TiraMutex;  // This will also protect the callback map
		pthread_cond_t m_TiraCond;
		int m_iRepeat;

		// Private methods
public:
		// Public member variables
		bool m_bLearningIR,m_bAbortLearning; // True when the next IR code that comes in should be learned
		void GotIRCommand(const char *pRemote,const char *pCommand);
		virtual void SendIR(string Port, string IRCode,int iRepeat); // Required from IRBase
		virtual void CreateChildren(); // Must override so we can call IRBase::Start() after creating children
		virtual void OurCallback(const char *pButton);
		void StartLearning(int PK_Device,int PK_Command,int PK_Orbiter,int PK_Text);
		void StopLearning();
		void LearningThread();

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Tira(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Tira();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

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
	int DATA_Get_Repeat();
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

int __stdcall OurCalback(const char * eventstring);
