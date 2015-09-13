/*
     Copyright (C) 2013 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "Weather_PlugIn.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DCERouter.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_Room.h"
#include "pluto_main/Define_Array.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_FloorplanObjectType.h"
#include "pluto_main/Define_FloorplanObjectType_Color.h"
#include "pluto_main/Define_Text.h"
#include "PlutoUtils/HttpUtils.h"

#ifdef SIM_RADAR
#define SIM_RADAR_URL1 "http://images.weather.com/looper/archive/us_radar_plus_usen/1L.jpg"
#define SIM_RADAR_URL2 "http://images.weather.com/looper/archive/us_radar_plus_usen/2L.jpg"
#define SIM_RADAR_URL3 "http://images.weather.com/looper/archive/us_radar_plus_usen/3L.jpg"
#define SIM_RADAR_URL4 "http://images.weather.com/looper/archive/us_radar_plus_usen/4L.jpg"
#define SIM_RADAR_URL5 "http://images.weather.com/looper/archive/us_radar_plus_usen/5L.jpg"
#endif

#define FILE_NO_RADAR_DATA "/usr/pluto/share/weather_no_radar_data.jpg"

// Weather Icons
#define WEATHER_ICON_UNKNOWN 1

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Weather_PlugIn::Weather_PlugIn(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Weather_PlugIn_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	,m_Weather_PlugInMutex("weather plugin")
{
  m_Weather_PlugInMutex.Init(NULL);
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
Weather_PlugIn::Weather_PlugIn(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: Weather_PlugIn_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
	,m_Weather_PlugInMutex("weather plugin")
{
  m_Weather_PlugInMutex.Init(NULL);
}

//<-dceag-dest-b->
Weather_PlugIn::~Weather_PlugIn()
//<-dceag-dest-e->
{
  m_dequeRadarFrames.clear();
}

//<-dceag-getconfig-b->
bool Weather_PlugIn::GetConfig()
{
	if( !Weather_PlugIn_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

#ifdef SIM_RADAR
	// Load 5 temporary images in from
	string sResponse;
	string sBuffer1,sBuffer2,sBuffer3,sBuffer4,sBuffer5;
	sResponse=HttpGet(SIM_RADAR_URL1,&sBuffer1);
	sResponse=HttpGet(SIM_RADAR_URL2,&sBuffer2);
	sResponse=HttpGet(SIM_RADAR_URL3,&sBuffer3);
	sResponse=HttpGet(SIM_RADAR_URL4,&sBuffer4);
	sResponse=HttpGet(SIM_RADAR_URL5,&sBuffer5);
	m_dequeRadarFrames.push_back(RadarFrame(720,486,1,sBuffer1));
	m_dequeRadarFrames.push_back(RadarFrame(720,486,1,sBuffer2));
	m_dequeRadarFrames.push_back(RadarFrame(720,486,1,sBuffer3));
	m_dequeRadarFrames.push_back(RadarFrame(720,486,1,sBuffer4));
	m_dequeRadarFrames.push_back(RadarFrame(720,486,1,sBuffer5));
#else
      size_t nSize=0;
      char *pData = FileUtils::ReadFileIntoBuffer(FILE_NO_RADAR_DATA, nSize);
      if (pData && m_dequeRadarFrames.empty())
        {
          LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Weather_PlugIn::GetConfig() - Setting placeholder.");
          string sPlaceHolderImage=string(pData,nSize);
          m_dequeRadarFrames.push_back(RadarFrame(0,0,1,sPlaceHolderImage));
	}
#endif

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Weather_PlugIn::Register()
//<-dceag-reg-e->
{
  m_pOrbiter_Plugin=(Orbiter_Plugin*)m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
  if (!m_pOrbiter_Plugin)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to weather plugin");
      return false;
    }

  /** 
   * Register the Event interceptors we need to capture weather data
   */
  RegisterMsgInterceptor((MessageInterceptorFn)(&Weather_PlugIn::DataChanged), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Outside_Temp_Changed_CONST);
  RegisterMsgInterceptor((MessageInterceptorFn)(&Weather_PlugIn::DataChanged), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Outside_Condition_Changed_CONST);
  RegisterMsgInterceptor((MessageInterceptorFn)(&Weather_PlugIn::DataChanged), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Outside_Humidity_Changed_CONST);
  RegisterMsgInterceptor((MessageInterceptorFn)(&Weather_PlugIn::DataChanged), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Outside_Pressure_Changed_CONST);
  RegisterMsgInterceptor((MessageInterceptorFn)(&Weather_PlugIn::DataChanged), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Outside_Wind_Speed_Change_CONST);
  RegisterMsgInterceptor((MessageInterceptorFn)(&Weather_PlugIn::DataChanged), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Outside_Wind_Direction_Ch_CONST);
  RegisterMsgInterceptor((MessageInterceptorFn)(&Weather_PlugIn::DataChanged), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Outside_Visibility_Change_CONST);
  RegisterMsgInterceptor((MessageInterceptorFn)(&Weather_PlugIn::DataChanged), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Outside_Feels_Like_Change_CONST);
  RegisterMsgInterceptor((MessageInterceptorFn)(&Weather_PlugIn::RadarChanged), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Radar_Images_Changed_CONST);

  /**
   * We also intercept Orbiter Registered commands, so that we can set the current weather icon on the main screen,
   * and to be able to change it on subsequent data changes.
   */
  RegisterMsgInterceptor((MessageInterceptorFn)(&Weather_PlugIn::OrbiterRegistered),0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_Orbiter_Registered_CONST);


  // Bootstrap weather_plugin variable to the Device ID of this Weather Plugin
  // Needed for the Radar.
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Setting name weather_plugin to device ID %d",m_dwPK_Device);
  m_mapWeatherValues["weather_plugin"]=m_dwPK_Device;

  return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Weather_PlugIn_Command *Create_Weather_PlugIn(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new Weather_PlugIn(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void Weather_PlugIn::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Weather_PlugIn::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!

void Weather_PlugIn::DumpData()
{
  /*  for (map<string, string>::iterator it=m_mapWeatherData.begin(); it!=m_mapWeatherData.end(); ++it)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Key: %s, Value: %d",it->first.c_str(), it->second.c_str());
      } */
}

/**
 * Event Callbacks
 */

/**
 * This is the catch-all for any of the weather events, except Radar images. Take the data out of 
 * The event parameters, and populate the maps.
 */
bool Weather_PlugIn::DataChanged(class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo)
{

  PLUTO_SAFETY_LOCK(wm, m_Weather_PlugInMutex);

  string sName = pMessage->m_mapParameters[EVENTPARAMETER_Name_CONST];
  int sValue = atoi(pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST].c_str());
  string sText = pMessage->m_mapParameters[EVENTPARAMETER_Text_CONST];

  if (sName.empty())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Refusing to change data value, name or value was empty.");
      return false;
    }

  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Setting Weather Value [%s]=%d",sName.c_str(),sValue);
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Setting Weather Text [%s]=%s",sName.c_str(),sText.c_str());

  m_mapWeatherTexts[sName]=sText;
  m_mapWeatherValues[sName]=sValue;

  UpdateOrbiterWeatherScenarios();

  return true;

}

