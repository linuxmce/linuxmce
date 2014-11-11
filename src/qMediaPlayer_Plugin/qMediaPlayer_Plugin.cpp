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
#include "qMediaPlayer_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Variable.h"
#include "../pluto_main/Define_DataGrid.h"
#include "../pluto_main/Define_DeviceData.h"
#include "../pluto_main/Define_Event.h"
#include "../pluto_main/Define_DeviceData.h"
#include "../pluto_main/Define_DeviceTemplate.h"
#include "../pluto_main/Table_Users.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
qMediaPlayer_Plugin::qMediaPlayer_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: qMediaPlayer_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
, m_QMediaMediaMutex("Qt media mutex")
{
  m_QMediaMediaMutex.Init(NULL);

}

//<-dceag-dest-b->
qMediaPlayer_Plugin::~qMediaPlayer_Plugin()
//<-dceag-dest-e->
{
//    PLUTO_SAFETY_LOCK(qm,m_QMediaMediaMutex);
//    pthread_mutex_destroy(&m_QMediaMediaMutex.mutex);
}

//<-dceag-getconfig-b->
bool qMediaPlayer_Plugin::GetConfig()
{
	if( !qMediaPlayer_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	// Put your code here to initialize the data in this class
	// The configuration parameters DATA_ are now populated
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool qMediaPlayer_Plugin::Register()
//<-dceag-reg-e->
{
    m_iPriority=DATA_Get_Priority();

    m_pMedia_Plugin=( Media_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Media_Plugin_CONST);
    m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
    if( !m_pMedia_Plugin || !m_pOrbiter_Plugin )
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to qMediaPlayer_Plugin plugin");
        return false;
    }

    vector<int> vectPK_DeviceTemplate;
    vectPK_DeviceTemplate.push_back(DEVICETEMPLATE_qMediaPlayer_CONST);
    m_pMedia_Plugin->RegisterMediaPlugin( this, this, vectPK_DeviceTemplate, true );

    RegisterMsgInterceptor(( MessageInterceptorFn )( &qMediaPlayer_Plugin::MenuOnScreen ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Menu_Onscreen_CONST );

	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
qMediaPlayer_Plugin_Command *Create_qMediaPlayer_Plugin(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new qMediaPlayer_Plugin(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void qMediaPlayer_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void qMediaPlayer_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
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

void qMediaPlayer_Plugin::SomeFunction()
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


MediaStream *qMediaPlayer_Plugin::CreateMediaStream(MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID)
{
    QMediaStream *pQtMediaStream;
    MediaDevice *pMediaDevice_PassedIn;

    PLUTO_SAFETY_LOCK(qm, m_QMediaMediaMutex );

    if (m_bQuit_get())
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "qMediaPlayer_Plugin::CreateMediaStream with m_bQuit");
        return NULL;
    }

    PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );
    pMediaDevice_PassedIn = NULL;
    if ( vectEntertainArea.size()==0 && pMediaDevice == NULL )
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "I can't create a media stream without an entertainment area or a media device");
        return NULL;
    }

    if ( pMediaDevice != NULL  && // test the media device only if it set
         pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate != DEVICETEMPLATE_qMediaPlayer_CONST &&
         pMediaDevice->m_pDeviceData_Router->m_dwPK_DeviceTemplate != DEVICETEMPLATE_SqueezeBox_Player_CONST )
    {
        pMediaDevice_PassedIn = pMediaDevice;
        pMediaDevice = m_pMedia_Plugin->m_mapMediaDevice_Find(m_pRouter->FindClosestRelative(DEVICETEMPLATE_qMediaPlayer_CONST, pMediaDevice->m_pDeviceData_Router->m_dwPK_Device));
    }

    if ( !pMediaDevice )
    {
        for(size_t s=0;s<vectEntertainArea.size();++s)
        {
            EntertainArea *pEntertainArea = vectEntertainArea[0];
            pMediaDevice = FindMediaDeviceForEntertainArea(pEntertainArea);
            if( pMediaDevice )
                break;
        }
        if( !pMediaDevice )
        {
            LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "I didn't find a device in the target ent area.");
            return NULL;
        }
    }

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Selected device (%d: %s) as playback device!",
            pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
            pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());

    pQtMediaStream = new QMediaStream( this, pMediaHandlerInfo,iPK_MediaProvider,
                            pMediaDevice,
                            iPK_Users, st_RemovableMedia, StreamID );

    // if the source device is a disk drive then we can't move this media stream around.
    if ( pMediaDevice_PassedIn && pMediaDevice_PassedIn->m_pDeviceData_Router->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Disk_Drive_CONST )
        pQtMediaStream->setIsMovable(false);

    return pQtMediaStream;

}

