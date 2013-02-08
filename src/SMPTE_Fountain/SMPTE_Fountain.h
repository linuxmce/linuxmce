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
#ifndef SMPTE_Fountain_h
#define SMPTE_Fountain_h

//	DCE Implemenation for #1815 SMPTE Fountain

#include "Gen_Devices/SMPTE_FountainBase.h"
//<-dceag-d-e->

extern long smpte_cur, smpte_stop;
extern bool alsa_soundout_shutdown;

#include "DCE/ClientSocket.h"
#include "Xine_Player/AskXine_Socket.h"

using namespace DCE;

namespace DCE
{
	class StartMediaInfo
	{
	public:
		bool m_bManuallyStart;
		string m_sFilename; 
		int m_iPK_MediaType, m_iStreamID;
		string m_sMediaPosition;
	};

	class TimeCodeInfo
	{
	public:
		long m_PadBefore,m_PadAfter;  // Seconds to pad 
		long m_StartTime; // The actual start time

		TimeCodeInfo(long PadBefore, long PadAfter, long StartTime) { m_PadBefore=PadBefore; m_PadAfter=PadAfter; m_StartTime=StartTime; }
	};

}


//<-dceag-decl-b->
namespace DCE
{
	class SMPTE_Fountain : public SMPTE_Fountain_Command
	{
//<-dceag-decl-e->
		// Private member variables
		StartMediaInfo *m_pStartMediaInfo;
		DeviceData_Base *m_pDevice_Xine;
		map<string, TimeCodeInfo *> m_mapFilesTimeCode;

		long m_smpteDefaultPreDelay, m_smpteXineStartupOffset;
		long m_smpteAdjustmentThreshold;
	
		long m_smpteSongOffset;

		long m_smpteStartXineTime, m_smpteXineReportedTime, m_smpteXineSongStop;

		bool m_bIsActive;


		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		SMPTE_Fountain(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~SMPTE_Fountain();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		void SynchronizationThread();
		void AskXineThread();

//<-dceag-const2-b->!

		bool MediaPlaying( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
		bool MediaStopped( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

		int GetLengthOfActiveXineStream();  // Returns the length of the currently active xine stream in milliseconds


//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Configuration();
	string DATA_Get_Alsa_Output_Device();

			*****EVENT***** accessors inherited from base class
	void EVENT_Device_Detected(string sMac_Address,string sText,string sIP_Address,int iPK_DeviceTemplate,string sVendorModelID,int iPK_CommMethod,int iPK_PnpProtocol,string sPNP_Serial_Number,string sDeviceData,string sCategory);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #192 - On */
	/** Turn on the device */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
		/** @param #98 PK_Device_Pipes */
			/** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

	virtual void CMD_On(int iPK_Pipe,string sPK_Device_Pipes) { string sCMD_Result; CMD_On(iPK_Pipe,sPK_Device_Pipes.c_str(),sCMD_Result,NULL);};
	virtual void CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #193 - Off */
	/** Turn off the device */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

	virtual void CMD_Off(int iPK_Pipe) { string sCMD_Result; CMD_Off(iPK_Pipe,sCMD_Result,NULL);};
	virtual void CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
