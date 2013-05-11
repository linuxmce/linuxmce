/*
     Copyright (C) 2013 LinuxMCE 

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef Roku_LTHDXDXS_Streaming_Pla_h
#define Roku_LTHDXDXS_Streaming_Pla_h

//	DCE Implemenation for #2049 Roku LT/HD/XD/XS Streaming Pla

#include "Gen_Devices/Roku_LTHDXDXS_Streaming_PlaBase.h"
//<-dceag-d-e->

#include "RokuDevice.h"

//<-dceag-decl-b->
namespace DCE
{
	class Roku_LTHDXDXS_Streaming_Pla : public Roku_LTHDXDXS_Streaming_Pla_Command
	{
//<-dceag-decl-e->
		// Private member variables
	  RokuDevice* m_pRokuDevice;
	  pluto_pthread_mutex_t m_RokuMutex;
		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Roku_LTHDXDXS_Streaming_Pla(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Roku_LTHDXDXS_Streaming_Pla();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		virtual void CreateChildren();

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Roku_LTHDXDXS_Streaming_Pla(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_COM_Port_on_PC();
	int DATA_Get_TCP_Port();
	int DATA_Get_FK_Device_Capture_Card_Port();
	string DATA_Get_AV_Adjustment_Rules();
	bool DATA_Get_Dont_Consolidate_Media();
	int DATA_Get_FK_Device_Capture_Card_Audio_P();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #48 - Eject Disk */
	/**  */
		/** @param #151 Slot Number */
			/** For jukeboxes, which slot to eject */

	virtual void CMD_Eject_Disk(int iSlot_Number) { string sCMD_Result; CMD_Eject_Disk(iSlot_Number,sCMD_Result,NULL);};
	virtual void CMD_Eject_Disk(int iSlot_Number,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** Change channels greater (larger numbers) */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID) { string sCMD_Result; CMD_Skip_Fwd_ChannelTrack_Greater(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
	/** Change channels lower (smaller numbers) */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Skip_Back_ChannelTrack_Lower(int iStreamID) { string sCMD_Result; CMD_Skip_Back_ChannelTrack_Lower(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Skip_Back_ChannelTrack_Lower(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #86 - Menu (Show Menu) */
	/** Show the on-screen menu */

	virtual void CMD_Menu_Show_Menu() { string sCMD_Result; CMD_Menu_Show_Menu(sCMD_Result,NULL);};
	virtual void CMD_Menu_Show_Menu(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #92 - Pause */
	/**  */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Pause(int iStreamID) { string sCMD_Result; CMD_Pause(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Pause(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #93 - Scan Fwd/Fast Fwd */
	/**  */

	virtual void CMD_Scan_FwdFast_Fwd() { string sCMD_Result; CMD_Scan_FwdFast_Fwd(sCMD_Result,NULL);};
	virtual void CMD_Scan_FwdFast_Fwd(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #95 - Stop */
	/**  */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #203 Eject */
			/** If true, the drive will be ejected if there is no media currently playing, so a remote's stop button acts as stop/eject. */

	virtual void CMD_Stop(int iStreamID,bool bEject) { string sCMD_Result; CMD_Stop(iStreamID,bEject,sCMD_Result,NULL);};
	virtual void CMD_Stop(int iStreamID,bool bEject,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #98 - Slow Scan Back */
	/**  */

	virtual void CMD_Slow_Scan_Back() { string sCMD_Result; CMD_Slow_Scan_Back(sCMD_Result,NULL);};
	virtual void CMD_Slow_Scan_Back(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #99 - Slow Scan Forward */
	/**  */

	virtual void CMD_Slow_Scan_Forward() { string sCMD_Result; CMD_Slow_Scan_Forward(sCMD_Result,NULL);};
	virtual void CMD_Slow_Scan_Forward(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #102 - Record */
	/**  */

	virtual void CMD_Record() { string sCMD_Result; CMD_Record(sCMD_Result,NULL);};
	virtual void CMD_Record(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #125 - Scan Back/Rewind */
	/**  */

	virtual void CMD_Scan_BackRewind() { string sCMD_Result; CMD_Scan_BackRewind(sCMD_Result,NULL);};
	virtual void CMD_Scan_BackRewind(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #139 - Play */
	/**  */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Play(int iStreamID) { string sCMD_Result; CMD_Play(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Play(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #190 - Enter/Go */
	/** Selects an item on the on-screen menu */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_EnterGo(int iStreamID) { string sCMD_Result; CMD_EnterGo(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_EnterGo(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #200 - Move Up */
	/** Move the highlighter to select something on-screen */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Up(int iStreamID) { string sCMD_Result; CMD_Move_Up(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #201 - Move Down */
	/** Move the highlighter to select something on-screen */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Down(int iStreamID) { string sCMD_Result; CMD_Move_Down(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #202 - Move Left */
	/** Move the highlighter to select something on-screen */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Left(int iStreamID) { string sCMD_Result; CMD_Move_Left(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #203 - Move Right */
	/** Move the highlighter to select something on-screen */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Move_Right(int iStreamID) { string sCMD_Result; CMD_Move_Right(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #225 - Display */
	/** Toggles the on-screen menu/display on/off */

	virtual void CMD_Display() { string sCMD_Result; CMD_Display(sCMD_Result,NULL);};
	virtual void CMD_Display(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #240 - Back / Prior Menu */
	/** Exits/goes back from the menu */
		/** @param #41 StreamID */
			/** ID of stream to apply */

	virtual void CMD_Back_Prior_Menu(int iStreamID) { string sCMD_Result; CMD_Back_Prior_Menu(iStreamID,sCMD_Result,NULL);};
	virtual void CMD_Back_Prior_Menu(int iStreamID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #350 - Process Incoming Data */
	/** This Internal command is sent to Ruby interpreter when data is availabe on input. Is used only in Generic Serial Devices. */

	virtual void CMD_Process_Incoming_Data() { string sCMD_Result; CMD_Process_Incoming_Data(sCMD_Result,NULL);};
	virtual void CMD_Process_Incoming_Data(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #351 - Process IDLE */
	/** This Internal command is sent to Ruby interpreter when it is in IDLE state. */

	virtual void CMD_Process_IDLE() { string sCMD_Result; CMD_Process_IDLE(sCMD_Result,NULL);};
	virtual void CMD_Process_IDLE(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #355 - Process Initialize */
	/** This Internal command is sent to Ruby interpreter when initialize occurs. */

	virtual void CMD_Process_Initialize() { string sCMD_Result; CMD_Process_Initialize(sCMD_Result,NULL);};
	virtual void CMD_Process_Initialize(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #356 - Process Release */
	/** This Internal command is sent to Ruby interpreter when release occurs. */

	virtual void CMD_Process_Release() { string sCMD_Result; CMD_Process_Release(sCMD_Result,NULL);};
	virtual void CMD_Process_Release(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #368 - Help */
	/** Bring up a help menu */

	virtual void CMD_Help() { string sCMD_Result; CMD_Help(sCMD_Result,NULL);};
	virtual void CMD_Help(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #373 - Private Method Listing */
	/** Used for ruby code mapping where user can add several private helper members. */

	virtual void CMD_Private_Method_Listing() { string sCMD_Result; CMD_Private_Method_Listing(sCMD_Result,NULL);};
	virtual void CMD_Private_Method_Listing(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #384 - Process Receive Command For Child */
	/** Method that will be called when command arrives for child device */

	virtual void CMD_Process_Receive_Command_For_Child() { string sCMD_Result; CMD_Process_Receive_Command_For_Child(sCMD_Result,NULL);};
	virtual void CMD_Process_Receive_Command_For_Child(string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
