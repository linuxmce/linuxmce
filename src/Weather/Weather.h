/*
     Copyright (C) 2013 LinuxMCE 

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef Weather_h
#define Weather_h

//	DCE Implemenation for #2265 Weather

#include "Gen_Devices/WeatherBase.h"
//<-dceag-d-e->
//class Database_pluto_main;
#include <curl/curl.h>
#include <curl/easy.h>
#include "OpenWM.h"

//<-dceag-decl-b->
namespace DCE
{
	class Weather : public Weather_Command
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
		std::vector<std::string> &split_C(const std::string &s, char delim, std::vector<std::string> &elems);
		std::vector<std::string> split_C(const std::string &s, char delim);
public:
		// Public member variables

		//Add additional APIs here
		bool use_NOAA_;
		bool use_OWM_;
		bool use_WWO_;

		pluto_pthread_mutex_t m_MsgMutex;
		pthread_t m_thr_, i_thr_;
		pthread_mutex_t lock_x_;
		pthread_mutex_t m_p_mutx_;
		std::string lat_;
		std::string lon_;
		std::string units_;
		std::string lang_;
		std::string radar_;
		std::string api_key_;
		std::string city_;
		double timer_;
		pluto_pthread_mutex_t m_CurlMutex;
		//CURLM* m_pCurl;
		std::string cid;

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Weather(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Weather();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		// Database_pluto_main *m_pDatabase_pluto_main;
		std::string download(const std::string& url);

		/**This is the message sender to the Weather Plug-in.
		 * It is expecting to receive Vector<Vector<String> >
		 * as 2 or 3 fields defined as "Variable", "Text", "Value".
		 * Some weather variables do not need values**/
		void send_msg(vector<vector<string> > &v);

//<-dceag-const2-b->
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
		// You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
		Weather(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Configuration();

			*****EVENT***** accessors inherited from base class
	void EVENT_Outside_Temp_Changed(string sText,int iValue,string sName);
	void EVENT_Outside_Condition_Changed(string sText,int iValue,string sName);
	void EVENT_Outside_Humidity_Changed(string sText,int iValue,string sName);
	void EVENT_Outside_Pressure_Changed();
	void EVENT_Outside_Wind_Speed_Change(string sText,int iValue,string sName);
	void EVENT_Outside_Wind_Direction_Ch(string sText,int iValue,string sName);
	void EVENT_Outside_Visibility_Change(string sText,int iValue,string sName);
	void EVENT_Outside_Feels_Like_Change(string sText,int iValue,string sName);

			*****COMMANDS***** we need to implement
	*/

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
