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
#include "qMediaPlayer.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->
#include <QRegExp>
#include <QStringList>
#include "mediabase/mediamanagerbase.h"
#include <QDebug>
#include <QNetworkInterface>
#include <QTimer>
//<-dceag-const-b->

// The primary constructor when the class is created as a stand-alone device
qMediaPlayer::qMediaPlayer(int DeviceID, string ServerAddress, MediaManagerBase *manager, bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
    : qMediaPlayer_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
    //<-dceag-const-e->
{
    setCommandResponse("Created media player as standalone with device number " + QString::number(DeviceID));
    qDebug() << "Created media player as standalone with device number " << QString::number(DeviceID);
    qWarning() << "Standalone Constructor";
    mp_manager = manager;
    qDebug() << Q_FUNC_INFO;

    if(GetConfig() && Connect(DEVICETEMPLATE_qMediaPlayer_CONST)){
        setCommandResponse("qMediaPlayer::Device "+QString::number(m_dwPK_Device)+" Connected.");
        setConnectionStatus(true);
    }else{
        setCommandResponse("qMediaPlayer::Connection failed for "+QString::fromStdString(this->m_sIPAddress)+" and device number"+QString::number(m_dwPK_Device));
        setConnectionStatus(false);
    }
    qWarning() << Q_FUNC_INFO << "end";
    seekDelayTimer = new QTimer(this);
    seekDelayTimer->setSingleShot(true);

    m_iTitle = 0;
    m_iChapter =0;
    s_totalTime="";
    s_audioTracks="";
    s_subTitleTracks ="";
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
qMediaPlayer::qMediaPlayer(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
    : qMediaPlayer_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
    //<-dceag-const2-e->
{
    if(GetConfig()){
        setCommandResponse("created media player as embedded device ");
        setCommandResponse("Device:: "+ m_dwPK_Device );
        setConnectionStatus(true);
    }else{
        setCommandResponse("error getting embedded video player started");
        setConnectionStatus(false);
    }
}

//<-dceag-dest-b->
qMediaPlayer::~qMediaPlayer()
//<-dceag-dest-e->
{
    Disconnect();
}

//<-dceag-getconfig-b->
bool qMediaPlayer::GetConfig()
{
    qDebug() << Q_FUNC_INFO << " enter ";
    if( !qMediaPlayer_Command::GetConfig() ){

        qWarning() << "QMediaPlayer is screwed up!";
        return false;
    }

    //<-dceag-getconfig-e->

    // Put your code here to initialize the data in this class
    // The configuration parameters DATA_ are now populated


    QString badMatch = QString::fromStdString(this->m_sIPAddress);
    int f = badMatch.lastIndexOf(".");
    qDebug() << "QMediaPlayer::" << badMatch.length() - f ;
    badMatch.remove(f, badMatch.length() - f);

    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            if(address.toString().contains(badMatch)){
                qDebug() << "QMediaPlayer::My Ip's" << address.toString() << ":: badMatch==>"<<badMatch;
                QString t = address.toString();
                DATA_Set_TCP_Address(t.toStdString(), true);
                qDebug() << Q_FUNC_INFO << " exit ";
                return true;
            }
        //CMD_Set_Device_Data setIp(this->m_dwPK_Device, 8, m_dwPK_Device, t.toStdString(), )
    }
    m_bPaused = false;
    qDebug() << Q_FUNC_INFO << " exit ";
    return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool qMediaPlayer::Register()
//<-dceag-reg-e->
{
    return Connect(PK_DeviceTemplate_get());
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
    cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You
    can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
qMediaPlayer_Command *Create_qMediaPlayer(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
    return new qMediaPlayer(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void qMediaPlayer::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void qMediaPlayer::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN COMMAND";
}

void qMediaPlayer::restart()
{
    if((GetConfig() ==true) && Connect(PK_DeviceTemplate_get())==true){
        setCommandResponse("qMediaPlayer::Device "+QString::number(m_dwPK_Device)+" Connected.");
        connected = true;
    }else{
        connected = false;
        setCommandResponse("qMediaPlayer::Connection failed for "+QString::fromStdString(this->m_sIPAddress)+" and device number "+QString::number(m_dwPK_Device));

    }
    mp_manager->setConnected(connected);
}

void qMediaPlayer::OnDisconnect()
{
    connected= false;
    mp_manager->setConnected(connected);
}

void qMediaPlayer::onReload()
{
    connected=false;
    mp_manager->setConnected(connected);
}

void qMediaPlayer::OnUnexpectedDisconnect()
{
    connected=false;
    mp_manager->setConnected(connected);
}


void qMediaPlayer::run()
{

}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void qMediaPlayer::SomeFunction()
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

//<-dceag-c28-b->

/** @brief COMMAND: #28 - Simulate Keypress */
/** Send a key to the device's OSD, or simulate keypresses on the device's panel */
/** @param #26 PK_Button */
/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
/** @param #41 StreamID */
/** ID of stream to apply */
/** @param #50 Name */
/** The application to send the keypress to. If not specified, it goes to the DCE device. */

void qMediaPlayer::CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
    cout << "Need to implement command #28 - Simulate Keypress" << endl;
    cout << "Parm #26 - PK_Button=" << sPK_Button << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
    cout << "Parm #50 - Name=" << sName << endl;
}

//<-dceag-c29-b->

/** @brief COMMAND: #29 - Simulate Mouse Click */
/** Simlate a mouse click at a certain position on the screen */
/** @param #11 Position X */
/** position X */
/** @param #12 Position Y */
/** position Y */
/** @param #41 StreamID */
/** ID of stream to apply */

void qMediaPlayer::CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c29-e->
{
    setCommandResponse("Need to implement command #29 - Simulate Mouse Click");
    cout << "Parm #11 - Position_X=" << iPosition_X << endl;
    cout << "Parm #12 - Position_Y=" << iPosition_Y << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c32-b->

/** @brief COMMAND: #32 - Update Object Image */
/** Display an image on the media player */
/** @param #3 PK_DesignObj */
/** The object in which to put the bitmap */
/** @param #14 Type */
/** 1=bmp, 2=jpg, 3=png */
/** @param #19 Data */
/** The contents of the bitmap, like reading from the file into a memory buffer */
/** @param #23 Disable Aspect Lock */
/** If 1, the image will be stretched to fit the object */

void qMediaPlayer::CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage)
//<-dceag-c32-e->
{
    cout << "qMediaPlayer::Need to implement command #32 - Update Object Image" << endl;
    cout << "Parm #3 - PK_DesignObj=" << sPK_DesignObj << endl;
    cout << "Parm #14 - Type=" << sType << endl;
    cout << "Parm #19 - Data  (data value)" << endl;
    cout << "Parm #23 - Disable_Aspect_Lock=" << sDisable_Aspect_Lock << endl;
}

//<-dceag-c37-b->

/** @brief COMMAND: #37 - Play Media */
/** This command will instruct a Media Player to play a media stream identified by a media descriptor created by the "Create Media" command. */
/** @param #29 PK_MediaType */
/** The type of media */
/** @param #41 StreamID */
/** The media that we need to play. */
/** @param #42 MediaPosition */
/** The position at which we need to start playing. */
/** @param #59 MediaURL */
/** The file to play, or other media id.  The format is specific on the media type and the media player. */

void qMediaPlayer::CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c37-e->
{
    qWarning() << "qMediaPlayer::Got Play Command";
    setCommandResponse("Recieved play command!");
    setMediaType(iPK_MediaType);
    setStreamID(iStreamID);
    setInternalMediaUrl(QString::fromStdString(sMediaURL));
    setStartPosition(QString::fromStdString(sMediaPosition));

    qWarning() << "Need to implement command #37 - Play Media" << endl;
    qWarning() << "Parm #29 - PK_MediaType=" << iPK_MediaType << endl;
    qWarning() << "Parm #41 - StreamID=" << iStreamID << endl;
    qWarning() << "Parm #42 - MediaPosition=" << QString::fromStdString(sMediaPosition) << endl;
    qWarning() << "Parm #59 - MediaURL=" << sMediaURL.c_str() << endl;


    QString p1 = QString::fromStdString(sMediaPosition);
    int posStart = p1.indexOf("POS:")+4;
    int posEnd = p1.indexOf(" TITLE:");
    QString p2 = p1.mid(posStart, posEnd-posStart);

    QString deviceNumber;
    QString path;
    QString finishedPath;
    QString localPath;

    QRegExp deviceNo("(\\\[\\\d+\\\]/)");
    int l = deviceNo.indexIn(QString::fromStdString(sMediaURL));

    if (l ==-1){
        setCommandResponse("Stored in /mediaType");
    } else {
        if(!deviceNo.isEmpty()){
            qDebug() << deviceNo.capturedTexts();
            QString f = deviceNo.cap(0);
            f.remove("["); f.remove("/"); f.remove("]");
            deviceNumber = f;

            localPath = (QString::fromStdString(sMediaURL)).split(deviceNo).at(1);
            setCurrentStorageDevice(f.toInt());
            qDebug() << localPath;
        }
    }


    //#ifdef NULL
    //    if(iPK_MediaType==5)
    //    {
    //        path = "/public/data/videos/";
    //    }
    //    else if(iPK_MediaType ==4){
    //        path = "/public/data/audio/";
    //    }

    //    if(QString::fromStdString(sMediaURL).contains("http")){
    //        finishedPath = QString::fromStdString(sMediaURL);
    //    }else if(iPK_MediaType==43){
    //        if(QString::fromStdString(sMediaURL).contains("http://")){
    //            finishedPath = QString::fromStdString(sMediaURL);
    //        }else{
    //            finishedPath = "http://"+QString::fromStdString(sMediaURL);
    //        }
    //    }
    //    else
    //    {
    //        //begin ugly bit to determine the storage device for later use. its not passed explicitly, so i use a regex to determine it for media files

    //            }
    //            else
    //            {

    //            }
    //        }
    //        //end ugly bit of regex. pretty because it works unless a user decides to also have [\d\d\d] type directories. I try to counter that by choosing only
    //        //the first match as that will be the first indexed by the regex engine.
    //        QString adjPath = QString::fromStdString(sMediaURL).remove("//home");
    //        finishedPath = "/mnt/remote/"+deviceNumber+path+localPath;
    //    }

    //#endif

    QString cmp = QString::fromStdString(sMediaURL.c_str());

    if(iPK_MediaType==43){
        if(cmp.contains("http://")){
            setCurrentMediaUrl(QString::fromStdString(sMediaURL));
        }else{
            setCurrentMediaUrl("http://"+QString::fromStdString(sMediaURL));
        }

    } else if(cmp.contains("http://")){
        setCurrentMediaUrl(QString::fromStdString(sMediaURL));

    }else{
        QString mod = QString::fromStdString(sMediaURL);

        if(mod.contains("/home/public/data")){
            mod = QString::fromStdString(sMediaURL).remove("/home/public/");
        } else {
            //handle private/upnp media?
        }

        QString externalPlayerPath = "http://"+QString::fromStdString(m_sIPAddress)+"/lmce-admin/"+mod;
        qWarning() << "Sending plugin modified original MRL==>" << externalPlayerPath;
        setCurrentMediaUrl(externalPlayerPath);
    }
    emit currentMediaFileChanged(cmp);

    //EVENT_Playback_Started(sMediaURL, i_StreamId, "Stored Media", "", "");
    emit startPlayback();

    if(p2.toInt() != 0){

        connect(seekDelayTimer, &QTimer::timeout, [=](){
            handleDelayedSeek( p2.length() < 4 ? p2.toInt() :  p2.toInt()/1000 );
        }  );
        QMetaObject::invokeMethod(seekDelayTimer, "start");
    }
    sCMD_Result="OK";


}

//<-dceag-c38-b->

/** @brief COMMAND: #38 - Stop Media */
/** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
/** @param #41 StreamID */
/** The media needing to be stopped. */
/** @param #42 MediaPosition */
/** The position at which this stream was last played. */

void qMediaPlayer::CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c38-e->
{
    setCommandResponse("Need to implement command #38 - Stop Media");
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
    //  cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;


    iStreamID = this->i_StreamId;
    string endPosition = getDcePosition();
    *sMediaPosition =endPosition;
    qDebug() << "Media end position " << endPosition.c_str();
    emit stopCurrentMedia();
    setCurrentMediaUrl("");
    sCMD_Result="OK";
}

//<-dceag-c39-b->

/** @brief COMMAND: #39 - Pause Media */
/** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
/** @param #41 StreamID */
/** The media stream for which we need to pause playback. */

void qMediaPlayer::CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c39-e->
{
    qDebug() << "qMediaPlayer::CMD_Pause_Media";
    setCommandResponse("Need to implement command #39 - Pause Media");

#if defined (QT4) && ! defined (ANDROID)
    if(mp_manager->mediaObject->state()==Phonon::PausedState){
        mp_manager->mediaObject->play();
        m_bPaused=false;
    } else {
        mp_manager->mediaObject->pause();
        m_bPaused=true;
    }



#else
    emit pausePlayback();
    setCommandResponse("Need to implement command #39 - Pause Media");
#endif
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
    sCMD_Result="OK";

}

//<-dceag-c40-b->

/** @brief COMMAND: #40 - Restart Media */
/** This will restart a media was paused with the above command */
/** @param #41 StreamID */
/** The media stream that we need to restart playback for. */

void qMediaPlayer::CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c40-e->
{
    cout << "Need to implement command #40 - Restart Media" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
#if defined(ANDROID) || defined(Q_OS_IOS)
    emit newMediaPosition(0);
#endif

#ifndef RPI
#if defined (QT4) && !defined (ANDROID)
    mp_manager->mediaObject->seek(0);
#endif
#endif
}

//<-dceag-c41-b->

/** @brief COMMAND: #41 - Change Playback Speed */
/** Will make the playback to FF with a configurable amount of speed. */
/** @param #41 StreamID */
/** The media needing the playback speed change. */
/** @param #43 MediaPlaybackSpeed */
/** The requested media playback speed * 1000.  -1000 = rev, 4000 = 4x fwd, -500 = rev 1/2.  Less than 10 = relative.  +2 = double, -1 = reverse.   See Media_Plugin::ReceivedMessage */
/** @param #220 Report */
/** If true, report this speed to the user on the OSD */

void qMediaPlayer::CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport,string &sCMD_Result,Message *pMessage)
//<-dceag-c41-e->
{
    cout << "Need to implement command #41 - Change Playback Speed" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
    cout << "Parm #43 - MediaPlaybackSpeed=" << iMediaPlaybackSpeed << endl;
    cout << "Parm #220 - Report=" << bReport << endl;
    if(iStreamID==i_StreamId){
        if(iMediaPlaybackSpeed==0){
            pausePlayback();
        } else if(iMediaPlaybackSpeed==1000) {
            pausePlayback();
        }
    }
    sCMD_Result="OK";
}

//<-dceag-c42-b->

/** @brief COMMAND: #42 - Jump to Position in Stream */
/** Jump to a position in the stream, specified in seconds. */
/** @param #5 Value To Assign */
/** The number of seconds.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.  A simpler command than Set Media Position */
/** @param #41 StreamID */
/** The stream */

void qMediaPlayer::CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c42-e->
{
    setCommandResponse(" Jump to Position in Stream");
    cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
    emit jumpToStreamPosition(QString::fromStdString(sValue_To_Assign).toInt());
    qDebug() << "qMediaPlayer::Jump_to_position_in_stream "<< sValue_To_Assign.c_str();

    //  quint64 tg = QString::fromStdString(sValue_To_Assign.c_str()).toInt();
    //  emit newMediaPosition(QString::fromStdString(sValue_To_Assign.c_str()).toInt());
    sCMD_Result="OK";
}

//<-dceag-c63-b->

/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
/** Raise  the channel, track, station, etc. by 1.  Same as Jump to Pos in Playlist with value +1 for a smart media player */
/** @param #41 StreamID */
/** ID of stream to apply */

void qMediaPlayer::CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c63-e->
{
    cout << "Need to implement command #63 - Skip Fwd - Channel/Track Greater" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c64-b->

/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
/** Lower the channel, track, station, etc. by 1.  Same as Jump to Pos in Playlist with value -1 for a smart media player */
/** @param #41 StreamID */
/** ID of stream to apply */

void qMediaPlayer::CMD_Skip_Back_ChannelTrack_Lower(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c64-e->
{
    cout << "Need to implement command #64 - Skip Back - Channel/Track Lower" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c65-b->

/** @brief COMMAND: #65 - Jump Position In Playlist */
/** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
/** @param #5 Value To Assign */
/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
/** @param #41 StreamID */
/** ID of stream to apply */

void qMediaPlayer::CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c65-e->
{
    cout << "Need to implement command #65 - Jump Position In Playlist" << endl;
    cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c81-b->

/** @brief COMMAND: #81 - Navigate Next */
/** Nagivate to the next possible navigable area. (The actual outcome depends on the specifc device) */
/** @param #41 StreamID */
/** The stream on which to do the navigation. */

void qMediaPlayer::CMD_Navigate_Next(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c81-e->
{
    cout << "Need to implement command #81 - Navigate Next" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c82-b->

/** @brief COMMAND: #82 - Navigate Prev */
/** Nagivate the previous possible navigable area. (The actual outcome depends on the specific device). */
/** @param #41 StreamID */
/** The stream on which to do the navigation. */

void qMediaPlayer::CMD_Navigate_Prev(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c82-e->
{
    cout << "Need to implement command #82 - Navigate Prev" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c84-b->

/** @brief COMMAND: #84 - Get Video Frame */
/** Get's the current video frame from the media player. */
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

void qMediaPlayer::CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage)
//<-dceag-c84-e->
{
    cout << "Need to implement command #84 - Get Video Frame" << endl;
    cout << "Parm #19 - Data  (data value)" << endl;
    cout << "Parm #20 - Format=" << sFormat << endl;
    cout << "Parm #23 - Disable_Aspect_Lock=" << sDisable_Aspect_Lock << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
    cout << "Parm #60 - Width=" << iWidth << endl;
    cout << "Parm #61 - Height=" << iHeight << endl;
#ifndef RPI
#if defined QT4 && ! defined (__ANDROID__)
    QImage t = mp_manager->getScreenShot();
    // t.copy(0, 0, mp_manager->videoSurface->width(), mp_manager->videoSurface->height());
    qWarning() << "ScreenShot Size==>" << t.size();

    char **mData =(char**)t.bits();
    int mIdata_size =  t.byteCount();

    pData =mData;
    iData_Size = &mIdata_size;
#endif
#endif

}

//<-dceag-c87-b->

/** @brief COMMAND: #87 - Goto Media Menu */
/** Goto to the current media Root Menu. */
/** @param #41 StreamID */
/** The stream ID */
/** @param #64 MenuType */
/** The type of menu that the user want to jump to.
(For DVD handlers usually this applies)
0 - Root menu
1 - Title menu
2 - Media menu */

void qMediaPlayer::CMD_Goto_Media_Menu(int iStreamID,int iMenuType,string &sCMD_Result,Message *pMessage)
//<-dceag-c87-e->
{
    cout << "Need to implement command #87 - Goto Media Menu" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
    cout << "Parm #64 - MenuType=" << iMenuType << endl;
}

//<-dceag-c92-b->

/** @brief COMMAND: #92 - Pause */
/** Pause the media */
/** @param #41 StreamID */
/** ID of stream to apply */

void qMediaPlayer::CMD_Pause(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c92-e->
{
    qDebug() << "qMediaPlayer::CMD_Pause";
    setCommandResponse("Need to implement command #92 - Pause");


#if defined (QT4) && ! defined (ANDROID)
    if(mp_manager->mediaObject->state()==Phonon::PausedState){
        mp_manager->mediaObject->play();
        m_bPaused=false;
    } else {
        mp_manager->mediaObject->pause();
        m_bPaused=true;
    }



#else
    if(!m_bPaused){
        emit pausePlayback();
        setCommandResponse("Need to implement command #39 - Pause Media");
    }
    else
    {
        emit startPlayback();

    }

    m_bPaused = !m_bPaused;
#endif
    cout << "Parm #41 - StreamID=" << iStreamID << endl;

#ifdef __ANDROID__
    emit pausePlayback();
#endif
    sCMD_Result="OK";
}

//<-dceag-c95-b->

/** @brief COMMAND: #95 - Stop */
/** Stop the media */
/** @param #41 StreamID */
/** ID of stream to apply */
/** @param #203 Eject */
/** If true, the drive will be ejected if there is no media currently playing, so a remote's stop button acts as stop/eject. */

void qMediaPlayer::CMD_Stop(int iStreamID,bool bEject,string &sCMD_Result,Message *pMessage)
//<-dceag-c95-e->
{
    cout << "Need to implement command #95 - Stop" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
    cout << "Parm #203 - Eject=" << bEject << endl;
    emit stopCurrentMedia();

}

//<-dceag-c97-b->

/** @brief COMMAND: #97 - Mute */
/** mute */

void qMediaPlayer::CMD_Mute(string &sCMD_Result,Message *pMessage)
//<-dceag-c97-e->
{
    cout << "Need to implement command #97 - Mute" << endl;
#ifndef RPI
#if defined (QT4) && ! defined (ANDROID)
    qreal c = mp_manager->audioSink->volume();
    qWarning() << "Current volume" << c;

    if(mp_manager->audioSink->isMuted()){
        mp_manager->audioSink->setMuted(false);
    } else {
        mp_manager->audioSink->setMuted(true);
    }
#else
    mp_manager->setMuted(true);
#endif

#endif
    qWarning("Toggled Mute");
}

//<-dceag-c123-b->

/** @brief COMMAND: #123 - Info */
/** VDR info */
/** @param #9 Text */
/** nimic */

void qMediaPlayer::CMD_Info(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c123-e->
{
    cout << "Need to implement command #123 - Info" << endl;
    cout << "Parm #9 - Text=" << sText << endl;
}

//<-dceag-c126-b->

/** @brief COMMAND: #126 - Guide */
/** Show guide information.  For a dvd this may be the menu, just like the menu command */

void qMediaPlayer::CMD_Guide(string &sCMD_Result,Message *pMessage)
//<-dceag-c126-e->
{
    cout << "Need to implement command #126 - Guide" << endl;
}

//<-dceag-c139-b->

/** @brief COMMAND: #139 - Play */
/** Play the media */
/** @param #41 StreamID */
/** ID of stream to apply */

void qMediaPlayer::CMD_Play(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c139-e->
{

    if(iStreamID==i_StreamId){
        setCommandResponse("Need to implement command #139 - Play");
        cout << "Parm #41 - StreamID=" << iStreamID << endl;
        emit startPlayback();
        sCMD_Result="PLAY-OK";
    } else {
        sCMD_Result="FAIL-BAD STREAM ID";
    }

}

//<-dceag-c140-b->

/** @brief COMMAND: #140 - Audio Track */
/** Go to an audio track */
/** @param #5 Value To Assign */
/** The audio track to go to.  Simple A/V equipment ignores this and just toggles. */
/** @param #41 StreamID */
/** ID of stream to apply */

void qMediaPlayer::CMD_Audio_Track(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c140-e->
{
    cout << "Need to implement command #140 - Audio Track" << endl;
    cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c141-b->

/** @brief COMMAND: #141 - Subtitle */
/** Go to a subtitle */
/** @param #5 Value To Assign */
/** The subtitle to go to.  Simple A/V equipment ignores this and just toggles. */
/** @param #41 StreamID */
/** ID of stream to apply */

void qMediaPlayer::CMD_Subtitle(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c141-e->
{
    cout << "Need to implement command #141 - Subtitle" << endl;
    cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c142-b->

/** @brief COMMAND: #142 - Angle */
/** Go to an angle */
/** @param #5 Value To Assign */
/** The angle to go to.  Simple A/V equipment ignores this and just toggles. */
/** @param #41 StreamID */
/** ID of stream to apply */

void qMediaPlayer::CMD_Angle(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c142-e->
{
    cout << "Need to implement command #142 - Angle" << endl;
    cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c190-b->

/** @brief COMMAND: #190 - Enter/Go */
/** Select the currently highlighted menu item */
/** @param #41 StreamID */
/** ID of stream to apply */

void qMediaPlayer::CMD_EnterGo(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c190-e->
{
    cout << "Need to implement command #190 - Enter/Go" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c200-b->

/** @brief COMMAND: #200 - Move Up */
/** Move the highlighter */
/** @param #41 StreamID */
/** ID of stream to apply */

void qMediaPlayer::CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c200-e->
{
    cout << "Need to implement command #200 - Move Up" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
#ifndef QT5
    //this->mp_manager->p
#endif
}

//<-dceag-c201-b->

/** @brief COMMAND: #201 - Move Down */
/** Move the highlighter */
/** @param #41 StreamID */
/** ID of stream to apply */

void qMediaPlayer::CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c201-e->
{
    cout << "Need to implement command #201 - Move Down" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c202-b->

/** @brief COMMAND: #202 - Move Left */
/** Move the highlighter */
/** @param #41 StreamID */
/** ID of stream to apply */

void qMediaPlayer::CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c202-e->
{
    cout << "Need to implement command #202 - Move Left" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c203-b->

/** @brief COMMAND: #203 - Move Right */
/** Move the highlighter */
/** @param #41 StreamID */
/** ID of stream to apply */

void qMediaPlayer::CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c203-e->
{
    cout << "Need to implement command #203 - Move Right" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c204-b->

/** @brief COMMAND: #204 - 0 */
/** 0 */

void qMediaPlayer::CMD_0(string &sCMD_Result,Message *pMessage)
//<-dceag-c204-e->
{
    cout << "Need to implement command #204 - 0" << endl;
}

//<-dceag-c205-b->

/** @brief COMMAND: #205 - 1 */
/** 1 */

void qMediaPlayer::CMD_1(string &sCMD_Result,Message *pMessage)
//<-dceag-c205-e->
{
    cout << "Need to implement command #205 - 1" << endl;
}

//<-dceag-c206-b->

/** @brief COMMAND: #206 - 2 */
/** 2 */

void qMediaPlayer::CMD_2(string &sCMD_Result,Message *pMessage)
//<-dceag-c206-e->
{
    cout << "Need to implement command #206 - 2" << endl;
}

//<-dceag-c207-b->

/** @brief COMMAND: #207 - 3 */
/** 3 */

void qMediaPlayer::CMD_3(string &sCMD_Result,Message *pMessage)
//<-dceag-c207-e->
{
    cout << "Need to implement command #207 - 3" << endl;
}

//<-dceag-c208-b->

/** @brief COMMAND: #208 - 4 */
/** 4 */

void qMediaPlayer::CMD_4(string &sCMD_Result,Message *pMessage)
//<-dceag-c208-e->
{
    cout << "Need to implement command #208 - 4" << endl;
}

//<-dceag-c209-b->

/** @brief COMMAND: #209 - 5 */
/** 5 */

void qMediaPlayer::CMD_5(string &sCMD_Result,Message *pMessage)
//<-dceag-c209-e->
{
    cout << "Need to implement command #209 - 5" << endl;
}

//<-dceag-c210-b->

/** @brief COMMAND: #210 - 6 */
/** 6 */

void qMediaPlayer::CMD_6(string &sCMD_Result,Message *pMessage)
//<-dceag-c210-e->
{
    cout << "Need to implement command #210 - 6" << endl;
}

//<-dceag-c211-b->

/** @brief COMMAND: #211 - 7 */
/** 7 */

void qMediaPlayer::CMD_7(string &sCMD_Result,Message *pMessage)
//<-dceag-c211-e->
{
    cout << "Need to implement command #211 - 7" << endl;
}

//<-dceag-c212-b->

/** @brief COMMAND: #212 - 8 */
/** 8 */

void qMediaPlayer::CMD_8(string &sCMD_Result,Message *pMessage)
//<-dceag-c212-e->
{
    cout << "Need to implement command #212 - 8" << endl;
}

//<-dceag-c213-b->

/** @brief COMMAND: #213 - 9 */
/** 9 */

void qMediaPlayer::CMD_9(string &sCMD_Result,Message *pMessage)
//<-dceag-c213-e->
{
    cout << "Need to implement command #213 - 9" << endl;
}

//<-dceag-c214-b->

/** @brief COMMAND: #214 - Save playlist */
/** This will instruct the device to save the currently playing list */
/** @param #17 PK_Users */
/** The user that will own the new playlist. Can be missing. It will pick the current user then. */
/** @param #45 PK_EntertainArea */
/** Which playlist to save. You can direct the command to save a specific entertainment area's playlist or you can leave it blank to pick the current entertainment area's playlist */
/** @param #50 Name */
/** It will use the this name when saving. If it is not specified it will either use the name of the loaded playlist in the database or it will generate a new one. */
/** @param #77 Save as new */
/** Save the playlist as a new playlist. This will override the default behaviour. (If this playlist was not loaded from the database it will be saved as new. If it was loaded it will be overridded). This will make it always save it as new. */

void qMediaPlayer::CMD_Save_playlist(int iPK_Users,string sPK_EntertainArea,string sName,bool bSave_as_new,string &sCMD_Result,Message *pMessage)
//<-dceag-c214-e->
{
    cout << "Need to implement command #214 - Save playlist" << endl;
    cout << "Parm #17 - PK_Users=" << iPK_Users << endl;
    cout << "Parm #45 - PK_EntertainArea=" << sPK_EntertainArea << endl;
    cout << "Parm #50 - Name=" << sName << endl;
    cout << "Parm #77 - Save_as_new=" << bSave_as_new << endl;
}

//<-dceag-c231-b->

/** @brief COMMAND: #231 - Load Playlist */
/** This will instruct the device to load the specific playlist. */
/** @param #45 PK_EntertainArea */
/** The entertainment area in which to load the  playlist. By defualt it will be the entertainment in which the current orbiter is running. */
/** @param #78 EK_Playlist */
/** The id of the playlist to load */

void qMediaPlayer::CMD_Load_Playlist(string sPK_EntertainArea,int iEK_Playlist,string &sCMD_Result,Message *pMessage)
//<-dceag-c231-e->
{
    cout << "Need to implement command #231 - Load Playlist" << endl;
    cout << "Parm #45 - PK_EntertainArea=" << sPK_EntertainArea << endl;
    cout << "Parm #78 - EK_Playlist=" << iEK_Playlist << endl;
}

//<-dceag-c240-b->

/** @brief COMMAND: #240 - Back / Prior Menu */
/** Navigate back .. ( Escape ) */
/** @param #41 StreamID */
/** ID of stream to apply */

void qMediaPlayer::CMD_Back_Prior_Menu(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c240-e->
{
    cout << "Need to implement command #240 - Back / Prior Menu" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c249-b->

/** @brief COMMAND: #249 - Start Streaming */
/** Like play media, but it means the destination device is not the same as the source */
/** @param #29 PK_MediaType */
/** The type of media */
/** @param #41 StreamID */
/** Identifier for this streaming session. */
/** @param #42 MediaPosition */
/** Where to start playing from */
/** @param #59 MediaURL */
/** The url to use to play this stream. */
/** @param #105 StreamingTargets */
/** Target destinations for streaming. Semantics dependent on the target device. */

void qMediaPlayer::CMD_Start_Streaming(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string sStreamingTargets,string &sCMD_Result,Message *pMessage)
//<-dceag-c249-e->
{
    qWarning() << "Need to implement command #249 - Start Streaming" << endl;
    qWarning() << "Parm #29 - PK_MediaType=" << iPK_MediaType << endl;
    qWarning() << "Parm #41 - StreamID=" << iStreamID << endl;
    qWarning() << "Parm #42 - MediaPosition=" << sMediaPosition.c_str() << endl;
    qWarning() << "Parm #59 - MediaURL=" << sMediaURL.c_str() << endl;
    qWarning() << "Parm #105 - StreamingTargets=" << sStreamingTargets.c_str() << endl;

    QStringList spl= QString::fromStdString(sStreamingTargets).split(",");
    QStringList::const_iterator it;
    for (it = spl.constBegin(); it != spl.constEnd();++it){
        qWarning()<< "\t Target::"<< (*it).toLocal8Bit().constData();
    }

    //DCE::CMD_Play_Media slavePlay(this->m_dwPK_Device, tDevice,iPK_MediaType, sMediaPosition, sMediaURL);
    sCMD_Result="OK";
    qWarning() << "qMediaPlayer::Got Stream Play Command";
    setCommandResponse("Recieved stream play command!");
    setMediaType(iPK_MediaType);
    setStreamID(iStreamID);
    setStartPosition(QString::fromStdString(sMediaPosition));

    QString deviceNumber;
    QString path;
    QString finishedPath;
    QString localPath;

    if(iPK_MediaType==5)
    {
        path = "/public/data/videos/";
    }
    else if(iPK_MediaType ==4){
        path = "/public/data/audio/";
    }

    if(QString::fromStdString(sMediaURL).contains("http")){
        finishedPath = QString::fromStdString(sMediaURL);
    }else if(iPK_MediaType==43){
        if(QString::fromStdString(sMediaURL).contains("http://")){
            finishedPath = QString::fromStdString(sMediaURL);
        }else{
            finishedPath = "http://"+QString::fromStdString(sMediaURL);
        }
    }
    else {
        //begin ugly bit to determine the storage device for later use. its not passed explicitly, so i use a regex to determine it for media files
        QRegExp deviceNo("(\\\[\\\d+\\\]/)");
        int l = deviceNo.indexIn(QString::fromStdString(sMediaURL));

        if (l ==-1){
            setCommandResponse("Stored in /mediaType");
        } else {
            if(!deviceNo.isEmpty()){
                qDebug() << deviceNo.capturedTexts();
                QString f = deviceNo.cap(0);
                f.remove("["); f.remove("/"); f.remove("]");
                deviceNumber = f;
                localPath = (QString::fromStdString(sMediaURL)).split(deviceNo).at(1);
#ifndef ANDROID
                mp_manager->setCurrentStorageDevice(f.toLong());
#endif
            }
            else
            {

            }
        }
        //end ugly bit of regex. pretty because it works unless a user decides to also have [\d\d\d] type directories. I try to counter that by choosing only
        //the first match as that will be the first indexed by the regex engine.
        QString adjPath = QString::fromStdString(sMediaURL).remove("//home");
        finishedPath = "/mnt/remote/"+deviceNumber+path+localPath;
    }


#if defined(ANDROID) || defined(Q_OS_IOS)
    QString cmp = QString::fromStdString(sMediaURL.c_str());

    if(iPK_MediaType==43){
        if(cmp.contains("http://")){
            setCurrentMediaUrl(QString::fromStdString(sMediaURL));
        }else{
            setCurrentMediaUrl("http://"+QString::fromStdString(sMediaURL));
        }

    } else if(cmp.contains("http://")){
        setCurrentMediaUrl(QString::fromStdString(sMediaURL));

    }else{
        QString androidPath = "http://"+QString::fromStdString(m_sIPAddress)+"/lmce-admin/qOrbiterGenerator.php?id="+QString::fromStdString(sMediaURL);
        qWarning() << "Sending android modified original MRL==>" << androidPath;
        setCurrentMediaUrl(androidPath);
    }
#else
    qWarning() << "Sending string URL::" << finishedPath;
    setCurrentMediaUrl(finishedPath);
    //EVENT_Playback_Started(sMediaURL, i_StreamId, "Stored Media", "", "");
    emit startPlayback();
#endif

    emit startPlayback();
    sCMD_Result="OK";
}

//<-dceag-c259-b->

/** @brief COMMAND: #259 - Report Playback Position */
/** This will report the playback position of the current stream. */
/** @param #9 Text */
/** A human readable representation of the current position */
/** @param #41 StreamID */
/** The stream ID on which to report the position. */
/** @param #42 MediaPosition */
/** A media player readable representation of the current position including all options */

void qMediaPlayer::CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c259-e->
{
    cout << "Need to implement command #259 - Report Playback Position" << endl;
    cout << "Parm #9 - Text=" << sText << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
    cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;
    if(iStreamID==this->i_StreamId){
        string sTextReturn=mp_manager->currentPosition().toStdString().c_str();
        sText= &sTextReturn;
        string returnString = mp_manager->currentPosition().toStdString().c_str();
        sMediaPosition = &returnString;
    }

    sCMD_Result="OK";
}

//<-dceag-c269-b->

/** @brief COMMAND: #269 - Move Playlist entry Up */
/** Moves a entry up in the current playlist. */
/** @param #48 Value */
/** The id of the entry that needs to be moved up. */

void qMediaPlayer::CMD_Move_Playlist_entry_Up(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c269-e->
{
    cout << "Need to implement command #269 - Move Playlist entry Up" << endl;
    cout << "Parm #48 - Value=" << iValue << endl;
}

//<-dceag-c270-b->

/** @brief COMMAND: #270 - Move Playlist entry Down */
/** Moves a entry down in the current playlist. */
/** @param #48 Value */
/** The id of the entry that needs to be moved down in the playlist. */

void qMediaPlayer::CMD_Move_Playlist_entry_Down(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c270-e->
{
    cout << "Need to implement command #270 - Move Playlist entry Down" << endl;
    cout << "Parm #48 - Value=" << iValue << endl;
}

//<-dceag-c271-b->

/** @brief COMMAND: #271 - Remove playlist entry. */
/** Removes an entry from the playlist. */
/** @param #48 Value */
/** The Id of the entry that needs to be removed from the playlist. */

void qMediaPlayer::CMD_Remove_playlist_entry(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c271-e->
{
    cout << "Need to implement command #271 - Remove playlist entry." << endl;
    cout << "Parm #48 - Value=" << iValue << endl;
}

//<-dceag-c412-b->

/** @brief COMMAND: #412 - Set Media Position */
/** Jump to a certain media position */
/** @param #41 StreamID */
/** The stream to set */
/** @param #42 MediaPosition */
/** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

void qMediaPlayer::CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage)
//<-dceag-c412-e->
{
    cout << "Need to implement command #412 - Set Media Position" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
    cout << "Parm #42 - MediaPosition=" << sMediaPosition << endl;

    QStringList positionParams = QString::fromStdString(sMediaPosition).split(" ");
    QMap<QString, QString> positionValues;

    foreach (QString t, positionParams){
        qDebug() << t;
        QStringList tm = t.split(":");
        if(tm.count()==2)
            positionValues.insert(tm.at(0), tm.at(1));
    }

    QString val =positionValues.value("POS");
    qDebug() << "Qmediaplayer::set_media_position::"<<val;

    emit newMediaPosition(val.toInt());
}

//<-dceag-c455-b->

/** @brief COMMAND: #455 - Blue */
/** VDR blue */

void qMediaPlayer::CMD_Blue(string &sCMD_Result,Message *pMessage)
//<-dceag-c455-e->
{
    cout << "Need to implement command #455 - Blue" << endl;
}

//<-dceag-c518-b->

/** @brief COMMAND: #518 - Green */
/** VDR Green */

void qMediaPlayer::CMD_Green(string &sCMD_Result,Message *pMessage)
//<-dceag-c518-e->
{
    cout << "Need to implement command #518 - Green" << endl;
}

//<-dceag-c548-b->

/** @brief COMMAND: #548 - Menu */
/** Show a menu associated with this media */
/** @param #9 Text */
/** A string indicating which menu should appear.  The parameter is only used for smart media devices */
/** @param #41 StreamID */
/** ID of stream to apply */

void qMediaPlayer::CMD_Menu(string sText,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c548-e->
{
    cout << "Need to implement command #548 - Menu" << endl;
    cout << "Parm #9 - Text=" << sText << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c605-b->

/** @brief COMMAND: #605 - Red */
/** VDR red */

void qMediaPlayer::CMD_Red(string &sCMD_Result,Message *pMessage)
//<-dceag-c605-e->
{
    cout << "Need to implement command #605 - Red" << endl;
}

//<-dceag-c651-b->

/** @brief COMMAND: #651 - Thumbs Down */
/** Signify currently playing song has thumbs down. */
/** @param #41 StreamID */
/** For Smart Media Players, the currently Playing Stream ID */

void qMediaPlayer::CMD_Thumbs_Down(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c651-e->
{
    cout << "Need to implement command #651 - Thumbs Down" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c652-b->

/** @brief COMMAND: #652 - Thumbs Up */
/** Signify currently playing song in stream has thumbs up. */
/** @param #41 StreamID */
/** For smart media players, the currently playing media stream. */

void qMediaPlayer::CMD_Thumbs_Up(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c652-e->
{
    cout << "Need to implement command #652 - Thumbs Up" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c682-b->

/** @brief COMMAND: #682 - Yellow */
/** VDR yellow */

void qMediaPlayer::CMD_Yellow(string &sCMD_Result,Message *pMessage)
//<-dceag-c682-e->
{
    cout << "Need to implement command #682 - Yellow" << endl;
}

//<-dceag-c780-b->

/** @brief COMMAND: #780 - Remove playlist */
/** Removes a playlist from database */
/** @param #78 EK_Playlist */
/** The id of the playlist to be removed. */

void qMediaPlayer::CMD_Remove_playlist(int iEK_Playlist,string &sCMD_Result,Message *pMessage)
//<-dceag-c780-e->
{
    cout << "Need to implement command #780 - Remove playlist" << endl;
    cout << "Parm #78 - EK_Playlist=" << iEK_Playlist << endl;
}

//<-dceag-c812-b->

/** @brief COMMAND: #812 - Application Exited */
/** Notify us that Myth Player exited */
/** @param #227 PID */
/** Process ID to be passed to the ApplicationExited function */
/** @param #228 Exit Code */
/** Exit Code to be passed to the ApplicationExited function */

void qMediaPlayer::CMD_Application_Exited(int iPID,int iExit_Code,string &sCMD_Result,Message *pMessage)
//<-dceag-c812-e->
{
    cout << "Need to implement command #812 - Application Exited" << endl;
    cout << "Parm #227 - PID=" << iPID << endl;
    cout << "Parm #228 - Exit_Code=" << iExit_Code << endl;
}

//<-dceag-c916-b->

/** @brief COMMAND: #916 - Set Aspect Ratio */
/** Force aspect ratio */
/** @param #41 StreamID */
/** ID of stream to apply */
/** @param #260 Aspect Ratio */
/** aspect ratio to set: auto, 1:1, 4:3, 16:9, 2.11:1 */

void qMediaPlayer::CMD_Set_Aspect_Ratio(int iStreamID,string sAspect_Ratio,string &sCMD_Result,Message *pMessage)
//<-dceag-c916-e->
{
    cout << "Need to implement command #916 - Set Aspect Ratio" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
    cout << "Parm #260 - Aspect_Ratio=" << sAspect_Ratio << endl;
    emit setAspectRatio(QString::fromStdString(sAspect_Ratio));
}

//<-dceag-c917-b->

/** @brief COMMAND: #917 - Set Zoom */
/** Sets zoom level, relative, absolute or 'auto' */
/** @param #41 StreamID */
/** ID of stream to apply */
/** @param #261 Zoom Level */
/** Zoom level to set */

void qMediaPlayer::CMD_Set_Zoom(int iStreamID,string sZoom_Level,string &sCMD_Result,Message *pMessage)
//<-dceag-c917-e->
{
    cout << "Need to implement command #917 - Set Zoom" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
    cout << "Parm #261 - Zoom_Level=" << sZoom_Level << endl;
    emit setZoomLevel(QString::fromStdString(sZoom_Level));
}

//<-dceag-c920-b->

/** @brief COMMAND: #920 - Set Media ID */
/** Set Media ID - information about media stream */
/** @param #10 ID */
/** Media ID (special format) */
/** @param #41 StreamID */
/** ID of stream to set media information for */

void qMediaPlayer::CMD_Set_Media_ID(string sID,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c920-e->
{
    cout << "Need to implement command #920 - Set Media ID" << endl;
    cout << "Parm #10 - ID=" << sID << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
    setMediaID(QString::fromStdString(sID));

}

//<-dceag-c1076-b->

/** @brief COMMAND: #1076 - Add Station */
/** Add Radio Station to this User's list of stations. */
/** @param #10 ID */
/** The Returned Station ID */
/** @param #14 Type */
/** Station Type. For Pandora this is default or genre */
/** @param #41 StreamID */
/** The Stream ID to modify. */
/** @param #50 Name */
/** The name of the Station to add. For genre this is a Genre ID. For Default, This is used as a Search term. */

void qMediaPlayer::CMD_Add_Station(string sType,int iStreamID,string sName,string *sID,string &sCMD_Result,Message *pMessage)
//<-dceag-c1076-e->
{
    cout << "Need to implement command #1076 - Add Station" << endl;
    cout << "Parm #10 - ID=" << sID << endl;
    cout << "Parm #14 - Type=" << sType << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
    cout << "Parm #50 - Name=" << sName << endl;
}

//<-dceag-c1077-b->

/** @brief COMMAND: #1077 - Remove Station */
/** Remove Radio Station from this User's List of stations. */
/** @param #10 ID */
/** Station ID to remove. */
/** @param #41 StreamID */
/** The Stream ID from which to remove */

void qMediaPlayer::CMD_Remove_Station(string sID,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c1077-e->
{
    cout << "Need to implement command #1077 - Remove Station" << endl;
    cout << "Parm #10 - ID=" << sID << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c1078-b->

/** @brief COMMAND: #1078 - Rename Station */
/** Rename Radio Station in a user's list of radio stations. */
/** @param #10 ID */
/** The Radio Station ID to rename */
/** @param #41 StreamID */
/** The Stream ID */
/** @param #50 Name */
/** The New Name of the Radio Station */

void qMediaPlayer::CMD_Rename_Station(string sID,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c1078-e->
{
    cout << "Need to implement command #1078 - Rename Station" << endl;
    cout << "Parm #10 - ID=" << sID << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
    cout << "Parm #50 - Name=" << sName << endl;
}

//<-dceag-c1079-b->

/** @brief COMMAND: #1079 - Explain Song */
/** Ask the radio station for an explanation of why this song is playing. */
/** @param #41 StreamID */
/** The Stream ID of the currently playing song. */

void qMediaPlayer::CMD_Explain_Song(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c1079-e->
{
    cout << "Need to implement command #1079 - Explain Song" << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c1080-b->

/** @brief COMMAND: #1080 - Add Music to Station */
/** Add Music to an already existing Radio Station */
/** @param #10 ID */
/** The Radio Station ID to add terms to */
/** @param #41 StreamID */
/** The Stream ID */
/** @param #50 Name */
/** Search terms such as an artist or song title. */

void qMediaPlayer::CMD_Add_Music_to_Station(string sID,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c1080-e->
{
    cout << "Need to implement command #1080 - Add Music to Station" << endl;
    cout << "Parm #10 - ID=" << sID << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
    cout << "Parm #50 - Name=" << sName << endl;
}

//<-dceag-c1081-b->

/** @brief COMMAND: #1081 - Add Station to QuickMix */
/** Add A Radio Station to the Currently Playing QuickMix */
/** @param #10 ID */
/** The Station ID to add */
/** @param #41 StreamID */
/** The StreamID with the quickmix to modify */

void qMediaPlayer::CMD_Add_Station_to_QuickMix(string sID,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c1081-e->
{
    cout << "Need to implement command #1081 - Add Station to QuickMix" << endl;
    cout << "Parm #10 - ID=" << sID << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

//<-dceag-c1082-b->

/** @brief COMMAND: #1082 - Remove Station from QuickMix */
/** Remove Radio Station from the currently playing QuickMix */
/** @param #10 ID */
/** The Station ID */
/** @param #41 StreamID */
/** The Stream ID */

void qMediaPlayer::CMD_Remove_Station_from_QuickMix(string sID,int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c1082-e->
{
    cout << "Need to implement command #1082 - Remove Station from QuickMix" << endl;
    cout << "Parm #10 - ID=" << sID << endl;
    cout << "Parm #41 - StreamID=" << iStreamID << endl;
}

void qMediaPlayer::setCommandResponse(QString r)
{
    {commandResponse = QTime::currentTime().toString()+"-QMediaPlayer:: "+ r; emit commandResponseChanged(commandResponse);}
}
long qMediaPlayer::getCurrentStorageDevice() const
{
    return m_currentStorageDevice;
}

void qMediaPlayer::setCurrentStorageDevice(long currentStorageDevice)
{
    m_currentStorageDevice = currentStorageDevice;
    emit currentStorageDeviceChanged(currentStorageDevice);
}


string qMediaPlayer::getDcePosition()
{
    string dcePosition;

    QString currentTime = QString::number( (mp_manager->currentPositionNumeric() )  );
    dcePosition+=" CHAPTER:0";
    dcePosition+=" POS:"+currentTime.toStdString();
    dcePosition+=" TITLE:0";
    dcePosition+=" SUBTITLE:-1";
    dcePosition+=" AUDIO:-1";
    dcePosition+=" TOTAL:"+StringUtils::itos( (mp_manager->totalTimeNumeric() ) );

    return dcePosition;

}
int qMediaPlayer::getCurrentFkFileType() const
{
    return m_currentFkFileType;
}

void qMediaPlayer::setCurrentFkFileType(int currentFkFileType)
{
    m_currentFkFileType = currentFkFileType;
}

void qMediaPlayer::handleDelayedSeek(int seekTime)
{
    qDebug() << Q_FUNC_INFO << seekTime;
    jumpToStreamPosition(seekTime);
}

int qMediaPlayer::getCurrentSpeed() const
{
    return m_currentSpeed;
}

void qMediaPlayer::setCurrentSpeed(int currentSpeed)
{
    m_currentSpeed = currentSpeed;
}

QString qMediaPlayer::getInternalMediaUrl() const
{
    return m_internalMediaUrl;
}

void qMediaPlayer::setInternalMediaUrl(const QString &internalMediaUrl)
{
    m_internalMediaUrl = internalMediaUrl;
}


//<-dceag-c102-b->

/** @brief COMMAND: #102 - Record */
/** Record the current game. Toggle on off */

void qMediaPlayer::CMD_Record(string &sCMD_Result,Message *pMessage)
//<-dceag-c102-e->
{}
//<-dceag-c882-b->

/** @brief COMMAND: #882 - Abort Task */
/** Abort a pending task */
/** @param #248 Parameter ID */
/** The ID of the task to abort */

void qMediaPlayer::CMD_Abort_Task(int iParameter_ID,string &sCMD_Result,Message *pMessage)
//<-dceag-c882-e->
{}
//<-dceag-c942-b->

/** @brief COMMAND: #942 - Get Ripping Status */
/** Tell Game to Start 1 Player */
/** @param #199 Status */
/** Ripping status */

void qMediaPlayer::CMD_Get_Ripping_Status(string *sStatus,string &sCMD_Result,Message *pMessage)
//<-dceag-c942-e->
{}
//<-dceag-c943-b->

/** @brief COMMAND: #943 - Game 1P Start */
/** 1P start */

void qMediaPlayer::CMD_Game_1P_Start(string &sCMD_Result,Message *pMessage)
//<-dceag-c943-e->
{}
//<-dceag-c944-b->

/** @brief COMMAND: #944 - Game 2P Start */
/** 2P Start */

void qMediaPlayer::CMD_Game_2P_Start(string &sCMD_Result,Message *pMessage)
//<-dceag-c944-e->
{}
//<-dceag-c945-b->

/** @brief COMMAND: #945 - Game 3P Start */
/** 3P Start */

void qMediaPlayer::CMD_Game_3P_Start(string &sCMD_Result,Message *pMessage)
//<-dceag-c945-e->
{}
//<-dceag-c946-b->

/** @brief COMMAND: #946 - Game 4P Start */
/** 4P Start */

void qMediaPlayer::CMD_Game_4P_Start(string &sCMD_Result,Message *pMessage)
//<-dceag-c946-e->
{}
//<-dceag-c947-b->

/** @brief COMMAND: #947 - Game Insert Coin */
/** Insert Coin */

void qMediaPlayer::CMD_Game_Insert_Coin(string &sCMD_Result,Message *pMessage)
{}
//<-dceag-c947-e->

//<-dceag-c948-b->

/** @brief COMMAND: #948 - Game Service */
/** Service Mode */

void qMediaPlayer::CMD_Game_Service(string &sCMD_Result,Message *pMessage)
//<-dceag-c948-e->
{}
//<-dceag-c949-b->

/** @brief COMMAND: #949 - Game Start */
/** Game Start */

void qMediaPlayer::CMD_Game_Start(string &sCMD_Result,Message *pMessage)
{}
//<-dceag-c949-e->
//<-dceag-c950-b->

/** @brief COMMAND: #950 - Game Select */
/** Game Select */

void qMediaPlayer::CMD_Game_Select(string &sCMD_Result,Message *pMessage)
{

}
//<-dceag-c950-e->

//<-dceag-c951-b->

/** @brief COMMAND: #951 - Game Option */
/** Game Option */
void qMediaPlayer::CMD_Game_Option(string &sCMD_Result,Message *pMessage)

{

}
//<-dceag-c951-e->
//<-dceag-c952-b->

/** @brief COMMAND: #952 - Game Reset */
/** Game Reset */

void qMediaPlayer::CMD_Game_Reset(string &sCMD_Result,Message *pMessage)
{

}
//<-dceag-c952-e->

//<-dceag-c982-b->

/** @brief COMMAND: #982 - Set Game Options */
/** Set Options for the running Game System driver. */
/** @param #5 Value To Assign */
/** Dependent on driver, but usually a single line in the format of key,value */
/** @param #219 Path */
/** The Option to Set */

void qMediaPlayer::CMD_Set_Game_Options(string sValue_To_Assign,string sPath,string &sCMD_Result,Message *pMessage)
{

}
//<-dceag-c982-e->

//<-dceag-c983-b->

/** @brief COMMAND: #983 - Get Game Options */
/** Get Options for the currently running driver. */
/** @param #5 Value To Assign */
/** The Returned value. */
/** @param #219 Path */
/** The Parameter to return, usually left side of comma in Set Game Options. */

void qMediaPlayer::CMD_Get_Game_Options(string sPath,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c983-e->
{}


void qMediaPlayer::mediaEnded(bool status)
{
    qDebug("Stream Ended");
    // DCE::CMD_Stop finished(this->m_dwPK_Device, this->m_dwPK_Device, i_StreamId, false );

    string mrl = getInternalMediaUrl().toStdString();

    EVENT_Playback_Completed(mrl, i_StreamId, status);
    // EVENT_Playback_Started(currentMediaUrl.toStdString(), i_StreamId, "Media", "none", "none");
}
//<-dceag-c616-b->

/** @brief COMMAND: #616 - Select A */
/** Select P1 keypad on supported game systems. */

void qMediaPlayer::CMD_Select_A(string &sCMD_Result,Message *pMessage){

}
//<-dceag-c616-e->
//<-dceag-c617-b->

/** @brief COMMAND: #617 - Select B */
/** Select P2 keypad on supported game systems. */

void qMediaPlayer::CMD_Select_B(string &sCMD_Result,Message *pMessage){

}
//<-dceag-c617-e->
//<-dceag-c89-b->

/** @brief COMMAND: #89 - Vol Up */
/** volume up */
/** @param #72 Repeat Command */
/** If specified, repeat the volume up this many times */

void qMediaPlayer::CMD_Vol_Up(int iRepeat_Command,string &sCMD_Result,Message *pMessage){
    cout << "Need to implement command #89 - Volume Up" << endl;
    cout << "Repeat Count=" << iRepeat_Command << endl;
#ifndef RPI
#if defined (QT4) && ! defined (ANDROID)
    qreal c = mp_manager->audioSink->volume();
    qWarning() << "Current volume" << c;
    qreal d = c+.10;
    if(d > 1.5){
        mp_manager->setErrorStatus("Max volume");
    } else {
        qWarning() << "New volume" << d;
        mp_manager->audioSink->setVolume(d);
    }

#endif
#endif

    pluginVolumeUp();

    qWarning("Set audio level Up.");
    //  emit volumeDown(iRepeat_Command);
    sCMD_Result="OK";
}

//<-dceag-c89-e->
//<-dceag-c90-b->

/** @brief COMMAND: #90 - Vol Down */
/** volume down */
/** @param #72 Repeat Command */
/** If specified, repeat the volume down this many times. */

void qMediaPlayer::CMD_Vol_Down(int iRepeat_Command,string &sCMD_Result,Message *pMessage){
#ifndef RPI
#if defined (QT4) && ! defined (ANDROID)
    qreal c = mp_manager->audioSink->volume();
    qWarning() << "Current volume" << c;
    qreal d = c-0.10;
    if(d < 0 ){
        mp_manager->setErrorStatus("Min volume");
    } else {
        mp_manager->audioSink->setVolume(d);
        qWarning() << "New volume" << d;
    }

#endif
#endif


    pluginVolumeDown();
    qWarning("Set audio level down.");
    sCMD_Result="OK";

}
//<-dceag-c90-e->
//<-dceag-c184-b->

/** @brief COMMAND: #184 - Set Level */
/** Set discreet volume level */
/** @param #76 Level */
/** The level to set, as a value between 0 (off) and 100 (full).  It can be preceeded with a - or + indicating a relative value.  +20 means up 20%. */

void qMediaPlayer::CMD_Set_Level(string sLevel,string &sCMD_Result,Message *pMessage){
    QString t = QString::fromStdString(sLevel.c_str());
#ifndef RPI
#if defined (QT4) && ! defined (ANDROID)
    if(t.contains("+") || t.contains("-")){
        if(t.contains("+")){
            qreal e = mp_manager->audioSink->volume();
            qreal inc = t.remove("+").toDouble()/100;
            qreal n = e+inc;
            mp_manager->audioSink->setVolume(n);
        }
        else if( t.contains("-")){
            qreal e = mp_manager->audioSink->volume();
            qreal inc = t.remove("-").toInt();
            qreal n = e-inc;
            if(n>0)
                mp_manager->audioSink->setVolume(n);
            else
                mp_manager->audioSink->setVolume(0);
        }

    }
    else{
        int csLevl = t.toInt();
        mp_manager->audioSink->setVolume(csLevl);
        sCMD_Result = "OK";
    }

#endif
#endif
    emit audioLevelChanged(t);
    sCMD_Result="OK";
}
//<-dceag-c184-e->


void qMediaPlayer::updateMetadata(QString mediaTitle, QString mediaSubtitle, QString name, int screen)
{


    string mst = mediaSubtitle.toStdString();
    string mt = mediaTitle.toStdString();
    string nm = name.toStdString()+"\t" +mt;
    EVENT_Playback_Info_Changed(nm,mt,mst);

}

void qMediaPlayer::confirmMediaStarted(QString description)
{
    string mrl = getInternalMediaUrl().toStdString();
    string desc = description.toStdString();

    EVENT_Playback_Started(m_internalMediaUrl.toStdString(), i_StreamId, desc, "true", "true");
}

void qMediaPlayer::confirmMediaEnded(bool witherror)
{
    qDebug() << Q_FUNC_INFO;
    string mrl = getInternalMediaUrl().toStdString();
    EVENT_Playback_Completed(m_internalMediaUrl.toStdString(), i_StreamId, witherror);
}

void qMediaPlayer::positionChanged(QString total, QString current)
{

    string mediaurl = getInternalMediaUrl().toStdString();
    string media_id = getMediaId().toStdString();
    string tt = total.toStdString();
    string ct = current.toStdString();
    EVENT_Media_Position_Changed(
                getMediaType(),
                mediaurl,
                media_id,
                i_StreamId,
                tt,
                ct,
                1
                );
}