bool qMediaPlayer_Plugin::StartMedia(MediaStream *pMediaStream, string &sError)
{

    PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

    LoggerWrapper::GetInstance()->Write( LV_STATUS, "qMediaPlayer_Plugin::StartMedia() Starting media stream playback. pos: %d", pMediaStream->m_iDequeMediaFile_Pos );

    QMediaStream *pQtMediaStream = NULL;
    if ( (pQtMediaStream = ConvertToQtMediaStream(pMediaStream, "qMediaPlayer_Plugin::StartMedia(): ")) == NULL )
        return false;

    string sFileToPlay;

    MediaFile *pMediaFile = NULL;
    // HACK: -- todo: get real informations.
    if( pQtMediaStream->m_dequeMediaFile.size()>pQtMediaStream->m_iDequeMediaFile_Pos )
        pMediaFile = pQtMediaStream->m_dequeMediaFile[pQtMediaStream->m_iDequeMediaFile_Pos];

    if( pMediaFile && (pMediaFile->m_dwPK_Device_Disk_Drive || pMediaFile->m_dwPK_Disk)  )
    {
        sFileToPlay = m_pMedia_Plugin->GetMRLFromDiscID(pMediaFile->m_dwPK_Disk,pMediaFile->m_dwPK_Device_Disk_Drive);

        //don't reset media stream's PK_Disc
        if(pMediaFile->m_dwPK_Disk)
            pQtMediaStream->m_dwPK_Disc = pMediaFile->m_dwPK_Disk;
    }
    else
        sFileToPlay = pQtMediaStream->GetFilenameToPlay("Empty file name");

    LoggerWrapper::GetInstance()->Write( LV_STATUS, "qMediaPlayer_Plugin::StartMedia() Media type %d %s", pMediaStream->m_iPK_MediaType, sFileToPlay.c_str());

    string mediaURL;
    string Response;

    mediaURL = sFileToPlay;

    // If there's a \t(xxx)\t embedded in the file name, than this is a disc and xxx is the drive
    string::size_type size = mediaURL.size();
    string::size_type posDrive;
    if( size>3 && mediaURL[size-1]=='\t' && mediaURL[size-2]==')' && (posDrive=mediaURL.find("\t("))!=string::npos )
    {
        int PK_Device_Drive = atoi( mediaURL.substr( posDrive+2 ).c_str() );
        DeviceData_Router *pDevice_Drive = m_pMedia_Plugin->m_pRouter->m_mapDeviceData_Router_Find(PK_Device_Drive);
        mediaURL = mediaURL.substr(0,posDrive);  // It didn't work with this and the above as 1 command

        if( pDevice_Drive && pDevice_Drive->GetTopMostDevice() != pQtMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->GetTopMostDevice() )
        {
            // We don't have the correct source qMediaPlayer_Plugin.  Find one that's on the same pc as the disk drive, and it will be the source
            DeviceData_Router *pDevice_qMediaPlayer_Plugin = (DeviceData_Router *) pDevice_Drive->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_qMediaPlayer_Plugin_CONST);
            MediaDevice *pDevice_Source = NULL;
            if( pDevice_qMediaPlayer_Plugin==NULL || (pDevice_Source=m_pMedia_Plugin->m_mapMediaDevice_Find(pDevice_qMediaPlayer_Plugin->m_dwPK_Device))==NULL )
                // This probably isn't want they want, but go ahead and let it play the local disc anyway
                LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"qMediaPlayer_Plugin::StartMedia -- need to play a disc on a pc with no qMediaPlayer_Plugin!");
            else
                pQtMediaStream->m_pMediaDevice_Source = pDevice_Source;
        }
    }

    if ( pQtMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_DVD_CONST || FileUtils::FindExtension(mediaURL)=="dvd" || FileUtils::FindExtension(mediaURL)=="iso" ) {
            mediaURL = "dvd://" + mediaURL;

        // If the source is one ea and the destination in another, it could be remotely playing a disc, so
        // let ConfirmSourceIsADestination process it
        // and see if it can still make it non-streaming by using network block
        // If it changes the source to use an nbd device, then the subsequent StreamingRequired will return true
        // Only for DVD, Audio CDs cannot use network block(nbd), so we must always stream
        if( pQtMediaStream->StreamingRequired() )
        {
                // For now we're not able to have a qMediaPlayer_Plugin that renders to a NULL window and can do dvd's.  They require
                // a live window with events.  So for the moment this function will confirm that if we're playing a dvd disc remotely that we make the
                // source be one of the destinations, and change the mrl to reference the source disk
                if( !ConfirmSourceIsADestination(mediaURL,pQtMediaStream,pMediaFile ? pMediaFile->m_dwPK_Device_Disk_Drive : 0) )
                    LoggerWrapper::GetInstance()->Write(LV_WARNING,"qMediaPlayer_Plugin::StartMedia don't know how media will get to destination.  Unless there's some output zones in the mix results won't be right");
            }
    }
    else if( mediaURL.size()>5 && mediaURL.substr(0,5)=="/dev/" && pQtMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_CD_CONST )
    {
        mediaURL = "cdda://" + mediaURL;
        if( pMediaFile && pMediaFile->m_iTrack )
            mediaURL += "/" + StringUtils::itos(pMediaFile->m_iTrack);
    }

