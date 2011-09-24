#ifndef QORBITERMANAGER_H
#define QORBITERMANAGER_H

/*---qt includes----*/
#include <QWidget>
#include <QDeclarativeView>
#include <qdeclarativecontext.h>
#include <QDeclarativeItem>
#include <QStringList>
#include <datamodels/usermodel.h>
#include <datamodels/locationmodel.h>
#include <datamodels/lightingscenariomodel.h>
#include <datamodels/mediascenariomodel.h>
#include <datamodels/climatescenariomodel.h>
#include <datamodels/telecomscenariomodel.h>
#include <datamodels/securityscenariomodel.h>
#include <datamodels/floorplanmodel.h>
#include <datamodels/skindatamodel.h>
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
#include <QtNetwork/QTcpSocket>

#include <QThread>


#include <QFile>
#include <QDir>
#include <QDataStream>

/*----custom classes-------*/
//own version of OrbiterData.h
#include <datamodels/listModel.h>                             //custom item model
//#include <contextobjects/epgchannellist.h>
#include <datamodels/gridItem.h>
#include <uiclasses/uicontroller.h>                           //experimental
#include <contextobjects/epgchannellist.h>
#include <imageProviders/basicImageProvider.h>                 //qml image provider
#include <imageProviders/gridimageprovider.h>                  //qml image provider for grids !not implemented!
//#include <imageProviders/abstractimageprovider.h>
#include <screensaver/screensavermodule.h>
#include <datamodels/DataModelItems/sleepingalarm.h>
/*-------Dce Includes----*/

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



namespace DCE
{
class qOrbiter;
}

class qorbiterManager : public QWidget
{
    Q_OBJECT

    Q_PROPERTY (QString q_mediaType READ getSorting NOTIFY gridTypeChanged)
    Q_PROPERTY (QString sPK_User READ getCurrentUser WRITE setCurrentUser NOTIFY userChanged)
    Q_PROPERTY (QString dceResponse READ getDceResponse WRITE setDceResponse NOTIFY dceResponseChanged)


public:
    qorbiterManager(int deviceno, QString routerip, QWidget *parent = 0);  //constructor

    QThread *processingThread; //threaded class
    QThread *timecodeThread; //for timecode
    QTcpSocket *timeCodeSocket;
    QString sPK_User;
    QString *buildType;
    //QByteArray *skin;
    QMap <QString*, QString*> availibleSkins;
    QString qmlPath;
    QString skinsPath;
    QDir skinsDir;
    QString m_SkinsDirectoryPath;
    QString aspect; //-- true poster || false landscape

    //------------------------------------------------------playlist classes

    PlaylistClass *storedVideoPlaylist;
    EPGChannelList *simpleEPGmodel;
    ListModel *playlistModel;

    //------CUSTOM QML TYPES------------------------------------------------------------------------------------
    ScreenParamsClass *ScreenParameters;
    ScreenSaverModule *ScreenSaver;
    SecurityVideoClass *SecurityVideo;


    //-------------sleeping menu----------------------
    QList<QObject*> sleeping_alarms;

    //------------media vars-----------------------------------
    FileDetailsClass *filedetailsclass;
    NowPlayingClass *nowPlayingButton;


    //ui variables
    QString currentSkin;
    QString currentSkinURL;
    QDeclarativeView  *qorbiterUIwin;               //Qml declarativeview
    QObject *item;                                  //qObject reference to UI
    bool refreshUI();
    Q_INVOKABLE void swapSkins(QString incSkin);
    QString dceResponse;


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
    bool backwards;

    bool requestMore;
   Q_INVOKABLE void setRequestMore(bool state);
    bool getRequestMore();

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

    QImage updatedObjectImage;

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
    void orbiterReady();
    void orbiterClosing();
    void locationChanged(int cRoom, int cEA);
    void modelChanged();
    void gridTypeChanged();
    void objectUpdated();
    void liveTVrequest();
    void userChanged();
    void requestMoreGridData();
    void dceResponseChanged();
    void imageAspectChanged();

public slots: //note: Q_INVOKABLE means it can be called directly from qml

    Q_INVOKABLE void writeConfig();
    bool readLocalConfig();
    void setDceResponse(QString response) {dceResponse = response; emit dceResponseChanged();}
    QString getDceResponse () {return dceResponse;}



    //security related
    Q_INVOKABLE void requestSecurityPic(int i_pk_camera_device, int h, int w);

    //livetv related
    Q_INVOKABLE void changeChannels(QString chan);
    Q_INVOKABLE void gridChangeChannel(QString chan, QString chanid);

    //media related
    Q_INVOKABLE void getMediaPlaylist();
    Q_INVOKABLE void setNowPlayingData();
    Q_INVOKABLE void setNowPlayingTv();
    Q_INVOKABLE void getLiveTVPlaylist();
    Q_INVOKABLE void playMedia(QString FK_Media);
    Q_INVOKABLE void stopMedia();
    Q_INVOKABLE void ff_media(int speed);
    Q_INVOKABLE void rw_media(int speed);
    Q_INVOKABLE void pauseMedia();
    Q_INVOKABLE void bindMediaRemote(bool state);
    void updateImageChanged(QImage img);
    void updateTimecode();
    void showTimeCode();

    Q_INVOKABLE  void getcurrentSkins(QStringList skinPaths);
    void qmlSetupLmce(int incdeviceid, QString incrouterip);
    void changedPlaylistPosition(int position);
    void changedTrack(QString increment);

    //automation related
    void adjustVolume(int vol);
    void adjustLighting(int l);
    void mute();

    //datagrid related
    void setSorting(int i);
    QString getSorting() {return q_mediaType;}
    void initializeSortString();
    void clearMediaModel();
    bool addMediaItem(QString mText, QString temp, QImage cell);
    void updateModel();
    Q_INVOKABLE void setStringParam(int paramType, QString param);
    Q_INVOKABLE void goBackGrid();

    void showFileInfo(QString fk_file);

    //ui related
    int getlocation() const ;
    void setLocation(const int& , const int& ) ;
    Q_INVOKABLE void gotoQScreen(QString ) ;
    void setNowPlayingIcon(bool b);
    void initializeContexts();
    void initializeGridModel();
    Q_INVOKABLE void setZoom(QString qs_zoom);
    Q_INVOKABLE void setAspect(QString qs_aspect);

    //initialization related
    void regenOrbiter(int deviceNo);
    void regenComplete(int i);
    QString adjustPath(const QString&);

    //dce related slots
    Q_INVOKABLE void execGrp(int grp);        //for command groups
    Q_INVOKABLE void closeOrbiter();

    //floorplans
    Q_INVOKABLE void showfloorplan(int fptype);
    //random c++ related slots
    bool requestDataGrid();

    //sleeping menu
    Q_INVOKABLE void sleepingMenu(bool toggle, int grp);

    //security
    void setHouseMode(int mode, int pass);
    \
};

#endif // QORBITERMANAGER_H
