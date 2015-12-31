#ifndef LINUXMCEDATA_H
#define LINUXMCEDATA_H

#include <QObject>
#include <QString>

class LinuxmceData : public QObject{
    Q_OBJECT

public:
    explicit LinuxmceData(QObject *parent =0);

};

/*!
 * \brief The HostSystemData class is used as a simple enum to provide
 *  a qml enum for the host operating system.
 * import into qml with statement: import org.linuxmce.enums
 * use: HostDevices.OSX_DESKTOP
 */
class HostSystemData : public QObject{
    Q_OBJECT
    Q_ENUMS(HostTypes)
public:

    HostSystemData(){}
    virtual ~HostSystemData(){}
    enum HostTypes{
        RASPBERRY_PI=1,
        OSX_DESKTOP=2,
        LINUX_DESKTOP=3,
        WINDOWS_DESKTOP=4,
        ANDROID_PHONE=5,
        ANDROID_TABLET=6,
        IOS_PHONE=7,
        IOS_TABLET=8,
        OTHER_EMBEDDED=9,
        SAILFISH_OS=10
    };

signals:

public slots:

    Q_INVOKABLE static const QString getSystemName(int s){
        switch(s){
        case RASPBERRY_PI: return tr("Raspberry Pi"); break;
        case OSX_DESKTOP:  return tr("OS X Desktop"); break;
        case LINUX_DESKTOP: return tr("Linux Desktop"); break;
        case ANDROID_TABLET:  return tr("Android Tablet"); break;
        case ANDROID_PHONE: return tr("Android Phone"); break;
        case IOS_PHONE: return tr("IOS Phone"); break;
        case IOS_TABLET: return tr("IOS Tablet"); break;
        default: return tr("Dunno, Windows 98?"); break;
        }
    }

};



class HouseModesHelper : public QObject
{
    Q_OBJECT
    Q_ENUMS(HouseModes)
public:
    HouseModesHelper() {}
    virtual ~HouseModesHelper() {}
    enum HouseModes{
        Unarmed_at_home=1,
        Armed_away=2,
        Armed_at_home=3,
        Sleeping=4,
        Entertaining=5,
        Armed_Extended_away=6
    };


public slots:
  Q_INVOKABLE static const QString translateType(int m){
        switch (m) {
        case Unarmed_at_home:
            return tr("Unarmed, at home.", "UserMode status");
            break;
        case Armed_away:
            return tr("Armed, Away.", "UserMode status");
            break;
        case Armed_at_home:
            return tr("Armed, at home.", "UserMode status");
            break;
        case Sleeping:
            return tr("Sleeping.", "UserMode status");
            break;
        case Entertaining:
            return tr("Entertaining.", "UserMode status");
            break;
        case Armed_Extended_away:
            return tr("Armed, Extended Away.", "UserMode status");
            break;
        default:
            break;
        }
    }
};

class UserModesHelper : public QObject
{
    Q_OBJECT
     Q_ENUMS(UserModes)
public:
    UserModesHelper(){}
    virtual ~UserModesHelper(){}
    enum UserModes{
        USERMODE_At_Home =1,
        USERMODE_Away,
        USERMODE_Sleeping,
        USERMODE_Do_not_disturb
    };


public slots:
   Q_INVOKABLE static const QString translateType(int m){
        switch (m) {
        case USERMODE_At_Home:
            return tr("Home.", "UserMode status");
            break;
        case USERMODE_Away:
            return tr("Away.", "UserMode status");
            break;
        case USERMODE_Sleeping:
            return tr("Sleeping.", "UserMode status");
            break;
        case USERMODE_Do_not_disturb:
            return tr("Do Not Disturb.", "UserMode status");
            break;
        default:
            break;
        }
    }
};