#ifdef WIN32
    mediaURL = StringUtils::Replace(mediaURL, "\\", "/"); // replacing all the \ in a windows path with /
#endif

    // hack: redirect MOV, EVO, M2TS files to MPlayer_Player
    // TODO rework Media Dispatcher to make it more clear - later
    LoggerWrapper::GetInstance()->Write(LV_WARNING, "Doing MPlayer redirection check...");
    bool bRedirectToMPlayer = ( pQtMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_HDDVD_CONST ) ||
            ( pQtMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_BD_CONST ) ||
            ( (pQtMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_StoredVideo_CONST) &&
            ( StringUtils::EndsWith(mediaURL, ".EVO", true) || StringUtils::EndsWith(mediaURL, ".M2TS", true) )
            );

    // hack: VDR recordings are detected by the info.vdr or info in the recording dir. T
    bool bVDRRecording = ( StringUtils::EndsWith(mediaURL,"INFO.VDR", true) || StringUtils::EndsWith(mediaURL,"INFO",true) );

    // hack: redirect MythTV recordings to MythTV_Player
    LoggerWrapper::GetInstance()->Write(LV_WARNING, "Doing MythTV redirection check...");

//	MediaDevice *pMediaDevice_CheckMythTV = m_pMedia_Plugin->m_mapMediaDevice_Find(m_pRouter->FindClosestRelative(DEVICETEMPLATE_MythTV_Player_CONST, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device));
//
//	bool bMythTVExists = false;
//
//	if (!pMediaDevice_CheckMythTV)
//	{
//		bMythTVExists = false;
//	} else
//	{
//		bMythTVExists = true;
//	}
//
    bool bRedirectToMythTV = false;

    if( !bRedirectToMythTV && pQtMediaStream->StreamingRequired() )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "sending CMD_Play_Media from %d to %d with deq pos %d",
            m_dwPK_Device, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
            pMediaStream->m_iDequeMediaFile_Pos);
        DCE::CMD_Start_Streaming cmd(m_dwPK_Device,
                                pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
                                pQtMediaStream->m_iPK_MediaType,
                                pQtMediaStream->m_iStreamID_get( ),
                                pMediaFile && pMediaFile->m_sStartPosition.size() ? pMediaFile->m_sStartPosition : pQtMediaStream->m_sStartPosition,
                                mediaURL,
                                pQtMediaStream->GetTargets(DEVICETEMPLATE_qMediaPlayer_Plugin_CONST));
        // No handling of errors (it will in some cases deadlock the router.)
        SendCommand(cmd);
    }
    else if (bRedirectToMythTV)
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING,"Redirecting to MythTV.");
        MediaDevice *pMediaDevice_MythTV = NULL;

        if (bRedirectToMythTV)
        {
            pMediaDevice_MythTV = m_pMedia_Plugin->m_mapMediaDevice_Find(m_pRouter->FindClosestRelative(DEVICETEMPLATE_MythTV_Player_CONST, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device));
            if (!pMediaDevice_MythTV)
            {
                LoggerWrapper::GetInstance()->Write(LV_WARNING, "Failed to find MythTV for redirect, cancelling redirect :(");
                bRedirectToMythTV = false;
            }
            else
            {
                LoggerWrapper::GetInstance()->Write(LV_WARNING, "Found MythTV for redirect, device #%i", pMediaDevice_MythTV->m_pDeviceData_Router->m_dwPK_Device);
                pMediaStream->m_pMediaDevice_Source = pMediaDevice_MythTV;
            }
        }

        // changing window name
        if (bRedirectToMythTV)
        {
            pMediaStream->m_sAppName = "mythfrontend.real.Mythfrontend.real";
            pMediaStream->m_bContainsTitlesOrSections = false;

        }

        LoggerWrapper::GetInstance()->Write(LV_WARNING, "sending CMD_Play_Media from %d to %d with deq pos %d",
            m_dwPK_Device, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
            pMediaStream->m_iDequeMediaFile_Pos);
        DCE::CMD_Play_Media cmd(m_dwPK_Device,
                                pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
                                pQtMediaStream->m_iPK_MediaType,
                                pQtMediaStream->m_iStreamID_get( ),
                                pMediaFile && pMediaFile->m_sStartPosition.size() ? pMediaFile->m_sStartPosition : pQtMediaStream->m_sStartPosition,
                                mediaURL);

        // No handling of errors (it will in some cases deadlock the router.)
        SendCommand(cmd);
    } else if( !bRedirectToMPlayer && pQtMediaStream->StreamingRequired() )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "sending CMD_Play_Media from %d to %d with deq pos %d",
            m_dwPK_Device, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
            pMediaStream->m_iDequeMediaFile_Pos);
        DCE::CMD_Start_Streaming cmd(m_dwPK_Device,
                                pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
                                pQtMediaStream->m_iPK_MediaType,
                                pQtMediaStream->m_iStreamID_get( ),
                                pMediaFile && pMediaFile->m_sStartPosition.size() ? pMediaFile->m_sStartPosition : pQtMediaStream->m_sStartPosition,
                                mediaURL,
                                pQtMediaStream->GetTargets(DEVICETEMPLATE_qMediaPlayer_Plugin_CONST));

        // No handling of errors (it will in some cases deadlock the router.)
        SendCommand(cmd);
    }
    else
    {
        MediaDevice *pMediaDevice_MPlayer = NULL;

        if (bRedirectToMPlayer)
        {
            LoggerWrapper::GetInstance()->Write(LV_WARNING, "Redirecting CMD_Play_Media to MPlayer instead of qMediaPlayer_Plugin: media type is %i", pQtMediaStream->m_iPK_MediaType);
            pMediaDevice_MPlayer = m_pMedia_Plugin->m_mapMediaDevice_Find(m_pRouter->FindClosestRelative(DEVICETEMPLATE_MPlayer_Player_CONST, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device));
            if (!pMediaDevice_MPlayer)
            {
                LoggerWrapper::GetInstance()->Write(LV_WARNING, "Failed to find MPlayer for redirect, cancelling redirect :(");
                bRedirectToMPlayer = false;
            }
            else
            {
                LoggerWrapper::GetInstance()->Write(LV_WARNING, "Found MPlayer for redirect, device #%i", pMediaDevice_MPlayer->m_pDeviceData_Router->m_dwPK_Device);
                pMediaStream->m_pMediaDevice_Source = pMediaDevice_MPlayer;
            }
        }

        // changing window name
        if (bRedirectToMPlayer)
        {
            pMediaStream->m_sAppName = "xv.MPlayer";
            pMediaStream->m_bContainsTitlesOrSections = false;

            // if this is not a file, we should populate all files from disk folder
            bool bAppendFiles = ! ( StringUtils::EndsWith(mediaURL, ".EVO", true) || StringUtils::EndsWith(mediaURL, ".M2TS", true) );

            if (bAppendFiles)
            {
                // need this for bookmarking to work
                int iPK_File = pMediaFile->m_dwPK_File;

                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Appending extra items to list: ");
                string sFolder = sFileToPlay;
                string sExt = "";

                if ( pQtMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_HDDVD_CONST )
                {
                    sFolder += "/HVDVD_TS";
                    sExt = "*.evo";
                }
                else if ( pQtMediaStream->m_iPK_MediaType == MEDIATYPE_pluto_BD_CONST )
                {
                    sFolder += "/BDMV/STREAM";
                    sExt = "*.m2ts";
                }

                list<string> vItems;
                FileUtils::FindFiles(vItems, sFolder, sExt);
                vItems.sort();

                pMediaStream->m_dequeMediaFile.clear();

                string sStartPosition = pMediaFile && pMediaFile->m_sStartPosition.size() ? pMediaFile->m_sStartPosition : pQtMediaStream->m_sStartPosition;
                string sStartFile;
                string::size_type sPos = sStartPosition.find("FILE:");
                if (sPos != string::npos)
                {
                    sStartFile = sStartPosition.substr(sPos+5);
                }
                else
                {
                    long int iMaxSize = -1;
                    for (list<string>::iterator li=vItems.begin(); li!=vItems.end(); ++li)
                    {
                        string sFullName = sFolder+"/"+ *li;
                        long int iFileSize = FileUtils::FileSize(sFullName);
                        if ( iFileSize > iMaxSize )
                        {
                            iMaxSize = iFileSize;
                            sStartFile = *li;
                        }
                    }

                }

                int iQueuePos = 0;

                for (list<string>::iterator li=vItems.begin(); li!=vItems.end(); ++li)
                {
                    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Item: %s [ripped file %i]", li->c_str(), iPK_File);
                    MediaFile *pItem = new MediaFile(sFolder+"/"+*li);
                    pItem->m_dwPK_File = iPK_File;
                    long long iFileSize = FileUtils::FileSize64(sFolder+"/"+*li) / 1024 / 1024;
                    pItem->m_sDescription = *li + ", " +StringUtils::itos(iFileSize) + "Mb";
                    pMediaStream->m_dequeMediaFile.push_back(pItem);
                    if (sStartFile == *li)
                        pMediaStream->m_iDequeMediaFile_Pos = iQueuePos;
                    iQueuePos++;
                }

            }
            else
            {
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Not appending extra items to list");
            }
        }

        if ( bVDRRecording )
        {
            // need this for bookmarking to work
            int iPK_File = pMediaFile->m_dwPK_File;

            LoggerWrapper::GetInstance()->Write(LV_STATUS, "Appending extra items to list: ");
            string sFolder = sFileToPlay;
            string sExt = "";

            if ( StringUtils::EndsWith(mediaURL,"INFO.VDR",true) ) {
                            sFolder = StringUtils::RemoveStringFromEnd(mediaURL,9);
                sFileToPlay = sFolder;
                        sExt = "0*.vdr";
                        mediaURL = sFolder + "/001.vdr";
            }
                        if ( StringUtils::EndsWith(mediaURL,"INFO",true) ) {
                            sFolder = StringUtils::RemoveStringFromEnd(mediaURL,5);
                            sFileToPlay = sFolder;
                                sExt = "0*.ts";
                        mediaURL = sFolder + "/00001.ts";
            }

            list<string> vItems;
            FileUtils::FindFiles(vItems, sFolder, sExt);
            vItems.sort();

            pMediaStream->m_dequeMediaFile.clear();

            string sStartPosition = pMediaFile && pMediaFile->m_sStartPosition.size() ? pMediaFile->m_sStartPosition : pQtMediaStream->m_sStartPosition;
            string sStartFile;
            string::size_type sPos = sStartPosition.find("FILE:");
            if (sPos != string::npos)
            {
                sStartFile = sStartPosition.substr(sPos+5);
            }
            else
            {
                long int iMaxSize = -1;
                for (list<string>::iterator li=vItems.begin(); li!=vItems.end(); ++li)
                {
                    string sFullName = sFolder+"/"+ *li;
                    long int iFileSize = FileUtils::FileSize(sFullName);
                    if ( iFileSize > iMaxSize )
                    {
                        iMaxSize = iFileSize;
                        sStartFile = *li;
                    }
                }

            }

            int iQueuePos = 0;

            for (list<string>::iterator li=vItems.begin(); li!=vItems.end(); ++li)
            {
                LoggerWrapper::GetInstance()->Write(LV_WARNING, "Item: %s [ripped file %i]", li->c_str(), iPK_File);
                MediaFile *pItem = new MediaFile(sFolder+"/"+*li);
                pItem->m_dwPK_File = iPK_File;
                long long iFileSize = FileUtils::FileSize64(sFolder+"/"+*li) / 1024 / 1024;
                pItem->m_sDescription = *li + ", " +StringUtils::itos(iFileSize) + "Mb";
                pMediaStream->m_dequeMediaFile.push_back(pItem);
                if (sStartFile == *li)
                    pMediaStream->m_iDequeMediaFile_Pos = iQueuePos;
                iQueuePos++;
            }
        }

        LoggerWrapper::GetInstance()->Write(LV_WARNING, "sending CMD_Play_Media from %d to %d with deq pos %d",
            m_dwPK_Device, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
            pMediaStream->m_iDequeMediaFile_Pos);
        DCE::CMD_Play_Media cmd(m_dwPK_Device,
                                pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device,
                                pQtMediaStream->m_iPK_MediaType,
                                pQtMediaStream->m_iStreamID_get( ),
                                pMediaFile && pMediaFile->m_sStartPosition.size() ? pMediaFile->m_sStartPosition : pQtMediaStream->m_sStartPosition,
                                mediaURL);

        // No handling of errors (it will in some cases deadlock the router.)
        SendCommand(cmd);
    }

        // if the disk ID not known, trying to see if we have a file with this name
        int iPK_File=0;
        if (pMediaFile)
          iPK_File = pMediaFile->m_dwPK_File;
        // if file or disk ID is already known, sending it
        if ( iPK_File || pMediaStream->m_dwPK_Disc )
        {
          string sMediaID = (iPK_File?"F":"D");
          sMediaID += StringUtils::itos(iPK_File?iPK_File:pMediaStream->m_dwPK_Disc);
          DCE::CMD_Set_Media_ID cmd(m_dwPK_Device, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device, sMediaID, pQtMediaStream->m_iStreamID_get());
          SendCommand(cmd);
          LoggerWrapper::GetInstance()->Write(LV_WARNING, "set media id command sent with id %s", sMediaID.c_str());
        }
        else
        {
          LoggerWrapper::GetInstance()->Write(LV_WARNING, "set media id command not sent");
        }

    if( pMediaFile && pQtMediaStream->m_iPK_Playlist==0 )  // If this is part of a playlist, rather than just a normal bookmark, the user will likely want it to keep resuming at the set position
        pMediaFile->m_sStartPosition=""; // Be sure to reset the start position so next time we start at the beginning of the file if this is in a queue

    LoggerWrapper::GetInstance()->Write(LV_WARNING, "play media command sent from %d to %d!", m_dwPK_Device, pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);

    return MediaHandlerBase::StartMedia(pMediaStream,sError);

}

