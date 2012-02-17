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
  //  Q_PROPERTY (bool b_orientation READ getOrientation WRITE setOrientation NOTIFY orientationChanged)


public:
    qorbiterManager(int deviceno, QString routerip, QDeclarativeView * view,  QObject *parent =0);  //constructor

    QThread *processingThread; //threaded class
    QThread *timecodeThread; //for timecode
     QThread *gridThread;

    QTcpSocket *timeCodeSocket;
    QString sPK_User;
    QString buildType;
    //QByteArray *skin;

    SkinDataModel* tskinModel;
    QString qrcPath;
    //TODO, remove the below in favour of the data structure
    QMap <QString*, QString*> availibleSkins;
    QString qmlPath;
    QString skinsPath;
    QDir skinsDir;
    QString m_SkinsDirectoryPath;
    QString aspect; //-- true poster || false landscape
    bool b_orientation;
    int appHeight;
    int appWidth;

    ScreenSaverClass ScreenSaver;

    //------------------------------------------------------playlist classes

    PlaylistClass *storedVideoPlaylist;
    EPGChannelList *simpleEPGmodel;
    ListModel *playlistModel;

    //------CUSTOM QML TYPES------------------------------------------------------------------------------------
    ScreenParamsClass *ScreenParameters;

    SecurityVideoClass *SecurityVideo;
    QList<QObject*> screenshotVars;

    //-------------sleeping menu----------------------
    QList<QObject*> sleeping_alarms;

    //------------media vars-----------------------------------
    FileDetailsClass *filedetailsclass;
    NowPlayingClass *nowPlayingButton;


    //ui variables
    QString currentSkin;
    QString currentSkinURL;
    QString remoteDirectoryPath;
    SkinDataItem* skin;
    QDeclarativeView  *qorbiterUIwin;               //Qml declarativeview
    QObject *item;                                  //qObject reference to UI
    Q_INVOKABLE void refreshUI(QUrl url);
    bool cleanupData();
    Q_INVOKABLE void swapSkins(QString incSkin);
    QString dceResponse;

    bool connectedState;

    //----ANDROID----///
    QString droidPath;  //specific path for android files that relates to the package name 'com.linuxmce.qOrbiter/files
    //-END---ANDROID----///

    //splash related


    basicImageProvider *basicProvider;
    GridIndexProvider *advancedProvider;

    AbstractImageProvider *modelimageprovider;
    QString *gridReqType;

    //carried over variables from old OrbiterData.h
    map<int,string> m_mapTextString;
    vector<int> m_vectPK_Users_RequiringPIN;
    map<int,int> m_mapDesignObj; //Used to map a screen to a DesignObj
    map<int,int> m_mapPK_Screen_GoBackToScreen;  // For screens in this map, if there's a go back
    map<int,int> m_mapPK_MediaType_PK_Attribute_Sort;  // The default sort for each type of media
    map<int,int> m_mapScreen_Interrupt; // Map of which scripts can be interrupted
    map<int,string> m_mapPK_MediaType_Description; // The description for all the media
    map<int,string> m_mapPK_AttributeType_Description; // The description for all attribute types
    bool m_bIsOSD,m_bNewOrbiter,m_bUseAlphaBlending,m_bUseMask;

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
    QStringList goBack;
    QString qs_seek;
    bool backwards;

    //for the media grid to allow pauses in loading and allow non locking operation
    bool requestMore;
    Q_INVOKABLE void setRequestMore(bool state);
    bool getRequestMore();
    Q_INVOKABLE void setSeekLetter(QString letter);

    //listmodels
    QByteArray binaryConfig;
    LocationModel *m_lRooms;
    ListModel *model;
    UserModel *userList;
    SkinDataModel *skinModel;


    MediaSubTypeModel *mediaTypeFilter;
    FilterModel *uiFileFilter;
    AttributeSortModel *attribFilter;
    GenreModel *genreFilter;

    int i_current_mediaType;

    LightingScenarioModel *roomLights;
    MediaScenarioModel *roomMedia;
    ClimateScenarioModel *roomClimate;
    TelecomScenarioModel *roomTelecom;
    SecurityScenarioModel *roomSecurity;

    //---------IMPORTANT-1-OFF-VARIABLES!!!!!---------------------------------------------//
    int i_current_command_grp;
    //int i_stream_id;

    /*
    update object image variables. they need a place to be store because they can be sent or updated at any time
    with this method, the current screen can get its data in an async fashion and multiple components can request the
    data
    */

    QImage updatedObjectImage; //used for the current image for a given media item on screen
    QImage mediaScreenShot;    //used for screen shots

    //ui functions
    Q_INVOKABLE QDateTime getCurrentDateTime() const { return QDateTime::currentDateTimeUtc();}
    Q_INVOKABLE void setActiveRoom(int room,int ea);
    Q_INVOKABLE void setCurrentUser(QString inc_user );
    Q_INVOKABLE  QString getCurrentUser() {return sPK_User;}

    //class objects
    DCE::qOrbiter * pqOrbiter;                  //reference to forward declared dce object
    bool mediaPlaying;

    //QT Functions to initialize lmce data
    bool initialize(int dev_id);
    Q_INVOKABLE bool setupLmce(int PK_Device, string sRouterIP, bool, bool bLocalMode);     //init's dce object
    /*
      getConf() is the part of the equation that should read the orbiter conf. not implemented fully
      */
    bool getConf(int pPK_Device);

    bool OrbiterGen();              //prelim orbter generation
    Q_INVOKABLE void quickReload();

    /*
      DCE variables
      */
    bool m_bStartingUp;
    string m_sLocalDirectory;
    int m_pOrbiterCat;
    char *pData;                   //config size, pointer to pointer
    int iSize;                     //size of pData aka the config
    bool bAppError;                 //error flagging
    bool bReload;                   //reload flag
    bool needRegen;                 //regen flag

    string s_RouterIP;               // string of the router ip
    QString qs_routerip;
    bool dceBool;                   //
    bool bLocalMode;                //local running flag, for running without router.
    //LightingScenarioModel *dummy;

    string sEntertainArea;          //current entertain area int
    int iPK_User;                    //current user
    int iFK_Room;                    //current room
    int iea_area;
    QString sPK_Room;
    QMap <QString, int> mp_rooms;
    QMap <int, LightingScenarioModel*> roomLightingScenarios;
    QMap <int, MediaScenarioModel*> roomMediaScenarios;
    QMap <int, ClimateScenarioModel*> roomClimateScenarios;
    QMap <int, TelecomScenarioModel*> roomTelecomScenarios;
    QMap <int, SecurityScenarioModel*> roomSecurityScenarios;
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
    long iPK_Device;                   //this orbiters device number, passed in from command line

