/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org

    Langston Ball  golgoj4@gmail.com
    QOrbiter is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QOrbiter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QOrbiter.  If not, see <http://www.gnu.org/licenses/>.
*/



//<-dceag-d-b->
#ifndef qOrbiter_h
#define qOrbiter_h

//	DCE Implemenation for #2186 qOrbiter

#include "Gen_Devices/qOrbiterBase.h"
//<-dceag-d-e->
#include <DataGrid.h>
#include "Virtual_Device_Translator.h"
#include "PreformedCommand.h"

#include <contextobjects/floorplandevice.h>
#include <contextobjects/modelpage.h>
#include <datamodels/gridItem.h>
#include <datamodels/genericmodelitem.h>
#include <contextobjects/epgchannellist.h>
#include <contextobjects/playlistitemclass.h>
#include <datamodels/DataModelItems/attributesortitem.h>
#include <contextobjects/floorplandevice.h>
#include <QList>
#include <datamodels/avdevice.h>
#include <contextobjects/existingorbiter.h>
#include <contextobjects/promptdata.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QVariant>
#include "DCECommand.h"


#if !defined(QT5) && !defined(Q_OS_ANDROID)
#include <QX11EmbedWidget>
#endif

//<-dceag-d-b->!



//dceag-d-e->

//<-dceag-decl-b->!
//class basicImageProvider;
//<-dceag-decl-e->

//<-dceag-decl-b->
namespace DCE
{
/*!
 *\class qOrbiter
 * \ingroup dce_controllers
 *\brief The object represents the c++ api for the LinuxMCE system.
 *
 * This Object is the 1/2 man, 1/2 machine interface for the Linuxmce System. It begins as a series of auto generated stubs, which then
 *are filled in by the software developer building the interface. The object runs in its own thread as it is prone to long wait times
 *and represents the server in a client / server model. By implementing this in  its own thread, the GUI is not subject to blocking by the
 *processing of data. Information is emitted across threads via connections.
 *
 */
class qOrbiter : public qOrbiter_Command
{
    //<-dceag-decl-e->
    Q_OBJECT

    Q_PROPERTY(QString dceIP READ getdceIP WRITE setdceIP NOTIFY dceIPChanged)
    Q_PROPERTY(QString DCEHost READ getDCEHost WRITE setDCEHost NOTIFY DCEHostChanged)

    Q_PROPERTY (int modelPages READ getModelPages WRITE setModelPages NOTIFY modelPagesChanged)
    Q_PROPERTY (int i_current_mediaType READ getMediaType WRITE setMediaType NOTIFY mediaTypeChanged)
    Q_PROPERTY (int m_dwPK_Device READ getDeviceId WRITE setDeviceId NOTIFY deviceIdChanged)
    Q_PROPERTY(int qMediaPlayerID READ getqMediaPlayerID WRITE setqMediaPlayerID NOTIFY qMediaPlayerIDChanged)
    Q_PROPERTY(int qCommunicatorID READ getqCommunicatorID WRITE setqCommunicatorID NOTIFY qCommunicatorIDChanged)

    // Private member variables

    // Private methods
public:

    enum DceRemoteCommands {
        BackClearEntry=363,    /*!< For Keyboard use */
        BackPriorMenu=240,     /*!< For intermenu use */
        EnterGo=190,           /*!< Confirm Button */
        Guide=126,             /*!< Any form of guide the orbiter can show */
        Menu=548,              /*!< Contextual Menu */
        MoveDown=201,          /*!< Movie focus down in context */
        MoveLeft=202,          /*!< Move focus left in context */
        MoveRight=203,         /*!< Move focus right in context */
        MoveUp=200,            /*!< Move focus up in contex */
        TogglePower=194        /*!< Toggle the power of the gui*/
    };

    enum DceUtilityCommands{
        Off=193,
        On=192
    };

    // Public member variables
    /*!
   *@name MediaPlayer
   */
    //@{
    QString videoFileUrl;               /*!< \brief Contains the full path of the video media  */
    QString audioFileUrl;               /*!< \brief Contains the full path of the audio media */
    QString streamingVideoIp;           /*!< \brief Contains the ip address with port if included of streamed video. */
    QString streamingAudioIp;           /*!< \brief Contains the ip address with port if included of stremed audio */
    QString mediaPlayerTimeCode;        /*!< \brief Contains the timecode for the local media player (QMediaPlayer)  */
    int mediaPlaylistSize;              /*!< \brief Contains the current media player playlist length, if possible */
    //@}

    /*! @name Misc Member Variables */
    //@{
    typedef QMap <int, QString> myMap;

    char *screenieData;                 /**< char pointer to raw screenshot data from dce socket */
    int screenieDataSize;               /**< int to size of \ref screenieData */
    Virtual_Device_Translator coreDevices;/**< \warning not implemented */
    bool m_bOrbiterConnected;
    QString currentScreen;
    //@}
QString deviceName;
    /*! @name Child Device Member Variables */
    //@{
    int qMediaPlayerID;/**< qMediaPlayer device ID */
    int qCommunicatorID;/**< qCommunicator device ID \warning not implemented */
    //@}

    /*! \name Now Playing Member Variables */
    //@{
    int m_dwPK_Device_NowPlaying;                       /**< set by the media engine, the current media player id  */
    int m_dwPK_Device_NowPlaying_Video;                 /**< set by the media engine with the current video device id  */
    int m_dwPK_Device_NowPlaying_Audio;                 /**< set by the media engine with the current audio device id  */
    int m_dwPK_Device_CaptureCard;                      /** < set by the media engine, this is whatever media device is currently playing.  Capture Card is non null if we're displaying media via this card */
    bool m_bPK_Device_NowPlaying_Audio_DiscreteVolume;  /**< set by the media engine if the audio device supports discrete audio  */
    int m_bContainsVideo;                               /**< set by the media engine if the content contains video */
    int m_bUsingLiveAVPath;                             /**< set by the media engine if the media is using a direct control path (DirectAV)  */
    bool retrieving;
    bool finished;
    bool b_mediaPlaying;                                /**< set by the media engine   */
    int internal_streamID;
    int internal_playback_speed;
    //@}

    /*! @name Datagrid Member Variables. */
    //@{
    bool gridPaused;
    int i_mediaModelRows;       /*!< \brief The total rows in the media model */
    int media_currentRow;       /*!< \brief The current row of the media model, representing a paged position */
    int media_totalPages;       /*!< \brief The total pages as divided by the media_pageSeperator */
    int media_currentPage;      /*!< \brief The current page of the grid */
    int media_pos;              /*!< \todo find out what this is */
    int media_pageSeperator;    /*!< \brief The amount of cells on a given page. Set high for infinite scrolling. */
    string media_seek;          /*!< \brief Where the grid is seeking to. */
    int modelPages;             /*!< \brief The total number of pages in the model. */
    bool b_cancelRequest;
    // DataGridTable *pMediaGridTable;
    //@}

    /*! @name Address and ip Member variables */
    //@{
    QString dceIP;          /**< The ip address of the router  */
    QString DCEHost;        /**< The hostname of the router */
    string m_sExternalIP;   /**< The external ip address of the router as seen from a WAN */
    QString m_localIp;
    QString m_localMac;
    //@}

    /*! @name Device Id Member Variables */
    //@{
    int iPK_Device_DatagridPlugIn;      /*!< \brief Datagrid plugin id. */
    int iPK_Device_OrbiterPlugin;       /*!< \brief Orbiter plugin id. */
    int iPK_Device_GeneralInfoPlugin;   /*!< \brief GeneralInfoPlugin id. */
    int  iPK_Device_SecurityPlugin;     /*!< \brief SecurityPlugin id. */
    int  iPK_Device_LightingPlugin;     /*!< \brief LightingPlugin id. */
    int  m_dwIDataGridRequestCounter;   /*!< \brief Datagrid request counter. Incremented per dg request for identifying the current grid. */
    int iOrbiterPluginID;
    int  iMediaPluginID;
    int iPK_Device_eventPlugin;
    int m_pOrbiterCat;
    QMap<int, QString> childrenDeviceList;
#if !defined(QT5) && !defined(Q_OS_ANDROID)
    QMap<int, QX11EmbedContainer*> windowList;
#endif
    //@}

    /*! @name Location, user, and Floorplan Member Variables */
    //@{
    int i_ea;
    int i_room;
    int i_user;
    int i_current_mediaType;
    int i_current_floorplanType;
    string *s_user;
    int i_currentGridType;
    //@}

    /*! @name Orbiter Setup Member Variables */
    //@{
    int sPK_Room;
    int sPK_Lang;
    int sPK_Skin;
    int sPK_Users;
    int sHeight;
    int sWidth;

    QNetworkAccessManager * httpOrbiterSettings;
    QNetworkRequest * httpSettingsRequest;
    QNetworkReply * httpSettingsReply;
    //@}

    /*! @name Filter Parameter Member Variables */
    //@{
    int cellsToRender;              /*!< Total Cells in this data grid request */
    QString *datagridVariableString;/*!< The String that will be used for the datagrid command */
    bool requestMore;               /*!< Related to the requesting operations of filling the grid.
    //@}

    /*! @name Legacy Member Variables
     * \brief carried over variables from old OrbiterData.h
     * They still need to be integrated into the user interface if possible, but they relate to db tables no longer used for UI generation
     */
    //@{
    map<int,string> m_mapTextString;                    /*!< Unknown use */
    vector<int> m_vectPK_Users_RequiringPIN;            /*!< Needs to be reimplemented */
    map<int,int> m_mapDesignObj;                        /*!< Used to map a screen to a DesignObj */
    map<int,int> m_mapPK_Screen_GoBackToScreen;         /*!< For screens in this map, if there's a go back */
    map<int,int> m_mapPK_MediaType_PK_Attribute_Sort;   /*!  The default sort for each type of media*/
    map<int,int> m_mapScreen_Interrupt;                 /*!<  Map of which scripts can be interrupted. Unknown use */
    map<int,string> m_mapPK_MediaType_Description;      /*!< The description for all the mediatypes */
    map<int,string> m_mapPK_AttributeType_Description;  /*!< Carried over from old orbter. The description for all attribute types. */
    bool m_bIsOSD ;                                     /*!< Tracking if this is an onscreen qOrbiter */
    bool m_bNewOrbiter;                                 /*!< Tracking if this is a new orbiter */
    bool m_bUseAlphaBlending ;                          /*!< Carried over from old orbiter, may or may not apply */
    bool m_bUseMask;                                    /*!< Carried Over from old Orbiter. May or may not apply */
    long currentDeviceTemplate;                         /*!< Used to track which DT we actually are: Onscreen QOrbiter, or Android, or iOS, or Desktop */
    //@}
    QTimer *watchdog;