bool qMediaPlayer_Plugin::StopMedia(MediaStream *pMediaStream)
{
    PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Stopping media in QMediaPlayer_Plugin!");

    QMediaStream *pQMediaStream = NULL;

    if ( (pQMediaStream = ConvertToQtMediaStream(pMediaStream, "qMediaPlayer_Plugin::StopMedia() ")) == NULL )
        return false;

    if( !pQMediaStream->m_pMediaDevice_Source )
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Stopping media in qMediaPlayer_Plugin but mediadevice_source is null");
        return false;
    }
    int PK_Device = pQMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device;
    int StreamID = pQMediaStream->m_iStreamID_get( );
    string SavedPosition;

    DCE::CMD_Stop_Media cmd(m_dwPK_Device,                          // Send from us
                            PK_Device,  		// Send to the device that is actually playing
                            StreamID,      		// Send the stream ID that we want to actually stop
                            &SavedPosition);

    // TODO: Remove the device from the list of players also.
    string Response;
    if( !SendCommand( cmd ) ) // hack - todo see above, &Response ) )
    {
        // TODO: handle failure when sending the command. This is ignored now.
        LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "The target device %d didn't respond to stop media command!", PK_Device );
    }
    else
    {
        pQMediaStream->m_sLastPosition=SavedPosition;
        if( pQMediaStream->m_iDequeMediaFile_Pos>=0 && pQMediaStream->m_iDequeMediaFile_Pos<pQMediaStream->m_dequeMediaFile.size() )
        {
            pQMediaStream->m_dequeMediaFile[pQMediaStream->m_iDequeMediaFile_Pos]->m_sStartPosition = SavedPosition;
            LoggerWrapper::GetInstance()->Write( LV_STATUS, "Media stopped at %s",SavedPosition.c_str());
        }

        LoggerWrapper::GetInstance()->Write( LV_STATUS, "The target device %d responded to stop media command",
                                            pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device);
    }

    return MediaHandlerBase::StopMedia(pMediaStream);
}

