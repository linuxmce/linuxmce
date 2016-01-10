/*
     Copyright (C) 2013 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "Linuxmce_NVR.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->
#include "cameraClasses/abstractpicamera.h"
#include "cameraClasses/motionpicamera.h"
#include "cameraClasses/httpsnapshotcamera.h"
#include "managerClasses/NvrManager.h"
#include "qurl.h"
#include "qdebug.h"
#include "qthread.h"
#include "qvector.h"


//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Linuxmce_NVR::Linuxmce_NVR(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
    : Linuxmce_NVR_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
    //<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
Linuxmce_NVR::Linuxmce_NVR(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
    : Linuxmce_NVR_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
    //<-dceag-const2-e->
{
}

//<-dceag-dest-b->
Linuxmce_NVR::~Linuxmce_NVR()
//<-dceag-dest-e->
{

}

//<-dceag-getconfig-b->
bool Linuxmce_NVR::GetConfig()
{
    if( !Linuxmce_NVR_Command::GetConfig() )
        return false;
    //<-dceag-getconfig-e->

    // Put your code here to initialize the data in this class
    // The configuration parameters DATA_ are now populated


    return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Linuxmce_NVR::Register()
//<-dceag-reg-e->
{
    return Connect(PK_DeviceTemplate_get());
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
    cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You
    can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Linuxmce_NVR_Command *Create_Linuxmce_NVR(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
    return new Linuxmce_NVR(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void Linuxmce_NVR::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
    sCMD_Result = "OK";

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Command %d received for child.", pMessage->m_dwID);

    NvrCameraBase *base;

    for (int i = 0; i < cam_list.size(); ++i) {
        if (cam_list.at(i)->dceDeviceId() == pMessage->m_dwPK_Device_To){
            base=cam_list.at(i);
            break;
        }

    }

    switch(pMessage->m_dwID) {
    case COMMAND_Get_Video_Frame_CONST: {

        qDebug() << QString("COMMAND_Get_Video_Frame_CONST for device %1").arg(pMessage->m_dwPK_Device_To);
        emit requestCameraImage(pMessage->m_dwPK_Device_To);

        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Taking Snapshot...");
        string FilePath =base->getImage(); //  "/tmp/" + StringUtils::itos(pMessage->m_dwPK_Device_To) + "/lastsnap.jpg";


        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Checking file existance...");

        if(FileUtils::FileExists(FilePath)){
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "File exists. Reading and sending snapshot picture...");
        } else {
            LoggerWrapper::GetInstance()->Write(LV_WARNING, "Missing snapshot file: %s.", FilePath.c_str());
            FilePath = "/tmp/please_wait.png";
        }


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

    }
        break;
    case COMMAND_Generic_On_CONST: {
        QMetaObject::invokeMethod(base, "setMotionEnabled",  Qt::QueuedConnection, Q_ARG(bool, true));
       // base->setMotionEnabled(true);
       // emit setDetectionState(true, pMessage->m_dwPK_Device_To);
    }
        break;
    case COMMAND_Generic_Off_CONST: {
          QMetaObject::invokeMethod(base, "setMotionEnabled",  Qt::QueuedConnection, Q_ARG(bool, false));
     //   base->setMotionEnabled(false);
      //  emit setDetectionState(false, pMessage->m_dwPK_Device_To);

    }
        break;
    default:
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unknown command %d received.", pMessage->m_dwID);
        qDebug() << QString( "Unknown command %1 received.").arg( pMessage->m_dwID);
    }
}

/*
    When you received a valid command, but it wasn't for one of your children,
    then ReceivedUnknownCommand gets called.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Linuxmce_NVR::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN COMMAND";
}

bool Linuxmce_NVR::OnReplaceHandler(string r)
{
    this->m_bQuit_set(true);

    return true;

}

void Linuxmce_NVR::CreateChildren()
{
    qDebug() << this->thread();
    if(!createManager()) return;

    for( int i=0; i < (int)m_pData->m_vectDeviceData_Impl_Children.size(); i++ )
    {
        DeviceData_Impl *pDeviceData_Impl_Child = m_pData->m_vectDeviceData_Impl_Children[i];

        // This device was marked as disabled
        if (pDeviceData_Impl_Child->m_bDisabled)
        {
            qDebug() << "Disabled device.";
            LoggerWrapper::GetInstance()->Write(LV_WARNING, "Child device %d is disabled", pDeviceData_Impl_Child->m_dwPK_Device);
            continue;
        }
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Creating child %d", pDeviceData_Impl_Child->m_dwPK_Device);

        // This device has it's own executible. Try to spawn it. If that fails, we will try to create it ourselves
        if( pDeviceData_Impl_Child->m_bImplementsDCE && !pDeviceData_Impl_Child->m_bIsEmbedded )
        {
            qDebug() << "Own Executable device.";
            if( SpawnChildDevice(pDeviceData_Impl_Child->m_dwPK_Device,pDeviceData_Impl_Child->m_sCommandLine) )
                continue;
        }
        Event_Impl *pEvent = m_pEvent->CreateEvent( pDeviceData_Impl_Child->m_dwPK_DeviceTemplate, m_pPrimaryDeviceCommand->m_pEvent->m_pClientSocket, pDeviceData_Impl_Child->m_dwPK_Device );
        if ( !pEvent )
        {
            pEvent = new Event_Impl( m_pPrimaryDeviceCommand->m_pEvent->m_pClientSocket, pDeviceData_Impl_Child->m_dwPK_Device );
            LoggerWrapper::GetInstance()->Write( LV_WARNING, "Note: Device manager has attached a device of type %d that this has no custom event handler for.  It will not fire events.",
                                                 pDeviceData_Impl_Child->m_dwPK_DeviceTemplate);
        }
        Command_Impl *pCommand = m_pPrimaryDeviceCommand->CreateCommand( pDeviceData_Impl_Child->m_dwPK_DeviceTemplate, m_pPrimaryDeviceCommand, pDeviceData_Impl_Child, pEvent );
        if ( !pCommand )
        {
            pCommand = new Command_Impl(m_pPrimaryDeviceCommand, pDeviceData_Impl_Child, pEvent, m_pRouter);
            pCommand->m_bGeneric=true;
            LoggerWrapper::GetInstance()->Write(LV_WARNING, "Note: Device manager has attached a device of type %d that this has no custom handler for.  This is normal for IR.", pDeviceData_Impl_Child->m_dwPK_DeviceTemplate);
        }
        pCommand->m_pParent = this;
        pCommand->CreateChildren();
        m_mapCommandImpl_Children[pDeviceData_Impl_Child->m_dwPK_Device] = pCommand;

        QString userName= QString::fromStdString(pDeviceData_Impl_Child->mapParameters_Find(DEVICEDATA_AuthUser_CONST));
        QString pass= QString::fromStdString(pDeviceData_Impl_Child->mapParameters_Find(DEVICEDATA_AuthPassword_CONST));
        QString cameraType=QString::fromStdString(pDeviceData_Impl_Child->mapParameters_Find(DEVICEDATA_Type_CONST)).toLower();
        QString cameraIp ="http://"+QString::fromStdString( pDeviceData_Impl_Child->GetIPAddress());
        int cameraId = pDeviceData_Impl_Child->m_dwPK_Device;
        int imagePort=QString::fromStdString(pDeviceData_Impl_Child->mapParameters_Find(DEVICEDATA_Port_CONST)).toInt();
        int commandPort =8080;
        QString cPath = QString::fromStdString(pDeviceData_Impl_Child->mapParameters_Find(DEVICEDATA_Path_CONST));
        QString name =QString::fromStdString(pDeviceData_Impl_Child->m_sDescription);
        QUrl url(cameraIp);
       QString callbackAddr = QString::fromStdString(m_sMyIPAddress);
        if(cameraType=="motion"){
            commandPort= QString::fromStdString(pDeviceData_Impl_Child->mapParameters_Find(DEVICEDATA_TCP_Port_CONST)).toInt();

            MotionPiCamera *p = new MotionPiCamera
                    (name,
                     userName,
                     pass,
                     imagePort,
                     commandPort,
                     url,
                     cameraId,
                     cPath,
                     callbackAddr
                     );

            //    mpManager->addCamera(p);
            connect(p, &MotionPiCamera::dispatchMotionEvent, this, &Linuxmce_NVR::handleMotionEvent, Qt::QueuedConnection);

            cam_list.append(p);

        } else if(cameraType=="http"){

            HttpSnapshotCamera *h = new HttpSnapshotCamera(
                        name,
                        userName,
                        pass,
                        imagePort,
                        commandPort,
                        url,
                        cameraId,
                        cPath
                        );
            connect(h, &MotionPiCamera::dispatchMotionEvent, this, &Linuxmce_NVR::handleMotionEvent, Qt::QueuedConnection);
            cam_list.append(h);

        } else if(cameraType=="rtsp"){

        };
        //create proxy
    }
}

void Linuxmce_NVR::OnReload()
{
   exit(0);
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void Linuxmce_NVR::SomeFunction()
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

//<-dceag-c277-b->

/** @brief COMMAND: #277 - Get Capture Video Frame */
/** Get a video frame from a specified camera number */
/** @param #19 Data */
/** The pointer to the video frame */
/** @param #112 CameraID */
/** The number (ID) of the camera where to capture from */

