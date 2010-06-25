/*
     Written for LinuxMCE. (c) LinuxMCE Foundation.
     Licensed under the GNU GPL v3, see COPYING for more details.

     Author: Adam Shortland <los93sol@gmail.com>
*/
//<-dceag-d-b->
#include "HAI_Omni_Series_RS232.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DCE/Message.h"
#include "DCE/DeviceData_Impl.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "AlarmManager.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <map>
#include <utility>
#include <queue>
#include <deque>

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
HAI_Omni_Series_RS232::HAI_Omni_Series_RS232(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: HAI_Omni_Series_RS232_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
        m_pAlarmManager = NULL;
        m_pHAI_Omni_Series_RS232 = this;
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
HAI_Omni_Series_RS232::HAI_Omni_Series_RS232(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: HAI_Omni_Series_RS232_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
        m_pAlarmManager = NULL;
        m_pHAI_Omni_Series_RS232 = this;
}

//<-dceag-dest-b->
HAI_Omni_Series_RS232::~HAI_Omni_Series_RS232()
//<-dceag-dest-e->
{
        m_pAlarmManager = NULL;
        delete m_pHAI_Omni_Series_RS232;
        m_pHAI_Omni_Series_RS232 = NULL;	
}

//<-dceag-getconfig-b->
bool HAI_Omni_Series_RS232::GetConfig()
{
	if( !HAI_Omni_Series_RS232_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
        string sPassword = DATA_Get_Password();
        string sUnits = DATA_Get_Units();
        if(sPassword == "" || sUnits == "")
        {
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Device data needs to be set from web admin");
                return false;
        }

        else
        {
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Device HAI Omni Panel has started.");
                m_iCurrentHouseMode = 0;
                // Check for changes to /etc/hai.conf and update as necessary.
                UpdateHAIConfig();
                // Update hainame.cache for panel polling
                QueueMessageToPanel("hai getnames > /etc/hainame.cache", Q_FUNCT_ADDCHILDREN);
                return true;
       }
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool HAI_Omni_Series_RS232::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

bool HAI_Omni_Series_RS232::UpdateHAIConfig()
{
        // Let's go ahead and setup the AlarmManager for panel polling
        m_pAlarmManager = new DCE::AlarmManager();
        m_pAlarmManager->Start(1);
        m_pAlarmManager->AddRelativeAlarm(10,this,MODE_POLL_FAST,NULL);
        m_pAlarmManager->AddRelativeAlarm(10,this,MODE_POLL_SLOW,NULL);
        m_pAlarmManager->AddRelativeAlarm(10,this,MODE_TIMEUPDATE,NULL);
        m_pAlarmManager->AddRelativeAlarm(10,this,MODE_QUEUE,NULL);

        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Waiting 10 seconds for the device to start.");

        // Vars to replace inside the config file
        string s_UserCode = DATA_Get_Password();                // Master code for the panel
        string s_SerialPort = DATA_Get_COM_Port_on_PC();        // Serial port the panel is connected to
//        string s_IPAddress = DATA_Get_TCP_Address();            // IP of the panel
//        int i_IPPort = DATA_Get_TCP_Port();                     // Port to connect to the panel on

        // Convert to string for our config file
        stringstream out;
//        out << i_IPPort;
//        string s_IPPort = out.str();

        const string s_OutputFile = "/etc/hai.conf";
        const string s_OutputDir = "/etc";
        string s_ConfigFile;

        // Grab our connector and write our config file for the appropriate method
        string s_Connector = DATA_Get_Connector();

        if (s_Connector == "RS232")
        {
        s_ConfigFile =
                "# Specify user code\r\n"
                "code=##USERCODE##\r\n"

                "# Specify serial port path\r\n"
                "serial=##SERIALPORT##\r\n"

                "# Specify serial port baud rate\r\n"
                "baud=9600\r\n"

                "# Specify name cache file\r\n"
                "name=/etc/hainame.cache\r\n";
        // Replace and write the config file.
        s_ConfigFile = StringUtils::Replace(s_ConfigFile,"##USERCODE##",s_UserCode);
        s_ConfigFile = StringUtils::Replace(s_ConfigFile,"##SERIALPORT##",s_SerialPort);
        }

        if (s_Connector == "IP")
        {
        s_ConfigFile =
                "# Specify user code\r\n"
                "code=##USERCODE##\r\n"

                "# Specify serial port path\r\n"
                "ip=##IPADDRESS##\r\n"

                "# Specify serial port baud rate\r\n"
                "port=##IPPORT##\r\n"

                "# Specify name cache file\r\n"
                "name=/etc/hainame.cache\r\n";
        // Replace and write the config file.
        s_ConfigFile = StringUtils::Replace(s_ConfigFile,"##USERCODE##",s_UserCode);
//        s_ConfigFile = StringUtils::Replace(s_ConfigFile,"##IPADDRESS##",s_IPAddress);
//        s_ConfigFile = StringUtils::Replace(s_ConfigFile,"##IPPORT##",s_IPPort);
        }
        if (!FileUtils::DirExists(s_OutputDir))
        {
                // Let's try to create it.
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"%s didn't exist, attempting to create.",s_OutputDir.c_str());
                FileUtils::MakeDir(s_OutputDir);
                if (!FileUtils::DirExists(s_OutputDir))
                {
                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Couldn't create %s, bailing...",s_OutputDir.c_str());
                        return false;
                }
                else
                {
                        LoggerWrapper::GetInstance()->Write(LV_STATUS,"Created %s Successfully.",s_OutputDir.c_str());
                }
        }

        if (!FileUtils::WriteTextFile(s_OutputFile,s_ConfigFile))
        {
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Couldn't write %s",s_OutputFile.c_str());
                return false;
        }
        else
        {
                LoggerWrapper::GetInstance()->Write(LV_STATUS,"Wrote %s",s_OutputFile.c_str());
                return true;
        }
}

void HAI_Omni_Series_RS232::AddChildren()
{
        // Open file for reading
        ifstream myfile ("/etc/hainame.cache");
        while (! myfile.eof() )
        {
                string SensorLine;
                char buffer[256];

                // Break the file into lines to retrieve what we're after
                myfile.getline(buffer,100);
                SensorLine = buffer;

                // Check for errors
                unsigned int Error = SensorLine.find("error");
                unsigned int CRC = SensorLine.find("CRC from");

                if(SensorLine.size() >> 0 && (Error == string::npos || CRC == string::npos))
                {
                        // Pull out our information
                        string sDescription;
                        string sZone;
                        string sSensorType;
                        vector<string> vectBuffer;
                        StringUtils::Tokenize(SensorLine,":",vectBuffer);
                        sDescription = vectBuffer[1];
                        string sZoneDetails = vectBuffer[0];
                        StringUtils::Tokenize(sZoneDetails," ",vectBuffer);
                        sSensorType = vectBuffer[2];
                        sZone = vectBuffer[3];

                        // Clean up any whitespace
                        sSensorType=StringUtils::Replace(sSensorType,"\t","");
                        sSensorType=StringUtils::TrimSpaces(sSensorType);
                        sZone=StringUtils::Replace(sZone,"\t","");
                        sZone=StringUtils::TrimSpaces(sZone);
                        sDescription=StringUtils::Replace(sDescription,"\t","");
                        sDescription=StringUtils::TrimSpaces(sDescription);

			// Debug sensor autodetection
                        // LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"sSensorType %s",sSensorType.c_str());
                        // LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"sZone %s",sZone.c_str());
                        // LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"sDescription %s",sDescription.c_str());

                        // Convert our zone to an int and get ready to do a lookup
                        int iSensorZone = atoi(sZone.c_str());
                        int iPK_Device;
                        int iDeviceTemplate=0;
                        string sCategory;
			string sUnhandled;
			string sZoneByType;

                        // Lookup the zone
                        if (sSensorType == "Zone")
                        {
                                iPK_Device = m_pHAI_Omni_Series_RS232->getDeviceForZone(iSensorZone, DEVICECATEGORY_Security_Device_CONST);
                                iDeviceTemplate = DEVICETEMPLATE_Generic_Sensor_CONST;
                                sCategory = "security";
				sUnhandled = "False";
				sZoneByType = "Z" + sZone;
				// LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "SensorType detected properly as zone %i", iPK_Device);
                        }	
                        else if (sSensorType == "Unit")
                        {
                                iPK_Device = m_pHAI_Omni_Series_RS232->getExactDeviceForZone(iSensorZone, DEVICECATEGORY_Security_Device_CONST, DEVICETEMPLATE_HAI_Unit_CONST);
                                iDeviceTemplate = DEVICETEMPLATE_HAI_Unit_CONST;
                                sCategory = "unit";
				sUnhandled = "False";
				sZoneByType = "U" + sZone;
				// LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "SensorType detected properly as unit %i", iPK_Device);
                        }
                        else if (sSensorType == "Thermo")
                        {
                                iPK_Device = m_pHAI_Omni_Series_RS232->getDeviceForZone(iSensorZone, DEVICECATEGORY_Climate_Device_CONST);
                                iDeviceTemplate = DEVICETEMPLATE_Standard_Thermostat_CONST;
                                sCategory = "climate";
				sUnhandled = "False";
				sZoneByType = "T" + sZone;
				// LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "SensorType detected properly as thermo %i", iPK_Device);
                        }
			else
			{
				sUnhandled = "True";
				// LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "SensorType is unhandled");
			}

			string sNewChild = m_mvNewChildren[sZoneByType];

                        // If we couldn't find it and it is a sensor we handle then add it
			if (iPK_Device == 0 && sUnhandled == "False" && sNewChild != "Acknowledged")
                        {
                                //CMD_Create_Device_Callback add_command(m_dwPK_Device,m_dwPK_Device,"","","","");
/*
                                string sDetected = "New " + sCategory + " sensor detected on zone " + sZone + " and labeled as " + sDescription +
                                        " please choose what type of sensor this is.";

                        CMD_Display_Dialog_Box_On_Orbiter add_command(m_dwPK_Device,9,sDetected, // Message displayed on Orbiter

                        // Options for Display Dialog Box On Orbiter
                        // Air Quality
                        StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(m_dwPK_Device) + " " + // DeviceTo DeviceFrom
                        StringUtils::itos(MESSAGETYPE_COMMAND) + " " + StringUtils::itos(COMMAND_Create_Device_Callback_CONST) + " " + // Command to send
                        StringUtils::itos(COMMANDPARAMETER_Description_CONST) + " '" + sDescription + "' " + // Description
                        StringUtils::itos(COMMANDPARAMETER_Data_String_CONST) + " '" + StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "_" + sZone + "_" + // Port/Channel
                        StringUtils::itos(DEVICEDATA_PK_FloorplanObjectType_CONST) + "_" + "1' " + // PK_FloorplanObjectType
                        StringUtils::itos(COMMANDPARAMETER_DeviceTemplate_CONST) + " '" + StringUtils::itos(DEVICETEMPLATE_Air_Quality_Sensor_CONST) + // DeviceTemplate
                        "'|Air Quality|" +
                        // Chime
                        StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(m_dwPK_Device) + " " + // DeviceTo DeviceFrom
                        StringUtils::itos(MESSAGETYPE_COMMAND) + " " + StringUtils::itos(COMMAND_Create_Device_Callback_CONST) + " " + // Command to send
                        StringUtils::itos(COMMANDPARAMETER_Description_CONST) + " '" + sDescription + "' " + // Description
                        StringUtils::itos(COMMANDPARAMETER_Data_String_CONST) + " '" + StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "_" + sZone + "_" + // Port/Channel
                        StringUtils::itos(DEVICEDATA_PK_FloorplanObjectType_CONST) + "_" + "3' " + // PK_FloorplanObjectType
                        StringUtils::itos(COMMANDPARAMETER_DeviceTemplate_CONST) + " '" + StringUtils::itos(DEVICETEMPLATE_Chime_CONST) + // DeviceTemplate
                        "'|Chime|" +
                        // Door
                        StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(m_dwPK_Device) + " " + // DeviceTo DeviceFrom
                        StringUtils::itos(MESSAGETYPE_COMMAND) + " " + StringUtils::itos(COMMAND_Create_Device_Callback_CONST) + " " + // Command to send
                        StringUtils::itos(COMMANDPARAMETER_Description_CONST) + " '" + sDescription + "' " + // Description
                        StringUtils::itos(COMMANDPARAMETER_Data_String_CONST) + " '" + StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "_" + sZone + "_" + // Port/Channel
                        StringUtils::itos(DEVICEDATA_PK_FloorplanObjectType_CONST) + "_" + "4' " + // PK_FloorplanObjectType
                        StringUtils::itos(COMMANDPARAMETER_DeviceTemplate_CONST) + " '" + StringUtils::itos(DEVICETEMPLATE_Door_Sensor_CONST) + // DeviceTemplate
                        "'|Door|" +
                        // Doorbell
                        StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(m_dwPK_Device) + " " + // DeviceTo DeviceFrom
                        StringUtils::itos(MESSAGETYPE_COMMAND) + " " + StringUtils::itos(COMMAND_Create_Device_Callback_CONST) + " " + // Command to send
                        StringUtils::itos(COMMANDPARAMETER_Description_CONST) + " '" + sDescription + "' " + // Description
                        StringUtils::itos(COMMANDPARAMETER_Data_String_CONST) + " '" + StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "_" + sZone + "_" + // Port/Channel
                        StringUtils::itos(DEVICEDATA_PK_FloorplanObjectType_CONST) + "_" + "0' " + // PK_FloorplanObjectType
                        StringUtils::itos(COMMANDPARAMETER_DeviceTemplate_CONST) + " '" + StringUtils::itos(DEVICETEMPLATE_Doorbell_button_CONST) + // DeviceTemplate
                        "'|Doorbell|" +
                        // Generic
                        StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(m_dwPK_Device) + " " + // DeviceTo DeviceFrom
                        StringUtils::itos(MESSAGETYPE_COMMAND) + " " + StringUtils::itos(COMMAND_Create_Device_Callback_CONST) + " " + // Command to send
                        StringUtils::itos(COMMANDPARAMETER_Description_CONST) + " '" + sDescription + "' " + // Description
                        StringUtils::itos(COMMANDPARAMETER_Data_String_CONST) + " '" + StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "_" + sZone + "_" + // Port/Channel
                        StringUtils::itos(DEVICEDATA_PK_FloorplanObjectType_CONST) + "_" + "0' " + // PK_FloorplanObjectType
                        StringUtils::itos(COMMANDPARAMETER_DeviceTemplate_CONST) + " '" + StringUtils::itos(DEVICETEMPLATE_Generic_Sensor_CONST) + // DeviceTemplate
                        "'|Generic|" +
                        // Glass Break
                        StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(m_dwPK_Device) + " " + // DeviceTo DeviceFrom
                        StringUtils::itos(MESSAGETYPE_COMMAND) + " " + StringUtils::itos(COMMAND_Create_Device_Callback_CONST) + " " + // Command to send
                        StringUtils::itos(COMMANDPARAMETER_Description_CONST) + " '" + sDescription + "' " + // Description
                        StringUtils::itos(COMMANDPARAMETER_Data_String_CONST) + " '" + StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "_" + sZone + "_" + // Port/Channel
                        StringUtils::itos(DEVICEDATA_PK_FloorplanObjectType_CONST) + "_" + "0' " + // PK_FloorplanObjectType
                        StringUtils::itos(COMMANDPARAMETER_DeviceTemplate_CONST) + " '" + StringUtils::itos(DEVICETEMPLATE_Glass_Break_Sensor_CONST) + // DeviceTemplate
                        "'|Glass Break|" +
                        // Motion
                        StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(m_dwPK_Device) + " " + // DeviceTo DeviceFrom
                        StringUtils::itos(MESSAGETYPE_COMMAND) + " " + StringUtils::itos(COMMAND_Create_Device_Callback_CONST) + " " + // Command to send
                        StringUtils::itos(COMMANDPARAMETER_Description_CONST) + " '" + sDescription + "' " + // Description
                        StringUtils::itos(COMMANDPARAMETER_Data_String_CONST) + " '" + StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "_" + sZone + "_" + // Port/Channel
                        StringUtils::itos(DEVICEDATA_PK_FloorplanObjectType_CONST) + "_" + "6' " + // PK_FloorplanObjectType
                        StringUtils::itos(COMMANDPARAMETER_DeviceTemplate_CONST) + " '" + StringUtils::itos(DEVICETEMPLATE_Motion_Detector_CONST) + // DeviceTemplate
                        "'|Motion|" +
                        // Door
                        StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(m_dwPK_Device) + " " + // DeviceTo DeviceFrom
                        StringUtils::itos(MESSAGETYPE_COMMAND) + " " + StringUtils::itos(COMMAND_Create_Device_Callback_CONST) + " " + // Command to send
                        StringUtils::itos(COMMANDPARAMETER_Description_CONST) + " '" + sDescription + "' " + // Description
                        StringUtils::itos(COMMANDPARAMETER_Data_String_CONST) + " '" + StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "_" + sZone + "_" + // Port/Channel
                        StringUtils::itos(DEVICEDATA_PK_FloorplanObjectType_CONST) + "_" + "4' " + // PK_FloorplanObjectType
                        StringUtils::itos(COMMANDPARAMETER_DeviceTemplate_CONST) + " '" + StringUtils::itos(DEVICETEMPLATE_Door_Sensor_CONST) + // DeviceTemplate
                        "'|Door", "") ; // Door button
*/
                                // CMD_Display_Dialog_Box_On_Orbiter(long DeviceIDFrom, long DeviceIDTo,string sText,string sOptions,string sList_PK_Device)
                                CMD_Create_Device add_command(m_dwPK_Device,4,iDeviceTemplate,"",0,"",
                                        StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "|" + sZone, 0,m_dwPK_Device,sDescription,0,0,&iPK_Device) ;
                                SendCommand(add_command);

                                // Update our map so we don't keep prompting
				m_mvNewChildren[sZoneByType] = "Acknowledged";

                                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Added LMCE Device for sensor of type %s, on zone %i",sSensorType.c_str(),iSensorZone);
                        }
                }
                else
                {
                }
        }
}

int HAI_Omni_Series_RS232::GetZoneNumber(string ZoneLine)
{
        // Strip off everything but the zone number.
        string sZoneNumber;
        vector<string> vectBufferData;
        StringUtils::Tokenize(ZoneLine,":",vectBufferData);
        sZoneNumber = vectBufferData[0];

        // Convert it to an integer.
        int iZoneNumber;
        istringstream tmpStream;
        tmpStream.str(sZoneNumber);
        tmpStream >> iZoneNumber;

        // Now return the integer zone number.
        return iZoneNumber;
}

void HAI_Omni_Series_RS232::AlarmCallback(int id, void* param)
{
        if (id==MODE_POLL_FAST)
        {
                m_pAlarmManager->CancelAlarmByType(MODE_POLL_FAST);
                m_pAlarmManager->AddRelativeAlarm(3,this,MODE_POLL_FAST,NULL);

                // Queue message to panel to poll for security zone status
                QueueMessageToPanel("hai zones > /tmp/haizones.tmp",Q_FUNCT_POLL_SECURITYZONESTATUS);
        }

        if (id==MODE_POLL_SLOW)
        {
                m_pAlarmManager->CancelAlarmByType(MODE_POLL_SLOW);
                m_pAlarmManager->AddRelativeAlarm(20,this,MODE_POLL_SLOW,NULL);

                // Queue message to panel to poll for security mode changes
                QueueMessageToPanel("hai sum > /tmp/haisum.tmp",Q_FUNCT_POLL_SECURITYMODE);

                // Queue message to panel to poll for thermostat mode and fan mode
                QueueMessageToPanel("hai thermos > /tmp/haithermos.tmp",Q_FUNCT_POLLTHERMOS);
        }

        if (id==MODE_TIMEUPDATE)
        {
                m_pAlarmManager->CancelAlarmByType(MODE_TIMEUPDATE);
                m_pAlarmManager->AddRelativeAlarm(86400,this,MODE_TIMEUPDATE,NULL);
                // TODO: Write method to get system time and send it to panel, will be called from here.
        }

        if (id==MODE_QUEUE)
        {
                m_pAlarmManager->CancelAlarmByType(MODE_QUEUE);
                m_pAlarmManager->AddRelativeAlarm(2,this,MODE_QUEUE,NULL);
                QueueProcess();
        }
}

void HAI_Omni_Series_RS232::QueueProcess()
{
        if (m_qCommand.size() >> 0)
        {
                // Send the command to the panel
                //int iFunctionCallback = m_qFunctionCallback.front();
                //int iCommand = m_qCommand.size();
                //int iCallback = m_qFunctionCallback.size();

                system(m_qCommand.front().c_str());

                //LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"%s Message sent to panel.",m_qCommand.front().c_str());
                //LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"%i Size m_qCommand.",iCommand);
                //LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"%i Size m_qFunctionCallback.",iCallback);

                // Execute the correct callback to handle additional processing
                switch (m_qFunctionCallback.front()) {

                case Q_FUNCT_ADDCHILDREN :
                        // LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"AddChildren()");
                        AddChildren();
                        break;

                case Q_FUNCT_POLL_SECURITYMODE :
                        // LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PollSecurityMode()");
                        PollSecurityMode();
                        break;

                case Q_FUNCT_POLL_SECURITYZONESTATUS :
                        // LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PollSecurityZoneStatus()");
                        PollSecurityZoneStatus();
                        break;

                case Q_FUNCT_POLLTHERMOS :
                        // LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PollThermos()");
                        PollThermos();
                        break;

                case Q_FUNCT_CONTROLPANEL :
                        // Used when no further processing is needed other than giving the command to the panel.
                        break;
                }

                // Remove the commands we just processed from the queue
                m_qCommand.pop();
                m_qFunctionCallback.pop();
        }

        else
        {
                // Nothing to do here.
        }
}

void HAI_Omni_Series_RS232::QueueMessageToPanel(string thisCommand, int thisFunction)
{
        m_qCommand.push(thisCommand);
        m_qFunctionCallback.push(thisFunction);
}

void HAI_Omni_Series_RS232::PollSecurityZoneStatus()
{
        // Open the file for reading the the end
        ifstream myfile ("/tmp/haizones.tmp");
        while (! myfile.eof() )
        {
                string TrippedLine;
                char buffer[256];

                // Break the file into individual lines so we capture the status of each zone
                myfile.getline(buffer,100);
                TrippedLine = buffer;

                if(TrippedLine.size() >> 2)
                {

                // Get our mode
                vector<string> vectBufferArea;
                StringUtils::Tokenize(TrippedLine,",",vectBufferArea);
                string sTripDetected = vectBufferArea[1];

                // Clean whitespace
                sTripDetected=StringUtils::Replace(sTripDetected,"\t","");
                sTripDetected=StringUtils::TrimSpaces(sTripDetected);

                // LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"sTripDetected: %s",sTripDetected.c_str());

                // Detect whether or not the zone has been tripped
                if (sTripDetected == "Not ready")
                {
                        int iTrippedZone = GetZoneNumber(TrippedLine);

                        // Lookup detected zone number and retrieve the device number for the sensor
                        int iPK_Device = m_pHAI_Omni_Series_RS232->getDeviceForZone(iTrippedZone, DEVICECATEGORY_Security_Device_CONST);

                        if (iPK_Device == 0)
                        {
                                // QueueMessageToPanel("hai getnames > /etc/hainame.cache", Q_FUNCT_ADDCHILDREN);
                        }

                        // Check our map to make sure the zone's status has changed
                        if (m_mvSecurityZoneStatus[iTrippedZone] != "Tripped" && iPK_Device >> 0)
                        {
                                // Send the zone tripped message back to LinuxMCE.
                                Message *pMessage = new Message(iPK_Device, DEVICETEMPLATE_VirtDev_Security_Plugin_CONST, PRIORITY_NORMAL, MESSAGETYPE_EVENT, EVENT_Sensor_Tripped_CONST,
                                        1, EVENTPARAMETER_Tripped_CONST, "1");
                                m_pHAI_Omni_Series_RS232->QueueMessageToRouter(pMessage);

                                // Update our map with the change
                                m_mvSecurityZoneStatus[iTrippedZone] = "Tripped";

                                LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Sensor was tripped: %i",iPK_Device);
                        }
                        else
                        {
                                // Zone status hasn't changed, nothing to do here
                        }
                }
                if (sTripDetected == "Secure")
                {
                        int iTrippedZone = GetZoneNumber(TrippedLine);

                        // Lookup detected zone number and retrieve the device number for the sensor
                        int iPK_Device = m_pHAI_Omni_Series_RS232->getDeviceForZone(iTrippedZone, DEVICECATEGORY_Security_Device_CONST);

                        if (iPK_Device == 0)
                        {
                                // QueueMessageToPanel("hai getnames > /etc/hainame.cache", Q_FUNCT_ADDCHILDREN);
                        }

                        // Check our map to make sure the zone's status has changed
                        if (m_mvSecurityZoneStatus[iTrippedZone] != "Normal" && iPK_Device >> 0)
                        {
                                // Send the zone normal message back to LinuxMCE.
                                Message *pMessage = new Message(iPK_Device, DEVICETEMPLATE_VirtDev_Security_Plugin_CONST, PRIORITY_NORMAL, MESSAGETYPE_EVENT, EVENT_Sensor_Tripped_CONST,
                                        1, EVENTPARAMETER_Tripped_CONST, "0");
                                m_pHAI_Omni_Series_RS232->QueueMessageToRouter(pMessage);

                                // Update our map with the change
                                m_mvSecurityZoneStatus[iTrippedZone] = "Normal";

                                LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Sensor returned to normal state: %i",iPK_Device);
                        }
                        else
                        {
                                // Zone status hasn't changed, nothing to do here
                        }
                }
                else
                {
                        // Either read an empty line or something went wrong, nothing to do here.
                }
                }
                else
                {
                }
        }
}

int HAI_Omni_Series_RS232::getDeviceForZone(int iZone, unsigned int DeviceCategory)
{
        string sPort;                   // The port devicedata
        int iPort;                      // Port Device data converted to int.
        VectDeviceData_Impl *nomination = NULL;
        VectDeviceData_Impl *result;
        VectDeviceData_Impl children;

        nomination=NULL;
        result=NULL;
        iPort = 0;
        int retValue = 0;

        children = m_pHAI_Omni_Series_RS232->m_pData->m_vectDeviceData_Impl_Children;

        // Loop through all of the children of our device
        for(unsigned int i=0; i< children.size(); i ++)
        {
                // Check to see if this device is from the group we want
                if(children[i]->m_dwPK_DeviceCategory == DeviceCategory && children[i]->m_dwPK_DeviceTemplate != DEVICETEMPLATE_HAI_Unit_CONST)
                {
                        // DeviceData_Impl *child = children[i];
                        // Grab the port/channel from the devicedata and convert it to an integer
                        string sPort = children[i]->m_mapParameters[DEVICEDATA_PortChannel_Number_CONST];
                        istringstream tmpStream;
                        tmpStream.str(sPort);
                        tmpStream >> iPort;

                        // Compate the port/channel of the device to what we're searching for, if it matches then we've found our device
                        if (iPort == iZone)
                        {
                                retValue = children[i]->m_dwPK_Device;
                        }
                        else
                        {
                                // Member of the right category but the wrong channel, keep trying.
                        }
                }
                else
                {
                        // Member of the wrong category, keep trying.
                }
        }
        return retValue;
}

int HAI_Omni_Series_RS232::getExactDeviceForZone(int iZone, unsigned int DeviceCategory, int DeviceTemplate)
{
        string sPort;                   // The port devicedata
        int iPort;                      // Port Device data converted to int.
        VectDeviceData_Impl *nomination = NULL;
        VectDeviceData_Impl *result;
        VectDeviceData_Impl children;

        nomination=NULL;
        result=NULL;
        iPort = 0;
        int retValue = 0;

        children = m_pHAI_Omni_Series_RS232->m_pData->m_vectDeviceData_Impl_Children;

        // Loop through all of the children of our device
        for(unsigned int i=0; i< children.size(); i ++)
        {
                // Check to see if this device is from the group we want
                if(children[i]->m_dwPK_DeviceCategory == DeviceCategory && children[i]->m_dwPK_DeviceTemplate == DeviceTemplate)
                {
                        // DeviceData_Impl *child = children[i];
                        // Grab the port/channel from the devicedata and convert it to an integer
                        string sPort = children[i]->m_mapParameters[DEVICEDATA_PortChannel_Number_CONST];
                        istringstream tmpStream;
                        tmpStream.str(sPort);
                        tmpStream >> iPort;

                        // Compate the port/channel of the device to what we're searching for, if it matches then we've found our device
                        if (iPort == iZone)
                        {
                                retValue = children[i]->m_dwPK_Device;
                        }
                        else
                        {
                                // Member of the right category but the wrong channel, keep trying.
                        }
                }
                else
                {
                        // Member of the wrong category, keep trying.
                }
        }
        return retValue;
}

void HAI_Omni_Series_RS232::PollSecurityMode()
{
        // Open the file for reading
        ifstream myfile ("/tmp/haisum.tmp");
        while (! myfile.eof() )
        {
                string PanelLine;
                string PanelState = "WrongLine";
                char buffer[256];

                // Break the file into individual lines
                myfile.getline(buffer,100);
                PanelLine = buffer;

                // Detect whether or not this is a partition line
                unsigned int Area = PanelLine.find("Area 1");
                if (Area != string::npos)
                {
                        // Get our mode
                        vector<string> vectBufferArea;
                        StringUtils::Tokenize(PanelLine,":",vectBufferArea);
                        PanelState = vectBufferArea[1];

                        // Clean whitespace
                        PanelState=StringUtils::Replace(PanelState,"\t","");
                        PanelState=StringUtils::TrimSpaces(PanelState);

                        // Prepare to send the message to let LMCE know the panel's mode has changed
                        string m_sHouseCode = DATA_Get_Password();
                        string sHouseMode;
                        // Set our variable to pass back to LMCE
                        if(PanelState=="off")
                        {
                                sHouseMode = "1";

                                // Make sure the mode isn't already set.
                                if(m_iCurrentHouseMode != 1)
                                {
                                        // Let's go ahead and send the message to let LMCE know the panel mode has changed.
                                        DCE::CMD_Set_House_Mode CMD_Set_House_Mode(m_dwPK_Device,13,sHouseMode,0,m_sHouseCode,0,"W");
                                        SendCommand(CMD_Set_House_Mode);

                                        // Log that the panel mode has changed
                                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Panel mode changed from keypad to: %s",PanelState.c_str());
                                        m_iCurrentHouseMode = 1;
                                }
                        }

                        else if(PanelState=="day")
                        {
                                sHouseMode = "2";

                                // Make sure the mode isn't already set.
                                if(m_iCurrentHouseMode != 2)
                                {
                                        // Let's go ahead and send the message to let LMCE know the panel mode has changed.
                                        DCE::CMD_Set_House_Mode CMD_Set_House_Mode(m_dwPK_Device,13,sHouseMode,0,m_sHouseCode,0,"W");
                                        SendCommand(CMD_Set_House_Mode);

                                        // Log that the panel mode has changed.
                                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Panel mode changed from keypad to: %s",PanelState.c_str());
                                        m_iCurrentHouseMode = 2;
                                }
                        }

                        else if(PanelState=="night")
                        {
                                sHouseMode = "3";

                                // Make sure the mode isn't already set.
                                if(m_iCurrentHouseMode != 3)
                                {
                                        // Let's go ahead and send the message to let LMCE know the panel mode has changed.
                                        DCE::CMD_Set_House_Mode CMD_Set_House_Mode(m_dwPK_Device,13,sHouseMode,0,m_sHouseCode,0,"W");
                                        SendCommand(CMD_Set_House_Mode);

                                        // Log that the panel mode has changed.
                                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Panel mode changed from keypad to: %s",PanelState.c_str());
                                        m_iCurrentHouseMode = 3;
                                }
                        }

                        else if(PanelState=="vac" || PanelState=="vacation")
                        {
                                sHouseMode = "6";

                                // Make sure the mode isn't already set.
                                if(m_iCurrentHouseMode != 6)
                                {
                                        // Let's go ahead and send the message to let LMCE know the panel mode has changed.
                                        DCE::CMD_Set_House_Mode CMD_Set_House_Mode(m_dwPK_Device,13,sHouseMode,0,m_sHouseCode,0,"W");
                                        SendCommand(CMD_Set_House_Mode);

                                        // Log that the panel mode has changed.
                                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Panel mode changed from keypad to: %s",PanelState.c_str());
                                        m_iCurrentHouseMode = 6;
                                }
                        }

                        else
                        {
                                LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Unknown PanelState: %s",PanelState.c_str());
                        }
                }
                else
                {
                        // Just abort because it wasn't the right line or something went terribly wrong
                }
        }
}

string HAI_Omni_Series_RS232::ProcessTemps(string sTempLine)
{
        // Strip off everything but the current temps
        string sTemps;
        string sTemp;
        vector<string> vectBufferTemp;
        StringUtils::Tokenize(sTempLine,":",vectBufferTemp);
        sTemps = vectBufferTemp[1];

        // Split the temps up
        StringUtils::Tokenize(sTemps,",",vectBufferTemp);

        if(DATA_Get_Units() == "C")
        {
                sTemp = vectBufferTemp[3];
                sTemp=StringUtils::Replace(sTemp,"C","");
        }
        if(DATA_Get_Units() == "F")
        {
                sTemp = vectBufferTemp[2];
                sTemp=StringUtils::Replace(sTemp,"F","");
        }

        // Get rid of any whitespace
        sTemp=StringUtils::Replace(sTemp,"\t","");
        sTemp=StringUtils::TrimSpaces(sTemp);

        return sTemp;
}

void HAI_Omni_Series_RS232::PollThermos()
{
        // Open file for reading
        ifstream myfile ("/tmp/haithermos.tmp");
        while (! myfile.eof() )
        {
                string ThermoLine;
                char buffer[256];

                // Break the file into lines to retrieve what we're after
                myfile.getline(buffer,100);
                ThermoLine = buffer;

                // Check for errors
                unsigned int Error = ThermoLine.find("error");
                unsigned int CRC = ThermoLine.find("CRC from");
                if (Error == string::npos || CRC == string::npos)
                {

                        if(ThermoLine.size() >> 0)
                        {
                                // Get our zone number and lookup the device number
                                int iThermoZone = GetZoneNumber(ThermoLine);
                                int iPK_Device = m_pHAI_Omni_Series_RS232->getDeviceForZone(iThermoZone,DEVICECATEGORY_Climate_Device_CONST);

                                if (iPK_Device == 0)
                                {
                                        // QueueMessageToPanel("hai getnames > /etc/hainame.cache", Q_FUNCT_ADDCHILDREN);
                                }

                                else if (iPK_Device >> 0)
                                {
                                        // Strip off everything but the thermostat modes
                                        string Modes;
                                        string Mode;
                                        string sMode;
                                        string FanMode;
                                        string sFanMode;
                                        vector<string> vectBufferModes;
                                        StringUtils::Tokenize(ThermoLine,":",vectBufferModes);
                                        Modes = vectBufferModes[2];

                                        // Split the modes up and put them in a variable
                                        StringUtils::Tokenize(Modes,",",vectBufferModes);
                                        Mode = vectBufferModes[3];
                                        FanMode = vectBufferModes[4];

                                        // Strip off the extra stuff from our modes
                                        Mode=StringUtils::Replace(Mode,"\t","");
                                        Mode=StringUtils::TrimSpaces(Mode);
                                        FanMode=StringUtils::Replace(FanMode,"Fan","");
                                        FanMode=StringUtils::Replace(FanMode,"\t","");
                                        FanMode=StringUtils::TrimSpaces(FanMode);

                                        // Prep our mode to be sent back to LMCE
                                        // Mode == "Auto" is handled later because we need to make sure the setpoints have been obtained first!
                                        if(Mode == "Heat")
                                        {
                                                sMode = "1";
                                        }
                                        if(Mode == "Cool")
                                        {
                                                sMode = "2";
                                        }
                                        if(Mode == "Off")
                                        {
                                                sMode = "3";
                                        }
                                        // Prep our fan mode to be sent back to LMCE
                                        if(FanMode == "Auto")
                                        {
                                                sFanMode = "1";
                                        }
                                        if(FanMode == "On")
                                        {
                                                sFanMode = "3";
                                        }

                                        myfile.getline(buffer,100);
                                        ThermoLine = buffer;

                                        // Fetch our current temperature
                                        string CurrentTemp = m_pHAI_Omni_Series_RS232->ProcessTemps(ThermoLine);

                                        // Update our map and tell LMCE the temperature changed
                                        if(m_mvThermoTemp[iPK_Device] != CurrentTemp)
                                        {
                                                Message *pMessage = new Message(iPK_Device,DEVICETEMPLATE_VirtDev_Climate_PlugIn_CONST, PRIORITY_NORMAL,
                                                        MESSAGETYPE_EVENT, EVENT_Temperature_Changed_CONST,0);
                                                pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST]=CurrentTemp;
                                                m_pHAI_Omni_Series_RS232->QueueMessageToRouter(pMessage);

                                                m_mvThermoTemp[iPK_Device] = CurrentTemp;
                                                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Current temperature of thermostat %i is now: %s",iPK_Device,CurrentTemp.c_str());
                                        }

                                        myfile.getline(buffer,100);
                                        ThermoLine = buffer;

                                        // Fetch our current heating setpoint
                                        string HeatSetPoint = m_pHAI_Omni_Series_RS232->ProcessTemps(ThermoLine);

                                        // Update our map
                                        if(m_mvThermoHeatTemp[iPK_Device] != HeatSetPoint)
                                        {
                                                m_mvThermoHeatTemp[iPK_Device] = HeatSetPoint;
                                                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Heat setpoint of thermostat %i changed from keypad: %s",iPK_Device,HeatSetPoint.c_str());
                                        }

                                        myfile.getline(buffer,100);
                                        ThermoLine = buffer;

                                        // Fetch our cooling setpoint
                                        string CoolSetPoint = m_pHAI_Omni_Series_RS232->ProcessTemps(ThermoLine);

                                        // Update our map
                                        if(m_mvThermoCoolTemp[iPK_Device] != CoolSetPoint)
                                        {
                                                m_mvThermoCoolTemp[iPK_Device] = CoolSetPoint;
                                                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cool Setpoint of thermostat %i changed from keypad: %s",iPK_Device,CoolSetPoint.c_str());
                                        }

                                        // In auto mode, figure out whether our thermostat is heating or cooling and set the right auto mode
                                        if (Mode == "Auto")
                                        {
                                                int iCurrentTemp = atoi(m_mvThermoTemp[iPK_Device].c_str());
                                                int iHeatSetPoint = atoi(m_mvThermoHeatTemp[iPK_Device].c_str());
                                                int iCoolSetPoint = atoi(m_mvThermoCoolTemp[iPK_Device].c_str());

                                                // NOTE: The HAI Panel will correct if our setpoints are invalid by allowing the invalid value
                                                // to be set and incrementing/decrementing the other setpoint by 2 degrees Fahrenheit!
                                                int iAverageSetPoint = (iCoolSetPoint - iHeatSetPoint)/2;
                                                iAverageSetPoint = iHeatSetPoint + iAverageSetPoint;

                                                if(iCurrentTemp <= iAverageSetPoint)
                                                {
                                                        sMode = "5";
                                                        Mode = "Auto (Heat)";
                                                }
                                                if(iCurrentTemp >= iAverageSetPoint)
                                                {
                                                        sMode = "4";
                                                        Mode = "Auto (Cool)";
                                                }
                                        }

                                        // Check for mode change and send message as necessary
                                        if (m_mvThermoMode[iPK_Device] != sMode)
                                        {
                                                Message *pMessage = new Message(iPK_Device,DEVICETEMPLATE_VirtDev_Climate_PlugIn_CONST, PRIORITY_NORMAL,
                                                        MESSAGETYPE_EVENT, EVENT_Thermostat_Mode_Changed_CONST,0);
                                                pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST]=sMode;
                                                m_pHAI_Omni_Series_RS232->QueueMessageToRouter(pMessage);

                                                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Thermostat %i mode changed from keypad: %s",iPK_Device,Mode.c_str());
                                        }

                                        // Update our map
                                        m_mvThermoMode[iPK_Device] = sMode;

                                        // Check for fan mode change and send message as necessary
                                        if (m_mvThermoFanMode[iPK_Device] != sFanMode)
                                        {
                                                Message *pMessage = new Message(iPK_Device,DEVICETEMPLATE_VirtDev_Climate_PlugIn_CONST, PRIORITY_NORMAL,
                                                        MESSAGETYPE_EVENT, EVENT_Fan_Mode_Changed_CONST,0);
                                                pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST]=sFanMode;
                                                m_pHAI_Omni_Series_RS232->QueueMessageToRouter(pMessage);

                                                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Thermostat %i fan mode changed from keypad: %s",iPK_Device,FanMode.c_str());
                                        }

                                        // Update our map
                                        m_mvThermoFanMode[iPK_Device] = sFanMode;

                                        // Update Orbiter of new Heating Setpoint
                                        if(m_mvThermoMode[iPK_Device] == "1" || m_mvThermoMode[iPK_Device] == "5")
                                        {
                                                Message *pMessage = new Message(iPK_Device,DEVICETEMPLATE_VirtDev_Climate_PlugIn_CONST, PRIORITY_NORMAL,
                                                        MESSAGETYPE_EVENT, EVENT_Thermostat_Set_Point_Chan_CONST ,0);
                                                pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST]=HeatSetPoint;
                                                m_pHAI_Omni_Series_RS232->QueueMessageToRouter(pMessage);
                                        }

                                        // Update Orbiter of new Cooling Setpoint
                                        if(m_mvThermoMode[iPK_Device] == "2" || m_mvThermoMode[iPK_Device] == "4")
                                        {
                                                // Send our message
                                                Message *pMessage = new Message(iPK_Device,DEVICETEMPLATE_VirtDev_Climate_PlugIn_CONST, PRIORITY_NORMAL,
                                                        MESSAGETYPE_EVENT, EVENT_Thermostat_Set_Point_Chan_CONST ,0);
                                                pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST]=CoolSetPoint;
                                                m_pHAI_Omni_Series_RS232->QueueMessageToRouter(pMessage);
                                        }
                                }
                        }
                        else
                        {
                                // Probably read an empty line, just abort
                        }
                }
                else
                {
                        // Detected an error, aborting to avoid a crash
                }
        }
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
HAI_Omni_Series_RS232_Command *Create_HAI_Omni_Series_RS232(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new HAI_Omni_Series_RS232(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void HAI_Omni_Series_RS232::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
        // Use this to get the device the message is being sent to: pMessage->m_dwPK_Device_To
        // The next line will give us the Zone Number of the thermostat so we can use it to form our command we'll send to the panel.
        string sChannel = pDeviceData_Impl->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
	int iPK_Device = pDeviceData_Impl->m_dwPK_Device;


        switch(pMessage->m_dwID)
        {
		case COMMAND_Trip_Unit_CONST: // Trip a unit on the panel
		{
			int iDuration = atoi(pDeviceData_Impl->mapParameters_Find(DEVICEDATA_Duration_CONST).c_str());
			string iDefaultState = pDeviceData_Impl->mapParameters_Find(DEVICEDATA_Default_State_CONST);
			string TripUnit;

			if(iDefaultState == "0")
			{
			TripUnit = "hai unit " + sChannel + " on";
			QueueMessageToPanel(TripUnit.c_str(),Q_FUNCT_CONTROLPANEL);
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Tripped unit %i from Orbiter.",iPK_Device);
			sleep(iDuration);
			TripUnit = "hai unit " + sChannel + " off";
			QueueMessageToPanel(TripUnit.c_str(),Q_FUNCT_CONTROLPANEL);
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Resetting unit %i.",iPK_Device);
			}
                        if(iDefaultState == "1")
                        {
                        TripUnit = "hai unit " + sChannel + " off";
                        QueueMessageToPanel(TripUnit.c_str(),Q_FUNCT_CONTROLPANEL);
                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Tripped unit %i from Orbiter.",iPK_Device);
                        sleep(iDuration);
                        TripUnit = "hai unit " + sChannel + " on";
                        QueueMessageToPanel(TripUnit.c_str(),Q_FUNCT_CONTROLPANEL);
                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Resetting unit %i.",iPK_Device);
                        }
			if(iDefaultState == "2")
			{
				if(m_mvUnitState[iPK_Device] == "on")
				{
					TripUnit = "hai unit " + sChannel + " off";
					QueueMessageToPanel(TripUnit.c_str(),Q_FUNCT_CONTROLPANEL);
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Turned unit %i off from Orbiter.",iPK_Device);
					m_mvUnitState[iPK_Device] = "off";
				}
				if(m_mvUnitState[iPK_Device] == "off")
                                {
                                        TripUnit = "hai unit " + sChannel + " on";
                                        QueueMessageToPanel(TripUnit.c_str(),Q_FUNCT_CONTROLPANEL);
                                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Turned unit %i on from Orbiter.",iPK_Device);
                                        m_mvUnitState[iPK_Device] = "on";
                                }
			}
		}
			break;
                case COMMAND_Set_HeatCool_CONST: // Set Auto, Heat, Cool
                {
                        // Fetch the command parameter and let's prepare to send our message.
                        string SetMode = pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST];
                        if(SetMode == "A")
                        {
                                int iCurrentTemp = atoi(m_mvThermoTemp[iPK_Device].c_str());
                                int iHeatSetPoint = atoi(m_mvThermoHeatTemp[iPK_Device].c_str());
                                int iCoolSetPoint = atoi(m_mvThermoCoolTemp[iPK_Device].c_str());

                                // NOTE: The HAI Panel will correct if our setpoints are invalid by allowing the invalid value
                                // to be set and incrementing/decrementing the other setpoint by 2 degrees Fahrenheit!
                                int iAverageSetPoint = (iCoolSetPoint - iHeatSetPoint)/2;
                                iAverageSetPoint = iHeatSetPoint + iAverageSetPoint;

                                if(iCurrentTemp <= iAverageSetPoint)
                                {
                                        m_mvThermoMode[iPK_Device] = "5"; // Auto (Heat)
                                }
                                if(iCurrentTemp >> iAverageSetPoint)
                                {
                                        m_mvThermoMode[iPK_Device] = "4"; // Auto (Cool)
                                }

                                string SetThermoMode = "hai thermo " + sChannel + " mode auto";
                                QueueMessageToPanel(SetThermoMode.c_str(),Q_FUNCT_CONTROLPANEL);
                                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Set thermostat %i to auto mode from Orbiter.",iPK_Device);
                        }

                        if(SetMode == "H")
                        {
                                string SetThermoMode = "hai thermo " + sChannel + " mode heat";
				m_mvThermoMode[iPK_Device] = "1";
                                QueueMessageToPanel(SetThermoMode.c_str(),Q_FUNCT_CONTROLPANEL);
                                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Set thermostat %i to heating mode from Orbiter.",iPK_Device);
                        }

                        if(SetMode == "C")
                        {
                                string SetThermoMode = "hai thermo " + sChannel + " mode cool";
				m_mvThermoMode[iPK_Device] = "2";
                                QueueMessageToPanel(SetThermoMode.c_str(),Q_FUNCT_CONTROLPANEL);
				QueueMessageToPanel("hai thermos > /tmp/haithermos.tmp",Q_FUNCT_POLLTHERMOS);
                                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Set thermostat %i to cooling mode from Orbiter.",iPK_Device);
                        }
                }
                        break;

                case COMMAND_Set_Fan_CONST: // Fan On, Fan Auto
                {
                        // Fetch the command parameter and let's prepare to send our message.
                        string SetMode = pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST];
                        if (SetMode == "0")
                        {
                                string SetThermoMode = "hai thermo " + sChannel + " fan auto";
				m_mvThermoFanMode[iPK_Device] = "1";
                                QueueMessageToPanel(SetThermoMode.c_str(),Q_FUNCT_CONTROLPANEL);
				QueueMessageToPanel("hai thermos > /tmp/haithermos.tmp",Q_FUNCT_POLLTHERMOS);
                                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Set thermostat %i fan to auto from Orbiter.",iPK_Device);
                        }

                        if (SetMode == "1")
                        {
                                string SetThermoMode = "hai thermo " + sChannel + " fan on";
				m_mvThermoFanMode[iPK_Device] = "3";
                                QueueMessageToPanel(SetThermoMode.c_str(),Q_FUNCT_CONTROLPANEL);
				QueueMessageToPanel("hai thermos > /tmp/haithermos.tmp",Q_FUNCT_POLLTHERMOS);
                                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Set thermostat %i fan to on from Orbiter.",iPK_Device);
                        }
                        break;
                }
                case COMMAND_Generic_Off_CONST: // Thermostat Off
                {
			string SetThermoMode = "hai thermo " + sChannel + " mode off";
			m_mvThermoMode[iPK_Device] = "3";
			QueueMessageToPanel(SetThermoMode.c_str(),Q_FUNCT_CONTROLPANEL);
			QueueMessageToPanel("hai thermos > /tmp/haithermos.tmp",Q_FUNCT_POLLTHERMOS);
                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Set thermostat %i to off from Orbiter.",iPK_Device);
                }
                        break;

                case COMMAND_Set_Temperature_CONST: // Change Temperature
                {
                        // Define our variables and retrieve what we need
                        string temp = pMessage->m_mapParameters[COMMANDPARAMETER_Value_To_Assign_CONST];
			string sMode;
			string sNewTemp;
                        int newtemp;
			int oldtemp;

			// Get our existing setpoint
			if(m_mvThermoMode[iPK_Device] == "1" || m_mvThermoMode[iPK_Device] == "5")
			{
				sMode = "heat";
                        	oldtemp = atoi(m_mvThermoHeatTemp[iPK_Device].c_str());
			}
			if(m_mvThermoMode[iPK_Device] == "2" || m_mvThermoMode[iPK_Device] == "4")
			{
				sMode = "cool";
				oldtemp = atoi(m_mvThermoCoolTemp[iPK_Device].c_str());
			}

			// Calculate our new temperature
                        if(temp == "+1")
                        {
                        	newtemp = oldtemp + 1;
                                stringstream tmpStream;
                                tmpStream << newtemp;
                                sNewTemp = tmpStream.str();
                        }
                        if(temp == "-1")
                        {
                                newtemp = oldtemp - 1;
                                stringstream tmpStream;
                                tmpStream << newtemp;
                                sNewTemp = tmpStream.str();
                        }

                        // Update our map
			if(m_mvThermoMode[iPK_Device] == "1" || m_mvThermoMode[iPK_Device] == "5")
			{
                        	m_mvThermoHeatTemp[iPK_Device]=sNewTemp;
			}
			if(m_mvThermoMode[iPK_Device] == "2" || m_mvThermoMode[iPK_Device] == "4")
			{
				m_mvThermoCoolTemp[iPK_Device]=sNewTemp;
			}

                        // Determine if we are to send degrees in Fahrenheit or Celcius
                        if(DATA_Get_Units() == "C")
                        {
                        	sNewTemp = sNewTemp + "C";
                        }
                        if(DATA_Get_Units() == "F")
                        {
                        	sNewTemp = sNewTemp + "F";
                        }

                        // Build the message
                        string SetThermoMode = "hai thermo " + sChannel + " " +  sMode + " " + sNewTemp;

                        // Send the command
                        QueueMessageToPanel(SetThermoMode.c_str(),Q_FUNCT_CONTROLPANEL);
			QueueMessageToPanel("hai thermos > /tmp/haithermos.tmp",Q_FUNCT_POLLTHERMOS);
                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Changed %s setpoint for thermostat %i to %s from Orbiter",sMode.c_str(),iPK_Device,sNewTemp.c_str());
                }
			break;
                default:
                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unknown command %d received.", pMessage->m_dwID);
        }
        sCMD_Result = "OK";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void HAI_Omni_Series_RS232::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void HAI_Omni_Series_RS232::SomeFunction()
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