MediaDevice *qMediaPlayer_Plugin::FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea)
{
    PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

    MediaDevice *pMediaDevice;
    pMediaDevice = GetMediaDeviceForEntertainArea(pEntertainArea, DEVICETEMPLATE_qMediaPlayer_CONST);

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Looking for a proper device in the ent area %d (%s)", pEntertainArea->m_iPK_EntertainArea, pEntertainArea->m_sDescription.c_str());
    if ( pMediaDevice == NULL )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Could not find a QMedia Player device (with device template id: %d) in the entertainment area: %d. Looking for a squeeze box.",
                DEVICETEMPLATE_qMediaPlayer_CONST,
                pEntertainArea->m_iPK_EntertainArea);

        return NULL;
    }

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Returning this device %d (%s)", pMediaDevice->m_pDeviceData_Router->m_dwPK_Device, pMediaDevice->m_pDeviceData_Router->m_sDescription.c_str());

    return pMediaDevice;
}

bool qMediaPlayer_Plugin::MenuOnScreen(Socket *pSocket, Message *pMessage, DeviceData_Base *pDeviceFrom, DeviceData_Base *pDeviceTo)
{
//    PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

//    /** Confirm this is from one of ours */
//    if( !pDeviceFrom || pDeviceFrom->m_dwPK_DeviceTemplate != DEVICETEMPLATE_Xine_Player_CONST )
//        return false; /** Some other media player. We only know xine's menu handling */

//    int StreamID = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Stream_ID_CONST].c_str( ) );
//    bool bOnOff = pMessage->m_mapParameters[EVENTPARAMETER_OnOff_CONST]=="1";

//    /** Find the stream */
//    XineMediaStream *pXineMediaStream = NULL;
//    MediaStream *pMediaStream = m_pMedia_Plugin->m_mapMediaStream_Find( StreamID,pMessage->m_dwPK_Device_From );
//    if( !pMediaStream || (pXineMediaStream = ConvertToXineMediaStream(pMediaStream, "Xine_Plugin::MenuOnScreen(): ")) == NULL )
//        return false;

//    pXineMediaStream->m_bUseAltScreens=bOnOff;

//    LoggerWrapper::GetInstance()->Write( LV_STATUS, "MediaStream %p with id %d and type %d reached an OnScreen Menu.", pXineMediaStream, pXineMediaStream->m_iStreamID_get( ), pXineMediaStream->m_iPK_MediaType );
//    LoggerWrapper::GetInstance()->Write( LV_STATUS, "MediaStream m_mapEntertainArea.size( ) %d", pXineMediaStream->m_mapEntertainArea.size( ) );


//    /** We're going to send a message to all the orbiters in this area so they know what the remote is,
//    and we will send all bound remotes to the new screen */
//    for( MapEntertainArea::iterator itEA = pXineMediaStream->m_mapEntertainArea.begin( );itEA != pXineMediaStream->m_mapEntertainArea.end( );++itEA )
//    {
//        EntertainArea *pEntertainArea = ( *itEA ).second;
//        LoggerWrapper::GetInstance()->Write( LV_STATUS, "Looking into the ent area (%p) with id %d and %d remotes", pEntertainArea, pEntertainArea->m_iPK_EntertainArea, (int) pEntertainArea->m_mapBoundRemote.size() );
//        for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
//        {
//            OH_Orbiter *pOH_Orbiter = (*it).second;
//            if( pOH_Orbiter->m_pEntertainArea!=pEntertainArea )
//                continue;
//            LoggerWrapper::GetInstance()->Write(LV_STATUS, "Processing remote: for orbiter: %d", pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
//            bool bBound = pEntertainArea->m_mapBoundRemote.find(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device)!=pEntertainArea->m_mapBoundRemote.end();
//            pXineMediaStream->SetNowPlaying(pOH_Orbiter,false,bBound);
//        }
//    }

    return false;
}