signals:
    void filterChanged(int);
    void locationChanged(int cRoom, int cEA);
    void modelChanged();
    void gridTypeChanged(int i);
    void mediaRequest(int);
    void objectUpdated();
    void liveTVrequest();
    void userChanged();
    void requestMoreGridData();
    void dceResponseChanged();
    void imageAspectChanged();
    void connectedStateChanged();
    void continueSetup();
    void screenChange(QString s);

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

    void loadingMessage(QString msg);
    void splashReady();
    void raiseSplash();
    void showSetup();

    void localConfigReady(bool b);
    void orbiterConfigReady(bool b);
    void deviceValid(bool b);
    void connectionValid(bool b);
    void skinIndexReady(bool b);
    void skinDataLoaded(bool b);

public slots: //note: Q_INVOKABLE means it can be called directly from qml

    int loadSplash();
    void startOrbiter();
    bool createAndroidConfig();
 void gotoQScreen(QString ) ;
    void checkOrientation(QSize);
    bool getOrientation (){return b_orientation;}
    void setOrientation (bool s) { b_orientation = s; setDceResponse("orientation changed!! "); emit orientationChanged();}

    Q_INVOKABLE bool writeConfig();
    bool readLocalConfig();
    void setConnectedState(bool state) { connectedState = state;  if(state == false) {checkConnection();} emit connectedStateChanged(); }
    bool getConnectedState () {return connectedState;}
    void setDceResponse(QString response);
    QString getDceResponse () ;

    //security related
    Q_INVOKABLE void requestSecurityPic(int i_pk_camera_device, int h, int w);

    //livetv related
    Q_INVOKABLE void changeChannels(QString chan);
    Q_INVOKABLE void gridChangeChannel(QString chan, QString chanid);

    //media related

    Q_INVOKABLE void setNowPlayingData();
    Q_INVOKABLE void setNowPlayingTv();
    Q_INVOKABLE void getLiveTVPlaylist();
    Q_INVOKABLE void playMedia(QString FK_Media);
    Q_INVOKABLE void stopMedia();
    Q_INVOKABLE void ff_media(int speed);
    Q_INVOKABLE void rw_media(int speed);
    Q_INVOKABLE void pauseMedia();
     void adjustVolume(int vol);

    void jogPosition(QString jog);
    void updateImageChanged(QImage img);
    void updateTimecode();
    void showTimeCode();
    Q_INVOKABLE void cleanupScreenie();

    Q_INVOKABLE void setActiveSkin(QString name);
    Q_INVOKABLE  bool loadSkins(QUrl url);
    void qmlSetupLmce(QString incdeviceid, QString incrouterip);
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
    void initializeGridModel();
    void showMessage(QString message, int duration, bool critical);

    //initialization related
    void regenOrbiter(int deviceNo);
    void regenComplete(int i);
    void regenError(QProcess::ProcessError);
    QString adjustPath(const QString&);
    void checkConnection();
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