    //<-dceag-const-b->
public:
    // Constructors/Destructor
    qOrbiter(QString name, int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL, QObject *parent = 0);

    //virtual bool GetConfig();

    //<-dceag-const-e->
      virtual bool GetConfig();
    virtual bool Register();
    virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
    virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
    virtual void OnDisconnect();
    virtual void OnUnexpectedDisconnect();
    virtual void OnReload();

    virtual bool OnReplaceHandler(string msg);
    virtual int PromptFor(std::string sToken);
    virtual int PromptUser(std::string sPrompt, int iTimeoutSeconds, map<int, std::string> *p_mapPrompts);
    virtual int DeviceIdInvalid();
    virtual int SetupNewOrbiter();
    virtual void CreateChildren();



    //<-dceag-const-e->

    //<-dceag-const2-b->
    // The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
    // You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
    qOrbiter(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
    //<-dceag-const2-e->

    //<-dceag-h-b->
    /*
                AUTO-GENERATED SECTION
                Do not change the declarations
    */

    /*
            *****DATA***** accessors inherited from base class
    int DATA_Get_PK_Users();
    void DATA_Set_PK_Users(int Value,bool bUpdateDatabase=false);
    int DATA_Get_PK_Distro();
    bool DATA_Get_Development();
    int DATA_Get_PK_FloorplanObjectType();
    void DATA_Set_PK_FloorplanObjectType(int Value,bool bUpdateDatabase=false);
    bool DATA_Get_No_Effects();
    int DATA_Get_PK_Skin();
    int DATA_Get_PK_Size();
    void DATA_Set_PK_Size(int Value,bool bUpdateDatabase=false);
    int DATA_Get_PK_Language();
    string DATA_Get_FK_EntertainArea();
    void DATA_Set_FK_EntertainArea(string Value,bool bUpdateDatabase=false);
    string DATA_Get_Timeout();
    string DATA_Get_CacheFolder();
    int DATA_Get_CacheSize();
    bool DATA_Get_Use_OCG_Format();
    int DATA_Get_VideoFrameInterval();
    int DATA_Get_ImageQuality();
    void DATA_Set_ImageQuality(int Value,bool bUpdateDatabase=false);
    bool DATA_Get_Ignore();
    bool DATA_Get_Dont_Auto_Jump_to_Remote();
    int DATA_Get_ScreenWidth();
    int DATA_Get_ScreenHeight();
    int DATA_Get_Rotation();
    int DATA_Get_PK_Screen();
    bool DATA_Get_Get_Time_Code_for_Media();
    string DATA_Get_AV_Adjustment_Rules();
    bool DATA_Get_Enable_Memory_Management();
    void DATA_Set_Enable_Memory_Management(bool Value,bool bUpdateDatabase=false);
    int DATA_Get_Border_Size();
    string DATA_Get_Model();
    string DATA_Get_Alert_Filter_Level();
    bool DATA_Get_Ignore_First_Event();
    void DATA_Set_Ignore_First_Event(bool Value,bool bUpdateDatabase=false);
    bool DATA_Get_Automatically_Go_to_Remote();
    bool DATA_Get_Queue_Instead_of_Instant_Play();
    void DATA_Set_Queue_Instead_of_Instant_Play(bool Value,bool bUpdateDatabase=false);

            *****EVENT***** accessors inherited from base class

            *****COMMANDS***** we need to implement
    */


    /** @brief COMMAND: #1 - Capture Keyboard To Variable */
    /** As the user types, using either the keyboard or simulate keypress commands, what he types will be stored in a variable and/or put into a text object. */
    /** @param #3 PK_DesignObj */
    /** The Design Object which contains the text Object */
    /** @param #4 PK_Variable */
    /** The variable in which to store the input */
    /** @param #8 On/Off */
    /** If 0, this stops capturing */
    /** @param #14 Type */
    /** 1=normal, 2=pin, 3=phone number */
    /** @param #24 Reset */
    /** if true, the next keypress will clear the variable and start new */
    /** @param #25 PK_Text */
    /** The text object in which to store the current input */
    /** @param #55 DataGrid */
    /** If 1, we'll scroll the data grid too when typing keys. */

    virtual void CMD_Capture_Keyboard_To_Variable(string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid) { string sCMD_Result; CMD_Capture_Keyboard_To_Variable(sPK_DesignObj.c_str(),iPK_Variable,sOnOff.c_str(),sType.c_str(),sReset.c_str(),iPK_Text,bDataGrid,sCMD_Result,NULL);};
    virtual void CMD_Capture_Keyboard_To_Variable(string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #2 - Orbiter Beep */
    /** Make the orbiter beep */

    virtual void CMD_Orbiter_Beep() { string sCMD_Result; CMD_Orbiter_Beep(sCMD_Result,NULL);};
    virtual void CMD_Orbiter_Beep(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #3 - Display On/Off */
    /** Turn the display on or off */
    /** @param #8 On/Off */
    /** 0=Off, 1=On */
    /** @param #125 Already processed */
    /** Normally Orbiter will forward the on/off through DCE so the other devices can turn on/off.  If this is true, it won't. */

    virtual void CMD_Display_OnOff(string sOnOff,bool bAlready_processed) { string sCMD_Result; CMD_Display_OnOff(sOnOff.c_str(),bAlready_processed,sCMD_Result,NULL);};
    virtual void CMD_Display_OnOff(string sOnOff,bool bAlready_processed,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #4 - Go back */
    /** Make the orbiter go back to the prior screen, like the back button in a web browser */
    /** @param #16 PK_DesignObj_CurrentScreen */
    /** If this is specified, the orbiter will ignore the command unless this is the current screen */
    /** @param #21 Force */
    /** Screens can be flagged, "Cant go back", meaning the go back will skip over them.  If Force=1, the Orbiter returns to the prior screen regardless */

    virtual void CMD_Go_back(string sPK_DesignObj_CurrentScreen,string sForce) { string sCMD_Result; CMD_Go_back(sPK_DesignObj_CurrentScreen.c_str(),sForce.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Go_back(string sPK_DesignObj_CurrentScreen,string sForce,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #5 - Goto DesignObj */
    /** Goto a specific design obj */
    /** @param #2 PK_Device */
    /** For this screen only, override the normal "control device" stored in variable #1, and treat this device as the control screen.  When the screen changes, it will be reset */
    /** @param #3 PK_DesignObj */
    /** The screen to go to.  Can be be fully qualified (x.y.z), or just contain the screen # */
    /** @param #10 ID */
    /** Assigns an optional ID to this particular "viewing" of the screen, used with Kill Screen.  There can be lots of instances of the same screen in the history queue (such as call in progress).  This allows a program to pop a particular one out of the queue. */
    /** @param #16 PK_DesignObj_CurrentScreen */
    /** If this is specified, the orbiter will ignore the command unless this is the current screen.  If this is -1, that will match a main menu or screen saver (ie the Orbiter is not in use). */
    /** @param #22 Store Variables */
    /** If 1, the Orbiter will store the current variable values, and restore them if a 'go back' causes it to return to this screen */
    /** @param #114 Cant Go Back */
    /** If true, then when this screen goes away the user won't be able to return to it -- it will be skipped over, unless Go Back with Force=1 is used.  This prevents layers of popup screens. */

    virtual void CMD_Goto_DesignObj(int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables,bool bCant_Go_Back) { string sCMD_Result; CMD_Goto_DesignObj(iPK_Device,sPK_DesignObj.c_str(),sID.c_str(),sPK_DesignObj_CurrentScreen.c_str(),bStore_Variables,bCant_Go_Back,sCMD_Result,NULL);};
    virtual void CMD_Goto_DesignObj(int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables,bool bCant_Go_Back,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #6 - Show Object */
    /** Change an objects visible state. */
    /** @param #3 PK_DesignObj */
    /** The object to show or hide */
    /** @param #4 PK_Variable */
    /** The variable to use in the comparisson.  See Comparisson Value. */
    /** @param #6 Comparisson Operator */
    /** A type of comparisson: =  <  <>  !=  > */
    /** @param #7 Comparisson Value */
    /** If a Variable, Comparisson Type, and Comparisson Value are specified, the command will be ignored if the comparisson is not true */
    /** @param #8 On/Off */
    /** 1=show object, 0=hide object */

    virtual void CMD_Show_Object(string sPK_DesignObj,int iPK_Variable,string sComparisson_Operator,string sComparisson_Value,string sOnOff) { string sCMD_Result; CMD_Show_Object(sPK_DesignObj.c_str(),iPK_Variable,sComparisson_Operator.c_str(),sComparisson_Value.c_str(),sOnOff.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Show_Object(string sPK_DesignObj,int iPK_Variable,string sComparisson_Operator,string sComparisson_Value,string sOnOff,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #7 - Terminate Orbiter */
    /** Causes the Orbiter application to exit */

    virtual void CMD_Terminate_Orbiter() { string sCMD_Result; CMD_Terminate_Orbiter(sCMD_Result,NULL);};
    virtual void CMD_Terminate_Orbiter(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #8 - Remove Screen From History */
    /** The orbiter keeps a history of visible screens, allowing the user to go back.  See Go_Back.  This removes screens from the queue that should not available anymore.  An example is when a call comes in, the controllers are sent to an incoming call screen. */
    /** @param #10 ID */
    /** If specified, only screens that match this ID will be removed */
    /** @param #159 PK_Screen */
    /** The screen to remove */

    virtual void CMD_Remove_Screen_From_History(string sID,int iPK_Screen) { string sCMD_Result; CMD_Remove_Screen_From_History(sID.c_str(),iPK_Screen,sCMD_Result,NULL);};
    virtual void CMD_Remove_Screen_From_History(string sID,int iPK_Screen,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #9 - Scroll Grid */
    /** Scroll a datagrid */
    /** @param #1 Relative Level */
    /** The grid will scroll this many lines.  If prefaced with a P, it will scroll this many pages.  If not specified, it will scroll 1 page. */
    /** @param #3 PK_DesignObj */
    /** The grid to scroll.  If not specified, any currently visible grids will scroll */
    /** @param #30 PK_Direction */
    /** The direction to scroll the grid */

    virtual void CMD_Scroll_Grid(string sRelative_Level,string sPK_DesignObj,int iPK_Direction) { string sCMD_Result; CMD_Scroll_Grid(sRelative_Level.c_str(),sPK_DesignObj.c_str(),iPK_Direction,sCMD_Result,NULL);};
    virtual void CMD_Scroll_Grid(string sRelative_Level,string sPK_DesignObj,int iPK_Direction,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #10 - Move Highlight */
    /** Move the current highlight pointer */
    /** @param #1 Relative Level */
    /** The grid will scroll this many lines.  If prefaced with a P, it will scroll this many pages.  If not specified, it will scroll 1 page. */
    /** @param #3 PK_DesignObj */
    /** The grid to scroll.  If not specified, any currently visible grids will scroll */
    /** @param #30 PK_Direction */
    /** The direction to move the highlight */

    virtual void CMD_Move_Highlight(string sRelative_Level,string sPK_DesignObj,int iPK_Direction) { string sCMD_Result; CMD_Move_Highlight(sRelative_Level.c_str(),sPK_DesignObj.c_str(),iPK_Direction,sCMD_Result,NULL);};
    virtual void CMD_Move_Highlight(string sRelative_Level,string sPK_DesignObj,int iPK_Direction,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #13 - Play Sound */
    /** Plays a sound file on the orbiter */
    /** @param #19 Data */
    /** A pointer to a block of memory representing the sound file to play */
    /** @param #20 Format */
    /** Indicates what type of data is in the memory block.  1=wav, 2=mp3 */

    virtual void CMD_Play_Sound(char *pData,int iData_Size,string sFormat) { string sCMD_Result; CMD_Play_Sound(pData,iData_Size,sFormat.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Play_Sound(char *pData,int iData_Size,string sFormat,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #14 - Refresh */
    /** Invalidates and redraws the current screen, optionally re-requesting data from a datagrid.  The OnLoad commands are not fired.  See Regen Screen. */
    /** @param #15 DataGrid ID */
    /** Normally refresh does not cause the orbiter to re-request data.  But if a specific grid ID is specified, that grid will be refreshed.  Specify * to re-request all grids on the current screen */

    virtual void CMD_Refresh(string sDataGrid_ID) { string sCMD_Result; CMD_Refresh(sDataGrid_ID.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Refresh(string sDataGrid_ID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #15 - Regen Screen */
    /** The screen is reloaded like the user was going to it for the first time.  The OnUnload and OnLoad commands are fired. */

    virtual void CMD_Regen_Screen() { string sCMD_Result; CMD_Regen_Screen(sCMD_Result,NULL);};
    virtual void CMD_Regen_Screen(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #16 - Requires Special Handling */
    /** When a button needs to do something too sophisticated for a normal command, attach this command.  When the controller sees it, it will pass execution to a local handler that must be added to the Orbiter's code. */

    virtual void CMD_Requires_Special_Handling() { string sCMD_Result; CMD_Requires_Special_Handling(sCMD_Result,NULL);};
    virtual void CMD_Requires_Special_Handling(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #17 - Seek Data Grid */
    /** Causes a datagrid to seek to a particular position. */
    /** @param #9 Text */
    /** If specified, the orbiter will jump to the first row which has a cell that starts with this text.  Specify Position X to use a column other than the first one. */
    /** @param #11 Position X */
    /** The column to jump to.  If Text is not blank, the column to search. */
    /** @param #12 Position Y */
    /** The row to jump to.  Ignored if Text is not blank */
    /** @param #15 DataGrid ID */
    /** The datagrid to scroll.  If not specified, the first visible one will be used */

    virtual void CMD_Seek_Data_Grid(string sText,int iPosition_X,int iPosition_Y,string sDataGrid_ID) { string sCMD_Result; CMD_Seek_Data_Grid(sText.c_str(),iPosition_X,iPosition_Y,sDataGrid_ID.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Seek_Data_Grid(string sText,int iPosition_X,int iPosition_Y,string sDataGrid_ID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #18 - Set Graphic To Display */
    /** All objects on screen can be either in "Normal" mode, "Selected mode", "Highlighted mode", or any number of "Alternate modes".  These are like "views".  A Selected mode may appear depressed, for example.  All children of this object will also be set. */
    /** @param #3 PK_DesignObj */
    /** The object to set */
    /** @param #10 ID */
    /** 0=standard mode, -1=selected -2=highlight a positive number is one of the alternates */

    virtual void CMD_Set_Graphic_To_Display(string sPK_DesignObj,string sID) { string sCMD_Result; CMD_Set_Graphic_To_Display(sPK_DesignObj.c_str(),sID.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Set_Graphic_To_Display(string sPK_DesignObj,string sID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #20 - Set Object Parameter */
    /** changes one of the object's DesignObjParameters */
    /** @param #3 PK_DesignObj */
    /** The object to change */
    /** @param #5 Value To Assign */
    /** The value to assign */
    /** @param #27 PK_DesignObjParameter */
    /** The parameter */

    virtual void CMD_Set_Object_Parameter(string sPK_DesignObj,string sValue_To_Assign,int iPK_DesignObjParameter) { string sCMD_Result; CMD_Set_Object_Parameter(sPK_DesignObj.c_str(),sValue_To_Assign.c_str(),iPK_DesignObjParameter,sCMD_Result,NULL);};
    virtual void CMD_Set_Object_Parameter(string sPK_DesignObj,string sValue_To_Assign,int iPK_DesignObjParameter,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #21 - Set Object Position */
    /** Change an objects's position on the screen */
    /** @param #3 PK_DesignObj */
    /** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
    /** @param #11 Position X */
    /**  */
    /** @param #12 Position Y */
    /**  */

    virtual void CMD_Set_Object_Position(string sPK_DesignObj,int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Set_Object_Position(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,NULL);};
    virtual void CMD_Set_Object_Position(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #22 - Set Object Size */
    /** Change an object's size */
    /** @param #3 PK_DesignObj */
    /** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
    /** @param #11 Position X */
    /**  */
    /** @param #12 Position Y */
    /**  */

    virtual void CMD_Set_Object_Size(string sPK_DesignObj,int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Set_Object_Size(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,NULL);};
    virtual void CMD_Set_Object_Size(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #23 - Set Pos Rel To Parent */
    /** Like Set Object Position, but the X and Y coordinates are assumed to be relative to the parent rather than absolute */
    /** @param #3 PK_DesignObj */
    /** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
    /** @param #11 Position X */
    /**  */
    /** @param #12 Position Y */
    /**  */

    virtual void CMD_Set_Pos_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Set_Pos_Rel_To_Parent(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,NULL);};
    virtual void CMD_Set_Pos_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #24 - Set Size Rel To Parent */
    /** Change an object's size, relative to it's parent object */
    /** @param #3 PK_DesignObj */
    /** The object to move.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
    /** @param #11 Position X */
    /** The percentage of the parent object's width.  100=the parent's full width. */
    /** @param #12 Position Y */
    /** The percentage of the parent object's height.  100=the parent's full height. */

    virtual void CMD_Set_Size_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Set_Size_Rel_To_Parent(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sCMD_Result,NULL);};
    virtual void CMD_Set_Size_Rel_To_Parent(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #25 - Set Text */
    /** Change the text within a text object on the fly */
    /** @param #3 PK_DesignObj */
    /** The Design Object which contains the text object.  Can be a fully qualified object (x.y.z), or just the object ID, in which case the orbiter will find all such objects currently on screen. */
    /** @param #9 Text */
    /** The text to assign */
    /** @param #25 PK_Text */
    /** The text object in which to store the current input */

    virtual void CMD_Set_Text(string sPK_DesignObj,string sText,int iPK_Text) { string sCMD_Result; CMD_Set_Text(sPK_DesignObj.c_str(),sText.c_str(),iPK_Text,sCMD_Result,NULL);};
    virtual void CMD_Set_Text(string sPK_DesignObj,string sText,int iPK_Text,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #26 - Set Bound Icon */
    /** Sets an icon that is bound to status.  "Bind Icon" is put in the object's on load commands, and then this command sets the status at runtime. */
    /** @param #5 Value To Assign */
    /** The value corresponding to an alt graphic. */
    /** @param #9 Text */
    /** Text for the binding */
    /** @param #14 Type */
    /** The type of bound icon. */

    virtual void CMD_Set_Bound_Icon(string sValue_To_Assign,string sText,string sType) { string sCMD_Result; CMD_Set_Bound_Icon(sValue_To_Assign.c_str(),sText.c_str(),sType.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Set_Bound_Icon(string sValue_To_Assign,string sText,string sType,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #27 - Set Variable */
    /** Change the value of a variable */
    /** @param #4 PK_Variable */
    /** The variable to change */
    /** @param #5 Value To Assign */
    /** The value to assign */

    virtual void CMD_Set_Variable(int iPK_Variable,string sValue_To_Assign) { string sCMD_Result; CMD_Set_Variable(iPK_Variable,sValue_To_Assign.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Set_Variable(int iPK_Variable,string sValue_To_Assign,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #28 - Simulate Keypress */
    /** Simulates that a key has been touched.  Touchable keys on screen can use this command to allow for simultaneous operation with keyboard or mouse.  Also works with the "Capture Keyboard to Variable" command. */
    /** @param #26 PK_Button */
    /** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
    /** @param #41 StreamID */
    /** ID of stream to apply */
    /** @param #50 Name */
    /** The application to send the keypress to. If not specified, it goes to the DCE device. */

    virtual void CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName) { string sCMD_Result; CMD_Simulate_Keypress(sPK_Button.c_str(),iStreamID,sName.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #29 - Simulate Mouse Click */
    /** Simulates a mouse click or touch on the indicated x & y coordinates */
    /** @param #11 Position X */
    /** position X */
    /** @param #12 Position Y */
    /** position Y */
    /** @param #41 StreamID */
    /** ID of stream to apply */

    virtual void CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,int iStreamID) { string sCMD_Result; CMD_Simulate_Mouse_Click(iPosition_X,iPosition_Y,iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Simulate_Mouse_Click(int iPosition_X,int iPosition_Y,int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #30 - Stop Sound */
    /** If a sound file is being played, it will be stopped. */

    virtual void CMD_Stop_Sound() { string sCMD_Result; CMD_Stop_Sound(sCMD_Result,NULL);};
    virtual void CMD_Stop_Sound(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #31 - Store Variables */
    /** The orbiter will store a snapshot of the variables at this moment, and if the user returns to this screen with a go back, it will restore the variables to this value. */

    virtual void CMD_Store_Variables() { string sCMD_Result; CMD_Store_Variables(sCMD_Result,NULL);};
    virtual void CMD_Store_Variables(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #32 - Update Object Image */
    /** Changes the background image within an object */
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


    /** @brief COMMAND: #58 - Set Current User */
    /** Sets what user is currently using the orbiter. */
    /** @param #17 PK_Users */
    /** The user currently using the orbiter. */

    virtual void CMD_Set_Current_User(int iPK_Users) { string sCMD_Result; CMD_Set_Current_User(iPK_Users,sCMD_Result,NULL);};
    virtual void CMD_Set_Current_User(int iPK_Users,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #59 - Set Entertainment Area */
    /** If you don't know the location, you can also set just the entertainment area */
    /** @param #45 PK_EntertainArea */
    /** The current entertainment area where the orbiter is. */

    virtual void CMD_Set_Entertainment_Area(string sPK_EntertainArea) { string sCMD_Result; CMD_Set_Entertainment_Area(sPK_EntertainArea.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Set_Entertainment_Area(string sPK_EntertainArea,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #66 - Select Object */
    /** The same as clicking on an object. */
    /** @param #3 PK_DesignObj */
    /** The object to select. */
    /** @param #16 PK_DesignObj_CurrentScreen */
    /** Will only happen if this is the current screen. */
    /** @param #102 Time */
    /** If specified, rather than happening immediately it will happen in x seconds. */

    virtual void CMD_Select_Object(string sPK_DesignObj,string sPK_DesignObj_CurrentScreen,string sTime) { string sCMD_Result; CMD_Select_Object(sPK_DesignObj.c_str(),sPK_DesignObj_CurrentScreen.c_str(),sTime.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Select_Object(string sPK_DesignObj,string sPK_DesignObj_CurrentScreen,string sTime,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #67 - Spawn Application */
    /** Spawn the given application.  Mainly used for windows orbiters. */
    /** @param #13 Filename */
    /** The name of the executable file to spawn */
    /** @param #50 Name */
    /** A name that we'll remember the application by for future kill commands */
    /** @param #51 Arguments */
    /** Command arguments, tab delimited */
    /** @param #94 SendOnFailure */
    /** Send this messages if the process exited with failure error code. */
    /** @param #95 SendOnSuccess */
    /** Send this messages if the process exited with success error code. */
    /** @param #115 Show logo */
    /** If this is set then we will first select the logo  before spawning the application. */
    /** @param #120 Retransmit */
    /** If false, and if Exclusive is true and another instance is killed, the 'send messages on termination' will not be sent. */
    /** @param #126 Exclusive */
    /** If true, then kill other apps with this same name */
    /** @param #241 Detach */
    /** Detach application after spawning / Don't kill this app on reload. */

    virtual void CMD_Spawn_Application(string sFilename,string sName,string sArguments,string sSendOnFailure,string sSendOnSuccess,bool bShow_logo,bool bRetransmit,bool bExclusive,bool bDetach) { string sCMD_Result; CMD_Spawn_Application(sFilename.c_str(),sName.c_str(),sArguments.c_str(),sSendOnFailure.c_str(),sSendOnSuccess.c_str(),bShow_logo,bRetransmit,bExclusive,bDetach,sCMD_Result,NULL);};
    virtual void CMD_Spawn_Application(string sFilename,string sName,string sArguments,string sSendOnFailure,string sSendOnSuccess,bool bShow_logo,bool bRetransmit,bool bExclusive,bool bDetach,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #72 - Surrender to OS */
    /** Let the O/S take over.  This is useful with the Orbiter running on the media director's desktop as a full screen app, and media is inserted, or the user starts a computer application on the mobile phone.  The orbiter will then let the other application ta */
    /** @param #8 On/Off */
    /** 1=Hide and let the OS take over.  0=The orbiter comes up again. */
    /** @param #54 Fully release keyboard */
    /** Only applies if on/off is 1.  If this is false, the orbiter will still filter keystrokes, looking for macros to implement, and only pass on keys that it doesn't catch.  If true, it will pass all keys.  True also releases the mouse. */
    /** @param #225 Always */
    /** If true, the mouse will always be ignored */

    virtual void CMD_Surrender_to_OS(string sOnOff,bool bFully_release_keyboard,bool bAlways) { string sCMD_Result; CMD_Surrender_to_OS(sOnOff.c_str(),bFully_release_keyboard,bAlways,sCMD_Result,NULL);};
    virtual void CMD_Surrender_to_OS(string sOnOff,bool bFully_release_keyboard,bool bAlways,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #77 - Set Current Room */
    /** If you don't know the location, you can also set just the room */
    /** @param #57 PK_Room */
    /** The room */

    virtual void CMD_Set_Current_Room(int iPK_Room) { string sCMD_Result; CMD_Set_Current_Room(iPK_Room,sCMD_Result,NULL);};
    virtual void CMD_Set_Current_Room(int iPK_Room,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #85 - Reset Highlight */
    /** Resets the currently highlighted object.  Do this when you hide or unhide blocks that have tab stops. */
    /** @param #3 PK_DesignObj */
    /** If specified, this object will be highlighted.  Otherwise the first detected one. */

    virtual void CMD_Reset_Highlight(string sPK_DesignObj) { string sCMD_Result; CMD_Reset_Highlight(sPK_DesignObj.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Reset_Highlight(string sPK_DesignObj,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #88 - Set Current Location */
    /** Sets the location the orbiter is in.  The location is a combination of room and entertainment area. */
    /** @param #65 LocationID */
    /** The location ID is a sequential number created by OrbiterGen which defines a combination of room and entertainment area. */

    virtual void CMD_Set_Current_Location(int iLocationID) { string sCMD_Result; CMD_Set_Current_Location(iLocationID,sCMD_Result,NULL);};
    virtual void CMD_Set_Current_Location(int iLocationID,string &sCMD_Result,Message *pMessage);


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


    /** @brief COMMAND: #97 - Mute */
    /** mute */

    virtual void CMD_Mute() { string sCMD_Result; CMD_Mute(sCMD_Result,NULL);};
    virtual void CMD_Mute(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #126 - Guide */
    /** Go to the media guide if applicable, otherwise forward to media plugin. */

    virtual void CMD_Guide() { string sCMD_Result; CMD_Guide(sCMD_Result,NULL);};
    virtual void CMD_Guide(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #190 - Enter/Go */
    /** Select the highlighted item */
    /** @param #41 StreamID */
    /** ID of stream to apply */

    virtual void CMD_EnterGo(int iStreamID) { string sCMD_Result; CMD_EnterGo(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_EnterGo(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #192 - On */
    /** Turn the device on */
    /** @param #97 PK_Pipe */
    /** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
    /** @param #98 PK_Device_Pipes */
    /** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

    virtual void CMD_On(int iPK_Pipe,string sPK_Device_Pipes) { string sCMD_Result; CMD_On(iPK_Pipe,sPK_Device_Pipes.c_str(),sCMD_Result,NULL);};
    virtual void CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #193 - Off */
    /** Turn the device off */
    /** @param #97 PK_Pipe */
    /** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

    virtual void CMD_Off(int iPK_Pipe) { string sCMD_Result; CMD_Off(iPK_Pipe,sCMD_Result,NULL);};
    virtual void CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #194 - Toggle Power */
    /** First time stops any media playing.  Second time turns the display off.  Third time powers off the media director. */
    /** @param #8 On/Off */
    /** Depending on each device On/Off can be interpreted differently, but in genereal On/Off has a value of 1 for on and 0 for Off */

    virtual void CMD_Toggle_Power(string sOnOff) { string sCMD_Result; CMD_Toggle_Power(sOnOff.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Toggle_Power(string sOnOff,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #200 - Move Up */
    /** Move the highlight up */
    /** @param #41 StreamID */
    /** ID of stream to apply */

    virtual void CMD_Move_Up(int iStreamID) { string sCMD_Result; CMD_Move_Up(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #201 - Move Down */
    /** Move the highlight down */
    /** @param #41 StreamID */
    /** ID of stream to apply */

    virtual void CMD_Move_Down(int iStreamID) { string sCMD_Result; CMD_Move_Down(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #202 - Move Left */
    /** Move the highlight left */
    /** @param #41 StreamID */
    /** ID of stream to apply */

    virtual void CMD_Move_Left(int iStreamID) { string sCMD_Result; CMD_Move_Left(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #203 - Move Right */
    /** Move the highlight right */
    /** @param #41 StreamID */
    /** ID of stream to apply */

    virtual void CMD_Move_Right(int iStreamID) { string sCMD_Result; CMD_Move_Right(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #238 - Continuous Refresh */
    /** Continuously refresh the current page.  Used when the page contains constantly changing data. */
    /** @param #102 Time */
    /** The interval time in seconds */

    virtual void CMD_Continuous_Refresh(string sTime) { string sCMD_Result; CMD_Continuous_Refresh(sTime.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Continuous_Refresh(string sTime,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #240 - Back / Prior Menu */
    /** If at a remote control, forward to media plugin.  Otherwise go back 1 screen. */
    /** @param #41 StreamID */
    /** ID of stream to apply */

    virtual void CMD_Back_Prior_Menu(int iStreamID) { string sCMD_Result; CMD_Back_Prior_Menu(iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Back_Prior_Menu(int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #242 - Set Now Playing */
    /** Used by the media engine to set the "now playing" text on an orbiter.  If the orbiter is bound to the remote for an entertainment area it will get more updates than just media,  like cover art, but this is the basic information that is visible on screens */
    /** @param #3 PK_DesignObj */
    /** 4 comma delimited objects: normal remote, popup remote, file list remote, popup file list remote, guide */
    /** @param #5 Value To Assign */
    /** The description of the media */
    /** @param #9 Text */
    /** The description of the current section (ie chapter in a dvd, etc.) */
    /** @param #29 PK_MediaType */
    /** The type of media playing */
    /** @param #41 StreamID */
    /** The ID of the current stream */
    /** @param #48 Value */
    /** The track number or position in the playlist */
    /** @param #50 Name */
    /** The name of the window for the application to remain in the foreground */
    /** @param #103 List PK Device */
    /** (comma-delimited list): The current source device, video device, the current audio device, 1/0 if audio device supports discrete volume */
    /** @param #120 Retransmit */
    /** If true, it will re-request the plist (current playlist) grid */

    virtual void CMD_Set_Now_Playing(string sPK_DesignObj,string sValue_To_Assign,string sText,int iPK_MediaType,int iStreamID,int iValue,string sName,string sList_PK_Device,bool bRetransmit) { string sCMD_Result; CMD_Set_Now_Playing(sPK_DesignObj.c_str(),sValue_To_Assign.c_str(),sText.c_str(),iPK_MediaType,iStreamID,iValue,sName.c_str(),sList_PK_Device.c_str(),bRetransmit,sCMD_Result,NULL);};
    virtual void CMD_Set_Now_Playing(string sPK_DesignObj,string sValue_To_Assign,string sText,int iPK_MediaType,int iStreamID,int iValue,string sName,string sList_PK_Device,bool bRetransmit,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #254 - Bind Icon */
    /** Used to make a button have an icon that reflects a current state, such as the user's status, the house mode, etc.  This is accomplished by creating an object with multiple alternate versions, and then executing a "Set  Bound Icon" to select the right one. */
    /** @param #3 PK_DesignObj */
    /** The object which contains the icon, or whose child objects contain the icon. */
    /** @param #14 Type */
    /** The type of binding, like "housemode", "userstatus_39288", etc. */
    /** @param #104 Child */
    /** If true, it will set the property for the child object(s), rather than the designated object. */

    virtual void CMD_Bind_Icon(string sPK_DesignObj,string sType,bool bChild) { string sCMD_Result; CMD_Bind_Icon(sPK_DesignObj.c_str(),sType.c_str(),bChild,sCMD_Result,NULL);};
    virtual void CMD_Bind_Icon(string sPK_DesignObj,string sType,bool bChild,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #258 - Clear Selected Devices */
    /** Floorplans, in particular, rely on a vector of selected devices, allowing the user to select more than one.  This command clears that list, removing any selected devices.  It can optionally cause the Object passed in as a parameter to be refreshed. */
    /** @param #3 PK_DesignObj */
    /** If specified, the object referenced here will be invalidated and redrawn. */

    virtual void CMD_Clear_Selected_Devices(string sPK_DesignObj) { string sCMD_Result; CMD_Clear_Selected_Devices(sPK_DesignObj.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Clear_Selected_Devices(string sPK_DesignObj,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #260 - Set Main Menu */
    /** Sets the Main Menu to 'Normal', 'Sleeping' or 'Screen Saver', optionally assigning new screens to those values. */
    /** @param #9 Text */
    /** Can be N, S, or V to set to the Normal, Sleeping, or Screen Saver, optionally followed by an = to assign a new screen to that menu.  e.g. N or N=1872.0.0 */

    virtual void CMD_Set_Main_Menu(string sText) { string sCMD_Result; CMD_Set_Main_Menu(sText.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Set_Main_Menu(string sText,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #265 - Quit */
    /** Exits the orbiter application */

    virtual void CMD_Quit() { string sCMD_Result; CMD_Quit(sCMD_Result,NULL);};
    virtual void CMD_Quit(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #324 - Set Timeout */
    /** Specifies when a given screen will timeout, executing the timeout actions.  This will also reset a pending timeout */
    /** @param #3 PK_DesignObj */
    /** The screen to set the timeout on.  If blank the current screen. */
    /** @param #102 Time */
    /** The timeout in seconds.  0 or blank means no timeout. */

    virtual void CMD_Set_Timeout(string sPK_DesignObj,string sTime) { string sCMD_Result; CMD_Set_Timeout(sPK_DesignObj.c_str(),sTime.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Set_Timeout(string sPK_DesignObj,string sTime,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #325 - Keep Screen On */
    /** Allow or don't allow the screen to blank with the screen saver. */
    /** @param #8 On/Off */
    /** If other than "0", the screen saver will be disabled. */

    virtual void CMD_Keep_Screen_On(string sOnOff) { string sCMD_Result; CMD_Keep_Screen_On(sOnOff.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Keep_Screen_On(string sOnOff,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #330 - Set Mouse Pointer Over Object */
    /** Positions the on-screen mouse pointer centered over a certain object */
    /** @param #3 PK_DesignObj */
    /** The object to center the mouse over. */

    virtual void CMD_Set_Mouse_Pointer_Over_Object(string sPK_DesignObj) { string sCMD_Result; CMD_Set_Mouse_Pointer_Over_Object(sPK_DesignObj.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Set_Mouse_Pointer_Over_Object(string sPK_DesignObj,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #354 - Show Mouse Pointer */
    /** Shows or hides the mouse pointer. */
    /** @param #8 On/Off */
    /** 1=show it, 0=hide it. */

    virtual void CMD_Show_Mouse_Pointer(string sOnOff) { string sCMD_Result; CMD_Show_Mouse_Pointer(sOnOff.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Show_Mouse_Pointer(string sOnOff,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #363 - Back / Clear Entry */
    /** If at a remote control, forward to media plugin.  Otherwise clear typing */

    virtual void CMD_Back_Clear_Entry() { string sCMD_Result; CMD_Back_Clear_Entry(sCMD_Result,NULL);};
    virtual void CMD_Back_Clear_Entry(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #366 - Activate Window */
    /** ActivateApplication - Used by Linux On Screeen Orbiters only */
    /** @param #50 Name */
    /** Name as known by ratpoison. */

    virtual void CMD_Activate_Window(string sName) { string sCMD_Result; CMD_Activate_Window(sName.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Activate_Window(string sName,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #389 - Send Message */
    /** Sends a message stored in a parameter as a text object. */
    /** @param #9 Text */
    /** The message in command line-style format */
    /** @param #144 Go Back */
    /** Go back after sending the command if it does not contain another goto screen or go back */

    virtual void CMD_Send_Message(string sText,bool bGo_Back) { string sCMD_Result; CMD_Send_Message(sText.c_str(),bGo_Back,sCMD_Result,NULL);};
    virtual void CMD_Send_Message(string sText,bool bGo_Back,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #397 - Show Popup */
    /** Shows a screen as a popup, at position x, y */
    /** @param #3 PK_DesignObj */
    /** The ID of the screen */
    /** @param #11 Position X */
    /** X position */
    /** @param #12 Position Y */
    /** Y position */
    /** @param #16 PK_DesignObj_CurrentScreen */
    /** If specified the popup will be local to this screen, otherwise it will be global.  Global and local popups are treated separately */
    /** @param #50 Name */
    /** The popup name */
    /** @param #126 Exclusive */
    /** Hide any other popups that are also visible, unless don't hide is set. */
    /** @param #127 Dont Auto Hide */
    /** If true, this popup will not be automatically hidden when the screen changes or another exclusive is shown.  It must be explicitly hidden. */

    virtual void CMD_Show_Popup(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string sName,bool bExclusive,bool bDont_Auto_Hide) { string sCMD_Result; CMD_Show_Popup(sPK_DesignObj.c_str(),iPosition_X,iPosition_Y,sPK_DesignObj_CurrentScreen.c_str(),sName.c_str(),bExclusive,bDont_Auto_Hide,sCMD_Result,NULL);};
    virtual void CMD_Show_Popup(string sPK_DesignObj,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string sName,bool bExclusive,bool bDont_Auto_Hide,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #398 - Remove Popup */
    /** Hides a popup. */
    /** @param #16 PK_DesignObj_CurrentScreen */
    /** (optional).  The screen on which it's a local popup */
    /** @param #50 Name */
    /** The name of the popup.  If not specified all popups will be removed */

    virtual void CMD_Remove_Popup(string sPK_DesignObj_CurrentScreen,string sName) { string sCMD_Result; CMD_Remove_Popup(sPK_DesignObj_CurrentScreen.c_str(),sName.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Remove_Popup(string sPK_DesignObj_CurrentScreen,string sName,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #399 - Show Shortcuts */
    /** Shows keyboard shortcuts for 10 seconds or until the screen changes. */

    virtual void CMD_Show_Shortcuts() { string sCMD_Result; CMD_Show_Shortcuts(sCMD_Result,NULL);};
    virtual void CMD_Show_Shortcuts(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #401 - Show File List */
    /** Shows the file list */
    /** @param #29 PK_MediaType */
    /** The type of media the user wants to browse. */

    virtual void CMD_Show_File_List(int iPK_MediaType) { string sCMD_Result; CMD_Show_File_List(iPK_MediaType,sCMD_Result,NULL);};
    virtual void CMD_Show_File_List(int iPK_MediaType,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #402 - Use Popup Remote Controls */
    /** If this command is executed the remote controls will be displayed as popups. */
    /** @param #11 Position X */
    /** The location of the popup */
    /** @param #12 Position Y */
    /** The location of the popup */
    /** @param #16 PK_DesignObj_CurrentScreen */
    /** The screen on which to put the popup */

    virtual void CMD_Use_Popup_Remote_Controls(int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen) { string sCMD_Result; CMD_Use_Popup_Remote_Controls(iPosition_X,iPosition_Y,sPK_DesignObj_CurrentScreen.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Use_Popup_Remote_Controls(int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #403 - Use Popup File List */
    /** If this command is executed the file lists will be displayed as popups. */
    /** @param #11 Position X */
    /** The location of the popup */
    /** @param #12 Position Y */
    /** The location of the popup */
    /** @param #16 PK_DesignObj_CurrentScreen */
    /** The screen to put the popup on */

    virtual void CMD_Use_Popup_File_List(int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen) { string sCMD_Result; CMD_Use_Popup_File_List(iPosition_X,iPosition_Y,sPK_DesignObj_CurrentScreen.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Use_Popup_File_List(int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #405 - Scale this object */
    /** If you add this command to the startup list of an object it will cause Orbiter Gen to scale this object and all it's children. */
    /** @param #48 Value */
    /** The value to scale to.  100=full size, 50=half size */

    virtual void CMD_Scale_this_object(int iValue) { string sCMD_Result; CMD_Scale_this_object(iValue,sCMD_Result,NULL);};
    virtual void CMD_Scale_this_object(int iValue,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #407 - Set Floorplan */
    /** Sets the object to use for one of the following types:
light, media, climate, security, telecom */
    /** @param #3 PK_DesignObj */
    /** The screen to use for this floorplan */
    /** @param #14 Type */
    /** One of the following:
light, climate, media, security, telecom */
    /** @param #119 True/False */
    /** True if this is a popup.  False if it's full screen */

    virtual void CMD_Set_Floorplan(string sPK_DesignObj,string sType,bool bTrueFalse) { string sCMD_Result; CMD_Set_Floorplan(sPK_DesignObj.c_str(),sType.c_str(),bTrueFalse,sCMD_Result,NULL);};
    virtual void CMD_Set_Floorplan(string sPK_DesignObj,string sType,bool bTrueFalse,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #408 - Show Floorplan */
    /** Shows the floorplan */
    /** @param #11 Position X */
    /** If the floorplan is not full screen, the location where it should be displayed */
    /** @param #12 Position Y */
    /** If the floorplan is not full screen, the location where it should be displayed */
    /** @param #14 Type */
    /** The type of floorplan */

    virtual void CMD_Show_Floorplan(int iPosition_X,int iPosition_Y,string sType) { string sCMD_Result; CMD_Show_Floorplan(iPosition_X,iPosition_Y,sType.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Show_Floorplan(int iPosition_X,int iPosition_Y,string sType,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #413 - Forward local k/b to OSD */
    /** Means this orbiter's keyboard should be controlling the application running on the media director. */
    /** @param #119 True/False */
    /** If 1, do it.  It 0, stop */

    virtual void CMD_Forward_local_kb_to_OSD(bool bTrueFalse) { string sCMD_Result; CMD_Forward_local_kb_to_OSD(bTrueFalse,sCMD_Result,NULL);};
    virtual void CMD_Forward_local_kb_to_OSD(bool bTrueFalse,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #415 - Set Mouse Position Relative */
    /** Move the mouse relative to its current position */
    /** @param #11 Position X */
    /** The X Position to move */
    /** @param #12 Position Y */
    /** The Y Position to move */

    virtual void CMD_Set_Mouse_Position_Relative(int iPosition_X,int iPosition_Y) { string sCMD_Result; CMD_Set_Mouse_Position_Relative(iPosition_X,iPosition_Y,sCMD_Result,NULL);};
    virtual void CMD_Set_Mouse_Position_Relative(int iPosition_X,int iPosition_Y,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #416 - Simulate Mouse Click At Present Pos */
    /** Simulates clicking the mouse button */
    /** @param #14 Type */
    /** If L or empty, the left button.  If R the right button. */

    virtual void CMD_Simulate_Mouse_Click_At_Present_Pos(string sType) { string sCMD_Result; CMD_Simulate_Mouse_Click_At_Present_Pos(sType.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Simulate_Mouse_Click_At_Present_Pos(string sType,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #548 - Menu */
    /** Shows the main menu */
    /** @param #9 Text */
    /** A string indicating which menu should appear.  The parameter is only used for smart media devices */
    /** @param #41 StreamID */
    /** ID of stream to apply */

    virtual void CMD_Menu(string sText,int iStreamID) { string sCMD_Result; CMD_Menu(sText.c_str(),iStreamID,sCMD_Result,NULL);};
    virtual void CMD_Menu(string sText,int iStreamID,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #689 - Update Time Code */
    /** Update the time code of the current media */
    /** @param #41 StreamID */
    /** The Stream to update */
    /** @param #102 Time */
    /** The current time.  If there is both a section time and total time, they should be \t delimited, like 1:03\t60:30 */
    /** @param #132 Total */
    /** If there is both a section time and total time, they should be \t delimited, like 1:03\t60:30 */
    /** @param #133 Speed */
    /** The current speed */
    /** @param #134 Title */
    /** For DVD's, the title */
    /** @param #135 Section */
    /** For DVD's, the section */

    virtual void CMD_Update_Time_Code(int iStreamID,string sTime,string sTotal,string sSpeed,string sTitle,string sSection) { string sCMD_Result; CMD_Update_Time_Code(iStreamID,sTime.c_str(),sTotal.c_str(),sSpeed.c_str(),sTitle.c_str(),sSection.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Update_Time_Code(int iStreamID,string sTime,string sTotal,string sSpeed,string sTitle,string sSection,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #741 - Goto Screen */
    /** Goto a specific screen. */
    /** @param #10 ID */
    /** Assigns an optional ID to this particular "viewing" of the screen, used with Kill Screen.  There can be lots of instances of the same screen in the history queue (such as call in progress).  This allows a program to pop a particular one out of the queue. */
    /** @param #159 PK_Screen */
    /** The screen id. */
    /** @param #251 Interruption */
    /** Indicates at what times to ignore the screen change if it would interrupt the user.  A value in: enum eInterruption */
    /** @param #252 Turn On */
    /** If true, turn the display on if it's off. */
    /** @param #253 Queue */
    /** If true, then if the screen change was ignored so as not to interrpt the user, queue it for when the user is done */

    virtual void CMD_Goto_Screen(string sID,int iPK_Screen,int iInterruption,bool bTurn_On,bool bQueue) { string sCMD_Result; CMD_Goto_Screen(sID.c_str(),iPK_Screen,iInterruption,bTurn_On,bQueue,sCMD_Result,NULL);};
    virtual void CMD_Goto_Screen(string sID,int iPK_Screen,int iInterruption,bool bTurn_On,bool bQueue,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #795 - Set Mouse Behavior */
    /** Indicates if the mouse should be locked to horizontal or vertical movements, how to handle range of motion, etc. */
    /** @param #3 PK_DesignObj */
    /** An object to lock the movement to */
    /** @param #39 Options */
    /** The following letter(s): [r/a]ramp/absolute */
    /** @param #126 Exclusive */
    /** If true, all existing mouse behavior will be removed */
    /** @param #211 Direction */
    /** a letter: [h]orizontal, [v]ertical, [b]oth */

    virtual void CMD_Set_Mouse_Behavior(string sPK_DesignObj,string sOptions,bool bExclusive,string sDirection) { string sCMD_Result; CMD_Set_Mouse_Behavior(sPK_DesignObj.c_str(),sOptions.c_str(),bExclusive,sDirection.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Set_Mouse_Behavior(string sPK_DesignObj,string sOptions,bool bExclusive,string sDirection,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #801 - Set Mouse Sensitivity */
    /** 1=Sensitive, 3=not sensitive */
    /** @param #48 Value */
    /** 1=sensitive, 3=least sensitive */

    virtual void CMD_Set_Mouse_Sensitivity(int iValue) { string sCMD_Result; CMD_Set_Mouse_Sensitivity(iValue,sCMD_Result,NULL);};
    virtual void CMD_Set_Mouse_Sensitivity(int iValue,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #809 - Display Alert */
    /** Displays a short alert that appears on the orbiter as a discrete popup message that goes away automatically. */
    /** @param #9 Text */
    /** The text in the alert */
    /** @param #70 Tokens */
    /** File this alert with this token, and if another alert comes in before timeout with the same token, replace it. */
    /** @param #182 Timeout */
    /** Make the alert go away after this many seconds */
    /** @param #251 Interruption */
    /** How to interrupt the user if something is happening */

    virtual void CMD_Display_Alert(string sText,string sTokens,string sTimeout,int iInterruption) { string sCMD_Result; CMD_Display_Alert(sText.c_str(),sTokens.c_str(),sTimeout.c_str(),iInterruption,sCMD_Result,NULL);};
    virtual void CMD_Display_Alert(string sText,string sTokens,string sTimeout,int iInterruption,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #810 - Set Active Application */
    /** Tells an On screen orbiter what application is currently active */
    /** @param #50 Name */
    /** A description of the app */
    /** @param #159 PK_Screen */
    /** The Screen for the OSD */
    /** @param #216 Identifier */
    /** The window identifier */
    /** @param #226 PK_Screen_GoTo */
    /** The screen for the orbiter remote */

    virtual void CMD_Set_Active_Application(string sName,int iPK_Screen,string sIdentifier,int iPK_Screen_GoTo) { string sCMD_Result; CMD_Set_Active_Application(sName.c_str(),iPK_Screen,sIdentifier.c_str(),iPK_Screen_GoTo,sCMD_Result,NULL);};
    virtual void CMD_Set_Active_Application(string sName,int iPK_Screen,string sIdentifier,int iPK_Screen_GoTo,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #811 - Get Active Application */
    /** Return the currently active application */
    /** @param #50 Name */
    /** A description of the app */
    /** @param #159 PK_Screen */
    /** The Screen for the OSD */
    /** @param #216 Identifier */
    /** The window identifier */
    /** @param #226 PK_Screen_GoTo */
    /** The Screen for the orbiter remote */

    virtual void CMD_Get_Active_Application(string *sName,int *iPK_Screen,string *sIdentifier,int *iPK_Screen_GoTo) { string sCMD_Result; CMD_Get_Active_Application(sName,iPK_Screen,sIdentifier,iPK_Screen_GoTo,sCMD_Result,NULL);};
    virtual void CMD_Get_Active_Application(string *sName,int *iPK_Screen,string *sIdentifier,int *iPK_Screen_GoTo,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #834 - Execute Shortcut */
    /** Execute the shortcut associated with a key.  Called when a key is held down. */
    /** @param #48 Value */
    /** The ascii value of the key (ie 65='A').  Valid are 0-9,*,#,A-Z */

    virtual void CMD_Execute_Shortcut(int iValue) { string sCMD_Result; CMD_Execute_Shortcut(iValue,sCMD_Result,NULL);};
    virtual void CMD_Execute_Shortcut(int iValue,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #838 - Bind to Wireless Keyboard */
    /** If the USB RF dongle is attached it causes it to do a bind request and add a remote. */

    virtual void CMD_Bind_to_Wireless_Keyboard() { string sCMD_Result; CMD_Bind_to_Wireless_Keyboard(sCMD_Result,NULL);};
    virtual void CMD_Bind_to_Wireless_Keyboard(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #912 - Activate PC Desktop */
    /** Activate or de-activate the PC-desktop */
    /** @param #119 True/False */
    /** If true, switch to the last PC desktop.  If false, switch to Orbiter's desktop */

    virtual void CMD_Activate_PC_Desktop(bool bTrueFalse) { string sCMD_Result; CMD_Activate_PC_Desktop(bTrueFalse,sCMD_Result,NULL);};
    virtual void CMD_Activate_PC_Desktop(bool bTrueFalse,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #923 - Assisted Make Call */
    /** Send make call command back to Orbiter and let it decide if we are going to make a direct call or a transfer/conference */
    /** @param #17 PK_Users */
    /** The called user. Only one is supported now. */
    /** @param #83 PhoneExtension */
    /** The phone number to be called. */
    /** @param #184 PK_Device_From */
    /** The device which starts the call. */
    /** @param #263 PK_Device_To */
    /** The called device. */

    virtual void CMD_Assisted_Make_Call(int iPK_Users,string sPhoneExtension,string sPK_Device_From,int iPK_Device_To) { string sCMD_Result; CMD_Assisted_Make_Call(iPK_Users,sPhoneExtension.c_str(),sPK_Device_From.c_str(),iPK_Device_To,sCMD_Result,NULL);};
    virtual void CMD_Assisted_Make_Call(int iPK_Users,string sPhoneExtension,string sPK_Device_From,int iPK_Device_To,string &sCMD_Result,Message *pMessage);

    //<-dceag-h-e->


signals:

    void timecodeEvent(QString ea, QMap<long, std::string> mp);
    void timecodeEvent(int room, QMap<long, std::string> mp);

     void creationComplete(bool finished);

    void transmitDceCommand(PreformedCommand cmd);
    void transmitDceCommandResp(DCECommand *pCmd);

    void deviceTemplateChanged(int dt);

    void dceGuiCommand(int i);

    /*!
     * \brief showHelp. Signals the GUI to show shortcuts/help.
     */

    void showHelp();

    //navigation
    /*!
     * \brief gotoQml Load a specific qml page.
     * \param qml
     */
    void gotoQml(QString qml);

    /*!
     * \brief gotoScreen. Load the screen in question.
     * \param screen
     */

    /*!
     * \brief menuKey. Signal the GUI to show the menu.
     */    void menuKey();

    void gotoScreen(QString screen);
    /*!
     * \brief bookmarksReady. Signal containing available bookmarks for this item.
     */
    void bookmarksReady(QList<QObject*>);
    /*!
     * \brief moveLeft. Tells the orbiter to move left in its context.
     */
    void moveLeft();
    /*!
     * \brief moveRight. Tells the orbiter to move right in its context.
     */
    void moveRight();
    /*!
     * \brief moveUp. Tells the orbiter to move up in its context.
     */
    void moveUp();
    /*!
     * \brief moveDown. Tells the orbiter to move down in its context.
     */
    void moveDown();

    void enterKey();            /*!< Tell qorbiter to go or select */
    void goKey();               /*!< Tell qorbiter to go / select */
    void backKey();             /*!< Tell qorbiter to go back */
    void clearKey();            /*!< Tell qorbiter to clear the entry in the current box */
    void scrollDirection(int d); /*!< Tell qorbiter to scroll its grid up or down. 1 for up, 0 for down */

    //setup
    /*!
     * \brief addExistingOrbiter. Emitted when a new orbiter is found.
     * \param o
     */
    void addExistingOrbiter(ExistingOrbiter* o);
    /*!
     * \brief clearExistingOrbiters. tell the gui to clear the list because its being reloaded.
     */
    void clearExistingOrbiters();
    /*!
     * \brief startManager. Signals the GUI to start is relevant information.
     */
    void startManager(QString, QString);
    /*!
     * \brief deviceInvalid. Emitted when the device is invalid
     */
    void deviceInvalid();
    /*!
     * \brief promptResponse. Sends a signal to prompt the user with a prompt type and list of options.
     */
    void promptResponse(int, QList<PromptData*>*p);
    /*!
     * \brief routerInvalid. Invalid router, no DCE connection available.
     */
    void routerInvalid();
    /*!
     * \brief connectionValid. track connection to the router
     * \param v
     */
    void connectionValid(bool v);
    /*!
     * \brief routerReloading. Indicates the router is reloading.
     * \param msg
     */
    void routerReloading(QString msg);
    /*!
     * \brief replaceDevice. Emitted when a device with the same device number connects.
     */
    void replaceDevice();
    /*!
     * \brief closeOrbiter Signal telling the GUI to close.
     */
    void closeOrbiter();
    /*!
     * \brief statusMessage. Primarily used to send splash screen messages.
     * \param s
     */
    void statusMessage(QString s);      // special signal for use when setting up at the splash screen.
    /*!
     * \brief configReady. Signal that sends the configuration data for this GUI when it is ready.
     * \param config
     */
    void configReady(QByteArray config);
    /*!
     * \brief setMyIp. Sets the local ip for the orbiter internally.
     * \param ip
     */
    void setMyIp(QString ip);
    /*!
     * \brief newTCport. Emitted when the tcp port for connecting for timecode is changed.
     * \param port
     */
    void newTCport(int port);
    /*!
     * \brief mediaResponseChanged. Used to transfer messages about media to the GUI.
     * \param r
     */
    void mediaResponseChanged(QString r);
    /*!
     * \brief deviceValid. Signals the validity of the device to the GUI.
     * \param s
     */
    void  deviceValid(bool s);
    /*!
     * \brief deviceIdChanged. Emitted when the device ID changes.
     */
    void deviceIdChanged();
    /*!
     * \brief setEa. emitted when the GUI is to be set to a specific EntertainArea and room.
     * \param room
     * \param ea
     */
    void setEa(int room, int ea);
    /*!
     * \brief screenSaverTimerOutChanged. Emitted when the screensaver timeout changes.
     * \param t
     */
    void screenSaverTimerOutChanged(int t);
    /*!
     * \brief connectionLost. Emitted when the connection is lost to the router.
     */
    void connectionLost();
    /*!
     * \brief connectionStarted. Emitted when initally connected to the router.
     */
    void connectionStarted();

    /*!
     * \brief externalOn. Bring the GUI awake.
     *  \todo Make the gui more seperate from router code, allowing for complete shutdown of gui while connection is maintained.
     */
    void externalOn();

    /*!
     * \brief externalOff. Puts the gui to 'sleep' mode.
     */
    void externalOff();

    /** @name Logging Signals
     * Signal used for logging.
    */
    ///@{
    void newStatusMessage(QString s);
    void newDceMessage(QString d);
    void newMediaMessage(QString m);
    void newTimeCodeMessage(QString t);
    void newEntertainAreaMessage(QString e);
    ///@}

    /** @name DCE Socket Connections
     * Signals used for connection and disconnection of sockets
     */
    ///@{
    void routerReload();
    void routerDisconnect();
    void routerReplace();
    void routerConnectionChanged(bool s);
    void routerFound(bool found);
    ///@}

    /** @name Floorplan Signals
     * Signals relating to flooplan operations
     */
    ///@{
    void floorPlanImageData(const uchar* ,int);
    void floorplanTypeChanged(int f);
    void addFloorplanDeviceCommand(QVariantMap c);
    void floorplanDeviceStatus(QString status, QString state, int device);
    ///@}

    /** @name Media Signals
     * Signals relating to operations on playing media.
     */

    ///@{
    void screenShotReady(QImage data);
    void waitForScreenShot(char picData, int picDataSize, string fileFormat);
    void videoGrabReady(QImage grab);
    void modelPagesChanged(int p);

    void screenSaverImages(QStringList images);
    void objectUpdate(QByteArray b);
    void objectDataUpdate( char,int);
    void mediaMessage(QString msg);
    void gridModelSizeChange(int size);
    void checkGridStatus();
    void liveTvUpdate(QString id);
    void mythTvUpdate(QString id);
    void screenshotVariablesReady();
    void addScreenShotVar(screenshotAttributes* t);
    ///@}

    /** @name Datagrid-media
 * Signals relating to datagrid operations
 */

    ///@{
    void showFileListMediaType(int mediaType);
    void gridPauseChanged(bool g);
    void newGrid();
    void initializeSorting();
    void newPageSeperator(int seperator);
    void mediaPageChanged(int page);
    void recievingStatusChanged(bool s);
    void gridStatusChanged();
    void addItem(gridItem*);
    void requestMediaGrid(int);
    void clearPageGrid();
    void modelPageCount(int t);
    void clearModel();
    void pageSeperatorChanged(int s);
    void newAttributeSort(AttributeSortItem*);
    void newMediaSubtype(AttributeSortItem*);
    void newGenreSort(AttributeSortItem*);
    void newFileFormatSort(AttributeSortItem*);
    void attributeSortFinished();
    void mediaAuthChanged(int user); /*!< For invalid private media authorization. Anything besides -1 correlates to the user associated with that number being set.*/
    ///@}

    /**
     * @name Generic datagrids
     */
    ///@{
    void addDataGridItem(QString dataGridID, int PK_DataGrid, int indexStart, int numRows, DataGridTable* pTable);
    void updateItemData(QString dataGridId, int row, int role, QVariant value);
    void prepareDataGrid(QString dataGridID, QString dgName, int height, int width);
    void dgRequestFinished(QString dataGridID);
    ///@}

    /** @name Now Playing Signals
 * Signals for notifying of new media, getting meta data.
 */
    ///@{
    void setNowPlaying(bool status);
    void nowPlayingDeviceListChanged(QVariantList devices);
    void addScreenParam(QString, int);
    void currentScreenChanged(QString screen);
    void playlistPositionChanged(int position);
    void subtitleChanged(QString subtitle);
    void streamIdChanged(int stream_id);
    void mediaTypeChanged(int mediaType);
    void addChannel(EPGItemClass* chan);
    void updateTimeCode(QString server, int port);
    void stopTimeCode();

    //attributes. granular instead of grouped because i can. i figure the strongly interwoven orbiter got us in trouble the 1st time right? Right?!
    void np_pathChanged(QString f);
    void np_localpathChanged(QString p);
    void np_storageDeviceChanged(QString f);
    void np_title1Changed(QString t1);
    void np_title2Changed(QString t2);
    void np_mediaTitleChanged(QString m_title);
    void np_synopsisChanged(QString synop);
    void np_imagePath(QString img_path);
    void np_filename(QString filename);
    void np_program(QString prog);
    void np_channel(QString chan);
    void np_channelID(QString chanID);
    void np_episode(QString episode);
    void np_performer(QString perf);
    void np_director(QString director);
    void np_genre(QString genre);
    void np_album(QString album);
    void np_track(QString track);
    void np_releaseDate(QString release_date);
    void np_playlistIndexChanged(int index);
    void np_network(QString s);

    //storemediaplaylist
    void playlistItemAdded(PlaylistItemClass*);
    void playlistItemRemoved(int index);
    void playlistItemMoved(int index);

    void mediaPositionChanged(QString pos);
    void mediaLengthChanged(QString length);
    void tcUpdated(QString tc);
    void updateMediaSpeed(int speed);

    void resetNowPlaying();
    void setPlaylistPosition(int);
    void clearPlaylist();
    void playlistSize(int);
    void clearTVplaylist();
    void playlistDone();
    void epgDone();
    void livetvDone();
    void mediaGridDone();
    ///@}

    /** @name File Details
 * For the file details functions, pop up or not.
 */
    ///@{
    void clearFileDetails();
    void fd_fileChanged(QString f);
    void fd_fileNameChanged(QString f);
    void fd_show(bool s);
    void fd_objectTitleChanged(QString t);
    void fd_synopChanged(QString s);
    void fd_imageUrlChanged(QString loc);
    void fd_pathChanged(QString path);
    void fd_storageDeviceChanged(QString device);
    void fd_titleChanged(QString t);
    void fd_programChanged(QString p);
    void fd_mediaTitleChanged(QString s);
    void fd_genreChanged(QString g);
    void fd_studioChanged(QString s);
    void fd_channelChanged(QString chan);
    void fd_chanIdChanged(QString chanid);
    void fd_episodeChanged(QString ep);
    void fd_directorChanged(QString dir);
    void fd_albumChanged(QString al);
    void fd_trackChanged(QString track);
    void fd_ratingChanged(QString rating);
    void fd_performersChanged(QString t);
    void fd_composersChanged(QString c);
    void fd_aspectH(int h);
    void fd_aspectW(int w);
    void fd_titleImageChanged(QImage t);
    ///@}

    /** @name Device Controls
     * Signal relating to controlling a specfic device
     */

    ///@{
    void addDevice(AvDevice *d);
    void resendAvButtonList(const QList<QObject*> &t);
    void newDeviceCommand(AvCommand *f);
    void bookmarkList(QList<QObject*> b);
    void deviceAudioLevelChanged(int l);
    ///@}

    /** @name Security Cam Signals
     * Signal for security cameras
     */
    ///@{
    void securityImageReady(int c, QImage s);
    ///@}

    /** @name Screen Saver Related
     * This contains signals relevant to the screensaver
     */
    ///@{
    void currentScreenSaverImage(QByteArray b);
    ///@}

    /** @name OSD properties
     * Properties relating specifically to and onscreen orbiter
     */
    ///@{
    void discreteAudioChanged(bool discreteState);
    void liveAvPath(bool liveTvAvailible);
    void containsVideo(bool videoPresent);
    void isOsd(bool osdState);
    void monitorStatusChanged(bool monitorModeState);
    ///@}

    /** @name Misc Messaging Signals
     * Will be depreciated mode likely
     **/
    ///@{
    //messaging
    void commandResponseChanged(QString r);
    void eventResponseChanged(QString e);
    void deviceResponseChanged(QString d);
    void commandComplete(DCECommand *pCmd = NULL);
    void commandFailed(int id = 0);
    void setText(QString designObj, QString text, int iPK_Text);
    ///@}

    /** @name Media Playback
     * Signals from this level that are relevant to media playback
     */
    ///@{
    void videoFileUrlChanged(QString fileUrl);
    void audioFileUrlChanged(QString fileUrl);
    void streamingVideoIpChanged(QString newVideoIp);
    void streamingAudioIpChanged(QString audioIp);
    void mediaPlayerTimeCodeChanged();
    void stopPlayer();
    void pausePlayer();

    ///@}

    /** Sub Device ID signals
     * Signals used when child id's are recieved
     */
    ///@{
    void qMediaPlayerIDChanged(int i);
    void qCommunicatorIDChanged(int i);
    ///@}

    void dceIPChanged();
    void DCEHostChanged();

public slots:

    void getHouseState(); /*! Used to update user status, weather, housemode, voicemails */

    void setGridPause(bool g){ gridPaused =g; emit gridPauseChanged(gridPaused); if(!gridPaused){populateAdditionalMedia();} qDebug() << "Grid is paused ::" << gridPaused; }
    bool getGridPause(){ return gridPaused;}

    /*!
     * \brief authorizePrivateMedia
     * \param mediaType
     * \param pin
     * \param user
     */
    void authorizePrivateMedia(int mediaType, QString pin, int user);


    /*!
     * \brief reInitialize - Restarts the orbiter without doing a full reload.
     */
    void reInitialize();

    /*!
     * \brief setLocalIp Set the local ip address for this device.
     * \param l :QString Local IP address
     */
    void setLocalIp(QString l) { m_localIp = l; }

    /*!
     * \brief initDceVars
     * \return
     */
    bool initDceVars();

    /*Special*/
    void setVariable(int pkvar, QString val);
    void checkRouterConnection();
    void cancelAllRequests(){
        b_cancelRequest = true;
        emit clearModel();
        qDebug() << "Requests cancelled!";
    }

    void setModelPages(int p) {modelPages = p; emit modelPagesChanged(modelPages);}
    int getModelPages() {return modelPages;}

    void osdBack();

    void executeMessageSend(QVariantMap outGoing);

    void getVolume();

    void setAlarm(QString dataGridId, int row, int role, bool toggle, int grp);

    void beginSetup();

    void updateScreenSaverTimeout(int t) ;

    void setPlaylistSize(int s){ mediaPlaylistSize = s; emit playlistSize(mediaPlaylistSize);}
    int getMediaPlaylistSize() {return mediaPlaylistSize;}

    void setdceIP(QString ip) {dceIP = ip; m_sIPAddress = ip.toStdString(); m_sHostName = m_sIPAddress; qDebug() << "DCE got IP"; emit dceIPChanged();}
    QString getdceIP() {return dceIP;}

    void setDCEHost(QString host) { DCEHost = host; emit DCEHostChanged();}
    QString getDCEHost() {return DCEHost;}

    void setqMediaPlayerID(int deviceID) {qMediaPlayerID = deviceID; emit qMediaPlayerIDChanged(qMediaPlayerID); }
    int getqMediaPlayerID(){return qMediaPlayerID;}

    void setqCommunicatorID(int deviceID ) {qMediaPlayerID = deviceID; emit qCommunicatorIDChanged(qCommunicatorID);}
    int getqCommunicatorID(){return qCommunicatorID;}

    void handleDceCommand(DCE::PreformedCommand cmd);
    void handleDceCommandResp(DCECommand *pCmd);

    void sendDCECommand(PreformedCommand cmd);
    void sendDCECommandResp(DCECommand *pCmd);

    void getAttributeImage(QString param);


    void showUiMenu();

    void setDeviceId(int d) {m_dwPK_Device = d; emit deviceIdChanged();}
    int getDeviceId() {return m_dwPK_Device;}

    void populateSetupInformation();

    void setOsd(bool osd) { m_bIsOSD = osd; emit isOsd(m_bIsOSD);}
    bool getOsd() { return m_bIsOSD;}


    //setup
    void executeCommandGroup(int cmdGrp);
    void shutdownMD();
    bool initialize();
    void requestConfigData();
    void processConfigData(QNetworkReply * r);
    void deinitialize();
    bool getConfiguration();
    void registerDevice(int user, QString ea, int room);
    void qmlSetup(int device, QString ip);
    void setCurrentScreen(QString s);
    void setOrbiterSetupVars(int users, int room, int skin, int lang, int height, int width, QString name);
    void finishSetup();
    void setupEa();
    void getMountDevices();


    void pingCore();
    void checkPing(QHostInfo info);

    void checkInstall();
    void verifyInstall(QNetworkReply*r);

    void moveDirection(int d);
    void JogStream(QString jump);
    void saveScreenAttribute(QString attribute);
    void cleanupScreenshotData() {screenieData=NULL; screenieData =0; }
    void showAdvancedButtons();

    //playlist manipulation
    void removePlaylistItem(int index);
    void saveCurrentPlaylist( QString name, bool SaveAsNew);
    void movePlaylistEntry(QString pos, int index);
    void addToPlaylist(bool now, string playlist);
    //-------------------------------------------------------
    void grabScreenshot(QString fileWithPath);




    //buttons from the more screen
    void extraButtons(QString button);
    void newOrbiter();

    //floorplans
    void getFloorPlanImage(QString fp_path);
    void getFloorplanDeviceStatus(int device);

    //screensave
    void getScreenSaverImage(QString inc_requested_img_path);

    //connections
    void connectionError();

    //media grid

    void requestAttributeTypes(int type);   //program, performer, etc
    void requestMediaSubtypes(int type);    //movies, music videos, etc
    void requestTypes(int type);            //should be fileFormats
    void requestGenres(int type);           //genres
    void requestFileFormats(int type);

    void setGridSeperator(int sep);
    int getGridSeperator() { return media_pageSeperator; emit newPageSeperator(media_pageSeperator);}

    void setCurrentPage(int page) {media_currentPage = page; emit mediaMessage("-Media- Page no: "+QString::number(media_currentPage));  emit mediaPageChanged(media_currentPage);  }
    int getCurrentPage() {return media_currentPage;}

    bool checkLoadingStatus();

    void requestLiveTvPlaylist();
    void prepareFileList(QString filterString);
    void cleanupGrid();
    void populateAdditionalMedia();
    void setRecievingStatus(bool b);
    bool getRecievingStatus();
    void setGridStatus(bool b);
    bool getGridStatus();
    void setCurrentRow(int row);
    int getCurrentRow();
    void initializeGrid();
    void setStringParam(int paramType, QString param);
    void requestPage(int page);
    void setMediaType(int t) { i_current_mediaType = t; emit mediaTypeChanged( i_current_mediaType);}
    int getMediaType () {return i_current_mediaType;}

    void jumpMobileGrid(int page);
    void getGridView(bool direction);
    void seekToGridPosition(QString s);

    void loadDataGrid(QString dataGridID, int PK_DataGrid, QString option);
    void loadDataForDataGrid(QString dataGridId, QString dgName, int PK_DataGrid, QString option, int startRow, int numRows, int numColumns, QString seek);

    //media
    void getContextImage(int attributeNumber);
    void displayToggle(bool t);
    void setMediaSpeed(int s);
    QImage getfileForDG(string filename);
    void GetFileInfoForQml(QString qs_file_reference);
    void GetMediaAttributeGrid(QString qs_fk_fileno);
    void GetSecurityCam(int i_inc_pkdevice, bool showScreen);

    void stop_AV();

    void moveMedia(QString eas, int streamID);

    void checkTimeCode(int npDevice);
    void getStreamingVideo();

    void changedPlaylistPosition(QString pos);
    void ShowFloorPlan(int floorplantype);
    void updateFloorPlan(QString p);
    void GetScreenSaverImages();
    void BindMediaRemote(bool onoff);
    void jumpToPlaylistPosition(QString pos);

    void setNowPlayingDetails();
    void SetSecurityStatus(string pin, string mode, int user, string special);
    void GetSingleSecurityCam(int cam_device, int iHeight, int iWidth, bool showScreen);
    void GetMultipleSecurityCams(QStringList cams);
    void GetNowPlayingAttributes();
    void TuneToChannel(QString channel, QString chanid);
    void adjustLighting(int level, myMap devices);
    void adjustRoomLights(QString level);


    void changedTrack(QString direction);
    void SetSecurityMode(QString pass, int mode);
    void setLocation(int location, int ea);
    void setUser(int user);
    void quickReload();
    void powerOn(QString devicetype);
    void powerOff(QString deviceType);

    void GetAdvancedMediaOptions(int device);
    void setZoom(QString zoomLevel);
    void setAspect(QString ratio);
    void GetText(int textno);
    void setPosition(QString position);
    void setPosition(int position);
    void showMenu();
    void CopyDisc();
    void ShowBookMarks();


    void sendAvCommand(int deviceto, int command);
    void sendStringMessage(QString msg);
    //floorplans
    void getFloorplanDeviceCommand(int device);

    void shutdown();
    void changeAndRestart(QString ip){
        if(!ip.isEmpty() && ip!=dceIP ) {
            qDebug() << "Switching ip address "<<ip;
            dceIP=ip; m_sIPAddress = ip.toStdString();
            m_sHostName = ip.toStdString();
            reInitialize();
        }
    }

    //child devices

    bool timeCodeInterceptor(class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo);

private:


protected:
    /*! @name logging slots*/
    //@{
    void logStatusMessage(QString statusM) {emit newStatusMessage(statusM); }
    void logDceMessage(QString dceM){ emit newDceMessage(dceM); }
    void logMediaMessage(QString mediaM){emit newMediaMessage(mediaM);}
    void logTimeCodeMessage(QString tcM){emit newTimeCodeMessage(tcM);}
    void logEntertainAreaMessage(QString eaM){emit newEntertainAreaMessage(eaM); }
    //@}
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