/**
 * Command Callbacks
 */
bool Weather_PlugIn::OrbiterRegistered(class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo)
{
  bool bRegistered = pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST]=="1";
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Weather_PlugIn::OrbiterRegistered() Orbiter %d registered %d",pMessage->m_dwPK_Device_From,(int)bRegistered);

  if (bRegistered)
    {
      // Orbiter is Registering.
      if (m_setOrbiters.find(pMessage->m_dwPK_Device_From) == m_setOrbiters.end())
	{
	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Weather_PlugIn::OrbiterRegistered Orbiter %d hasn't registered yet, adding.",pMessage->m_dwPK_Device_From);
	  m_setOrbiters.insert(pMessage->m_dwPK_Device_From);
	}
      else
	{
	  LoggerWrapper::GetInstance()->Write(LV_WARNING,"Weather_PlugIn::OrbiterRegistered Orbiter %d has attempted to re-register.",pMessage->m_dwPK_Device_From);
	}
    }
  else
    {
      // Orbiter is Unregistering.
      if (m_setOrbiters.find(pMessage->m_dwPK_Device_From) == m_setOrbiters.end())
	{
	  LoggerWrapper::GetInstance()->Write(LV_WARNING,"Weather_PlugIn::OrbiterRegistered Orbiter %d asked to unregister, but it wasn't registered!");
	}
      else
	{
	  set<long>::iterator it=m_setOrbiters.find(pMessage->m_dwPK_Device_From);
	  m_setOrbiters.erase(it);
	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Weather_PlugIn::OrbiterRegistered Orbiter %d has unregistered.",pMessage->m_dwPK_Device_From);
	}
    }

  UpdateOrbiterWeatherScenarios();

  return false; // Let this bubble up the chain. 

}

