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
#ifndef qMediaPlayer_h
#define qMediaPlayer_h

//	DCE Implemenation for #2205 qMediaPlayer

#include "Gen_Devices/qMediaPlayerBase.h"
//<-dceag-d-e->
#include <QTime>
#include <QString>
class QTimer;

class MediaManagerBase;

//<-dceag-decl-b->
namespace DCE
{
    class qMediaPlayer : public qMediaPlayer_Command
    {
//<-dceag-decl-e->
        Q_OBJECT
        Q_PROPERTY(QString commandResponse READ getCommandResponse WRITE setCommandResponse NOTIFY commandResponseChanged)
        Q_PROPERTY(QString mediaResponse READ getMediaResponse WRITE setMediaResponse NOTIFY mediaResponseChanged)
        Q_PROPERTY(QString currentMediaUrl READ getCurrentMediaUrl WRITE setCurrentMediaUrl NOTIFY currentMediaUrlChanged)
        Q_PROPERTY(QString startPosition READ getStartPosition WRITE setStartPosition NOTIFY startPositionChanged)
        Q_PROPERTY(int i_StreamId READ getStreamID WRITE setStreamID NOTIFY streamIdChanged)
        Q_PROPERTY(int i_pkMediaType READ getMediaType WRITE setMediaType NOTIFY mediaTypeChanged)
        // Private member variables

        // Private methods
public:
        // Public member variables
        QString commandResponse;
        QString mediaResponse;
        QString currentMediaUrl;

