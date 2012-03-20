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
#include <QtGui>
#include <QDeclarativeView>
#include <qdeclarativecontext.h>
#include <QDeclarativeItem>
#include <QStringList>
#include <QMainWindow>
#include <QThread>
#include <QFile>
#include <QDir>
#include <QDataStream>
#include <QtNetwork/QTcpSocket>
#include <QProcess>
#include <QtXml/QDomDocument>

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



namespace DCE
{
class qOrbiter;
}

class qorbiterManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY (QString q_mediaType READ getSorting NOTIFY gridTypeChanged)
    Q_PROPERTY (QString sPK_User READ getCurrentUser WRITE setCurrentUser NOTIFY userChanged)
    Q_PROPERTY (QString dceResponse READ getDceResponse WRITE setDceResponse NOTIFY dceResponseChanged)
    Q_PROPERTY (bool connectedState READ getConnectedState WRITE setConnectedState NOTIFY connectedStateChanged)
    Q_PROPERTY (bool b_orientation READ getOrientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY (QString currentScreen READ getCurrentScreen WRITE setCurrentScreen  NOTIFY screenChange)


public:
    qorbiterManager(QDeclarativeView * view, QObject *parent=0);  //constructor


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
    QTcpSocket * timeCodeSocket;

    //skins
    SkinDataModel* tskinModel;
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
    QDeclarativeView  *qorbiterUIwin;               //Qml declarativeview
    QObject *item;
    ScreenSaverClass *ScreenSaver;

    //media
    bool mediaPlaying;
    QImage updatedObjectImage; //used for the current image for a given media item on screen
    QImage mediaScreenShot;    //used for screen shots

    bool b_orientation;
    int appHeight;
    int appWidth;
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
    Q_INVOKABLE void setActiveRoom(int room,int ea);
    Q_INVOKABLE void setCurrentUser(QString inc_user );
    Q_INVOKABLE  QString getCurrentUser() {return sPK_User;}

    //class objects


    //QT Functions to initialize lmce data
    bool initialize(int dev_id);
    bool initializeManager(string router_address, int device_id);     //init's dce object
    //getConf() is the part of the equation that should read the orbiter conf. not implemented fully


    //DCE variables
    bool m_bStartingUp;
    string m_sLocalDirectory;
    int m_pOrbiterCat;
    char *pData;                   //config size, pointer to pointer
    int iSize;                     //size of pData aka the config
    bool bAppError;                 //error flagging
    bool bReload;                   //reload flag
    bool needRegen;                 //regen flag

    string s_RouterIP;               // string of the router ip
    QString m_ipAddress;
    QString qs_routerip;
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
    QString applicationPath;

signals:
    void filterChanged();
    void resetFilter();
    void locationChanged(int cRoom, int cEA);
    void modelChanged();
    void gridTypeChanged(int i);
    void mediaRequest(int);
    void objectUpdated();
    void setMediaDetails();
    void mediaScreenShotReady();
    void saveMediaScreenShot(QString attribute, QByteArray pic);

    void liveTVrequest();
    void managerPlaylistRequest();
    void bindMediaRemote(bool b);
    void userChanged(int user);
    void requestMoreGridData();

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

public slots: //note: Q_INVOKABLE means it can be called directly from qml
    void processConfig(QByteArray config);
    bool OrbiterGen();              //prelim orbter generation
    void quickReload();
    void showUI(bool b);
    void displayModelPages(QList<QObject*> pages);
    void setIpAddress(QString s);

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

    Q_INVOKABLE bool writeConfig();
    bool readLocalConfig();
    void setConnectedState(bool state) { connectedState = state;  if(state == false) {checkConnection("Connection Changed");} emit connectedStateChanged(); }
    bool getConnectedState () {return connectedState;}
    void setDceResponse(QString response);
    QString getDceResponse () ;

    //security related
    Q_INVOKABLE void requestSecurityPic(int i_pk_camera_device, int h, int w);

    //livetv related
    Q_INVOKABLE void changeChannels(QString chan);
    Q_INVOKABLE void gridChangeChannel(QString chan, QString chanid);

    //media related
    void getLiveTVPlaylist();
    void getStoredPlaylist();
    Q_INVOKABLE void setNowPlayingData();
    Q_INVOKABLE void setNowPlayingTv();
    void setScreenShotVariables(QList <QObject*> l);
    void setMediaScreenShot(QByteArray data);
    void saveScreenShot(QString attribute);
    void showDeviceCodes(QList<QObject*> t);
    void setCommandList(QList<QObject*> l);
    void setBoundStatus(bool b);

    Q_INVOKABLE void playMedia(QString FK_Media);
    Q_INVOKABLE void stopMedia();
    Q_INVOKABLE void ff_media(int speed);
    Q_INVOKABLE void rw_media(int speed);
    Q_INVOKABLE void pauseMedia();
    void adjustVolume(int vol);

    void jogPosition(QString jog);
    void updateImageChanged(QImage img);
    void updateTimecode(int port);
    void showTimeCode();
    Q_INVOKABLE void cleanupScreenie();

    Q_INVOKABLE void setActiveSkin(QString name);
    Q_INVOKABLE  bool loadSkins(QUrl url);

    void changedPlaylistPosition(QString position);

    //datagrid related
    void setSorting(int i);
    QString getSorting() {return q_mediaType;}
    void initializeSortString();
    void clearMediaModel();
    void getGrid(int i);
    void addMediaItem(gridItem* g);
    void updateModel();
    Q_INVOKABLE void setStringParam(int paramType, QString param);
    Q_INVOKABLE void goBackGrid();

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
    Q_INVOKABLE void execGrp(int grp);        //for command groups
    Q_INVOKABLE void closeOrbiter();
    void reloadHandler();

    //floorplans
    Q_INVOKABLE void showfloorplan(int fptype);
    //random c++ related slots
    bool requestDataGrid();

    //sleeping menu
    Q_INVOKABLE void sleepingMenu(bool toggle, int grp);

    //security
    void setHouseMode(int mode, int pass);
    void activateScreenSaver();
    void killScreenSaver();

    void skinLoaded(QDeclarativeView::Status status);
private:
    void initializeConnections();
    void setupQMLview();


};

#endif // QORBITERMANAGER_H