//<-dceag-c19-b->

	/** @brief COMMAND: #19 - Set House Mode */
	/** Set house mode */
		/** @param #5 Value To Assign */
			/** A value from the HouseMode table, or -1=toggle monitor mode */
		/** @param #17 PK_Users */
			/** The user setting the mode.  If this is 0, it will match any user who has permission to set the house mode. */
		/** @param #99 Password */
			/** The password or PIN of the user.  This can be plain text or md5. */
		/** @param #100 PK_DeviceGroup */
			/** DeviceGroups are treated as zones.  If this device group is specified, only the devices in these zones (groups) will be set. */
		/** @param #101 Handling Instructions */
			/** How to handle any sensors that we are trying to arm, but are blocked.  Valid choices are: R-Report, change to a screen on the orbiter reporting this and let the user decide, W-Wait, arm anyway, but wait for the sensors to clear and then arm them, B-Bypass */

void HAI_Omni_Series_RS232::CMD_Set_House_Mode(string sValue_To_Assign,int iPK_Users,string sPassword,int iPK_DeviceGroup,string sHandling_Instructions,string &sCMD_Result,Message *pMessage)
//<-dceag-c19-e->
{
	cout << "Need to implement command #19 - Set House Mode" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #17 - PK_Users=" << iPK_Users << endl;
	cout << "Parm #99 - Password=" << sPassword << endl;
	cout << "Parm #100 - PK_DeviceGroup=" << iPK_DeviceGroup << endl;
	cout << "Parm #101 - Handling_Instructions=" << sHandling_Instructions << endl;
        int iValue=atoi(sValue_To_Assign.c_str());
        if (iValue >> 6 || iValue == 0)
        {
                // Don't do anything, incorrect value passed
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Incorrect value to set mode: %i",iValue);
        }
        else
        {
        switch (iValue)
                {
                case 1:
                        if (m_iCurrentHouseMode != 1)
                        {
                        QueueMessageToPanel("hai arm off",Q_FUNCT_CONTROLPANEL);        // Disarmed
                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Panel Mode Set to Disarmed by Orbiter - Disarming panel.");
                        break;
                        }
                        else
                        {
                        break;
                        }
                case 2:
                        if (m_iCurrentHouseMode != 2)
                        {
                        QueueMessageToPanel("hai arm day",Q_FUNCT_CONTROLPANEL);        // Armed away
                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Panel Mode Set to Armed by Orbiter - Arming panel, day.");
                        break;
                        }
                        else
                        {
                        break;
                        }
                case 3:
                        if (m_iCurrentHouseMode != 3)
                        {
                        QueueMessageToPanel("hai arm night",Q_FUNCT_CONTROLPANEL);        // At Home Secure
                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Panel Mode Set to At Home Secure by Orbiter - Arming panel, night.");
                        break;
                        }
                        else
                        {
                        break;
                        }
                case 4:
                        if (m_iCurrentHouseMode != 4)
                        {
                        // Need to reassign iValue since we don't properly support this mode
                        iValue=3;
                        QueueMessageToPanel("hai arm night",Q_FUNCT_CONTROLPANEL);        // Sleeping
                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Panel Mode Set to Sleeping by Orbiter - Arming panel, night.");
                        break;
                        }
                        else
                        {
                        break;
                        }
                case 5:
                        if (m_iCurrentHouseMode != 5)
                        {
                        // Need to reassign iValue since we don't properly support this mode
                        iValue=1;
                        QueueMessageToPanel("hai arm off",Q_FUNCT_CONTROLPANEL);        // Entertaining
                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Panel Mode Set to Entertaining by Orbiter - Disarming panel.");
                        break;
                        }
                        else
                        {
                        break;
                        }
                case 6:
                        if (m_iCurrentHouseMode != 6)
                        {
                        QueueMessageToPanel("hai arm vac",Q_FUNCT_CONTROLPANEL);        // Armed Extended Away
                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Panel Mode Set to Extended Away by Orbiter - Arming panel, Vacation.");
                        break;
                        }
                        else
                        {
                        break;
                        }
                }

        m_iCurrentHouseMode = iValue;

        }

}