class RoomTypeHelper : public QObject
{
    Q_OBJECT
     Q_ENUMS(RoomTypes)
public:
    RoomTypeHelper() {}
    virtual ~RoomTypeHelper() {}
    enum RoomTypes{
        Living_RoomFamily_Room=1,
        ClosetStorage_Space=2,
        Bedroom_Other=3,
        Garage=4,
        Kitchen=5,
        Patio=6,
        Dining_Room=7,
        Outside_Area=8,
        Miscellaneous=9,
        Hallway=10,
        Bedroom_Master=11,
        Home_Theater=12,
        FoyerEntrance=13,
        Bathroom=14,
        Unmanaged=15
    };


public slots:
    static const QString translateType(int m){
        switch (m) {
        case Living_RoomFamily_Room:
            return tr("Living/Family Room", "Room Type");
            break;
        case ClosetStorage_Space:
            return tr("Closet Storage space.", "Room Type");
            break;
        case Bedroom_Other:
            return tr("Bedroom, Other.", "Room Type");
            break;
        case Bedroom_Master:
            return tr("Master Bedroom", "Room Type");
            break;
        case Garage:
            return tr("Garage", "Room Type");
            break;
        case Kitchen:
            return tr("Kitchen", "Room Type");
            break;
        case Patio:
            return tr("Patio", "Room Type");
            break;
        case Dining_Room:
            return tr("Dining Room", "Room Type");
            break;
        case Outside_Area:
            return tr("Outside Area", "Room Type");
            break;
        case Miscellaneous:
            return tr("Miscellaneous", "Room Type");
            break;
        case Hallway:
            return tr("Hallway", "Room Type");
            break;
        case Home_Theater:
            return tr("Home Theater", "Room Type");
            break;
        case FoyerEntrance:
            return tr("Foyer/Entrance", "Room Type");
            break;
        case Unmanaged:
            return tr("Unmanaged Area", "Room Type");
            break;
        default:
            return "Langston is lazy or out of range.";
            break;
        }
    }
};

