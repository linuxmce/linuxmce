/*
     Copyright (C) 2013 LinuxMCE 

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef Weather_PlugIn_h
#define Weather_PlugIn_h

//	DCE Implemenation for #2263 Weather PlugIn

#include "Gen_Devices/Weather_PlugInBase.h"
//<-dceag-d-e->

#include "DCE/DCERouter.h"
#include <deque>
#include <set>
#include "RadarFrame.h"

//<-dceag-decl-b->
namespace DCE
{
	class Weather_PlugIn : public Weather_PlugIn_Command
	{
//<-dceag-decl-e->
		// Private member variables
	  map<string, int> m_mapWeatherValues;
	  map<string, string> m_mapWeatherTexts;
	  deque<RadarFrame> m_dequeRadarFrames;
	  map<long, deque<RadarFrame>::iterator > m_mapitRadarFrameDeque;
	  deque<RadarFrame>::iterator m_itRadarFrameDeque;
	  pluto_pthread_mutex_t m_Weather_PlugInMutex;

	  // Used for OrbiterRegistered
	  set<long> m_setOrbiters;

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Weather_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Weather_PlugIn();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		class Orbiter_Plugin *m_pOrbiter_Plugin;
		
		/**
		 * Event Message Interceptors
		 */
		bool DataChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );
		bool RadarChanged( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

		/**
		 * Command Message Interceptors
		 */
		bool OrbiterRegistered( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

		void DumpData();
		void UpdateOrbiterWeatherScenarios();
		void UpdateBoundIcon(string sDeviceIDs, string sWeatherValue, string sType);
		void UpdateText(string sDeviceIDs, string sPK_DesignObj, int iPK_Text, string sWeatherText);

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Weather_PlugIn(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #84 - Get Video Frame */
	/** Get Video Frame for Radar Video */
		/** @param #19 Data */
			/** The video frame */
		/** @param #20 Format */
			/** Format of the frame */
		/** @param #23 Disable Aspect Lock */
			/** Disable Aspect Ratio */
		/** @param #41 StreamID */
			/** The ID of the stream */
		/** @param #60 Width */
			/** Frame width */
		/** @param #61 Height */
			/** Frame height */

	virtual void CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat) { string sCMD_Result; CMD_Get_Video_Frame(sDisable_Aspect_Lock.c_str(),iStreamID,iWidth,iHeight,pData,iData_Size,sFormat,sCMD_Result,NULL);};
	virtual void CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1116 - Get Weather Text */
	/** Get one of the pieces of Accumulated weather texts. */
		/** @param #4 PK_Variable */
			/** Assign Requested Text to this Orbiter Variable */
		/** @param #19 Data */
			/** The Requested Weather Text given Name */
		/** @param #50 Name */
			/** Name of Weather Text Parameter to Return, e.g. "current_temp" */

	virtual void CMD_Get_Weather_Text(int iPK_Variable,string sName,char **pData,int *iData_Size) { string sCMD_Result; CMD_Get_Weather_Text(iPK_Variable,sName.c_str(),pData,iData_Size,sCMD_Result,NULL);};
	virtual void CMD_Get_Weather_Text(int iPK_Variable,string sName,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #1117 - Get Weather Value */
	/** Get one of the pieces of Accumulated weather values. */
		/** @param #4 PK_Variable */
			/** Orbiter Variable # to set. */
		/** @param #48 Value */
			/** The returned integer value */
		/** @param #50 Name */
			/** The name of the Weather value to return, e.g. "temp_current" */

	virtual void CMD_Get_Weather_Value(int iPK_Variable,string sName,int *iValue) { string sCMD_Result; CMD_Get_Weather_Value(iPK_Variable,sName.c_str(),iValue,sCMD_Result,NULL);};
	virtual void CMD_Get_Weather_Value(int iPK_Variable,string sName,int *iValue,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