QMediaStream *qMediaPlayer_Plugin::ConvertToQtMediaStream(MediaStream *pMediaStream, std::string callerIdMessage)
{
    PLUTO_SAFETY_LOCK( mm, m_pMedia_Plugin->m_MediaMutex );

    if ( pMediaStream == NULL )
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "Stream is a NULL stream!").c_str());
        return NULL;
    }

    if ( pMediaStream->GetType() != MEDIASTREAM_TYPE_GENERIC )
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, (callerIdMessage + "Stream is not a QMediaStream!").c_str());
        return NULL;
    }

    return static_cast<QMediaStream*>(pMediaStream);
}

bool qMediaPlayer_Plugin::ConfirmSourceIsADestination(string &sMRL, QMediaStream *pQMediaStream, int PK_Device_Drive)
{
    MediaDevice *pQMediaPlayer = NULL; // Any qMediaplayer that's in the destination
    for( MapEntertainArea::iterator itEA = pQMediaStream->m_mapEntertainArea.begin( );itEA != pQMediaStream->m_mapEntertainArea.end( );++itEA )
    {
        EntertainArea *pEntertainArea = ( *itEA ).second;
        ListMediaDevice *pListMediaDevice = pEntertainArea->m_mapMediaDeviceByTemplate_Find(DEVICETEMPLATE_qMediaPlayer_CONST);
        if( !pListMediaDevice )
            continue;
        for(ListMediaDevice::iterator it=pListMediaDevice->begin();it!=pListMediaDevice->end();++it)
        {
            MediaDevice *pMediaDevice = *it;
            if( pQMediaStream->m_pMediaDevice_Source->IsInEntertainArea( pEntertainArea ) )
                return true; // We're good.  The source is one of the destinations
            if( !pQMediaPlayer )
                pQMediaPlayer = pMediaDevice;
        }
    return false;
    }

}