/**
 * Update bound icon with weather data.
 */
void Weather_PlugIn::UpdateBoundIcon(string sDeviceIDs, string sWeatherValue, string sType)
{
  string sText=""; // Doesn't really matter.
  string sValue_To_Assign;

  if (m_mapWeatherValues.find(sWeatherValue) == m_mapWeatherValues.end())
    {
      sValue_To_Assign="1";
    }
  else
    {
      sValue_To_Assign=StringUtils::itos(m_mapWeatherValues[sWeatherValue]);
    }
  CMD_Set_Bound_Icon_DL CMD_Set_Bound_Icon_DL(m_dwPK_Device, sDeviceIDs, sValue_To_Assign, sText, sType);
  SendCommand(CMD_Set_Bound_Icon_DL);

}

/**
 * Update Text objects on Orbiters
 */
void Weather_PlugIn::UpdateText(string sDeviceIDs, string sPK_DesignObj, int iPK_Text, string sWeatherText)
{
  string sText;
  if (m_mapWeatherTexts.find(sWeatherText) == m_mapWeatherTexts.end())
    {
      sText = "??";
    }
  else
    {
      sText = m_mapWeatherTexts[sWeatherText];
    }

  sPK_DesignObj+=".0.0"; // This requires a fully qualified designObj

  CMD_Set_Text_DL CMD_Set_Text_DL(m_dwPK_Device, sDeviceIDs, sPK_DesignObj, sText, iPK_Text);
  SendCommand(CMD_Set_Text_DL);

}

/**
 * Update the scenario icon on the main page of each orbiter with the most recent weather data.
 */