class ScreenList : public QObject
{
    Q_OBJECT
     Q_ENUMS(Screens)
public:
    ScreenList() {}
    virtual ~ScreenList() {}
    enum Screens{
        /*<-SAG->*/
        Main = 1,
        Lights = 2,
        Media = 3,
        Climate = 4,
        Security = 5,
        Telephony = 6,
        Misc = 7,
        UserStatus = 8,
        MakeCallPhonebook = 9,
        MakeCallFavorites = 10,
        MakeCallDialNumber = 11,
        MakeCallPlutoUser = 12,
        SecurityPanel = 13,
        SecurityStatusReport = 14,
        SingleCameraViewOnly = 15,
        Intercom = 16,
        QuadViewCameras = 17,
        VideoCallInProgress = 18,
        FileSave = 19,
        ManagePlaylist = 20,
        MakeCallDevice = 21,
        MakeCallPhonebook2 = 22,
        MakeCallIntercom = 23,
        CurrentUser = 24,
        CDTrackCopy = 25,
        CDRemote = 26,
        VoicemailMain = 27,
        TransferAllUsers = 28,
        Sleeping = 29,
        ControllerStatus = 30,
        OtherFolders = 31,
        ConfirmDelete = 32,
        WebURL = 33,
        Browser = 34,
        ModeChanged = 35,
        SensorsNotReady = 36,
        RecordedTVShows = 37,
        DeviceControl = 38,
        Computing = 39,
        OpenFile = 40,
        BrowserConsole = 41,
        FavoritesPDA = 42,
        TI994A_Keyboard = 43,
        AdvancedOptions = 44,
        MakingCall = 45,
        MakingLinPhoneBroadcast = 46,
        FileList_Music_Movies_Video = 47,
        PVR_Remote = 48,
        DVD_Remote = 49,
        DVDMenu = 50,
        MapMedia = 51,
        AdvancedMediaOptions = 52,
        PopupMessage = 53,
        MusicRemote = 54,
        LinPhones = 55,
        Orbiters = 56,
        Phones = 57,
        DVDOptions = 58,
        AnswertoLinPhones = 59,
        UserPinCode = 60,
        PreviewOptions = 61,
        TransferOutside = 62,
        SatelliteCableBox = 63,
        CurrentLocation = 64,
        MediaAttributeSearchInput = 65,
        MediaAttributeSearchResult = 66,
        NewPhoneDetected = 67,
        FirstTime = 68,
        FileList_PlayLists = 69,
        VideosRemote = 70,
        FileList_Pictures_Docs = 71,
        GenericTwoButtonQuestion = 72,
        TryCodes = 73,
        ScreenSaver = 74,
        Pick_Room_For_Device = 75,
        Power = 76,
        Generic_DVD_Remote = 77,
        ViewPlaylist = 78,
        FileList_withPictures_Mobil = 79,
        GenericDvdMenu = 80,
        ViewAttributes = 81,
        PendingTasks = 82,
        AddAttribute = 83,
        RadioRemote = 84,
        GenericCDRemote = 85,
        FileList_Popup_Container = 86,
        CreateViewBookmarks = 87,
        BrowseBookmarks = 88,
        GenericAppController = 89,
        BookmarksByMediaType = 90,
        MythTvRemote = 92,
        Myth_all_commands = 93,
        Mytv_watching_tv = 94,
        Myth_live_tv_browse_mode = 95,
        Myth_playback_recording = 96,
        GenericKeyboard = 97,
        PingPong = 98,
        TVEPG1 = 99,
        TVEPG2 = 100,
        TVEPG3 = 101,
        ShowInfo = 102,
        CreateViewBookmarksTV = 103,
        WebSites = 105,
        WhatModelMobileOrbiter = 106,
        Filelist = 107,
        DVDmenu = 108,
        Attributes = 109,
        DVDoptions = 110,
        AdvRemote = 111,
        Playlist = 112,
        AudioOptions = 113,
        Subtitles = 114,
        Angles = 115,
        NewPnpDevice = 116,
        VDR_Remote = 117,
        CdRemote = 118,
        PowerFile = 119,
        DevCallInProgress = 120,
        DevIncomingCall = 121,
        DevMoveCallsInProgress = 122,
        MapSecurity = 123,
        MapLighting = 124,
        MapClimate = 125,
        MapTelecom = 126,
        NavMode = 127,
        PVR_Full_Screen = 128,
        TI_Keyboard = 129,
        Help01 = 130,
        Help02 = 131,
        Help03 = 132,
        SetupMythTv01 = 133,
        SetupMythTv02 = 134,
        SetupMythTv03 = 135,
        SetupMythTv04 = 136,
        PVR_Full_Screen_Options = 137,
        DVDFullScreen = 138,
        DVDFullScreenOpt = 139,
        VideosFullScreen = 140,
        VideosFullScreenOpt = 141,
        GenericAppFullScreen = 142,
        DVDMenuFullScreen = 143,
        DVDMenuFullScreenOpt = 144,
        VlcFullScreen = 145,
        VlcFullScreenOpt = 146,
        Game_Commodore_Remote_Alt = 147,
        rokuRemote = 148,
        rokuKBD = 149,
        Game_Atari800KBD = 150,
        Game_Intellivision = 151,
        popOthers = 152,
        Game_Atari_5200 = 153,
        Game_Atari_5200_Monitor_Mode = 154,
        Game_Atari_5200_FS = 155,
        Game_Atari_5200_FS_Options = 156,
        Game_SG1000_SMS_NES_Famicom = 157,
        Game_SG1000_SMS_NES_Famicom_Monitor_Mode = 158,
        Game_SG1000_SMS_NES_Famicom_FS = 159,
        Game_SG1000_SMS_NES_Famicom_FS_Options = 160,
        Game_Menu_Onscreen_NonOSD = 161,
        Hulu_Remote = 162,
        Hulu_OSD = 163,
        Hulu_OSD_Options = 164,
        Get_Capture_Card_Audio_Port = 165,
        Apple_2_Remote = 166,
        Apple_2_FS = 167,
        Commodore_Remote = 168,
        AirTunes = 169,
        AirTunes_FS = 170,
        AirTunes_FS_Options = 171,
        Commodore_FS = 172,
        Commodore_FS_Options = 173,
        CleanScreen = 174,
        VideoWizard = 175,
        DialogCannotPlayMedia = 176,
        DialogRippingInProgress = 178,
        DialogCheckingDrive = 179,
        DialogUnableToSavePlaylist = 180,
        DialogPlaylistSaved = 181,
        DialogUnableToLoadPlaylist = 182,
        DialogRippingError = 183,
        DialogRippingInstructions = 184,
        DialogGenericError = 185,
        DialogCannotBookmark = 186,
        DialogAskToResume = 187,
        DialogGC100Error = 188,
        DialogPhoneInstructions = 189,
        DialogSendFileToPhoneFailed = 190,
        AudiHome = 191,
        RoomsWizard = 192,
        UsersWizard = 193,
        CountryWizard = 194,
        TV_Manufacturer = 196,
        Receiver = 197,
        LightsSetup = 198,
        AlarmPanel = 199,
        VOIP_Provider = 200,
        AV_Devices = 201,
        NAS_Options = 202,
        Need_Reload_Router = 204,
        Need_Regen_Orbiter = 205,
        Sensors_Viewed_By_Camera = 206,
        PVR_OSD = 207,
        PVR_Recordings = 208,
        Demo_Media = 209,
        Download_are_ready_to_install = 210,
        mnuAmbiance = 212,
        TVConfirmInputsToggle = 216,
        PVR_OSD_Full_Screen = 218,
        PVR_Recordings_Full_Screen = 219,
        CurrentlyActiveRemote = 220,
        mnuLights = 222,
        mnuPlaybackControl = 223,
        mnuSpeedControl = 224,
        mnuVolume = 225,
        Media_Tracks = 230,
        Which_Wizard = 231,
        This_Room = 232,
        NAS_Options_when_Mounting_device = 234,
        New_Pnp_Device_One_Possibility = 235,
        Wizard_Done = 236,
        House_Setup_Popup_Message = 237,
        Media_Player_Setup_Popup_Message = 238,
        Add_Software = 239,
        Choose_Folder = 240,
        Choose_Drive = 241,
        TV_Channels = 242,
        CD_Full_Screen_OSD = 243,
        Music_Full_Screen_OSD = 244,
        Choose_Provider_for_Device = 245,
        Get_Username_Password_For_Devices = 246,
        Get_Capture_Card_Port = 247,
        Floorplan = 248,
        Halt_System = 249,
        PVR_Software = 250,
        Final_House_Setup = 251,
        ExternalDeviceFullScreen = 252,
        ExternalDeviceFullScreen_Opts = 253,
        Remote_Assistance = 255,
        Legacy_AV_Speed_Control = 256,
        Thumbnail = 257,
        Scanning_Progress = 258,
        Manage_Drives = 259,
        Jukebox_Manager = 260,
        NAS_Manager = 261,
        AutoConfigure_TV = 262,
        AdjustScreenSettings = 263,
        PNP_Generic_Options = 264,
        Zoom__Aspect = 265,
        Legacy_PVR_Cable_Box = 266,
        Active_Calls = 267,
        Call_Dropped = 268,
        Assisted_Transfer_In_Progress = 269,
        Static_IP_settings = 270,
        Network_Settings = 271,
        PVR_Recordings_Full_Screen_Options = 272,
        VDR_Recordings_Full_Screen_Options = 273,
        VDR_OSD = 274,
        VDR_Recordings_Full_Screen = 275,
        VDR_OSD_Full_Screen = 276,
        VDR_Full_Screen_Options = 277,
        VDR_Full_Screen = 278,
        VDR_Recordings = 279,
        Simple_TV_Remote = 280,
        Non_Pluto_TV_Full_screen = 281,
        Non_Pluto_TV_Full_screen_options = 282,
        Cannot_Reload_Router = 283,
        Game = 284,
        Rip_Options = 285,
        Rip_format_options = 286,
        Privacy_options_for_rip = 287,
        Game_Screen_FS = 288,
        Game_Screen_FS_Options = 289,
        Current_Disc_Contents = 290,
        Edit_Attributes = 291,
        Game_Monitor_Mode = 292,
        Game_Atari_2600_Monitor_Mode = 293,
        Game_Atari_2600_FS_Options = 294,
        Game_Atari_2600 = 295,
        Game_Atari_2600_FS = 296,
        Game_Atari_2600_Controller_Options = 297,
        Generic_BluRay_Remote = 298,
        NonPluto_BluRay_FS = 299,
        NonPluto_BluRay_FS_Options = 300,
        Tivo_Series_3_Remote = 301,
        Tivo_Series_2_Remote = 302,
        Weather_Now = 303,
        Weather_Forecast = 304,
        Weather_3day = 305,
        Weather_6day = 306,
        Weather_Radar = 307,
        WD_HDTVLive_Remote = 308,
        XBOX_360 = 309,
        TI_99_FS = 310,
        TI_99_FS_Options = 311
        /*<-SAE->*/
    };


  Q_INVOKABLE QString static const translateType(int m){
        switch (m) {

        default:
            return tr("Nope");
            break;
        }
    }
};

#endif // LINUXMCEDATA_H