void Linuxmce_NVR::CMD_Get_Capture_Video_Frame(int iCameraID,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c277-e->
{
    qDebug() << "gimmie camera frame for " << iCameraID;
    cout << "Need to implement command #277 - Get Capture Video Frame" << endl;
    cout << "Parm #19 - Data  (data value)" << endl;
    cout << "Parm #112 - CameraID=" << iCameraID << endl;
}

//<-dceag-c756-b->

/** @brief COMMAND: #756 - Report Child Devices */
/** Report all the child sensors this has by firing an event 'Reporting Child Devices' */

void Linuxmce_NVR::CMD_Report_Child_Devices(string &sCMD_Result,Message *pMessage)
//<-dceag-c756-e->
{
    cout << "Need to implement command #756 - Report Child Devices" << endl;

}

//<-dceag-c757-b->

/** @brief COMMAND: #757 - Download Configuration */
/** Download new configuration data for this device */
/** @param #9 Text */
/** Any information the device may want to do the download */

void Linuxmce_NVR::CMD_Download_Configuration(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c757-e->
{
    cout << "Need to implement command #757 - Download Configuration" << endl;
    cout << "Parm #9 - Text=" << sText << endl;
}

//<-dceag-c760-b->

/** @brief COMMAND: #760 - Send Command To Child */
/** After reporting new child devices, there may be children we want to test, but we haven't done a quick reload router and can't send them messages directly.  This way we can send 'live' messages to children */
/** @param #10 ID */
/** The internal ID used for this device--not the Pluto device ID. */
/** @param #154 PK_Command */
/** The command to send */
/** @param #202 Parameters */
/** Parameters for the command in the format:
PK_CommandParameter|Value|... */

void Linuxmce_NVR::CMD_Send_Command_To_Child(string sID,int iPK_Command,string sParameters,string &sCMD_Result,Message *pMessage)
//<-dceag-c760-e->
{
    cout << "Need to implement command #760 - Send Command To Child" << endl;
    cout << "Parm #10 - ID=" << sID << endl;
    cout << "Parm #154 - PK_Command=" << iPK_Command << endl;
    cout << "Parm #202 - Parameters=" << sParameters << endl;
}

//<-dceag-c776-b->

/** @brief COMMAND: #776 - Reset */
/** Reset device. */
/** @param #51 Arguments */
/** Argument string
NOEMON or CANBUS */

void Linuxmce_NVR::CMD_Reset(string sArguments,string &sCMD_Result,Message *pMessage)
//<-dceag-c776-e->
{
    cout << "Need to implement command #776 - Reset" << endl;
    cout << "Parm #51 - Arguments=" << sArguments << endl;
}

//<-dceag-c788-b->

/** @brief COMMAND: #788 - StatusReport */
/** Test comand. Asq a report */
/** @param #51 Arguments */
/** Argument string */

void Linuxmce_NVR::CMD_StatusReport(string sArguments,string &sCMD_Result,Message *pMessage)
//<-dceag-c788-e->
{
    cout << "Need to implement command #788 - StatusReport" << endl;
    cout << "Parm #51 - Arguments=" << sArguments << endl;
}

void Linuxmce_NVR::handleMotionEvent(int device, bool motionDetected)
{

    DCE::Message *st = new Message(
                device,
                DEVICEID_EVENTMANAGER,
                DCE::PRIORITY_NORMAL,
                DCE::MESSAGETYPE_EVENT,
                EVENT_Movement_Detected_CONST,
                1 /* number of parameter's pairs (id, value) */,
                EVENTPARAMETER_OnOff_CONST,
                QString::number(motionDetected ? 1 : 0).toStdString().c_str()
                );
    this->m_pEvent->SendMessage(st);

    DCE::Message *tripped = new Message(
                device,
                DEVICEID_EVENTMANAGER,
                DCE::PRIORITY_NORMAL,
                DCE::MESSAGETYPE_EVENT,
                EVENT_Sensor_Tripped_CONST,
                1 /* number of parameter's pairs (id, value) */,
                EVENTPARAMETER_Tripped_CONST,
                QString::number(motionDetected ? 1 : 0).toStdString().c_str()
                );


    this->m_pEvent->SendMessage(tripped);

    DCE::Message *status = new Message(
                device,
                DEVICEID_EVENTMANAGER,
                DCE::PRIORITY_NORMAL,
                DCE::MESSAGETYPE_EVENT,
                EVENT_State_Changed_CONST,
                1 /* number of parameter's pairs (id, value) */,
                EVENTPARAMETER_State_CONST,
                QString(motionDetected ? "100" :"0").toStdString().c_str()
                );


    this->m_pEvent->SendMessage(status);

}


void Linuxmce_NVR::handleManagerMessage(QString msg)
{
    qDebug() << msg;
    LoggerWrapper::GetInstance()->Write(LV_STATUS, " %s ", msg.toStdString().c_str());
}

bool Linuxmce_NVR::createManager()
{
    int lp = DATA_Get_Listen_Port();
    qDebug() << lp;
    mp_manager = new NvrManager(lp);
    QObject::connect(this, &Linuxmce_NVR::addCamera, mp_manager, &NvrManager::addCamera, Qt::QueuedConnection);
    QObject::connect(mp_manager, &NvrManager::newManagerMessage, this, &Linuxmce_NVR::handleManagerMessage, Qt::QueuedConnection);
    QObject::connect(this, &Linuxmce_NVR::setDetectionState, mp_manager, &NvrManager::setDetectionStatus, Qt::QueuedConnection);

   // QObject::connect(mp_manager->listener(), &MotionEventListener::motionEvent, this, &Linuxmce_NVR::handleMotionEvent, Qt::QueuedConnection);
    return true;
}