void Weather_PlugIn::UpdateOrbiterWeatherScenarios()
{
  if (!m_setOrbiters.empty())
    {
      string sDeviceIDs;
      for (set<long>::iterator it=m_setOrbiters.begin(); it!=m_setOrbiters.end(); ++it)
	{
	  sDeviceIDs+=StringUtils::itos(*it)+",";
	}
      sDeviceIDs=sDeviceIDs.substr(0,sDeviceIDs.size()-1); // strip away trailing comma
      UpdateBoundIcon(sDeviceIDs, "cond_current", "weather");
      UpdateBoundIcon(sDeviceIDs, "cond_tonight", "weather_tonight");
      UpdateBoundIcon(sDeviceIDs, "cond_day1", "weather_day1");
      UpdateBoundIcon(sDeviceIDs, "cond_day2", "weather_day2");
      UpdateBoundIcon(sDeviceIDs, "cond_day3", "weather_day3");
      UpdateBoundIcon(sDeviceIDs, "cond_day1", "weather_day16");
      UpdateBoundIcon(sDeviceIDs, "cond_day2", "weather_day26");
      UpdateBoundIcon(sDeviceIDs, "cond_day3", "weather_day36");
      UpdateBoundIcon(sDeviceIDs, "cond_day4", "weather_day4");
      UpdateBoundIcon(sDeviceIDs, "cond_day5", "weather_day5");
      UpdateBoundIcon(sDeviceIDs, "cond_day6", "weather_day6");
 
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherNow_CONST),TEXT_Current_Description_CONST,"cond_current");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherNow_CONST),TEXT_Current_Temperature_CONST,"temp_current");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherNow_CONST),TEXT_Current_Humidity_Data_CONST,"humidity_current");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherNow_CONST),TEXT_Current_Pressure_Data_CONST,"pressure_current");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherNow_CONST),TEXT_Current_Wind_Data_CONST,"wind_current");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherNow_CONST),TEXT_Current_Direction_Data_CONST,"direction_current");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherNow_CONST),TEXT_Current_Visibility_Data_CONST,"visibility_current");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherNow_CONST),TEXT_Current_Feels_Like_Data_CONST,"feelslike_current");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherNow_CONST),TEXT_City_Data_CONST,"location");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherNowDesc_CONST),TEXT_City_Data_CONST,"location");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast3_CONST),TEXT_City_Data_CONST,"location");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast6_CONST),TEXT_City_Data_CONST,"location");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherNowDesc_CONST),TEXT_Current_Description_CONST,"cond_tonight");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherNowDesc_CONST),TEXT_Current_Temperature_CONST,"temp_tonight");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherNowDesc_CONST),TEXT_Tomorrow_Weather_Forecast_CONST,"forecast_tonight");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherNow_CONST),TEXT_Age_of_Data_CONST,"data_age");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherNowDesc_CONST),TEXT_Age_of_Data_CONST,"data_age");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast3_CONST),TEXT_Age_of_Data_CONST,"data_age");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast6_CONST),TEXT_Age_of_Data_CONST,"data_age");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast3_CONST),TEXT_Day_1_Condition_CONST,"cond_day1");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast3_CONST),TEXT_Day_2_Condition_CONST,"cond_day2");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast3_CONST),TEXT_Day_3_Condition_CONST,"cond_day3");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast3_CONST),TEXT_3day_Hi_Data_1_CONST,"temp_hi_day1");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast3_CONST),TEXT_3day_Lo_Data_1_CONST,"temp_lo_day1");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast3_CONST),TEXT_3day_Hi_Data_2_CONST,"temp_hi_day2");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast3_CONST),TEXT_3day_Lo_Data_2_CONST,"temp_lo_day2");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast3_CONST),TEXT_3day_Hi_Data_3_CONST,"temp_hi_day3");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast3_CONST),TEXT_3day_Lo_Data_3_CONST,"temp_lo_day3");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast3_CONST),TEXT_3day_dow1_CONST,"dow_day1");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast3_CONST),TEXT_3day_dow2_CONST,"dow_day2");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast3_CONST),TEXT_3day_dow3_CONST,"dow_day3");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast6_CONST),TEXT_3day_dow1_CONST,"dow_day1");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast6_CONST),TEXT_3day_dow2_CONST,"dow_day2");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast6_CONST),TEXT_3day_dow3_CONST,"dow_day3");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast6_CONST),TEXT_6day_dow4_CONST,"dow_day4");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast6_CONST),TEXT_6day_dow5_CONST,"dow_day5");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast6_CONST),TEXT_6day_dow6_CONST,"dow_day6");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast6_CONST),TEXT_3day_Hi_Data_1_CONST,"temp_hi_day1");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast6_CONST),TEXT_3day_Lo_Data_1_CONST,"temp_lo_day1");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast6_CONST),TEXT_3day_Hi_Data_2_CONST,"temp_hi_day2");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast6_CONST),TEXT_3day_Lo_Data_2_CONST,"temp_lo_day2");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast6_CONST),TEXT_3day_Hi_Data_3_CONST,"temp_hi_day3");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast6_CONST),TEXT_3day_Lo_Data_3_CONST,"temp_lo_day3");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast6_CONST),TEXT_6day_Hi_Data4_CONST,"temp_hi_day4");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast6_CONST),TEXT_6day_Lo_Data4_CONST,"temp_lo_day4");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast6_CONST),TEXT_6day_Hi_Data5_CONST,"temp_hi_day5");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast6_CONST),TEXT_6day_Lo_Data5_CONST,"temp_lo_day5");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast6_CONST),TEXT_6day_Hi_Data6_CONST,"temp_hi_day6");
      UpdateText(sDeviceIDs,TOSTRING(DESIGNOBJ_mnuWeatherForecast6_CONST),TEXT_6day_Lo_Data6_CONST,"temp_lo_day6");
    }
  else
    {
      LoggerWrapper::GetInstance()->Write(LV_WARNING,"Weather_PlugIn::UpdateOrbiterWeatherScenarios() called while I have no orbiters!");
    }
}

