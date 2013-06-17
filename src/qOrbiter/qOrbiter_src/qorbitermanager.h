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

#ifndef QORBITERMANAGER_H
#define QORBITERMANAGER_H

/*---qt includes----*/
#include <QtGlobal>
#if (QT5)

#include <QtQuick/QQuickView>
#include <QtQuick/QtQuick>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMainWindow>
#include <QtQuick/QQuickItem>

#else
#include <QtGui/QWidget>
#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/QDeclarativeItem>
#include <QMainWindow>
#endif

#include<datamodels/existingorbitermodel.h>
#include<datamodels/attributeobject.h>
#include <datamodels/sleepingalarmmodel.h>

#include <QStringList>
#include <QThread>
#include <QFile>
#include <QDir>
#include <QDataStream>

#include <QProcess>
#include <QtXml/QDomDocument>

#ifdef ANDROID
#include "plugins/AndroidInfo/androidsystem.h"
#endif

#if GLENABLED
#include <shaders/filereader.h>
#include <shaders/trace.h>
#endif
//for pi - for_pi now sets glenabled
//#include <shaders/filereader.h>
//#include <shaders/trace.h>


/*-------Custom Classes -----------------*/

#include <datamodels/usermodel.h>
#include <datamodels/locationmodel.h>
#include <datamodels/lightingscenariomodel.h>
#include <datamodels/mediascenariomodel.h>
#include <datamodels/climatescenariomodel.h>
#include <datamodels/telecomscenariomodel.h>
#include <datamodels/securityscenariomodel.h>
#include <datamodels/floorplanmodel.h>

#include <contextobjects/screensaverclass.h>


#include <datamodels/attributesortmodel.h>


#include <contextobjects/filedetailsclass.h>
#include <contextobjects/nowplayingclass.h>
#include <contextobjects/screenparamsclass.h>
#include <contextobjects/playlistclass.h>
#include <contextobjects/securityvideoclass.h>
#include <datamodels/devicemodel.h>
#include <datamodels/avcodegrid.h>

#include <datamodels/skindatamodel.h>
#include <contextobjects/floorplandevice.h>
#include <contextobjects/screenshotattributes.h>
#include <contextobjects/existingorbiter.h>


//own version of OrbiterData.h
#include <datamodels/listModel.h>                            //custom item model
#include <datamodels/gridItem.h>
#include <contextobjects/epgchannellist.h>
#include <imageProviders/basicImageProvider.h>                 //qml image provider
#include <imageProviders/gridimageprovider.h>                  //qml image provider for grids !not implemented!
#include <screensaver/screensavermodule.h>
#include <datamodels/DataModelItems/sleepingalarm.h>

/*-------Dce Includes----*/
#include <qOrbiter/qOrbiter.h>

/*---------------Threaded classes-----------*/


#include <qOrbiter/qOrbiter.h>
class EPGChannelList;
class basicImageProvider;

class ListModel;
class GridIndexProvider;
class LightingScenarioModel;

class MediaScenarioModel;
class ClimateScenarioModel;
class TelecomScenarioModel;
class SecurityScenarioModel;
class AbstractImageProvider;
class SkinDataModel;
class FloorPlanModel;


namespace DCE
{
class qOrbiter;
}

/*!
 *\class qorbiterManager
 *\brief The main object managing the flow of information between the QML interface and the DCE library
 *
 *\ingroup qt_controllers
 *
 *QOrbiterManager is responsible for managing the Qt side of the Qt/DCE connection. In addition, it supports all of the related
 *functions of the user interface not related to DCE.
 *
 *\sa QObject
 *
 */
class qorbiterManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY (int m_dwPK_Device READ getDeviceNumber WRITE setDeviceNumber NOTIFY deviceNumberChanged)/*! \brief Contains the current device number \ingroup qorbiter_properties */
    Q_PROPERTY (int deviceVolume READ getDeviceVolume WRITE setDeviceVolume NOTIFY deviceVolumeChanged)
    Q_PROPERTY (int appHeight READ getAppH WRITE setAppH NOTIFY orientationChanged)/*!< \brief Contains the application height \ingroup qorbiter_properties */
    Q_PROPERTY (int appWidth READ getAppW WRITE setAppW NOTIFY orientationChanged)/*!< \brief Contains the application width \ingroup qorbiter_properties */

    Q_PROPERTY(int i_current_mediaType READ getMediaType WRITE setMediaType NOTIFY mediaTypeChanged)/*!< \brief Contains the current media type of the playing media. \ingroup qorbiter_properties */
    Q_PROPERTY (QString q_mediaType READ getSorting WRITE setGridMediaType NOTIFY mediaGridTypeChanged) /*!< \brief Contains the current media type of the media grid being browsed \ingroup qorbiter_properties */
    Q_PROPERTY (QString q_attributetype_sort READ getTypeSort WRITE setTypeSort NOTIFY typeSortChanged) /*!< \brief attribute type sorting */
    Q_PROPERTY (QString q_subType READ getSubType WRITE setSubType NOTIFY subTypeChanged)
    Q_PROPERTY (QString sPK_User READ getCurrentUser WRITE setCurrentUser NOTIFY userChanged)/*!< \brief Contains string of the current user \ingroup qorbiter_properties */
    Q_PROPERTY (QString currentRoom READ getCurrentRoom WRITE setCurrentRoom NOTIFY roomChanged)/*!< \brief Contains the current EA or room with the EA  \ingroup qorbiter_properties */
    Q_PROPERTY (QString dceResponse READ getDceResponse WRITE setDceResponse NOTIFY dceResponseChanged)
    Q_PROPERTY (bool debugMode READ getDebugMode WRITE setDebugMode NOTIFY debugModeChanged )/*!< \brief Contains boolean of the current debug state. \ingroup qorbiter_properties */
    Q_PROPERTY (bool connectedState READ getConnectedState WRITE setConnectedState NOTIFY connectedStateChanged)
    Q_PROPERTY (bool b_orientation READ getOrientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY (QString currentScreen READ getCurrentScreen WRITE setCurrentScreen  NOTIFY screenChange)/*!< \brief Contains string of current screen. \code manager.currentScreen \endcode  \ingroup qorbiter_properties */
    Q_PROPERTY (QString m_ipAddress READ getInternalIp WRITE setInternalIp NOTIFY internalIpChanged)/*!< \brief Contains string of current ip address \code manager.m_ipAddress \endcode  \ingroup qorbiter_properties  */
    Q_PROPERTY (QString internalHost READ getInternalHost WRITE setInternalHost NOTIFY internalHostChanged)
    Q_PROPERTY (QString externalip READ getExternalIp WRITE setExternalIp NOTIFY externalIpChanged)
    Q_PROPERTY (QString externalHost READ getExternalHost WRITE setExternalHost NOTIFY externalHostChanged )
    Q_PROPERTY (int media_pageSeperator READ getGridSeperator WRITE setGridSeperator NOTIFY newPageSeperator )/*!< \brief Contains the number of cells returned in media grid \code manager.media_pageSeperator \endcode  \ingroup qorbiter_properties */
    Q_PROPERTY (int media_currentPage READ getCurrentPage WRITE setCurrentPage NOTIFY mediaPageChanged) /*!< \brief Contains the current page of the media grid is on. Starts at 0. \ingroup qorbiter_properties */
    Q_PROPERTY(bool osdStatus READ getDisplayStatus WRITE toggleDisplay NOTIFY osdChanged )
    Q_PROPERTY(int mediaPlayerID READ getMediaPlayerID WRITE setMediaPlayerID NOTIFY mediaPlayerIdChanged)
    Q_PROPERTY (bool discreteAudio READ getDiscreteAudio WRITE setDiscreteAudio NOTIFY discreteAudioChanged )
    Q_PROPERTY (bool m_bContainsVideo READ getContainsVideo WRITE setContainsVideo NOTIFY containsVideo)
    Q_PROPERTY (bool usingLiveAvPath READ getLiveAvPath WRITE setLiveAvPath NOTIFY liveAvPath)
    Q_PROPERTY (bool m_bIsOSD READ getOsd WRITE setOsd NOTIFY isOsd)
    Q_PROPERTY (bool monitorAvailible READ getMonitorStatus WRITE setMonitorStatus NOTIFY monitorStatusChanged)
    Q_PROPERTY(int deviceVolume READ getDeviceVolume WRITE setDeviceVolume NOTIFY deviceVolumeChanged)

    Q_PROPERTY (QString mediaResponse READ getMediaResponse WRITE setMediaResponse NOTIFY mediaResponseChanged)
    Q_PROPERTY (QString commandResponse READ getCommandResponse WRITE setCommandResponse NOTIFY commandResponseChanged)//for use in displaying command related dce replies.
    Q_PROPERTY (QString eventResponse READ getEventResponse WRITE setEventResponse NOTIFY eventResponseChanged) // for use in displaying message associated with incoming events
    Q_PROPERTY (QString deviceResponse READ getDeviceResponse WRITE setDeviceResponse NOTIFY deviceResponseChanged) // for use in display of messages associated with specific devices
    Q_PROPERTY (QString imagePath READ getImagePath WRITE setImagePath NOTIFY imagePathChanged)

    /*!
     * \warning enablescreensavermode - currently unused, should be built anyways
     * \warning networkload - currently is used for android devices
     */


public:
#if QT5 && !ANDROID
    qorbiterManager(QQuickView * view, QObject *parent=0);  //constructor
#elif ANDROID && QT5
    qorbiterManager(QQuickView *view, AndroidSystem *jniHelper,  QObject *parent=0);
#elif ANDROID
    qorbiterManager(QDeclarativeView *view, AndroidSystem *jniHelper,  QObject *parent =0);
#elif   QT4_8
    qorbiterManager(QDeclarativeView * view, QObject *parent=0);  //constructor

#endif
    //for Pi - for_pi now sets GLENABLED
    //FileReader * fileReader;

    bool b_glEnabled;
    int isPhone;
    //settings
    QString sPK_User;
    QString buildType;
    QByteArray binaryConfig;
    long iPK_Device;
    QString qs_ext_routerip;
    /*messaging*/
    QString commandResponse;
    QString eventResponse;
    QString deviceResponse;
    QString mediaResponse;

    /*state related*/
    bool discreteAudio;
    bool m_bContainsVideo;
    bool usingLiveAvPath;
    bool m_bIsOSD;
    bool monitorAvailible;

    //------------------------------------------------------playlist classes


    //------CUSTOM QML TYPES------------------------------------------------------------------------------------
    ScreenParamsClass *ScreenParameters;
    SecurityVideoClass *SecurityVideo;
    QList<QObject*> screenshotVars;

    //-------------sleeping menu----------------------
    SleepingAlarmModel * sleeping_alarms;

    //------------media vars-----------------------------------
    FileDetailsClass *filedetailsclass;
    NowPlayingClass *nowPlayingButton;
    QString aspect; //-- true poster || false landscape
    QString *gridReqType;


    //skins
    SkinDataModel* tskinModel;
    QList<QObject*> skins_list;
    ExistingOrbiterModel *myOrbiters;
    QString qrcPath;
    //TODO, remove the below in favour of the data structure
    QMap <QString*, QString*> availibleSkins;
    QString qmlPath;
    QString localDir;
    QString skinsPath;
    QDir skinsDir;
    QString m_SkinsDirectoryPath;
    QVariantList storageDevices;


    //ui
    QString currentSkin;
    QString currentSkinURL;
    QString remoteDirectoryPath;
    SkinDataItem* skin;

#if QT5 || defined(QANDROID)
    QQuickView *qorbiterUIwin;
#else
    QDeclarativeView *qorbiterUIwin;    //Qml declarativeview
#endif


#if QANDROID || defined(ANDROID)
    AndroidSystem *androidHelper;
#endif


    ScreenSaverClass *ScreenSaver;

    //media
    bool mediaPlaying;
    QImage updatedObjectImage; //used for the current image for a given media item on screen
    QImage mediaScreenShot;    //used for screen shots

    bool b_orientation;
    int appHeight;
    int appWidth;
    bool b_localLoading;
    bool b_skinReady;
    bool b_skinDataReady;
    bool b_orbiterReady;
    bool osdStatus;
    bool dvdMenuShowing;
    Q_INVOKABLE void refreshUI(QUrl url);
    void swapSkins(QString incSkin);


    //device state
    bool cleanupData();
    QString dceResponse;
    bool connectedState;
    int i_current_command_grp;
    int i_current_mediaType;

    //----ANDROID----///
    QString droidPath;  //specific path for android files that relates to the package name 'com.linuxmce.qOrbiter/files
    //-END---ANDROID----///

    //image providers
    basicImageProvider *basicProvider;
    GridIndexProvider *advancedProvider;

    //floorplans
    FloorPlanModel *floorplans;

    /*
datagrid variables
these correlate to the variable string sent to the datagrid.
param #'s                                               1 2 3 4  5  6  7 8 9 10
the sorting parameters are always sent as a string like 5| | | |1,2| |13| |2|
where
MediaBrowser parms: ,  ,   , 13, 0,  2, pk_attribute
Param 1 - media type -cannot be blank
Param 2 - submediatype can be blank for video, audio, images
Param 3 - fileformat -??
Param 4 - attribute_genres - can be blank for video, audio, pictures??
Param 5 - sources 1,2, -cannot be blank!
Param 6 - users_private - ??
Param 7 - attributetype_sort
Param 8 - ?? users
Param 9 - ??last_viewed
Param 10 - pk_attribute
      */
    QString q_mediaType;/*! \brief The media type of the grid being navigated */           //1
    QString q_subType;             //2
    QString q_fileFormat;          //3
    QString q_attribute_genres;    //4
    QString q_mediaSources;         //5
    QString q_usersPrivate;        //6
    QString q_attributetype_sort;  //7
    QString q_pk_users;             //8
    QString q_last_viewed;        //9
    QString q_pk_attribute;        //10
    QString *datagridVariableString;
    QString videoDefaultSort;
    QString audioDefaultSort;
    QString photoDefaultSort;
    QString gamesDefaultSort;


    QStringList goBack;
    QString qs_seek;
    bool backwards;
    bool requestMore;

    int media_currentPage;
    int modelPages;
    int media_pageSeperator;

    QString mobileStorageLocation;

    //listmodels
    AvCodeGrid *deviceCommands;
    LocationModel *m_lRooms;
    ListModel *model;      //media grid model
    UserModel *userList;
    DeviceModel *devices;
    SkinDataModel *skinModel;
    QList<QObject*> buttonList;
    QList<QObject*> commandList;
    QList<QObject*>pages;
    QStringList *skinNames;
    QList<QObject*> current_floorplan_devices;
    QList<QObject*> current_bookmarks;
    QList<QObject*> existingOrbiters;

    QStringList screensaverImages;

    QStringList *gotoScreenList;

    AttributeSortModel *mediaTypeFilter;
    AttributeSortModel *uiFileFilter;
    AttributeSortModel *attribFilter;
    AttributeSortModel *genreFilter;

    LightingScenarioModel *roomLights;
    MediaScenarioModel *roomMedia;
    ClimateScenarioModel *roomClimate;
    TelecomScenarioModel *roomTelecom;
    SecurityScenarioModel *roomSecurity;
    QMap <QString, int> mp_rooms;
    QMap <int, LightingScenarioModel*> roomLightingScenarios;
    QMap <int, MediaScenarioModel*> roomMediaScenarios;
    QMap <int, ClimateScenarioModel*> roomClimateScenarios;
    QMap <int, TelecomScenarioModel*> roomTelecomScenarios;
    QMap <int, SecurityScenarioModel*> roomSecurityScenarios;
    QMap <int, int> *defaultSort;

    //ui functions
    Q_INVOKABLE QDateTime getCurrentDateTime() const { return QDateTime::currentDateTimeUtc();}
    QString currentRoom;

    //class objects

    //QT Functions to initialize lmce data
    bool initialize(int dev_id);
    bool initializeManager(string router_address, int device_id);     //init's dce object
    //getConf() is the part of the equation that should read the orbiter conf. not implemented fully


    //DCE variables
    bool m_bStartingUp;
    std::string m_sLocalDirectory;
    int m_pOrbiterCat;
    char *pData;                   //config size, pointer to pointer
    int iSize;                     //size of pData aka the config
    bool bAppError;                 //error flagging
    bool bReload;                   //reload flag
    bool needRegen;                 //regen flag

    std::string s_RouterIP;               // string of the router ip
    QString m_ipAddress;
    QString qs_routerip;
    QString internalHost;
    QString externalip;
    QString externalHost;

    bool debugMode;
    bool dceBool;                   //
    bool bLocalMode;                //local running flag, for running without router.

    string sEntertainArea;          //current entertain area int
    int iPK_User;                    //current user
    int iFK_Room;                    //current room
    int iea_area;
    QString sPK_Room;
    QString s_onOFF;

    QStringList *sRoomList;          //Linked list of rooms in house
    QStringList *sUserList;          //linked list of users in house
    QStringList *sCurr_Room_Devices; //linked list of current devices (experimental)
    QString currentScreen;

    //plugin variables
    long iOrbiterPluginID;           //the orbiter plugin id for future use
    long iPK_Device_DatagridPlugIn;
    long iPK_Device_OrbiterPlugin;
    long iPK_Device_GeneralInfoPlugin;
    long iPK_Device_SecurityPlugin;
    long iPK_Device_LightingPlugin;
    long iPK_Device_eventPlugin;
    long iMediaPluginID;
    int m_pDevice_ScreenSaver;
    int m_dwIDataGridRequestCounter;
    int i_currentFloorplanType;
    int deviceVolume;
    QString applicationPath;
    QString imagePath;

    /*Child Device IDs*/
    int mediaPlayerID;
    int communicatorID;

signals:
    void skinMessage(QString s);
    void qtMessage(QString s);

    /*child devices */
    void mediaPlayerConnected(bool connect);
    void mediaPlayerIdChanged();
    void communicatorIdChanged();
    void communicatorConnected(bool connect);


    void newMessageSend(QString message);



    /* Media Playback Controls */
    void muteSound();
    void setVolume(int vol);
    void extraButton(QString b);
    void changeTrack(QString track);
    void pause();
    void newChannel(QString channel);
    void newGridChannel(QString channel, QString chanid);
    void setStreamSpeed(int speed);
    void stopPlayback();
    void seekGrid(QString s);
    void newPlaylistPosition(int pos);
    void bindMediaRemote(bool b);
    void startPlayback(QString file);
    void play();
    void zoomLevelChanged(QString zoom);
    void aspectRatioChanged(QString ratio);
    void mobileStorageChanged(QString location);
    void moveArrowDirection(int d);
    void signalGoBack();


    void redButton();
    void blueButton();
    void greenButton();
    void yellowButton();

    /*Dvd Specific*/
    void show_dvdMenu();

    /*Metadata signals*/
    void objectUpdated();
    void setMediaDetails();
    void mediaScreenShotReady();
    void saveMediaScreenShot(QString attribute);
    void requestStreamImage();
    void requestStoredMediaImage(QString i);
    void requestVideoFrame();
    void liveTVrequest();
    void managerPlaylistRequest();
    void movePlistEntry(QString , int);
    void removePlistEntry(int);
    void savePlist(QString, bool);


    /*Datagrid Signals*/
    void typeSortChanged();
    void subTypeChanged();
    void resetSearchParams();
    void gridStatus(bool s);
    void gridGoBack();
    void mediaTypeChanged();
    void requestDcePages(int i);
    void filterChanged();
    void resetFilter();
    void modelChanged();
    void mediaGridTypeChanged();
    void gridTypeChanged(int i);
    void mediaRequest(int);
    void mediaSeperatorChanged(int sep);
    void mediaPageChanged();
    void newPageSeperator(int t);
    void requestMoreGridData();
    void clearModel();
    void cancelRequests(); /*!< \brief signal used to cancel thread processing activity in the dce thread */
    void clearAndContinue(int t); /*! \brief signal used to tell the the listmodel to clear itself and request new data of the mediatype t */
    void showList();
    void setDceGridParam(int a, QString p );
    void keepLoading(bool s);
    void stillLoading(bool b);
    void requestSubtypes(int subtype);

    /*Message and notification signals*/
    void dceResponseChanged();
    void imageAspectChanged();
    void connectedStateChanged();
    void screenSaverImagesReady();

    /*Settings Signals*/
    void debugModeChanged();
    void userChanged(int user);
    void locationChanged(int cRoom, int cEA);
    void screenChange(QString s);
    void localConfigReady(bool b);
    void orbiterConfigReady(bool b);
    void deviceValid(bool b);
    void connectionValid(bool b);
    void skinIndexReady(bool b);
    void skinDataLoaded(bool b);
    void roomChanged();
    void floorplanTypeChanged(int t);
    void internalIpChanged(QString ip);
    void externalIpChanged();
    void internalHostChanged();
    void externalHostChanged();
    void deviceNumberChanged(int d);
    void applicationPathChanged();
    void imagePathChanged();
    void pingTheRouter();

    /*Media Device Control Signals*/
    void resendAvCodes();
    void populateDeviceCommands(int);
    void resendDeviceCode(int from, int to);
    void updateScreen(QString screen);
    void deviceVolumeChanged();
    void seekPositionChanged(QString p) ;
    void jogToPosition(QString j);

    /*device related*/
    void osdChanged(bool);
    void newLightLevel(QString l);
    void populate_floorplan_device_commands(int d);

    //setup related
    void orbiterReady(bool);
    void connectionsReady();
    void orbiterDataReady();
    void orbiterClosing();
    void scenariosReady();
    void roomsReady();
    void engineReady();
    void error(QString msg);
    void orientationChanged();
    void appPath(QString ap);
    void registerOrbiter(int user, QString ea, int room);
    void unregisterOrbiter(int user, QString ea, int room);
    void loadingMessage(QString msg);
    void splashReady();
    void raiseSplash();
    void reInitialize();
    void showSetup();
    void continueSetup();
    void gotMountDevices();

    /*DCE Signals*/
    void reloadRouter();
    void executeCMD(int);
    void setAlarm(bool s, int g);
    void getAlarms();
    void getSingleCam(int i_pk_camera_device, int h, int w);
    void dceGridSepChanged(int d);
    void commandCompleted();

    //runtime
    void commandResponseChanged();
    void eventResponseChanged();
    void deviceResponseChanged();
    void mediaResponseChanged();

    void discreteAudioChanged();
    void liveAvPath( );
    void containsVideo( );
    void isOsd( );
    void monitorStatusChanged( );


public slots:

    void setSubType(QString t) {q_subType = t; emit subTypeChanged();}
    QString getSubType(){return q_subType;}

    void setTypeSort(QString t) {q_attributetype_sort = t; emit typeSortChanged();}
    QString getTypeSort(){return q_attributetype_sort;}

    void setApplicationPath(QString p){applicationPath = p; emit applicationPathChanged();}
    QString getApplicationPath(){return applicationPath;}

    void setImagePath(QString i) {imagePath = i; emit imagePathChanged();}
    QString getImagePath(){return imagePath;}

    /*!
    * \brief setFormFactor
    * \param f
    * \ingroup qorbiter_properties
    */
    void setFormFactor(int f) {isPhone = f;}

    void sendDceMessage(QString m) {emit newMessageSend(m);}

    /*!
     * \brief Requests the media subtypes for given subtype
     * \param subtype : int value corresponding to media subtypes
     * -1 Tv Shows
     * -2 Movies
     * -3 Home Videos
     * -4 Sports Events
     * -5 music videos
     * -6 Alternative - whatever that is
     * -7 Popular music
     * -8 Classical music - clearly not popular *boom!*
     * -9 Learning
     * -10 Audio Books
     * -11 Arcade
     * -12 Console
     *
     */
    void requestMediaSubtypes(int subtype){emit requestSubtypes(subtype);}
    void setNewSubtype(){}

    /*!
     * \brief This requests the availible attribute types.
     */
    void requestAttributeTypes(){}


    QString translateAttribute(QString a){
        return a;
    }

    /*!
     * \brief requestGenres
     */
    void requestGenres(){}

    /*!
     * \brief setupMobileStorage Sets up the mobile storage location for Android devices.
     * Returns true if sucessfull, false if not.
     * @param externalStorage : This is set by the AndroidSytem code that gets this from the JNI interface.
     * \return
     */
    bool setupMobileStorage(QString externalStorage);

    /*!
     * \brief setMobileStorage
     * \param s
     */
    void setMobileStorage(QString s){mobileStorageLocation = s; emit mobileStorageChanged(mobileStorageLocation);}

    /*!
     * \brief getMobileStorage
     * \return
     */
    QString getMobileStorage(){return mobileStorageLocation;}

    /*! @name child devices */
    //@{
    /*!
     * \brief setCommunicatorID
     * \param i
     */
    void setCommunicatorID(int i){communicatorID = i;emit communicatorIdChanged(); }

    /*!
     * \brief getCommunicatorID
     * \return
     */
    int getCommunicatorID() {return communicatorID;}

    /*!
     * \brief setMediaPlayerID
     * \param i
     */
    void setMediaPlayerID(int i) { mediaPlayerID = i; emit mediaPlayerIdChanged(); qDebug() << "New Media Player ID" << mediaPlayerID;}

    /*!
     * \brief getMediaPlayerID
     * \return
     */
    int getMediaPlayerID(){return mediaPlayerID;}
    //@}

    /*Splash screen related slots*/
    void showExistingOrbiter(const QList<QObject *> l )  ;
    void connectionWatchdog();
    void showUI(bool b);
    int loadSplash();
    void startOrbiter();
    bool createAndroidConfig();
    void processConfig(QNetworkReply* config);
    void getConfiguration();
    bool writeConfig();
    bool readLocalConfig();
    void setConnectedState(bool state) { connectedState = state; checkConnection(); emit connectedStateChanged(); }
    bool getConnectedState () {return connectedState;}
    void setDceResponse(QString response);
    QString getDceResponse () ;

    /*Environment Slots. i.e. user, location, etc*/
    QString getCurrentUser() {return sPK_User;}
    void setCurrentUser(QString inc_user );
    void setCurrentRoom(QString room) {currentRoom = room; writeConfig(); emit roomChanged();}
    QString getCurrentRoom () {return currentRoom;}
    void setDebugMode(bool b) {debugMode = b; emit debugModeChanged();}
    bool getDebugMode() {return debugMode;}
    void setActiveRoom(int room,int ea);
    int getlocation() const ;
    void setLocation(const int& , const int& ) ;
    void qmlSetupLmce(QString incdeviceid, QString incrouterip);
    void displayModelPages(QList<QObject*> pages);
    void setAppH(int h) {appHeight = h;checkOrientation(qorbiterUIwin->size());}
    int getAppH() {return appHeight;}

    void setAppW(int w) {appWidth = w; checkOrientation(qorbiterUIwin->size());}
    int getAppW(){return appWidth; }

    /*Network State property functions*/
    void setInternalIp(QString s) { m_ipAddress = s; setDceResponse("got ip address, sending to dce"); emit internalIpChanged(m_ipAddress); }
    QString getInternalIp() {return m_ipAddress; }
    void setInternalHost(QString h) { internalHost = h; emit internalHostChanged();}
    QString getInternalHost() {return internalHost;}
    void setExternalIp(QString ex) {externalip = ex; emit externalIpChanged();}
    QString getExternalIp() {return externalip;}
    void setExternalHost(QString h) { externalHost = h; emit externalHostChanged();}
    QString getExternalHost() {return externalHost;}

    /*! @name floorplan slots*/
    //@{
    void getFloorplanDevices(int floorplantype);
    void setFloorplanType(int t);
    void showfloorplan(int fptype);
    void getFloorplanDeviceCommands(int device ) { emit populate_floorplan_device_commands(device);}

    void setFloorPlanCommand(QVariantMap t);
    //@}

    void setSkinStatus(bool status) { b_skinReady = status ; emit skinDataLoaded(b_skinReady); }
    void setOrbiterStatus(bool status) {b_orbiterReady = status ; emit orbiterReady(b_orbiterReady);}

    /*Runtime Screen handling*/
    //! This function uses the metaobject system to call the screenchange function.
    /*!
     * \brief This function can be called from the dce object in the server thread in response to events
     *in addition to certain user events which require server interaction. It has some special handling for
     *certain screens and states.
     * \todo Change this item from using the meta object system to using signals.
     *\todo Look into the addition of possible designer set options / custom actions
     */
    void gotoQScreen(QString ) ;


    void goBacktoQScreen();

    //! This function is called when the application window size changes.
    void checkOrientation(QSize);

    bool getOrientation (){return b_orientation;}
    void setOrientation (bool s) {  appHeight = qorbiterUIwin->height(); appWidth=qorbiterUIwin->width();  b_orientation = s; setDceResponse("orientation changed!! "); emit orientationChanged();}
    //! Returns the current screen in string format
    QString getCurrentScreen();
    //! Sets the current screen in string format.
    void setCurrentScreen(QString s);

    //security related

    /*!
     * \brief Qml invokable slot for the purpose of asking for a security picture.
     * \param i_pk_camera_device : The camera device from getScreenParams(2)
     * \param h : The height of the requested picture
     * \param w : The width of the requested picture
     * \warning Do not request the security pictures too quickly. The minimum
     *time should be no less than 1000msec (1 second) or you risk cacheing too many
     *requests and slowing the response time of the router.
     */
    void requestSecurityPic(int i_pk_camera_device, int h, int w);
    /*!
     * \brief setHouseMode
     * \param mode
     * \param pass
     * \ingroup qorbiter_properties
     */
    void setHouseMode(int mode, int pass);


    /*! @name Media Metadata Slots*/
    //{@
    /*!
     * \brief getLiveTVPlaylist
     */
    void getLiveTVPlaylist() { }
    /*!
     * \brief This requests the current playlist of stored media.
     * \ingroup qorbiter_properties
     */
    void getStoredPlaylist() { emit bindMediaRemote(true);}

    /*!
     * \brief This slot is called the PlaylistClickedHandler
     * \param position
     * \ingroup qorbiter_properties
     */
    void changedPlaylistPosition(int position) {emit newPlaylistPosition(position);}
    //@}

    /*! @name Media Mode control slots*/
    //@{
    void setNowPlayingData() {}
    /*!
     * \brief This sets the playlist for tv channels.
     * \ingroup qorbiter_properties
     */
    void setNowPlayingTv() {emit bindMediaRemote(true); emit liveTVrequest(); }

    /*!
     * \brief setBoundStatus
     * \param b
     */
    void setBoundStatus(bool b) {emit bindMediaRemote(b);}

    /*!
     * \brief setNowPlayingIcon
     * \param b
     */
    void setNowPlayingIcon(bool b);

    /*!
     * \brief nowPlayingChanged
     * \param b
     */
    void nowPlayingChanged(bool b);

    /*!
     * \brief addDeviceToList
     * \param d
     */
    void addDeviceToList(AvDevice* d) {devices->appendRow(d);}

    /*!
     * \brief addCommandToList
     * \param c
     */
    void addCommandToList(AvCommand* c) {deviceCommands->appendRow(c);}

    /*!
     * \brief moveDirection
     * \param d
     */
    void moveDirection(int d) {emit moveArrowDirection(d);}

    /*!
     * \brief osdBack
     */
    void osdBack() {emit signalGoBack();}

    /*!
     * \brief setDeviceVolume
     * \param d
     */
    void setDeviceVolume(int d){ deviceVolume = d; emit deviceVolumeChanged();}

    /*!
     * \brief getDeviceVolume
     * \return
     */
    int getDeviceVolume() {return deviceVolume;}

    /*!
     * \brief setLiveAvPath
     * \param path
     */
    void setLiveAvPath(bool path) { usingLiveAvPath = path; qDebug() ; emit liveAvPath();}

    /*!
     * \brief getLiveAvPath
     * \return
     */
    bool getLiveAvPath() { return usingLiveAvPath;}

    /*!
     * \brief setContainsVideo
     * \param video
     */
    void setContainsVideo(bool video) {m_bContainsVideo = video; emit containsVideo();}

    /*!
     * \brief getContainsVideo
     * \return
     */
    bool getContainsVideo() {return m_bContainsVideo;}

    /*!
     * \brief setOsd
     * \param osd
     */
    void setOsd(bool osd) { m_bIsOSD = osd; emit isOsd();}

    /*!
     * \brief getOsd
     * \return
     */
    bool getOsd() { return m_bIsOSD;}

    /*!
     * \brief setMonitorStatus
     * \param state
     */
    void setMonitorStatus(bool state) { monitorAvailible = state; emit monitorStatusChanged();}

    /*!
     * \brief getMonitorStatus
     * \return
     */
    bool getMonitorStatus() {return monitorAvailible;}

    /*!
     * \brief setDiscreteAudio
     * \param d
     */
    void setDiscreteAudio(bool d) {discreteAudio = d; emit discreteAudioChanged();}

    /*!
     * \brief getDiscreteAudio
     * \return
     */
    bool getDiscreteAudio() {return discreteAudio;}

    /*!
     * \brief setCommandResponse
     * \param response
     */
    void setCommandResponse(QString response) {commandResponse =QTime::currentTime().toString()+"::"+response; qDebug() << commandResponse; emit commandResponseChanged();}

    /*!
     * \brief getCommandResponse
     * \return
     */
    QString getCommandResponse() {return commandResponse;}

    /*!
     * \brief setEventResponse
     * \param eResponse
     */
    void setEventResponse(QString eResponse){eventResponse = eResponse; emit eventResponseChanged();}

    /*!
     * \brief getEventResponse
     * \return
     */
    QString getEventResponse() {return eventResponse;}

    /*!
     * \brief setDeviceResponse
     * \param dResponse
     */
    void setDeviceResponse(QString dResponse) {deviceResponse = deviceResponse; emit deviceResponseChanged();}

    /*!
     * \brief getDeviceResponse
     * \return
     */
    QString getDeviceResponse() {return deviceResponse;}

    /*!
     * \brief setMediaResponse
     * \param r
     */
    void setMediaResponse (QString r) {mediaResponse = r; emit mediaResponseChanged();}
    /*!
     * \brief getMediaResponse
     * \return
     */
    QString getMediaResponse(){return mediaResponse;}

    /*!
     * \brief mountMediaDevices
     * \param d
     */
    void mountMediaDevices();

    void getMediaDevices();

    void setMediaDevices(QNetworkReply* d);
    //@}

    /*! @name Media Control Slots*/
    //{@
    void playMedia(QString FK_Media) { emit startPlayback(FK_Media);}
    void mythTvPlay(){emit play();}
    void stopMedia() {emit stopPlayback();}
    void setPlaybackSpeed(int s) {emit setStreamSpeed(s);}
    void pauseMedia() {emit pause();}
    void adjustVolume(int vol) {emit setVolume(vol);}
    void newTrack(QString track) { emit changeTrack(track); }
    void jogPosition(QString jog) {emit jogToPosition(jog);}
    void showBookmarks(QList<QObject*> t);
    void changeChannels(QString chan) {emit newChannel(chan);  }
    void gridChangeChannel(QString chan, QString chanid) {emit newGridChannel(chan, chanid);}
    void extraButtonPressed(QString b) {emit extraButton(b);}
    void dvd_showMenu(bool b) { dvdMenuShowing = b ; emit show_dvdMenu();}
    void setZoomLevel(QString zoom) {emit zoomLevelChanged(zoom);}
    void setAspectRatio(QString r) {emit aspectRatioChanged(r);}
    void getVideoFrame() { emit requestVideoFrame();}
    void redButtonPress(){emit redButton(); }
    void blueButtonPress(){emit blueButton();}
    void greenButtonPress(){emit greenButton();}
    void yellowButtonPress(){emit yellowButton();}
    void mute(){emit muteSound();}

    void movePlaylistEntry(QString d, int index) {emit movePlistEntry(d, index); }
    void removePlaylistEntry(int index) {emit removePlistEntry(index);}
    void saveCurrentPlaylist(QString name, bool mode) {emit savePlist(name, mode);} /*true is public, false is private*/
    //@}

    /*! @nameScreenshot & Images slots*/
    //@{
    void updateImageChanged(QImage img);
    void grabStreamImage(){ if(m_bContainsVideo) emit requestStreamImage(); }/*!< \brief This slot is used for non-media devices*/
    void grabFileImage() {emit requestStoredMediaImage(nowPlayingButton->path+"/"+nowPlayingButton->filepath);  } /*!< \brief This slot is used for now playing screenshots. */
    void setScreenShotVariables(screenshotAttributes*t);
    void showScreenShotVariables() { qorbiterUIwin->rootContext()->setContextProperty("screenshotAttributes", QVariant::fromValue(screenshotVars)); }
    void setMediaScreenShot(QImage screen_shot);
    void saveScreenShot(QString attribute);
    void cleanupScreenie();

    void setScreenSaverImages(QStringList i){
        qDebug() << "Recieved Screen Saver Images" << i.count();
        screensaverImages = i;
        if(screensaverImages.contains("")){
            qDebug() <<"Invalid image url found in list, removing.";
            int d = screensaverImages.indexOf("");
            screensaverImages.removeAt(d);
            qDebug() << "Bad Data removed from list at entry " << d;
        }
        emit screenSaverImagesReady();
    }


    QString getNextScreenSaverImage(QString current){

        if(screensaverImages.isEmpty()){

            return "";
        } else if(current.isEmpty()){
            return screensaverImages.at(0);
        }
        else {

            QString search = current.split("&val=").at(1);
            int index = screensaverImages.lastIndexOf(search);
            if (index+1 < screensaverImages.count()){
                return  screensaverImages.at(index+1);
            }else{
                return screensaverImages.at(0);
            }
        }

    }
    /*! @name Media Devices slots*/
    //@{
    void showDeviceCodes(int code) {emit populateDeviceCommands(code);}
    void setCommandList(QList<QObject*> &l);
    void resendCode(int from, int to) { deviceCommands->clear(); emit resendDeviceCode( from,  to);}
    void toggleDisplay(bool display) { osdStatus = display; emit osdChanged(osdStatus); }
    bool getDisplayStatus() { return osdStatus; }
    void adjustLights(QString l) {emit newLightLevel(l); }
    void showAvControl() {emit resendAvCodes();}
    //@}


    /*! @name QML Skin Function slots*/
    //@{
    void setActiveSkin(QString name);
    bool loadSkins(QUrl url);
    void showSkin() { swapSkins(currentSkin); }
#if (QT5)
    void skinLoaded(QQuickView::Status status);
#else
    void skinLoaded(QDeclarativeView::Status status);
#endif
    //@}

    /*! @name Datagrid Slots*/
    //@{
    /*! Resets the datagrid parameters to default */
    void resetModelAttributes(){emit resetSearchParams(); }

    /*! Updates the datagrid with the attributes from the attribute model*/
    void updateSelectedAttributes(QString attributes);

    void setGridStatus(bool s) {emit gridStatus(s);}
    void setSorting(int i);

    void setMediaType(int m) {i_current_mediaType = m; emit mediaTypeChanged();}
    int getMediaType(){return i_current_mediaType;}

    void setGridMediaType(QString t){q_mediaType = t; qDebug() << q_mediaType; emit mediaGridTypeChanged();}
    QString getSorting() {return q_mediaType;}

    void initializeSortString();
    void clearMediaModel();
    void getGrid(int i);
    void addMediaItem(gridItem* g);
    void updateModel();
    void setStringParam(int paramType, QString param);
    void goBackGrid();
    void requestPage(int p){ setCurrentPage(p); emit requestDcePages(p);}
    void showFileInfo(QString fk_file);
    void initializeGridModel();
    void showMessage(QString message, int duration, bool critical);
    void setRequestMore(bool state);
    bool getRequestMore();
    bool requestDataGrid();
    void setSeekLetter(QString letter) {qs_seek = letter; emit seekGrid(qs_seek); }
    void setSeekPosition(QString p) {emit seekPositionChanged(p);}
    void setDceGridSep(int sep) {emit dceGridSepChanged(sep);}
    void setGridSeperator(int sep) { media_pageSeperator = sep; emit newPageSeperator(sep);}
    int getGridSeperator() { return media_pageSeperator; }

    void setCurrentPage(int page) {media_currentPage = page;   emit mediaPageChanged();  }
    int getCurrentPage() {return media_currentPage;}
    //@}

    //initialization related
    void regenOrbiter(int deviceNo);
    void regenComplete(int i);
    void regenError(QProcess::ProcessError);
    QString adjustPath(const QString&);
    void checkConnection();
    void processError(QString msg);

    void setDeviceNumber(int d) {iPK_Device = d; emit deviceNumberChanged(iPK_Device);}
    int getDeviceNumber() {return iPK_Device;}

    //dce related slots
    void execGrp(int grp);        //for command groups
    void closeOrbiter();
    void exitApp() { qorbiterUIwin->close();}
    void reloadHandler();
    void disconnectHandler();
    void replaceHandler();
    bool OrbiterGen();              //prelim orbter generation
    void quickReload();

    /*Sleeping menu slots. */
    void updateAlarm(bool toggle, int grp);
    void showSleepingAlarms(SleepingAlarm *s);
    void getSleepingAlarms() {sleeping_alarms->clear(); emit getAlarms();}

private:
    void initializeConnections();
    void setupQMLview();


};

#endif // QORBITERMANAGER_H

