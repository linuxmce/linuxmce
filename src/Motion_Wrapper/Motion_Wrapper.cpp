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
#include "Motion_Wrapper.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "PlutoUtils/ProcessUtils.h"
#include "DCERouter.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"

#define MOTION_CONF_PATH	"/etc/motion/"
#define MOTION_CONF_FILE	"motion.conf"
#define ALARM_PURGE_ARCHIVE	1
#define SNAPSHOT_SLEEP_TIME	100

static pid_t * my_motion_pid = NULL;
static std::vector<pid_t> dc1394_pids;

void StartMotion(pid_t * pid)
{
        if (pid != NULL)
                my_motion_pid = pid;
        if (my_motion_pid == NULL)
        {
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "StartMotion called with NULL parameter, but no previous call with non-NULL parameter");
                return;
        }

        * my_motion_pid = fork();
        if(* my_motion_pid)
        {
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "In parent process.");
        }
        else
        {
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "In child process.");
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Launching motion...");
                execl("/usr/bin/motion", NULL, NULL);
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Could not launch motion motion!");
                _exit(1);
        }
}



void sighandler(int sig)
{
        pid_t pid;

        switch (sig)
        {
                case SIGCHLD:
                        pid = wait(NULL);
                        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Child exited; pid: %d; motion_pid: %d", pid, my_motion_pid ? * my_motion_pid : 0);
                        if (my_motion_pid != NULL && pid == * my_motion_pid)
                                StartMotion(NULL);

                        std::vector<pid_t>::iterator i;
                        for (i=dc1394_pids.begin(); i != dc1394_pids.end(); i++) {
                                if (*i == pid) {
                                        dc1394_pids.erase(i);
                                        break;
                                }
                        }
                        break;
        }
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Motion_Wrapper::Motion_Wrapper(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
        : Motion_Wrapper_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-getconfig-b->
bool Motion_Wrapper::GetConfig()
{
        if( !Motion_Wrapper_Command::GetConfig() )
                return false;
//<-dceag-getconfig-e->

        return true;
}

void Motion_Wrapper::OnReload(){
    exit(0);
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
Motion_Wrapper::Motion_Wrapper(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
        : Motion_Wrapper_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
        m_pAlarmManager = NULL;
}

//<-dceag-dest-b->
Motion_Wrapper::~Motion_Wrapper()
//<-dceag-dest-e->
{
        if(motionpid_) {
                signal(SIGCHLD, SIG_DFL);
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Killing motion process: %d...", motionpid_);
                my_motion_pid = NULL;
                kill(motionpid_, SIGKILL);
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Waiting for motion process to finish: %d...", motionpid_);
                waitpid(motionpid_,NULL,0);
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Done.");
        }

        std::vector<pid_t>::iterator i;
        for (i=dc1394_pids.begin(); i!=dc1394_pids.end(); i++) {
                kill(*i, SIGKILL);
                waitpid(*i,NULL,0);
        }
        delete m_pAlarmManager;
        m_pAlarmManager = NULL;
}

bool Motion_Wrapper::Connect(int iPK_DeviceTemplate) {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Generating Motion configuration...");

        string sPath = MOTION_CONF_PATH MOTION_CONF_FILE;
        if(!FileUtils::FileExists(sPath.c_str()) == true) {
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Motion package not installed. Please install...");
                return false;
        }

        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Writing configuration to: %s", MOTION_CONF_FILE);

        std::ofstream mconffile;
        mconffile.exceptions ( ifstream::eofbit | ifstream::failbit | ifstream::badbit );
        try {
                mconffile.open(sPath.c_str(), fstream::out | fstream::trunc);
                string sVideoStandard = m_pData->mapParameters_Find(DEVICEDATA_Video_Standard_CONST);
                string sMotionParameters = m_pData->mapParameters_Find(DEVICEDATA_Motion_Parameters_CONST);
                string sMotionExtraParameters = m_pData->mapParameters_Find(DEVICEDATA_Extra_Parameters_CONST);

                mconffile 	<< "norm " << sVideoStandard << endl
                                        /* Using DEVICEDATA_Motion_Option_CONST 51 for default values */
                                        << endl << sMotionParameters << endl
                                        /*
                                        << "frequency 0" <<  endl
                                        << "rotate 0" <<  endl
                                        << "width 320" <<  endl
                                        << "height 240" <<  endl
                                        << "framerate 10" <<  endl
                                        << "auto_brightness on" <<  endl
                                        << "brightness 128" <<  endl
                                        << "contrast 128" <<  endl
                                        << "saturation 128" <<  endl
                                        << "hue 128" <<  endl

            filters << endl
                                        << "despeckle EedDl" << endl
                                        << "lightswitch 50" << endl
                                        << "minimum_motion_frames 5" << endl
                                        << "night_compensate on" << endl

            detection  << endl
                                        << "noise_tune on" << endl
                                        << "threshold_tune on" << endl
                                        << "smart_mask_speed 5" << endl
                                        << "gap 5" << endl
                                        << "post_capture 5" << endl
                                        << "pre_capture 3" << endl


            output  << endl
                                        << "locate_motion_mode on" << endl
                                        << "text_double on" << endl
                                        << "text_changes on" << endl
                                        << "text_right %d-%m-%Y\\n%T" << endl

                           movies  << endl
                                        << "ffmpeg_cap_new on" << endl
                                        << "ffmpeg_timelapse 60" << endl
                                        << "ffmpeg_timelapse_mode daily" << endl
                                        << "ffmpeg_video_codec msmpeg4" << endl

                                         snapshot config  << endl
                                        << "output_pictures off" << endl
                                        << "snapshot_interval 60" << endl
                                        << "snapshot_filename %Y/%m/%d/%H/%M_%S" << endl
                                        << "jpeg_filename %Y/%m/%d/%H/%M_%S" << endl
                                        << "movie_filename movies/%d_%m_%Y_%H_%M_%S" << endl
                                        << "timelapse_filename %Y/%m/%d-timelapse" << endl
                                        */
           /*DEVICEDATA_Extra_Parameters_CONST 139 for Extra Parameters*/
          << endl << sMotionExtraParameters << endl
          << endl << endl;

        } catch(ifstream::failure e) {
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot open %s for writing...", sPath.c_str());
                return false;
        }

        bool bFirstAdded = true;
        for(size_t i = 0; i < m_pData->m_vectDeviceData_Impl_Children.size(); ++i) {
                DeviceData_Impl *pDeviceData_Impl = m_pData->m_vectDeviceData_Impl_Children[i];

                if (pDeviceData_Impl->m_bDisabled)
                        continue;

                // Firewire cameras don't have a v4l device, we need to use vloopback to create it
                if (pDeviceData_Impl->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Generic_Firewire_Camera_CONST) {
                        CreateVideoDeviceFor1394(pDeviceData_Impl);
                }

                if(!bFirstAdded) {
                        if(AddChildDeviceToConfigFile(mconffile, pDeviceData_Impl, i)) {
                                bFirstAdded = true;
                        }
                        mconffile << endl << endl;
                } else {
                        char cthread = '0' + i;
                        sPath = string(MOTION_CONF_PATH "thread") + string(1, cthread) + ".conf";
                        ofstream tconffile;
                        tconffile.exceptions ( ifstream::eofbit | ifstream::failbit | ifstream::badbit );
                        try {
                                tconffile.open(sPath.c_str(), fstream::out | fstream::trunc);
                                if(AddChildDeviceToConfigFile(tconffile, pDeviceData_Impl, i)) {
                                        mconffile << "thread " << sPath << endl;
                                }
                        } catch(ifstream::failure e) {
                                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot open %s for writing...", sPath.c_str());
                                continue;
                        }
                }
        }

        m_pAlarmManager = new DCE::AlarmManager();
        m_pAlarmManager->Start(2);
        m_pAlarmManager->AddRelativeAlarm(1,this,ALARM_PURGE_ARCHIVE,NULL);

        signal(SIGCHLD, sighandler);
        StartMotion(&motionpid_);
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Forking to launch motion process...");

        return Motion_Wrapper_Command::Connect(iPK_DeviceTemplate);
}


//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Motion_Wrapper::Register()
//<-dceag-reg-e->
{
        return Connect(PK_DeviceTemplate_get());
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
        cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You
        can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Motion_Wrapper_Command *Create_Motion_Wrapper(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
        return new Motion_Wrapper(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void Motion_Wrapper::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
        sCMD_Result = "OK";

        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Command %d received for child.", pMessage->m_dwID);

        switch(pMessage->m_dwID) {
                case COMMAND_Get_Video_Frame_CONST: {
                                string sPortNumber = pDeviceData_Impl->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);

                                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Taking Snapshot...");
                                if(kill(motionpid_, SIGALRM)) {
                                        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Could not take snapshot, fail sending SIGALRM signal...");
                                } else {
                                        usleep(SNAPSHOT_SLEEP_TIME * 1100);
                                        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Checking file existance...");
                                        string FilePath = "/home/cameras/" + StringUtils::itos(pMessage->m_dwPK_Device_To) + "/lastsnap.jpg";
                                        if(FileUtils::FileExists(FilePath)) {
                                                LoggerWrapper::GetInstance()->Write(LV_STATUS, "File exists. Reading and sending snapshot picture...");
                                                size_t nDataLength;
                                                char *pFileData = FileUtils::ReadFileIntoBuffer(FilePath, nDataLength);

                                                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending Reply message to sender.");

                            pMessage->m_bRespondedToMessage=true;

                                                        Message *pMessageOut=new Message(pMessage->m_dwPK_Device_To, pMessage->m_dwPK_Device_From,
                                                                                                                                        PRIORITY_NORMAL, MESSAGETYPE_REPLY, 0, 0);
                                                        pMessageOut->m_mapData_Parameters[19]=pFileData;
                                                        pMessageOut->m_mapData_Lengths[19]=nDataLength;
                                                        pMessageOut->m_mapParameters[0]=sCMD_Result;
                                                        SendMessage(pMessageOut);

                                                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sent %d bytes to command sender.", nDataLength);
                                        } else {
                                                LoggerWrapper::GetInstance()->Write(LV_WARNING, "Missing snapshot file: %s.", FilePath.c_str());
                                        }
                                }
                        }break;
                default:
                        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unknown command %d received.", pMessage->m_dwID);
        }
}


bool Motion_Wrapper::CreateVideoDeviceFor1394(DeviceData_Impl* pDeviceData) {
        int PK_Device = pDeviceData->m_dwPK_Device;
        pid_t dc1394_pid = 0;

        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Generating v4l device for %d ieee1394 video device", PK_Device);

        string sDevice = pDeviceData->mapParameters_Find(DEVICEDATA_Device_CONST);

        dc1394_pid = fork();
        if( dc1394_pid == 0 )
        {	// dc_1394 --video1394=/dev/video1394/0 --vloopback=/dev/video10
                // V4L devices for video1394 get piped to: 10 + ( video1394/# ) * 2 [ video1394/0 -> video10 -> video11 ]
                string v4lParam = "--vloopback=/dev/video"+StringUtils::itos(atoi(sDevice.c_str()) * 2 + 10);
                string ieeeParam = "--video1394=/dev/video1394/"+sDevice;

                LoggerWrapper::GetInstance()->Write(LV_STATUS, "In child process.");
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Launching dc1394_vloopback ...");
                execl("/usr/bin/dc1394_vloopback", "dc1394_vloopback", v4lParam.c_str() , ieeeParam.c_str() ,NULL);
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Could not launch dc1394_vloopback !");

                _exit(1);
        } else {
                dc1394_pids.push_back(dc1394_pid);
        }

        return true;
}

bool Motion_Wrapper::AddChildDeviceToConfigFile(std::ofstream& conffile, DeviceData_Impl* pDeviceData,size_t i) {
        int PK_Device = pDeviceData->m_dwPK_Device;
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Using child device %d from Generic Analog Capture Card ", PK_Device);

        string sMotion = pDeviceData->mapParameters_Find(DEVICEDATA_Motion_Option_CONST);
        //TO DO = write camera record options & sync with house mode


        //video device
        string sDevice = pDeviceData->mapParameters_Find(DEVICEDATA_Device_CONST);

        if (pDeviceData->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Generic_Motion_IP_Camera_CONST) {
                string sUrl = "";
    string sProtocol = "";
    sProtocol = pDeviceData->mapParameters_Find(DEVICEDATA_Protocol_CONST);

                if ( sProtocol.empty() ) {
      LoggerWrapper::GetInstance()->Write(LV_STATUS, "Protocol Parameter Empty, set http as default ");
                        sProtocol = "http";
                }
                if (  pDeviceData->m_sIPAddress.empty() ) {
      LoggerWrapper::GetInstance()->Write(LV_STATUS, "IP Parameter Empty, EXIT ");
                        return false;
                }

                sUrl  = sProtocol + "://";
                sUrl += pDeviceData->m_sIPAddress;
                if ( ! pDeviceData->mapParameters_Find(DEVICEDATA_TCP_Port_CONST).empty() ) {
      LoggerWrapper::GetInstance()->Write(LV_STATUS, "IP Parameter Empty, skipping");
                        sUrl += ":" + pDeviceData->mapParameters_Find(DEVICEDATA_TCP_Port_CONST); /* DEVICEDATA_TCP_Port_CONST 69 */
                }
                if ( pDeviceData->mapParameters_Find(DEVICEDATA_Path_CONST).empty() ) {
                        sUrl += "/";
                } else {
                        sUrl += pDeviceData->mapParameters_Find(DEVICEDATA_Path_CONST);
                }

                conffile << "netcam_url " << sUrl << endl;

                if ( ! pDeviceData->mapParameters_Find(DEVICEDATA_Username_CONST).empty() ) {
                        conffile << "netcam_userpass " <<  pDeviceData->mapParameters_Find(DEVICEDATA_Username_CONST);
                        if ( ! pDeviceData->mapParameters_Find(DEVICEDATA_Password_CONST).empty() ) {
                                conffile  << ":" <<  pDeviceData->mapParameters_Find(DEVICEDATA_Password_CONST);
                        }
                        conffile << endl;
                }
        } else {
                if(!sDevice.empty()) {
                        if (pDeviceData->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Generic_Firewire_Camera_CONST) {
                                conffile	<< "videodevice /dev/video" << StringUtils::itos(atoi(sDevice.c_str()) * 2 + 11) << endl;
                        } else {
                                conffile 	<< "videodevice /dev/video" << sDevice << endl;
                        }
                } else {
                        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Device number not specified for device: %d.", PK_Device);
                        return false;
                }

                //input port
                string sPort = pDeviceData->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST);
                if(!sPort.empty()) {
                        conffile 	<< "input " << sPort << endl;
                } else {
                        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Port not specified for device: %d.", PK_Device);
                        return false;
                }
        }

        //noise level
        string sNoiseLevel = pDeviceData->mapParameters_Find(DEVICEDATA_Noise_CONST);
        if(!sNoiseLevel.empty()) {
                conffile	<< endl << "noise_level " << sNoiseLevel << endl;
        }

        //description: device description and roomname
        string sDescription =  pDeviceData->m_sDescription;
        string sRoom="";
        int nema=0;

        DCE::CMD_Get_Room_Description_DT CMD_Get_Room_Description_DT(pDeviceData->m_dwPK_Device, DEVICETEMPLATE_General_Info_Plugin_CONST, BL_SameHouse, pDeviceData->m_dwPK_Device, &sRoom, &nema);
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending CMD_Get_Room_Description_DT( %d, %d, %d, %d ) command", pDeviceData->m_dwPK_Device, DEVICETEMPLATE_General_Info_Plugin_CONST, BL_SameHouse, pDeviceData->m_dwPK_Device);
        SendCommand(CMD_Get_Room_Description_DT);

        if(!sDescription.empty() || !sRoom.empty()) {
    sDescription=sDescription.substr(0,16);
    sRoom=sRoom.substr(0,16);
                conffile	<< endl << "text_left " << StringUtils::Replace(&sDescription, " ", "_") << "\\n" << StringUtils::Replace(&sRoom, " ", "_") << endl << endl;
        }


        //Check Output Directory
        string sPath = "/home/cameras/" + StringUtils::itos(PK_Device);
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Looking for camera output directory: %s...", sPath.c_str());
        if(FileUtils::DirExists(sPath) == false) {
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Camera output directory not found, creating it");
                FileUtils::MakeDir(sPath);
        } else {
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Camera output found");
        }

        conffile 	<< "target_dir " << sPath << endl;

        //web port (+8000) and child number
        string sPortNumber = StringUtils::itos(i+8000);
        if(!sPortNumber.empty()) {
                conffile	<< "\n# webcam server settings" << endl;
                conffile	<< "stream_port " << sPortNumber << endl;
                conffile	<< "stream_quality 70 " << endl;
                conffile	<< "stream_localhost on " << endl;
                conffile        << "stream_maxrate 5" << endl;
        }

        //sending pluto tripped on/off events on motion start/end
        string sID = StringUtils::itos(PK_Device);
                conffile	<< "\n# sending pluto tripped on/off events on motion start/end" << endl;
                conffile	<< "on_event_start /usr/pluto/bin/MessageSend 127.0.0.1 " << sID << " -1001 2 9 25 1 "<< endl;
                conffile	<< "on_event_end /usr/pluto/bin/MessageSend 127.0.0.1 " << sID << " -1001 2 9 25 0 "<< endl;

/*
        Adding text from #59 Configuration parameter string as
        custom entries in threadX.conf.
*/
        string sConfiguration = pDeviceData->mapParameters_Find(DEVICEDATA_Configuration_CONST);
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got custom #59 configuration string %s", sConfiguration.c_str());
        if(!sConfiguration.empty()) {
                conffile	<< "\n# custom settings from Pluto's device parameter #59 Configuration" << endl;
                conffile	 << sConfiguration << endl;
        }

  return true;
}


/*
        When you received a valid command, but it wasn't for one of your children,
        then ReceivedUnknownCommand gets called.  If you handle the message, you
        should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Motion_Wrapper::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
        sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void Motion_Wrapper::SomeFunction()
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

DeviceData_Router* Motion_Wrapper::find_Device(int iPK_Device) {
    /*search device by id*/
        return m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
}

void Motion_Wrapper::AlarmCallback(int id, void* param)
{

        if (id==ALARM_PURGE_ARCHIVE) {
                m_pAlarmManager->CancelAlarmByType(ALARM_PURGE_ARCHIVE);
                // Purge Camera Archive once every 6 hours
                m_pAlarmManager->AddRelativeAlarm(60*60*6,this,ALARM_PURGE_ARCHIVE,NULL);
                string sCmd = "/usr/pluto/bin/Purge_Camera_Archive.pl";
                system(sCmd.c_str());
        }
}
/*

this came from generic_analog_camera

        string sPortNumber, sPath, sLine;
        FILE *fp;
        string FilePath, sData, sRep;
        unsigned long int size, iPortNumber, pid;

        iPortNumber = DATA_Get_Port_Number();
        iPortNumber++;
        sPath = "/etc/motion/thread" + StringUtils::itos(iPortNumber) + ".conf";
        FilePath = sPath;
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Looking for camera config file");
        if(FileUtils::FileExists(sPath) == true) {
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "File found, modifing...");
                sPath = "rm -f " + sPath;
                system(sPath.c_str());
        } else {
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Camera config file not found, writing new one");
        }
        fp = fopen(FilePath.c_str(),"wt+");
        //main config
        sLine = "videodevice /dev/video" + StringUtils::itos(iPortNumber) + "\n";
        fprintf(fp,sLine.c_str());
        fprintf(fp,"input 8\n");
        fprintf(fp,"norm 0\n");	//pal/secam
        fprintf(fp,"frequncy 0\n");
        fprintf(fp,"rotate 0\n");
        fprintf(fp,"width 320\n");
        fprintf(fp,"height 240\n");
        fprintf(fp,"framerate 2\n");
        fprintf(fp,"auto_brightness off\n");
        fprintf(fp,"brightness 0\n");
        fprintf(fp,"contrast 0\n");
        fprintf(fp,"saturation 0\n");
        fprintf(fp,"hue 0\n");
        //snapshot config
        fprintf(fp,"snapshot_interval 3600\n");
        //Output Directory
        sLine = "/home/cameras/" + StringUtils::itos(DeviceID);
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Looking for camera output directory");
        if(FileUtils::DirExists(sLine) == false) {
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Camera output directory not found, creating it");
                FileUtils::MakeDir(sLine);
        } else {
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Camera output found");
        }
        fprintf(fp,"target_dir %s\n",sLine.c_str());
        sLine = "snapshot_filename %Y/%m/%d/%H/%M_%S";
        fprintf(fp,"%s\n",sLine.c_str());
        sLine = "jpeg_filename %Y/%m/%d/%H/%M_%S";
        fprintf(fp,"%s\n",sLine.c_str());
        fclose(fp);

        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Reading Main configuration file of the motion server");
        sPath = "/etc/motion/motion.conf";
        size = FileUtils::FileSize(sPath);
        char *pData = new char(size+1);
        pData[size] = '\0';
        pData = FileUtils::ReadFileIntoBuffer(sPath,(size_t &)size);
        sLine = "#thread /etc/motion/thread" + StringUtils::itos(iPortNumber) + ".conf";
        sRep = "thread /etc/motion/thread" + StringUtils::itos(iPortNumber) + ".conf";

        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Replacing %s",sLine.c_str());
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "With %s",sRep.c_str());
        std::string std = pData;
        std = StringUtils::Replace(&std,sLine,sRep);

        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Adding child camera to main config file");
        strcpy(pData,std.c_str());
        FileUtils::WriteBufferIntoFile(sPath,pData,size);

        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Getting PID of the motion server");
        sLine = "ps -e | grep motion | awk '{print $1}' > camera_card.temp";
        system(sLine.c_str());
        size_t s;
        const char *ptr = FileUtils::ReadFileIntoBuffer("camera_card.temp",s);
        pid = atoi(ptr);
        if(pid == 0) {
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Motion Server is not running, exiting");
                system("rm -f camera_card.temp");
                exit(1);
        }
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Rehashing motion server with PID: %d",pid);

        sPath = "kill -s SIGHUP " + StringUtils::itos(pid);
        system(sPath.c_str());

*/