/**
 * Get the radar images specified by URLs. Each call wipes the deque
 * and repopulates it.
 */
bool Weather_PlugIn::RadarChanged(class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo)
{
  PLUTO_SAFETY_LOCK(wm, m_Weather_PlugInMutex); // This one is especially important.

  int iFormat = atoi(pMessage->m_mapParameters[EVENTPARAMETER_Format_CONST].c_str());
  string sText = pMessage->m_mapParameters[EVENTPARAMETER_Text_CONST];

  if (iFormat==0 || sText.empty())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Weather_PlugIn::RadarChanged() - Empty format or text parameters. Ignoring.");
      return true;
    }

  // urls are | delimited.
  deque<string> deque_urls;
  deque<string>::iterator itDeque_urls;
  string sFirstImageBuffer;
  StringUtils::Tokenize(sText,string("|"),deque_urls,false); // push_back
  itDeque_urls=deque_urls.begin();
  
  string sError=HttpGet(*itDeque_urls, &sFirstImageBuffer);

  // Somehow we failed to get the first image. If the deque is empty, we 
  // return back a placeholder graphic. Otherwise, we simply fail, and
  // the last successful radar images will be preserved.
  if (sError=="ERROR")
    {
      // URL failed to fetch, assume the rest will too, send back a placeholder image
      size_t nSize=0;
      char *pData = FileUtils::ReadFileIntoBuffer(FILE_NO_RADAR_DATA, nSize);
      if (pData && m_dequeRadarFrames.empty())
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Weather_PlugIn::RadarChanged() - Could not fetch first radar image. Sending back placeholder instead.");
	  string sPlaceHolderImage=string(pData,nSize);
	  m_dequeRadarFrames.push_back(RadarFrame(0,0,1,sPlaceHolderImage));
	  return true;
	}
      else
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Weather_PlugIn::RadarChanged() - Could not find placeholder image!");
	}
      return true;
    }

  // If we got here, we got at least one image, clear out the radar deque
  // and try to get the rest.

  m_dequeRadarFrames.clear();
  m_mapitRadarFrameDeque.clear();

  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Weather_Plugin::RadarChanged() - Clearing out the Radar images deque.");

  // Grab the next one
  while ( itDeque_urls != deque_urls.end() )
    {
      string sNextImageBuffer;
      sError=HttpGet(*itDeque_urls, &sNextImageBuffer);
      string sNextUrl=*itDeque_urls;
      if (sError=="ERROR")
	{
	  // This image failed, forget doing the rest of them.
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Weather_PlugIn::RadarChanged() - Could not fetch next radar image. Not retrieving the remaining images for this pass.");
	  itDeque_urls=deque_urls.end();
	  break;
	}
      
      // Add image to deque
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"Weather_PlugIn::RadarChanged() - Fetched image from %s",sNextUrl.c_str());
      m_dequeRadarFrames.push_back(RadarFrame(0,0,iFormat,sNextImageBuffer));
      ++itDeque_urls; // and back around...
    }

  return true;

}

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c1116-b->

	/** @brief COMMAND: #1116 - Get Weather Text */
	/** Get one of the pieces of Accumulated weather texts. */
		/** @param #4 PK_Variable */
			/** Assign Requested Text to this Orbiter Variable */
		/** @param #19 Data */
			/** The Requested Weather Text given Name */
		/** @param #50 Name */
			/** Name of Weather Text Parameter to Return, e.g. "current_temp" */