//<-dceag-c192-b->

	/** @brief COMMAND: #192 - On */
	/** Turn the thermostat on */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
		/** @param #98 PK_Device_Pipes */
			/** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

void HAI_Omni_Series_RS232::CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage)
//<-dceag-c192-e->
{
	cout << "Need to implement command #192 - On" << endl;
	cout << "Parm #97 - PK_Pipe=" << iPK_Pipe << endl;
	cout << "Parm #98 - PK_Device_Pipes=" << sPK_Device_Pipes << endl;
}

//<-dceag-c193-b->

	/** @brief COMMAND: #193 - Off */
	/** Turn the thermostat off */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

void HAI_Omni_Series_RS232::CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage)
//<-dceag-c193-e->
{
	cout << "Need to implement command #193 - Off" << endl;
	cout << "Parm #97 - PK_Pipe=" << iPK_Pipe << endl;
}

//<-dceag-c278-b->

	/** @brief COMMAND: #278 - Set Temperature */
	/** Sets the temperature to an absolute or relative value. */
		/** @param #5 Value To Assign */
			/** Can be absolute, like 20, 18, etc., or relative, like +2, -2, etc.  The temperature is always in degrees celsius, and the application is expected to convert to Fahrenheit if necesssary. */

void HAI_Omni_Series_RS232::CMD_Set_Temperature(string sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c278-e->
{
	cout << "Need to implement command #278 - Set Temperature" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
}

//<-dceag-c279-b->

	/** @brief COMMAND: #279 - Set Fan */
	/** Force the fan to be on all the time to circulate air. */
		/** @param #8 On/Off */
			/** If 1, the fan will always be on regardless of temperature, if 0, it will be turned on by the climate system when needed. */

void HAI_Omni_Series_RS232::CMD_Set_Fan(string sOnOff,string &sCMD_Result,Message *pMessage)
//<-dceag-c279-e->
{
	cout << "Need to implement command #279 - Set Fan" << endl;
	cout << "Parm #8 - OnOff=" << sOnOff << endl;
}

//<-dceag-c280-b->

	/** @brief COMMAND: #280 - Set Heat/Cool */
	/** Indicates if the thermostat should be heat-only, cool-only, or either one */
		/** @param #8 On/Off */
			/** H=heat only, C=cool only, A=automatic */

void HAI_Omni_Series_RS232::CMD_Set_HeatCool(string sOnOff,string &sCMD_Result,Message *pMessage)
//<-dceag-c280-e->
{
	cout << "Need to implement command #280 - Set Heat/Cool" << endl;
	cout << "Parm #8 - OnOff=" << sOnOff << endl;
}

//<-dceag-c756-b->

	/** @brief COMMAND: #756 - Report Child Devices */
	/** Report Child Devices */

void HAI_Omni_Series_RS232::CMD_Report_Child_Devices(string &sCMD_Result,Message *pMessage)
//<-dceag-c756-e->
{
	cout << "Need to implement command #756 - Report Child Devices" << endl;
}

//<-dceag-c759-b->

	/** @brief COMMAND: #759 - Get Sensors List */
	/** Get the list of sensors */
		/** @param #109 Data String */
			/** [internal id] \t [description] \t [room name] \t [device template] \t [floorplan id] \n */

void HAI_Omni_Series_RS232::CMD_Get_Sensors_List(string *sData_String,string &sCMD_Result,Message *pMessage)
//<-dceag-c759-e->
{
	cout << "Need to implement command #759 - Get Sensors List" << endl;
	cout << "Parm #109 - Data_String=" << sData_String << endl;
}

//<-dceag-c1073-b->

	/** @brief COMMAND: #1073 - Trip Unit */
	/** Trips the onboard "unit" relays for HAI security panels. */

void HAI_Omni_Series_RS232::CMD_Trip_Unit(string &sCMD_Result,Message *pMessage)
//<-dceag-c1073-e->
{
	cout << "Need to implement command #1073 - Trip Unit" << endl;
}
