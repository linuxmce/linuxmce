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

#include <QStringList>
#include <QThread>
#include <QFile>
#include <QDir>
#include <QDataStream>

#include <QProcess>
#include <QtXml/QDomDocument>
#ifndef __ANDROID__

#if GLENABLED
#include <shaders/filereader.h>
#include <shaders/trace.h>
#endif
//for pi - for_pi now sets glenabled
//#include <shaders/filereader.h>
//#include <shaders/trace.h>
#endif

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

#include <datamodels/filtermodel.h>
#include <datamodels/genremodel.h>
#include <datamodels/attributesortmodel.h>
#include <datamodels/mediatypemodel.h>
#include <datamodels/filedetailsmodel.h>
#include <contextobjects/filedetailsclass.h>
#include <contextobjects/nowplayingclass.h>
#include <contextobjects/screenparamsclass.h>
#include <contextobjects/playlistclass.h>
#include <contextobjects/securityvideoclass.h>

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

class qorbiterManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int i_current_mediaType READ getMediaType WRITE setMediaType NOTIFY mediaTypeChanged)
    Q_PROPERTY (QString q_mediaType READ getSorting NOTIFY gridTypeChanged)
    Q_PROPERTY (QString sPK_User READ getCurrentUser WRITE setCurrentUser NOTIFY userChanged)
    Q_PROPERTY (QString currentRoom READ getCurrentRoom WRITE setCurrentRoom NOTIFY roomChanged)
    Q_PROPERTY (QString dceResponse READ getDceResponse WRITE setDceResponse NOTIFY dceResponseChanged)
    Q_PROPERTY (bool debugMode READ getDebugMode WRITE setDebugMode NOTIFY debugModeChanged )
    Q_PROPERTY (bool connectedState READ getConnectedState WRITE setConnectedState NOTIFY connectedStateChanged)
    Q_PROPERTY (bool b_orientation READ getOrientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY (QString currentScreen READ getCurrentScreen WRITE setCurrentScreen  NOTIFY screenChange)
    Q_PROPERTY (QString m_ipAddress READ getInternalIp WRITE setInternalIp NOTIFY internalIpChanged)
    Q_PROPERTY (QString internalHost READ getInternalHost WRITE setInternalHost NOTIFY internalHostChanged)
    Q_PROPERTY (QString externalip READ getExternalIp WRITE setExternalIp NOTIFY externalIpChanged)
    Q_PROPERTY (QString externalHost READ getExternalHost WRITE setExternalHost NOTIFY externalHostChanged )

    /*
     *Need to move runtime options here so that we can tie into a configuration panel
     *first run - self explanitory?
     *skin - the current selected skin. defaults to 'default'
     *enablescreensavermode - currently unused, should be built anyways
     *default view - relates to how list views are configured.
     *phone - if the device is a phone or not. currently unused
     *networkload - currently is used for android devices
     *debug - allows the popup of various status messages.
     */


public:
#if (QT5)
    qorbiterManager(QQuickView * view, QObject *parent=0);  //constructor
#else
    qorbiterManager(QDeclarativeView * view, QObject *parent=0);  //constructor
#endif

#ifndef __ANDROID__
#if GLENABLED
    FileReader * fileReader;
#endif
    //for Pi - for_pi now sets GLENABLED
    //FileReader * fileReader;
#endif
    bool b_glEnabled;
    //settings
    QString sPK_User;
    QString buildType;
    QByteArray binaryConfig;
    long iPK_Device;
    QString qs_ext_routerip;

    //------------------------------------------------------playlist classes


    //------CUSTOM QML TYPES------------------------------------------------------------------------------------
    ScreenParamsClass *ScreenParameters;
    SecurityVideoClass *SecurityVideo;
    QList<QObject*> screenshotVars;

    //-------------sleeping menu----------------------
    QList<QObject*> sleeping_alarms;

    //------------media vars-----------------------------------
    FileDetailsClass *filedetailsclass;
    NowPlayingClass *nowPlayingButton;
    QString aspect; //-- true poster || false landscape
    QString *gridReqType;


    //skins
    SkinDataModel* tskinModel;
    ExistingOrbiterModel *myOrbiters;
    QString qrcPath;
    //TODO, remove the below in favour of the data structure
    QMap <QString*, QString*> availibleSkins;
    QString qmlPath;
    QString localDir;
    QString skinsPath;
    QDir skinsDir;
    QString m_SkinsDirectoryPath;


    //ui
    QString currentSkin;
    QString currentSkinURL;
    QString remoteDirectoryPath;
    SkinDataItem* skin;

#if (QT5)
    QQuickView *qorbiterUIwin;
#else
    QDeclarativeView *qorbiterUIwin;    //Qml declarativeview
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
    QString q_mediaType;           //1
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

    void setSeekLetter(QString letter);

    //listmodels
    LocationModel *m_lRooms;
    ListModel *model;      //media grid model
    UserModel *userList;
    SkinDataModel *skinModel;
    QList<QObject*> buttonList;
    QList<QObject*> commandList;
    QList<QObject*>pages;
    QStringList *skinNames;
    QList<QObject*> current_floorplan_devices;
    QList<QObject*> current_bookmarks;
    QList<QObject*> existingOrbiters;

    MediaSubTypeModel *mediaTypeFilter;
    FilterModel *uiFileFilter;
    AttributeSortModel *attribFilter;
    GenreModel *genreFilter;
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
    QString applicationPath;

signals:
    void mediaTypeChanged();
    void requestDcePages(int i);
    void showList();
    void filterChanged();
    void resetFilter();
    void locationChanged(int cRoom, int cEA);
    void modelChanged();
    void gridTypeChanged(int i);
    void mediaRequest(int);
    void objectUpdated();
    void setMediaDetails();
    void mediaScreenShotReady();
    void saveMediaScreenShot(QString attribute, QImage pic);
    void mediaSeperatorChanged(int sep);
    void requestStreamImage();
    void debugModeChanged();

    void liveTVrequest();
    void managerPlaylistRequest();
    void bindMediaRemote(bool b);
    void userChanged(int user);
    void requestMoreGridData();
    void resendDeviceCode(int from, int to);

    void dceResponseChanged();
    void imageAspectChanged();
    void connectedStateChanged();
    void continueSetup();
    void screenChange(QString s);
    void clearModel();
    void clearAndContinue();
    void registerOrbiter(int user, QString ea, int room);
    void unregisterOrbiter(int user, QString ea, int room);
    void startPlayback(QString file);
    void setDceGridParam(int a, QString p );
    void keepLoading(bool s);
    void updateScreen(QString screen);

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

    void loadingMessage(QString msg);
    void splashReady();
    void raiseSplash();
    void reInitialize();
    void showSetup();
    void reloadRouter();

    void stillLoading(bool b);
    void executeCMD(int);

    void localConfigReady(bool b);
    void orbiterConfigReady(bool b);
    void deviceValid(bool b);
    void connectionValid(bool b);
    void skinIndexReady(bool b);
    void skinDataLoaded(bool b);
    void roomChanged();

    void floorplanTypeChanged();
    void setAlarm(bool s, int g);
    void getSingleCam(int i_pk_camera_device, int h, int w);

    //runtime
    void internalIpChanged();
    void externalIpChanged();
    void internalHostChanged();
    void externalHostChanged();


public slots: //note: Q_INVOKABLE means it can be called directly from qml
    void showExistingOrbiter(const QList<QObject *> l )  ;
    void connectionWatchdog();
    QString getCurrentUser() {return sPK_User;}
    void setCurrentUser(QString inc_user );
    void setCurrentRoom(QString room) {currentRoom = room; writeConfig(); emit roomChanged();}
    QString getCurrentRoom () {return currentRoom;}
    void processConfig(QByteArray config);
    bool OrbiterGen();              //prelim orbter generation
    void quickReload();
    void showUI(bool b);

    void setDebugMode(bool b) {debugMode = b; emit debugModeChanged();}
    bool getDebugMode() {return debugMode;}

    void displayModelPages(QList<QObject*> pages);    

    void setInternalIp(QString s) { m_ipAddress = s; emit internalIpChanged(); }
    QString getInternalIp() {return m_ipAddress; }

    void setInternalHost(QString h) { internalHost = h; emit internalHostChanged();}
    QString getInternalHost() {return internalHost;}

    void setExternalIp(QString ex) {externalip = ex; emit externalIpChanged();}
    QString getExternalIp() {return externalip;}

    void setExternalHost(QString h) { externalHost = h; emit externalHostChanged();}
    QString getExternalHost() {return externalHost;}



    void getFloorplanDevices(int floorplantype);
    void setFloorplanType(int t);
    void setActiveRoom(int room,int ea);
    void setSkinStatus(bool status) { b_skinReady = status ; emit skinDataLoaded(b_skinReady); }
    void setOrbiterStatus(bool status) {b_orbiterReady = status ; emit orbiterReady(b_orbiterReady);}

    //void setAppPath(QString p) {appPath;}

    void qmlSetupLmce(QString incdeviceid, QString incrouterip);
    void setRequestMore(bool state);
    bool getRequestMore();
    int loadSplash();
    void startOrbiter();
    bool createAndroidConfig();
    void gotoQScreen(QString ) ;
    void checkOrientation(QSize);
    bool getOrientation (){return b_orientation;}
    void setOrientation (bool s) { b_orientation = s; setDceResponse("orientation changed!! "); emit orientationChanged();}
    QString getCurrentScreen();
    void setCurrentScreen(QString s);

    bool writeConfig();
    bool readLocalConfig();
    void setConnectedState(bool state) { connectedState = state;  if(state == false) {checkConnection("Connection Changed");} emit connectedStateChanged(); }
    bool getConnectedState () {return connectedState;}
    void setDceResponse(QString response);
    QString getDceResponse () ;

    //security related
    void requestSecurityPic(int i_pk_camera_device, int h, int w);

    //livetv related
    void changeChannels(QString chan);
    void gridChangeChannel(QString chan, QString chanid);

    //media related
    void getLiveTVPlaylist();
    void getStoredPlaylist();
    void setNowPlayingData();
    void setNowPlayingTv();
    void setScreenShotVariables(QList <QObject*> l);
    void setMediaScreenShot(QImage screen_shot);
    void saveScreenShot(QString attribute);
    void showDeviceCodes(QList<QObject*> t);
    void setCommandList(QList<QObject*> l);
    void setBoundStatus(bool b);
    void grabStreamImage();
    void resendCode(int from, int to) { emit resendDeviceCode( from,  to);}
    void showBookmarks(QList<QObject*> t);

    void playMedia(QString FK_Media);
    void stopMedia();
    void ff_media(int speed);
    void rw_media(int speed);
    void pauseMedia();
    void adjustVolume(int vol);

    void jogPosition(QString jog);
    void updateImageChanged(QImage img);

    void cleanupScreenie();

    void setActiveSkin(QString name);
    bool loadSkins(QUrl url);


    void changedPlaylistPosition(QString position);

    //datagrid related
    void setSorting(int i);
    void setMediaType(int m) {i_current_mediaType = m; emit mediaTypeChanged();}
    int getMediaType(){return i_current_mediaType;}
    QString getSorting() {return q_mediaType;}
    void initializeSortString();
    void clearMediaModel();
    void getGrid(int i);
    void addMediaItem(gridItem* g);
    void updateModel();
    void setStringParam(int paramType, QString param);
    void goBackGrid();
    void requestPage(int p);

    void showFileInfo(QString fk_file);

    //ui related
    int getlocation() const ;
    void setLocation(const int& , const int& ) ;

    void setNowPlayingIcon(bool b);
    void nowPlayingChanged(bool b);
    void initializeGridModel();
    void showMessage(QString message, int duration, bool critical);

    //initialization related
    void regenOrbiter(int deviceNo);
    void regenComplete(int i);
    void regenError(QProcess::ProcessError);
    QString adjustPath(const QString&);
    void checkConnection(QString s);
    void processError(QString msg);
    //dce related slots
    void execGrp(int grp);        //for command groups
    void closeOrbiter();
    void reloadHandler();

    //floorplans
    void showfloorplan(int fptype);
    //random c++ related slots
    bool requestDataGrid();

    //sleeping menu
    void sleepingMenu(bool toggle, int grp);
    void showSleepingAlarms(QList<QObject*> s);
    //security
    void setHouseMode(int mode, int pass);
    void activateScreenSaver();
    void killScreenSaver();
#if (QT5)
    void skinLoaded(QQuickView::Status status);
#else
    void skinLoaded(QDeclarativeView::Status status);
#endif
private:
    void initializeConnections();
    void setupQMLview();


};

#endif // QORBITERMANAGER_H

