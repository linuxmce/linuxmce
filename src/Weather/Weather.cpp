/*
     Copyright (C) 2013 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "Weather.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->
#include "NOAA.h"
#include "OpenWM.h"
#include "WWO.h"
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <pthread.h>
#include <curl/curl.h>
#include <curl/easy.h>
// #include <curl/curlbuild.h>
#include <stdexcept>      // std::out_of_range

void * msgThread(void * Arg);
void * initThread(void * Arg);
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Weather::Weather(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Weather_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter),
	  m_CurlMutex("curl_mutex"),m_MsgMutex("wmsg_mutex")
//<-dceag-const-e->
{
	  m_CurlMutex.Init (NULL);
	  m_MsgMutex.Init (NULL);
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
Weather::Weather(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: Weather_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter),
	  m_CurlMutex("curl_mutex"),m_MsgMutex("wmsg_mutex")
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
Weather::~Weather()
//<-dceag-dest-e->
{
}

//<-dceag-getconfig-b->
bool Weather::GetConfig()
{
	if( !Weather_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// For now, for testing, we just fire off the testing harness!
	//system("/usr/pluto/bin/weather-test.sh");

	//Set defaults
	string config=DATA_Get_Configuration();
	timer_=15*60; //Default to 15 minutes
	units_="0"; //Default imperial
	lang_="english"; //Default english
	lat_="28.5383355";
	lon_="-81.37923649";
	radar_="6";
        api_key_="";
	use_NOAA_ = false;
	use_OWM_ = false;
	use_WWO_ = false;

	if (config.empty())
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Refusing to start with empty configuration. Please specify a proper configuration string.");
	    return false;
	  }

	vector<string> vectConfiguration;
	
	StringUtils::Tokenize(config,",",vectConfiguration);

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Parsing %d configuration entries.",vectConfiguration.size());

	// Parse each entity
	for (vector<string>::iterator it=vectConfiguration.begin();it!=vectConfiguration.end();++it)
	  {
	    string sConfigurationItem = *it;
	    vector<string> vectConfigurationItems;
	    
	    StringUtils::Tokenize(config,":",vectConfigurationItems);

	    if (sConfigurationItem.find("API") != string::npos)
	      {
		try
		  {
		    string sApiType = vectConfigurationItems.at(1);
		    if (sApiType == "NOAA")
		      {
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Weather: NOAA API Selected");
			use_NOAA_=true;
			lang_=vectConfigurationItems.at(2);
			units_=vectConfigurationItems.at(3);
			lat_=vectConfigurationItems.at(4);
			lon_=vectConfigurationItems.at(5);
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Weather: NOAA: Language: %s",lang_.c_str());
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Weather: NOAA: Units: %s",units_.c_str());
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Weather: NOAA: Use %s as latitude",lat_.c_str());
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Weather: NOAA: Use %s as Longitude",lon_.c_str());
		      }
		    else if (sApiType == "WWO")
		      {
			use_WWO_=true;
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Weather: Use World Weather Online");
			lang_=vectConfigurationItems.at(2);
			api_key_=vectConfigurationItems.at(3);
			units_=vectConfigurationItems.at(4);
			city_=vectConfigurationItems.at(5);
			radar_=vectConfigurationItems.at(6);
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Weather: Use World Weather Online language %s", lang_.c_str());        
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Weather: Use World Weather Online api key %s", api_key_.c_str());        
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Weather: Use World Weather Online units %s", units_.c_str());        
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Weather: Use World Weather Online city %s", city_.c_str());        
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"Weather: Use World Weather Online Radaar %s", radar_.c_str());        
		      }
		    else if (sApiType == "OWM")
		      {
			use_OWM_=true;
			// Am assuming this needs to be fleshed out? -tschak
		      }
		    else
		      {
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Weather: Invalid API Specified: %s",sApiType.c_str());
		      }
		  }
		catch (const std::out_of_range& oor)
		  {
		    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Weather: Out of Range Error for API item %s - error: %s",sConfigurationItem.c_str(), oor.what());
		    return false;
		  }
	      }
	    else if (sConfigurationItem.find("lat") != string::npos)
	      {
		try
		  {
		    string sValue = vectConfigurationItems.at(1);
		    lat_=sValue;
		  }
		catch (const std::out_of_range& oor)
		  {
		    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Incorrectly formed latitude configuration item: %s - Out of range error: %s",sConfigurationItem.c_str(),oor.what());
		    return false;
		  }
	      }
	    else if (sConfigurationItem.find("lon") != string::npos)
	      {
		try
		  {
		    string sValue = vectConfigurationItems.at(1);
		    lon_=sValue;
		  }
		catch (const std::out_of_range& oor)
		  {
		    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Incorrectly formed longitude configuration item: %s - Out of range error: %s",sConfigurationItem.c_str(),oor.what());
		    return false;
		  }	      
	      }
	    else if (sConfigurationItem.find("units") != string::npos)
	      {
		try
		  {
		    string sValue = vectConfigurationItems.at(1);
		    units_=sValue;
		  }
		catch (const std::out_of_range& oor)
		  {
		    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Incorrectly formed units (measurement units) configuration item: %s - Out of range error: %s",sConfigurationItem.c_str(),oor.what());
		    return false;
		  }	      
	      }
	    else if (sConfigurationItem.find("lang") != string::npos)
	      {
		try
		  {
		    string sValue = vectConfigurationItems.at(1);
		    lang_=sValue;
		  }
		catch (const std::out_of_range& oor)
		  {
		    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Incorrectly formed language configuration item: %s - Out of range error: %s",sConfigurationItem.c_str(),oor.what());
		    return false;
		  }	      
	      }
	    else if (sConfigurationItem.find("timer") != string::npos)
	      {
		try
		  {
		    string sValue = vectConfigurationItems.at(1);
		    timer_=strtod(sValue.c_str(),NULL)*60;
		  }
		catch (const std::out_of_range& oor)
		  {
		    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Incorrectly formed timer configuration item: %s - Out of range error: %s",sConfigurationItem.c_str(),oor.what());
		    return false;
		  }	      
	      }
	  }
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Weather: Starting Init Thread");
	if (pthread_create(&i_thr_,NULL, initThread,(void *) this)) {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Weather: Error creating initThread");
		return false;
	}


	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Weather::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Weather_Command *Create_Weather(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new Weather(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void Weather::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Weather::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void Weather::SomeFunction()
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
std::vector<std::string> &Weather::split_C(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> Weather::split_C(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split_C(s, delim, elems);
    return elems;
}

void * msgThread(void * Arg){
	Weather * thr = (Weather *) Arg;
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Weather: Getting Weather Data using: %s", thr->use_NOAA_ ? "NOAA" : thr->use_OWM_ ? "OWM" : "??");
	vector<vector<string> > msg;
	if(thr->use_NOAA_ ) {

		const string& Url="http://forecast.weather.gov/MapClick.php?lat="+thr->lat_+"&lon="+thr->lon_+"&unit="+thr->units_+"&lg="+thr->lang_+"&FcstType=json";
		string res=thr->download(Url);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Weather: Result: %s",res.c_str());
		NOAA noaa;
		if(noaa.Get_NOAA(res,thr->units_,thr->lang_)){
			msg=noaa.get_msg();
		}
	}
	if(thr->use_WWO_) {
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Weather: Inside WWO");
//		const string& Url="http://api.worldweatheronline.com/free/v2/weather.ashx?q="+thr->city_+"&format=json&tp=24&extra=localObsTime,isDayTime&num_of_days=5&lang="+thr->lang_+"&key="+thr->api_key_;
		const string& Url="http://api2.worldweatheronline.com/free/v2/weather.ashx?q="+thr->city_+"&format=json&tp=6&extra=localObsTime,isDayTime&num_of_days=5&lang="+thr->lang_+"&key="+thr->api_key_;
                string res=thr->download(Url);
		WWO wwo;
		if(wwo.Get_WWO(res,thr->units_,thr->lang_,thr->radar_)){
			msg=wwo.get_msg();
		}

	}
	if(thr->use_OWM_){

		//Get Current conditions
		OpenWM owm;
		string res;
		const string& Url="http://api.openweathermap.org/data/2.5/weather?mode=json&APPID="+owm.api_key+
				"&units="+thr->units_+"&lang="+thr->lang_+"&id="+thr->cid;
		bool t=true;
		while(t){
			res=thr->download(Url);
			size_t found=res.find("failed to connect");
			if(found!=string::npos || res==""){
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Weather::OWM::Current - Failed To Connect");
				sleep(10);
				continue;
			}
			t=false;
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Weather: Current Result: %s",res.c_str());
		}

		if(owm.parse_cur(res,thr->units_)){
			const string& Url="http://api.openweathermap.org/data/2.5/forecast/daily?cnt=6&mode=json&APPID="+owm.api_key+
					"&units="+thr->units_+"&lang="+thr->lang_+"&id="+thr->cid;
			bool t=true;
			while(t){
				res=thr->download(Url);
				size_t found=res.find("failed to connect");
				if(found!=string::npos || res==""){
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Weather::OWM::Forecast - Failed To Connect");
					sleep(10);
					continue;
				}
				t=false;
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Weather: Forecast Result: %s",res.c_str());
				if(owm.parse_forc(res,thr->units_))
					msg=owm.get_msg();
			}
		}
	}
	thr->send_msg(msg);
	return NULL;
}

void * initThread(void * Arg){
	Weather * thr2 = (Weather *) Arg;
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Weather: At Init");

	if(thr2->use_OWM_){

		//Get city ID as OWM likes that
		OpenWM owm;
		string res;
		const string& cid_Url="http://api.openweathermap.org/data/2.5/weather?lat="+thr2->lat_+"&lon="+thr2->lon_+"&APPID="+owm.api_key;
		bool t=true;
		while(t){
			res=thr2->download(cid_Url);
			size_t found=res.find("failed to connect");
			if(found!=string::npos || res==""){
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Weather::OWM::CID - Failed To Connect");
				sleep(10);
				continue;
			}
			t=false;
		}
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Weather: Result: %s",res.c_str());
		thr2->cid=owm.get_cid(res);
	}
	double duration=thr2->timer_,t_check;//Seconds
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Weather: Timer is set to: %f minutes",thr2->timer_/60);
	while(true){

        	if (pthread_create(&thr2->m_thr_,NULL, msgThread,(void *) thr2)) {
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Get_Weather: Error creating msgThread");
				return NULL;
	 	}
             sleep(thr2->timer_);
	}
        return NULL;
}

void Weather::send_msg(vector<vector<string> > &msg){
	//Iterate msg and send
	PLUTO_SAFETY_LOCK (mm , m_MsgMutex);
	for (vector<vector<string> >::iterator it = msg.begin() ; it != msg.end(); ++it) {
 	 if(it->size()==3){
                        if ( strcmp((*it)[2].c_str(),"Radar") ==0){
			Message *p_Message = new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, EVENT_Radar_Images_Changed_CONST,2,40, (*it)[0].c_str(), 13, (*it)[1].c_str());
			QueueMessageToRouter(p_Message);
//                              cout << (*it)[0] << " : " << (*it)[1] << " : " << (*it)[2] << endl;
                        } else {
			//cout << (*it)[0] << " : " << (*it)[1] << " : " << (*it)[2] << endl;
			Message *p_Message = new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, EVENT_Outside_Temp_Changed_CONST,
					3, EVENTPARAMETER_Name_CONST, (*it)[0].c_str(), EVENTPARAMETER_Text_CONST, (*it)[1].c_str(), EVENTPARAMETER_Value_CONST, (*it)[2].c_str());
			QueueMessageToRouter(p_Message);
                       }
		} else {
			//cout << (*it)[0] << " : " << (*it)[1] << endl;
			Message *p_Message = new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, EVENT_Outside_Temp_Changed_CONST,
					2, EVENTPARAMETER_Name_CONST, (*it)[0].c_str(), EVENTPARAMETER_Text_CONST, (*it)[1].c_str());
			QueueMessageToRouter(p_Message);
		}

	}
	return;
}

string Weather::download(const string & url){
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Weather: at download");
	PLUTO_SAFETY_LOCK (cm, m_CurlMutex);
	CURLM* m_pCurl;
	curl_global_init (CURL_GLOBAL_ALL);
	m_pCurl = curl_easy_init ();
	if (!m_pCurl)
		return string("");
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Weather: at download after lock");
	curl_easy_setopt(m_pCurl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(m_pCurl, CURLOPT_FOLLOWLOCATION, 1L); //Follow redirect
	curl_easy_setopt(m_pCurl, CURLOPT_NOSIGNAL, 1); //Prevent "longjmp causes uninitialized stack frame" bug
	curl_easy_setopt(m_pCurl, CURLOPT_ACCEPT_ENCODING, "deflate");
	stringstream out;
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, &out);
	/* Perform the request, res will get the return code */
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Weather: at download before curl");
	CURLcode res = curl_easy_perform(m_pCurl);
	/* Check for errors */
	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
	}
	curl_easy_cleanup(m_pCurl);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Weather: at download after curl");
	return out.str();

}

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    string data((const char*) ptr, (size_t) size * nmemb);
    *((stringstream*) stream) << data << endl;
    return size * nmemb;
}