void Weather_PlugIn::CMD_Get_Weather_Text(int iPK_Variable,string sName,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c1116-e->
{
	PLUTO_SAFETY_LOCK(wm, m_Weather_PlugInMutex);

	if (sName.empty())
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Weather_Plugin::CMD_Get_Weather_Text -Cowardly refusing to return value for empty name.");
	    return;
	  }

	string sText = m_mapWeatherTexts[sName];
	long dwPK_DeviceTo = pMessage->m_dwPK_Device_From;

	if (iPK_Variable>0)
	  {
	    CMD_Set_Variable CMD_Set_Variable(m_dwPK_Device, dwPK_DeviceTo, iPK_Variable, sText);
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Weather_Plugin::CMD_Get_Weather_Text - Setting variable %d, to %s",iPK_Variable, sText.c_str());
	    SendCommand(CMD_Set_Variable);
	  }

	*pData = (char*)sText.data();
	*iData_Size = (int)sText.size();
}


//<-dceag-c84-b->

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

void Weather_PlugIn::CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage)
//<-dceag-c84-e->
{
  PLUTO_SAFETY_LOCK(wm, m_Weather_PlugInMutex);
  long dwPK_Device_From = pMessage->m_dwPK_Device_From;

  map<long, deque<RadarFrame>::iterator >::iterator it = m_mapitRadarFrameDeque.find(dwPK_Device_From);
  if (it==m_mapitRadarFrameDeque.end())
    {
      m_mapitRadarFrameDeque[dwPK_Device_From] = m_dequeRadarFrames.begin();
      m_itRadarFrameDeque=m_mapitRadarFrameDeque[dwPK_Device_From];
    }
  else
    {
      m_itRadarFrameDeque=it->second;
      if (m_itRadarFrameDeque==m_dequeRadarFrames.end())
	{
	  m_itRadarFrameDeque=m_dequeRadarFrames.begin();
	}
    }

  char *cTemp = new char[m_itRadarFrameDeque->data().size()];
  memcpy(cTemp,m_itRadarFrameDeque->data().data(),m_itRadarFrameDeque->data().size());
  *pData = cTemp;
  *iData_Size = m_itRadarFrameDeque->data().size();

  m_mapitRadarFrameDeque[dwPK_Device_From] = ++m_itRadarFrameDeque;

}
//<-dceag-c1117-b->

	/** @brief COMMAND: #1117 - Get Weather Value */
	/** Get one of the pieces of Accumulated weather values. */
		/** @param #4 PK_Variable */
			/** Orbiter Variable # to set. */
		/** @param #48 Value */
			/** The returned integer value */
		/** @param #50 Name */
			/** The name of the Weather value to return, e.g. "temp_current" */

void Weather_PlugIn::CMD_Get_Weather_Value(int iPK_Variable,string sName,int *iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c1117-e->
{
	PLUTO_SAFETY_LOCK(wm, m_Weather_PlugInMutex);

	if (sName.empty())
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Weather_Plugin::CMD_Get_Weather_Data -Cowardly refusing to return value for empty name.");
	    return;
	  }

	long dwPK_DeviceTo = pMessage->m_dwPK_Device_From;

	*iValue = m_mapWeatherValues[sName];

	if (iPK_Variable>0)
	  {
	    int iVarValue = m_mapWeatherValues[sName];
	    CMD_Set_Variable CMD_Set_Variable(m_dwPK_Device, dwPK_DeviceTo, iPK_Variable, StringUtils::itos(iVarValue));
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Weather_Plugin::CMD_Get_Weather_Value - Setting variable %d, to %d",iPK_Variable, iVarValue);
	    SendCommand(CMD_Set_Variable);
	  }

}