        int i_StreamId;
        int i_pkMediaType;
        QString mediaPosition;
        QString startPosition;
        bool m_bPaused;
        QString mediaID;
        bool connected;
//<-dceag-const-b->
public:
        // Constructors/Destructor
        qMediaPlayer(int DeviceID, string ServerAddress, MediaManagerBase *manager,bool bConnectEventHandler=true,bool bLocalMode=false, class Router *pRouter=NULL);
        virtual ~qMediaPlayer();
        virtual bool GetConfig();
        virtual bool Register();
        virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
        virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
        void restart();

//<-dceag-const-e->
        virtual void OnDisconnect();
        virtual void onReload();
        virtual void OnUnexpectedDisconnect();
//<-dceag-const2-b->
        // The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
        // You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
        qMediaPlayer(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
    /*
                AUTO-GENERATED SECTION
                Do not change the declarations
    */

    /*
            *****DATA***** accessors inherited from base class
    bool DATA_Get_No_Effects();
    void DATA_Set_No_Effects(bool Value,bool bUpdateDatabase=false);
    string DATA_Get_TCP_Address();
    void DATA_Set_TCP_Address(string Value,bool bUpdateDatabase=false);
    string DATA_Get_Mount_Point();
    void DATA_Set_Mount_Point(string Value,bool bUpdateDatabase=false);
    int DATA_Get_Port();
    void DATA_Set_Port(int Value,bool bUpdateDatabase=false);

			*****EVENT***** accessors inherited from base class
	void EVENT_Playback_Info_Changed(string sMediaDescription,string sSectionDescription,string sSynposisDescription);
	void EVENT_Menu_Onscreen(int iStream_ID,bool bOnOff);
	void EVENT_Playback_Completed(string sMRL,int iStream_ID,bool bWith_Errors);
	void EVENT_Media_Description_Changed(string sText);
	void EVENT_Playback_Started(string sMRL,int iStream_ID,string sSectionDescription,string sAudio,string sVideo);
	void EVENT_Media_Position_Changed(int iFK_MediaType,string sMRL,string sID,int iStream_ID,string sDateTime,string sCurrent_Time,int iSpeed);

			*****COMMANDS***** we need to implement
	*/


    /** @brief COMMAND: #28 - Simulate Keypress */
    /** Send a key to the device's OSD, or simulate keypresses on the device's panel */
        /** @param #26 PK_Button */
            /** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
        /** @param #41 StreamID */
            /** ID of stream to apply */
        /** @param #50 Name */
            /** The application to send the keypress to. If not specified, it goes to the DCE device. */

    virtual void CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName) { string sCMD_Result; CMD_Simulate_Keypress(sPK_Button.c_str(),iStreamID,sName.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #29 - Simulate Mouse Click */
    /** Simlate a mouse click at a certain position on the screen */
        /** @param #11 Position X */
            /** position X */
        /** @param #12 Position Y */
            /** position Y */
        /** @param #41 StreamID */
            /** ID of stream to apply */

    virtual void CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,int iStreamID) { string sCMD_Result; CMD_Simulate_Mouse_Click(iPosition_X,iPosition_Y,iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,int iStreamID,string &sCMD_Result,Message *pMessage);


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

    virtual void CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock) { string sCMD_Result; CMD_Update_Object_Image(sPK_DesignObj.c_str(),sType.c_str(),pData,iData_Size,sDisable_Aspect_Lock.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Update_Object_Image(string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock,string &sCMD_Result,Message *pMessage);


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

    virtual void CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL) { string sCMD_Result; CMD_Play_Media(iPK_MediaType,iStreamID,sMediaPosition.c_str(),sMediaURL.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Play_Media(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #38 - Stop Media */
    /** This will instruct the media player to stop the playback of a media started with the "Play Media" Command */
        /** @param #41 StreamID */
            /** The media needing to be stopped. */
        /** @param #42 MediaPosition */
            /** The position at which this stream was last played. */

    virtual void CMD_Stop_Media(int iStreamID,string *sMediaPosition) { string sCMD_Result; CMD_Stop_Media(iStreamID,sMediaPosition,sCMD_Result,NULL);};
    virtual void CMD_Stop_Media(int iStreamID,string *sMediaPosition,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #39 - Pause Media */
    /** This will stop a media that is currently played. This method should be paired with the "Restart Media" and used when the playback will be stopped and restarted on the same display device. */
        /** @param #41 StreamID */
            /** The media stream for which we need to pause playback. */

    virtual void CMD_Pause_Media(int iStreamID) { string sCMD_Result; CMD_Pause_Media(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Pause_Media(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #40 - Restart Media */
    /** This will restart a media was paused with the above command */
        /** @param #41 StreamID */
            /** The media stream that we need to restart playback for. */

    virtual void CMD_Restart_Media(int iStreamID) { string sCMD_Result; CMD_Restart_Media(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Restart_Media(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #41 - Change Playback Speed */
    /** Will make the playback to FF with a configurable amount of speed. */
        /** @param #41 StreamID */
            /** The media needing the playback speed change. */
        /** @param #43 MediaPlaybackSpeed */
            /** The requested media playback speed * 1000.  -1000 = rev, 4000 = 4x fwd, -500 = rev 1/2.  Less than 10 = relative.  +2 = double, -1 = reverse.   See Media_Plugin::ReceivedMessage */
        /** @param #220 Report */
            /** If true, report this speed to the user on the OSD */

    virtual void CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport) { string sCMD_Result; CMD_Change_Playback_Speed(iStreamID,iMediaPlaybackSpeed,bReport,sCMD_Result,NULL);};
    virtual void CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,bool bReport,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #42 - Jump to Position in Stream */
    /** Jump to a position in the stream, specified in seconds. */
        /** @param #5 Value To Assign */
            /** The number of seconds.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1.  A simpler command than Set Media Position */
        /** @param #41 StreamID */
            /** The stream */

    virtual void CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID) { string sCMD_Result; CMD_Jump_to_Position_in_Stream(sValue_To_Assign.c_str(),iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Jump_to_Position_in_Stream(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
    /** Raise  the channel, track, station, etc. by 1.  Same as Jump to Pos in Playlist with value +1 for a smart media player */
        /** @param #41 StreamID */
            /** ID of stream to apply */

    virtual void CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID) { string sCMD_Result; CMD_Skip_Fwd_ChannelTrack_Greater(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
    /** Lower the channel, track, station, etc. by 1.  Same as Jump to Pos in Playlist with value -1 for a smart media player */
        /** @param #41 StreamID */
            /** ID of stream to apply */

    virtual void CMD_Skip_Back_ChannelTrack_Lower(int iStreamID) { string sCMD_Result; CMD_Skip_Back_ChannelTrack_Lower(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Skip_Back_ChannelTrack_Lower(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #65 - Jump Position In Playlist */
    /** Jump to a specific position in the playlist, or a track, or a chapter.  Smart media players should also understand the skip fwd/skip back (which non-DCE media players use) to be the same thing as a jump +1 or -1 */
        /** @param #5 Value To Assign */
            /** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */
        /** @param #41 StreamID */
            /** ID of stream to apply */

    virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID) { string sCMD_Result; CMD_Jump_Position_In_Playlist(sValue_To_Assign.c_str(),iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #81 - Navigate Next */
    /** Nagivate to the next possible navigable area. (The actual outcome depends on the specifc device) */
        /** @param #41 StreamID */
            /** The stream on which to do the navigation. */

    virtual void CMD_Navigate_Next(int iStreamID) { string sCMD_Result; CMD_Navigate_Next(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Navigate_Next(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #82 - Navigate Prev */
    /** Nagivate the previous possible navigable area. (The actual outcome depends on the specific device). */
        /** @param #41 StreamID */
            /** The stream on which to do the navigation. */

    virtual void CMD_Navigate_Prev(int iStreamID) { string sCMD_Result; CMD_Navigate_Prev(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Navigate_Prev(int iStreamID,string &sCMD_Result,Message *pMessage);


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

    virtual void CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat) { string sCMD_Result; CMD_Get_Video_Frame(sDisable_Aspect_Lock.c_str(),iStreamID,iWidth,iHeight,pData,iData_Size,sFormat,sCMD_Result,NULL);};
    virtual void CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage);


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

    virtual void CMD_Goto_Media_Menu(int iStreamID,int iMenuType) { string sCMD_Result; CMD_Goto_Media_Menu(iStreamID,iMenuType,sCMD_Result,NULL);};
    virtual void CMD_Goto_Media_Menu(int iStreamID,int iMenuType,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #89 - Vol Up */
    /** volume up */
        /** @param #72 Repeat Command */
            /** If specified, repeat the volume up this many times */

    virtual void CMD_Vol_Up(int iRepeat_Command) { string sCMD_Result; CMD_Vol_Up(iRepeat_Command,sCMD_Result,NULL);};
    virtual void CMD_Vol_Up(int iRepeat_Command,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #90 - Vol Down */
    /** volume down */
        /** @param #72 Repeat Command */
            /** If specified, repeat the volume down this many times. */

    virtual void CMD_Vol_Down(int iRepeat_Command) { string sCMD_Result; CMD_Vol_Down(iRepeat_Command,sCMD_Result,NULL);};
    virtual void CMD_Vol_Down(int iRepeat_Command,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #92 - Pause */
    /** Pause the media */
        /** @param #41 StreamID */
            /** ID of stream to apply */

    virtual void CMD_Pause(int iStreamID) { string sCMD_Result; CMD_Pause(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Pause(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #95 - Stop */
    /** Stop the media */
        /** @param #41 StreamID */
            /** ID of stream to apply */
        /** @param #203 Eject */
            /** If true, the drive will be ejected if there is no media currently playing, so a remote's stop button acts as stop/eject. */

    virtual void CMD_Stop(int iStreamID,bool bEject) { string sCMD_Result; CMD_Stop(iStreamID,bEject,sCMD_Result,NULL);};
    virtual void CMD_Stop(int iStreamID,bool bEject,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #97 - Mute */
    /** mute */

    virtual void CMD_Mute() { string sCMD_Result; CMD_Mute(sCMD_Result,NULL);};
    virtual void CMD_Mute(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #102 - Record */
    /** Record the current game. Toggle on off */

    virtual void CMD_Record() { string sCMD_Result; CMD_Record(sCMD_Result,NULL);};
    virtual void CMD_Record(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #123 - Info */
    /** VDR info */
        /** @param #9 Text */
            /** nimic */

    virtual void CMD_Info(string sText) { string sCMD_Result; CMD_Info(sText.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Info(string sText,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #126 - Guide */
    /** Show guide information.  For a dvd this may be the menu, just like the menu command */

    virtual void CMD_Guide() { string sCMD_Result; CMD_Guide(sCMD_Result,NULL);};
    virtual void CMD_Guide(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #139 - Play */
    /** Play the media */
        /** @param #41 StreamID */
            /** ID of stream to apply */

    virtual void CMD_Play(int iStreamID) { string sCMD_Result; CMD_Play(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Play(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #140 - Audio Track */
    /** Go to an audio track */
        /** @param #5 Value To Assign */
            /** The audio track to go to.  Simple A/V equipment ignores this and just toggles. */
        /** @param #41 StreamID */
            /** ID of stream to apply */

    virtual void CMD_Audio_Track(string sValue_To_Assign,int iStreamID) { string sCMD_Result; CMD_Audio_Track(sValue_To_Assign.c_str(),iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Audio_Track(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #141 - Subtitle */
    /** Go to a subtitle */
        /** @param #5 Value To Assign */
            /** The subtitle to go to.  Simple A/V equipment ignores this and just toggles. */
        /** @param #41 StreamID */
            /** ID of stream to apply */

    virtual void CMD_Subtitle(string sValue_To_Assign,int iStreamID) { string sCMD_Result; CMD_Subtitle(sValue_To_Assign.c_str(),iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Subtitle(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #142 - Angle */
    /** Go to an angle */
        /** @param #5 Value To Assign */
            /** The angle to go to.  Simple A/V equipment ignores this and just toggles. */
        /** @param #41 StreamID */
            /** ID of stream to apply */

    virtual void CMD_Angle(string sValue_To_Assign,int iStreamID) { string sCMD_Result; CMD_Angle(sValue_To_Assign.c_str(),iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Angle(string sValue_To_Assign,int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #184 - Set Level */
    /** Set discreet volume level */
        /** @param #76 Level */
            /** The level to set, as a value between 0 (off) and 100 (full).  It can be preceeded with a - or + indicating a relative value.  +20 means up 20%. */

    virtual void CMD_Set_Level(string sLevel) { string sCMD_Result; CMD_Set_Level(sLevel.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Set_Level(string sLevel,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #190 - Enter/Go */
    /** Select the currently highlighted menu item */
        /** @param #41 StreamID */
            /** ID of stream to apply */

    virtual void CMD_EnterGo(int iStreamID) { string sCMD_Result; CMD_EnterGo(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_EnterGo(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #200 - Move Up */
    /** Move the highlighter */
        /** @param #41 StreamID */
            /** ID of stream to apply */

    virtual void CMD_Move_Up(int iStreamID) { string sCMD_Result; CMD_Move_Up(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #201 - Move Down */
    /** Move the highlighter */
        /** @param #41 StreamID */
            /** ID of stream to apply */

    virtual void CMD_Move_Down(int iStreamID) { string sCMD_Result; CMD_Move_Down(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #202 - Move Left */
    /** Move the highlighter */
        /** @param #41 StreamID */
            /** ID of stream to apply */

    virtual void CMD_Move_Left(int iStreamID) { string sCMD_Result; CMD_Move_Left(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #203 - Move Right */
    /** Move the highlighter */
        /** @param #41 StreamID */
            /** ID of stream to apply */

    virtual void CMD_Move_Right(int iStreamID) { string sCMD_Result; CMD_Move_Right(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #204 - 0 */
    /** 0 */

    virtual void CMD_0() { string sCMD_Result; CMD_0(sCMD_Result,NULL);};
    virtual void CMD_0(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #205 - 1 */
    /** 1 */

    virtual void CMD_1() { string sCMD_Result; CMD_1(sCMD_Result,NULL);};
    virtual void CMD_1(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #206 - 2 */
    /** 2 */

    virtual void CMD_2() { string sCMD_Result; CMD_2(sCMD_Result,NULL);};
    virtual void CMD_2(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #207 - 3 */
    /** 3 */

    virtual void CMD_3() { string sCMD_Result; CMD_3(sCMD_Result,NULL);};
    virtual void CMD_3(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #208 - 4 */
    /** 4 */

    virtual void CMD_4() { string sCMD_Result; CMD_4(sCMD_Result,NULL);};
    virtual void CMD_4(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #209 - 5 */
    /** 5 */

    virtual void CMD_5() { string sCMD_Result; CMD_5(sCMD_Result,NULL);};
    virtual void CMD_5(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #210 - 6 */
    /** 6 */

    virtual void CMD_6() { string sCMD_Result; CMD_6(sCMD_Result,NULL);};
    virtual void CMD_6(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #211 - 7 */
    /** 7 */

    virtual void CMD_7() { string sCMD_Result; CMD_7(sCMD_Result,NULL);};
    virtual void CMD_7(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #212 - 8 */
    /** 8 */

    virtual void CMD_8() { string sCMD_Result; CMD_8(sCMD_Result,NULL);};
    virtual void CMD_8(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #213 - 9 */
    /** 9 */

    virtual void CMD_9() { string sCMD_Result; CMD_9(sCMD_Result,NULL);};
    virtual void CMD_9(string &sCMD_Result,Message *pMessage);


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

    virtual void CMD_Save_playlist(int iPK_Users,string sPK_EntertainArea,string sName,bool bSave_as_new) { string sCMD_Result; CMD_Save_playlist(iPK_Users,sPK_EntertainArea.c_str(),sName.c_str(),bSave_as_new,sCMD_Result,NULL);};
    virtual void CMD_Save_playlist(int iPK_Users,string sPK_EntertainArea,string sName,bool bSave_as_new,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #231 - Load Playlist */
    /** This will instruct the device to load the specific playlist. */
        /** @param #45 PK_EntertainArea */
            /** The entertainment area in which to load the  playlist. By defualt it will be the entertainment in which the current orbiter is running. */
        /** @param #78 EK_Playlist */
            /** The id of the playlist to load */

    virtual void CMD_Load_Playlist(string sPK_EntertainArea,int iEK_Playlist) { string sCMD_Result; CMD_Load_Playlist(sPK_EntertainArea.c_str(),iEK_Playlist,sCMD_Result,NULL);};
    virtual void CMD_Load_Playlist(string sPK_EntertainArea,int iEK_Playlist,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #240 - Back / Prior Menu */
    /** Navigate back .. ( Escape ) */
        /** @param #41 StreamID */
            /** ID of stream to apply */

    virtual void CMD_Back_Prior_Menu(int iStreamID) { string sCMD_Result; CMD_Back_Prior_Menu(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Back_Prior_Menu(int iStreamID,string &sCMD_Result,Message *pMessage);


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

    virtual void CMD_Start_Streaming(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string sStreamingTargets) { string sCMD_Result; CMD_Start_Streaming(iPK_MediaType,iStreamID,sMediaPosition.c_str(),sMediaURL.c_str(),sStreamingTargets.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Start_Streaming(int iPK_MediaType,int iStreamID,string sMediaPosition,string sMediaURL,string sStreamingTargets,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #259 - Report Playback Position */
    /** This will report the playback position of the current stream. */
        /** @param #9 Text */
            /** A human readable representation of the current position */
        /** @param #41 StreamID */
            /** The stream ID on which to report the position. */
        /** @param #42 MediaPosition */
            /** A media player readable representation of the current position including all options */

    virtual void CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition) { string sCMD_Result; CMD_Report_Playback_Position(iStreamID,sText,sMediaPosition,sCMD_Result,NULL);};
    virtual void CMD_Report_Playback_Position(int iStreamID,string *sText,string *sMediaPosition,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #269 - Move Playlist entry Up */
    /** Moves a entry up in the current playlist. */
        /** @param #48 Value */
            /** The id of the entry that needs to be moved up. */

    virtual void CMD_Move_Playlist_entry_Up(int iValue) { string sCMD_Result; CMD_Move_Playlist_entry_Up(iValue,sCMD_Result,NULL);};
    virtual void CMD_Move_Playlist_entry_Up(int iValue,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #270 - Move Playlist entry Down */
    /** Moves a entry down in the current playlist. */
        /** @param #48 Value */
            /** The id of the entry that needs to be moved down in the playlist. */

    virtual void CMD_Move_Playlist_entry_Down(int iValue) { string sCMD_Result; CMD_Move_Playlist_entry_Down(iValue,sCMD_Result,NULL);};
    virtual void CMD_Move_Playlist_entry_Down(int iValue,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #271 - Remove playlist entry. */
    /** Removes an entry from the playlist. */
        /** @param #48 Value */
            /** The Id of the entry that needs to be removed from the playlist. */

    virtual void CMD_Remove_playlist_entry(int iValue) { string sCMD_Result; CMD_Remove_playlist_entry(iValue,sCMD_Result,NULL);};
    virtual void CMD_Remove_playlist_entry(int iValue,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #412 - Set Media Position */
    /** Jump to a certain media position */
        /** @param #41 StreamID */
            /** The stream to set */
        /** @param #42 MediaPosition */
            /** The media position.  When MediaPlugin gets this, it will be a bookmark ID, when a media player gets it, the string */

    virtual void CMD_Set_Media_Position(int iStreamID,string sMediaPosition) { string sCMD_Result; CMD_Set_Media_Position(iStreamID,sMediaPosition.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #455 - Blue */
    /** VDR blue */

    virtual void CMD_Blue() { string sCMD_Result; CMD_Blue(sCMD_Result,NULL);};
    virtual void CMD_Blue(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #518 - Green */
    /** VDR Green */

    virtual void CMD_Green() { string sCMD_Result; CMD_Green(sCMD_Result,NULL);};
    virtual void CMD_Green(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #548 - Menu */
    /** Show a menu associated with this media */
        /** @param #9 Text */
            /** A string indicating which menu should appear.  The parameter is only used for smart media devices */
        /** @param #41 StreamID */
            /** ID of stream to apply */

    virtual void CMD_Menu(string sText,int iStreamID) { string sCMD_Result; CMD_Menu(sText.c_str(),iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Menu(string sText,int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #605 - Red */
    /** VDR red */

    virtual void CMD_Red() { string sCMD_Result; CMD_Red(sCMD_Result,NULL);};
    virtual void CMD_Red(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #616 - Select A */
    /** Select P1 keypad on supported game systems. */

    virtual void CMD_Select_A() { string sCMD_Result; CMD_Select_A(sCMD_Result,NULL);};
    virtual void CMD_Select_A(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #617 - Select B */
    /** Select P2 keypad on supported game systems. */

    virtual void CMD_Select_B() { string sCMD_Result; CMD_Select_B(sCMD_Result,NULL);};
    virtual void CMD_Select_B(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #651 - Thumbs Down */
    /** Signify currently playing song has thumbs down. */
        /** @param #41 StreamID */
            /** For Smart Media Players, the currently Playing Stream ID */

    virtual void CMD_Thumbs_Down(int iStreamID) { string sCMD_Result; CMD_Thumbs_Down(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Thumbs_Down(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #652 - Thumbs Up */
    /** Signify currently playing song in stream has thumbs up. */
        /** @param #41 StreamID */
            /** For smart media players, the currently playing media stream. */

    virtual void CMD_Thumbs_Up(int iStreamID) { string sCMD_Result; CMD_Thumbs_Up(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Thumbs_Up(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #682 - Yellow */
    /** VDR yellow */

    virtual void CMD_Yellow() { string sCMD_Result; CMD_Yellow(sCMD_Result,NULL);};
    virtual void CMD_Yellow(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #780 - Remove playlist */
    /** Removes a playlist from database */
        /** @param #78 EK_Playlist */
            /** The id of the playlist to be removed. */

    virtual void CMD_Remove_playlist(int iEK_Playlist) { string sCMD_Result; CMD_Remove_playlist(iEK_Playlist,sCMD_Result,NULL);};
    virtual void CMD_Remove_playlist(int iEK_Playlist,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #812 - Application Exited */
    /** Notify us that Myth Player exited */
        /** @param #227 PID */
            /** Process ID to be passed to the ApplicationExited function */
        /** @param #228 Exit Code */
            /** Exit Code to be passed to the ApplicationExited function */

    virtual void CMD_Application_Exited(int iPID,int iExit_Code) { string sCMD_Result; CMD_Application_Exited(iPID,iExit_Code,sCMD_Result,NULL);};
    virtual void CMD_Application_Exited(int iPID,int iExit_Code,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #882 - Abort Task */
    /** Abort a pending task */
        /** @param #248 Parameter ID */
            /** The ID of the task to abort */

    virtual void CMD_Abort_Task(int iParameter_ID) { string sCMD_Result; CMD_Abort_Task(iParameter_ID,sCMD_Result,NULL);};
    virtual void CMD_Abort_Task(int iParameter_ID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #916 - Set Aspect Ratio */
    /** Force aspect ratio */
        /** @param #41 StreamID */
            /** ID of stream to apply */
        /** @param #260 Aspect Ratio */
            /** aspect ratio to set: auto, 1:1, 4:3, 16:9, 2.11:1 */

    virtual void CMD_Set_Aspect_Ratio(int iStreamID,string sAspect_Ratio) { string sCMD_Result; CMD_Set_Aspect_Ratio(iStreamID,sAspect_Ratio.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Set_Aspect_Ratio(int iStreamID,string sAspect_Ratio,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #917 - Set Zoom */
    /** Sets zoom level, relative, absolute or 'auto' */
        /** @param #41 StreamID */
            /** ID of stream to apply */
        /** @param #261 Zoom Level */
            /** Zoom level to set */

    virtual void CMD_Set_Zoom(int iStreamID,string sZoom_Level) { string sCMD_Result; CMD_Set_Zoom(iStreamID,sZoom_Level.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Set_Zoom(int iStreamID,string sZoom_Level,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #920 - Set Media ID */
    /** Set Media ID - information about media stream */
        /** @param #10 ID */
            /** Media ID (special format) */
        /** @param #41 StreamID */
            /** ID of stream to set media information for */

    virtual void CMD_Set_Media_ID(string sID,int iStreamID) { string sCMD_Result; CMD_Set_Media_ID(sID.c_str(),iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Set_Media_ID(string sID,int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #942 - Get Ripping Status */
    /** Tell Game to Start 1 Player */
        /** @param #199 Status */
            /** Ripping status */

    virtual void CMD_Get_Ripping_Status(string *sStatus) { string sCMD_Result; CMD_Get_Ripping_Status(sStatus,sCMD_Result,NULL);};
    virtual void CMD_Get_Ripping_Status(string *sStatus,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #943 - Game 1P Start */
    /** 1P start */

    virtual void CMD_Game_1P_Start() { string sCMD_Result; CMD_Game_1P_Start(sCMD_Result,NULL);};
    virtual void CMD_Game_1P_Start(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #944 - Game 2P Start */
    /** 2P Start */

    virtual void CMD_Game_2P_Start() { string sCMD_Result; CMD_Game_2P_Start(sCMD_Result,NULL);};
    virtual void CMD_Game_2P_Start(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #945 - Game 3P Start */
    /** 3P Start */

    virtual void CMD_Game_3P_Start() { string sCMD_Result; CMD_Game_3P_Start(sCMD_Result,NULL);};
    virtual void CMD_Game_3P_Start(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #946 - Game 4P Start */
    /** 4P Start */

    virtual void CMD_Game_4P_Start() { string sCMD_Result; CMD_Game_4P_Start(sCMD_Result,NULL);};
    virtual void CMD_Game_4P_Start(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #947 - Game Insert Coin */
    /** Insert Coin */

    virtual void CMD_Game_Insert_Coin() { string sCMD_Result; CMD_Game_Insert_Coin(sCMD_Result,NULL);};
    virtual void CMD_Game_Insert_Coin(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #948 - Game Service */
    /** Service Mode */

    virtual void CMD_Game_Service() { string sCMD_Result; CMD_Game_Service(sCMD_Result,NULL);};
    virtual void CMD_Game_Service(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #949 - Game Start */
    /** Game Start */

    virtual void CMD_Game_Start() { string sCMD_Result; CMD_Game_Start(sCMD_Result,NULL);};
    virtual void CMD_Game_Start(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #950 - Game Select */
    /** Game Select */

    virtual void CMD_Game_Select() { string sCMD_Result; CMD_Game_Select(sCMD_Result,NULL);};
    virtual void CMD_Game_Select(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #951 - Game Option */
    /** Game Option */

    virtual void CMD_Game_Option() { string sCMD_Result; CMD_Game_Option(sCMD_Result,NULL);};
    virtual void CMD_Game_Option(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #952 - Game Reset */
    /** Game Reset */

    virtual void CMD_Game_Reset() { string sCMD_Result; CMD_Game_Reset(sCMD_Result,NULL);};
    virtual void CMD_Game_Reset(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #982 - Set Game Options */
    /** Set Options for the running Game System driver. */
        /** @param #5 Value To Assign */
            /** Dependent on driver, but usually a single line in the format of key,value */
        /** @param #219 Path */
            /** The Option to Set */

    virtual void CMD_Set_Game_Options(string sValue_To_Assign,string sPath) { string sCMD_Result; CMD_Set_Game_Options(sValue_To_Assign.c_str(),sPath.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Set_Game_Options(string sValue_To_Assign,string sPath,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #983 - Get Game Options */
    /** Get Options for the currently running driver. */
        /** @param #5 Value To Assign */
            /** The Returned value. */
        /** @param #219 Path */
            /** The Parameter to return, usually left side of comma in Set Game Options. */

    virtual void CMD_Get_Game_Options(string sPath,string *sValue_To_Assign) { string sCMD_Result; CMD_Get_Game_Options(sPath.c_str(),sValue_To_Assign,sCMD_Result,NULL);};
    virtual void CMD_Get_Game_Options(string sPath,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage);


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

    virtual void CMD_Add_Station(string sType,int iStreamID,string sName,string *sID) { string sCMD_Result; CMD_Add_Station(sType.c_str(),iStreamID,sName.c_str(),sID,sCMD_Result,NULL);};
    virtual void CMD_Add_Station(string sType,int iStreamID,string sName,string *sID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #1077 - Remove Station */
    /** Remove Radio Station from this User's List of stations. */
        /** @param #10 ID */
            /** Station ID to remove. */
        /** @param #41 StreamID */
            /** The Stream ID from which to remove */

    virtual void CMD_Remove_Station(string sID,int iStreamID) { string sCMD_Result; CMD_Remove_Station(sID.c_str(),iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Remove_Station(string sID,int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #1078 - Rename Station */
    /** Rename Radio Station in a user's list of radio stations. */
        /** @param #10 ID */
            /** The Radio Station ID to rename */
        /** @param #41 StreamID */
            /** The Stream ID */
        /** @param #50 Name */
            /** The New Name of the Radio Station */

    virtual void CMD_Rename_Station(string sID,int iStreamID,string sName) { string sCMD_Result; CMD_Rename_Station(sID.c_str(),iStreamID,sName.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Rename_Station(string sID,int iStreamID,string sName,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #1079 - Explain Song */
    /** Ask the radio station for an explanation of why this song is playing. */
        /** @param #41 StreamID */
            /** The Stream ID of the currently playing song. */

    virtual void CMD_Explain_Song(int iStreamID) { string sCMD_Result; CMD_Explain_Song(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Explain_Song(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #1080 - Add Music to Station */
    /** Add Music to an already existing Radio Station */
        /** @param #10 ID */
            /** The Radio Station ID to add terms to */
        /** @param #41 StreamID */
            /** The Stream ID */
        /** @param #50 Name */
            /** Search terms such as an artist or song title. */

    virtual void CMD_Add_Music_to_Station(string sID,int iStreamID,string sName) { string sCMD_Result; CMD_Add_Music_to_Station(sID.c_str(),iStreamID,sName.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Add_Music_to_Station(string sID,int iStreamID,string sName,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #1081 - Add Station to QuickMix */
    /** Add A Radio Station to the Currently Playing QuickMix */
        /** @param #10 ID */
            /** The Station ID to add */
        /** @param #41 StreamID */
            /** The StreamID with the quickmix to modify */

    virtual void CMD_Add_Station_to_QuickMix(string sID,int iStreamID) { string sCMD_Result; CMD_Add_Station_to_QuickMix(sID.c_str(),iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Add_Station_to_QuickMix(string sID,int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #1082 - Remove Station from QuickMix */
    /** Remove Radio Station from the currently playing QuickMix */
        /** @param #10 ID */
            /** The Station ID */
        /** @param #41 StreamID */
            /** The Stream ID */

    virtual void CMD_Remove_Station_from_QuickMix(string sID,int iStreamID) { string sCMD_Result; CMD_Remove_Station_from_QuickMix(sID.c_str(),iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Remove_Station_from_QuickMix(string sID,int iStreamID,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->

        QString getInternalMediaUrl() const;
        void setInternalMediaUrl(const QString &internalMediaUrl);

        int getCurrentSpeed() const;
        void setCurrentSpeed(int currentSpeed);

        int getCurrentFkFileType() const;
        void setCurrentFkFileType(int currentFkFileType);

    signals:
        void commandResponseChanged(QString);
        void mediaResponseChanged(QString);
        void currentMediaUrlChanged(QString);

        void streamIdChanged(int);
        void mediaIdChanged(QString);
        void mediaTypeChanged(int);
        void startPositionChanged(QString);
        void newMediaPosition(int);

        void stopCurrentMedia();
        void startPlayback();
        void pausePlayback();

        void getScreenShot();
        void setZoomLevel(QString zoom);
        void setAspectRatio(QString aspect);

        void connectionStatusChanged(bool);
        void jumpToStreamPosition(int);
        void volumeUp(int repeat);
        void volumeDown(int repeat);
        void pluginVolumeUp();
        void pluginVolumeDown();
        void audioLevelChanged(QString lvl);
        void trackUp();
        void trackDown();


    public slots:
        //playback info changed event
        void handleDelayedSeek(int seekTime);
        void updateMetadata(QString mediaTitle, QString mediaSubtitle, QString name, int screen);

        void confirmMediaStarted(QString description );
        void confirmMediaEnded(bool witherror);

        void positionChanged(QString total, QString current);

        void setConnectionStatus(bool b) {connected = b; emit connectionStatusChanged(connected);}

        void run();

        void mediaEnded(bool status);

        void setMediaID(QString id) {mediaID = id; emit mediaIdChanged(mediaID);}
        QString getMediaId(){return mediaID;}

        void setStartPosition(QString s) {startPosition = s; emit startPositionChanged(startPosition);}
        QString getStartPosition() {return startPosition;}

        void setMediaType(int t) {i_pkMediaType = t; emit mediaTypeChanged(i_pkMediaType);}
        int getMediaType() {return i_pkMediaType;}

        void setStreamID(int i) {i_StreamId = i; emit streamIdChanged(i_StreamId);}
        int getStreamID(){return i_StreamId;}

        void setCurrentMediaUrl(QString m) {currentMediaUrl = m; emit currentMediaUrlChanged(currentMediaUrl);}
        QString getCurrentMediaUrl(){return currentMediaUrl;}


        void setCommandResponse(QString r);
        QString getCommandResponse() {return commandResponse;}

        void setMediaResponse(QString m) {mediaResponse = QTime::currentTime().toString()+"::QMediaPlayer:: "+ m; emit mediaResponseChanged(mediaResponse);}
        QString getMediaResponse() {return mediaResponse;}


    private:
        int m_currentSpeed;
        int m_currentFkFileType;
        MediaManagerBase * mp_manager;
        QString m_internalMediaUrl;

        int m_iChapter ;
        int m_iTitle;
        string s_totalTime;
        string s_audioTracks;
        string s_subTitleTracks;
        QTimer *seekDelayTimer;

    private:
        string getDcePosition();

    };

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
