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
#include <QFile>
#include <QDataStream>

/*----custom classes-------*/
                     //own version of OrbiterData.h
#include <datamodels/listModel.h>                          //custom item model
#include <datamodels/gridItem.h>
#include <uiclasses/uicontroller.h>               //experimental

#include <imageProviders/basicImageProvider.h>                 //qml image provider
#include <imageProviders/gridimageprovider.h>                  //qml image provider for grids !not implemented!

#include <screensaver/screensavermodule.h>

/*-------Dce Includes----*/

#include <qOrbiter/qOrbiter.h>

class basicImageProvider;

namespace DCE
{
class qOrbiter;
}

class qorbiterManager : public QWidget
{
    Q_OBJECT
    Q_PROPERTY (int getLocation READ getlocation NOTIFY locationChanged )
    Q_PROPERTY (bool addMediaItem NOTIFY modelChanged)
    Q_PROPERTY (QString q_mediaType READ getSorting NOTIFY gridTypeChanged)
public:
     qorbiterManager(QWidget *parent = 0);  //constructor

    QString *sPK_User;


    //ui variables
    QString currentSkin;
    QString currentSkinURL;
    QDeclarativeView  *qorbiterUIwin;               //Qml declarativeview
    QObject *item;                                  //qObject reference to UI
    bool refreshUI();
    void swapSkins();

    basicImageProvider *basicProvider;
    //GridIndexProvider advancedProvider;
    QString *gridReqType;


    //datagrid variables
    QString q_mediaType;
    QString q_publicUsers;
    QString q_fileFormat;
    QString q_subType;
    QString q_attribute;


    //listmodels
    QByteArray binaryConfig;
    LocationModel *m_lRooms;
    ListModel *model;
    UserModel *userList;

    LightingScenarioModel *roomLights;
    MediaScenarioModel *roomMedia;
    ClimateScenarioModel *roomClimate;
    TelecomScenarioModel *roomTelecom;
    SecurityScenarioModel *roomSecurity;

    //ui functions
    Q_INVOKABLE QDateTime getCurrentDateTime() const { return QDateTime::currentDateTimeUtc();}
    Q_INVOKABLE void setActiveRoom(int room,int ea);
    Q_INVOKABLE void setCurrentUser();
    //class objects
    DCE::qOrbiter * pqOrbiter;                  //reference to forward declared dce object

    //QT Functions to initialize lmce data
    bool initialize(int dev_id);
    bool setupLmce(int PK_Device, string sRouterIP, bool, bool bLocalMode);     //init's dce object
    /*
      getConf() is the part of the equation that should read the orbiter conf. not implemented fully
      */
    bool getConf(int pPK_Device);
    bool OrbiterGen();              //prelim orbter generation



    //virtual devices
    long iPK_Device_DatagridPlugIn;
    long iPK_Device_OrbiterPlugin;
    long iPK_Device_GeneralInfoPlugin;

    int m_pDevice_ScreenSaver;
    int m_dwIDataGridRequestCounter;
     long iPK_Device;                   //this orbiters device number, passed in from command line

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
    bool dceBool;                   //
    bool bLocalMode;                //local running flag, for running without router.
    //LightingScenarioModel *dummy;

    string sEntertainArea;          //current entertain area int
    int iPK_User;                    //current user


    QMap <QString, int> mp_Users;

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

    long iOrbiterPluginID;           //the orbiter plugin id for future use
    QStringList *sRoomList;          //Linked list of rooms in house
    QStringList *sUserList;          //linked list of users in house
    QStringList *sCurr_Room_Devices; //linked list of current devices (experimental)

    int CurrentScreen();

signals:
      void orbiterReady();
      void orbiterClosing();
      void locationChanged(int cRoom, int cEA);
      void modelChanged();
      void gridTypeChanged();


public slots: //note: Q_INVOKABLE means it can be called directly from qml

      //datagrid related
      void setSorting(int i);
      QString getSorting() {return q_mediaType;}
      void clearMediaModel();
      bool addMediaItem(QString mText, QString temp, QImage cell);
      void updateModel();

      //ui related
      int getlocation() const ;
      void setLocation(const int& , const int& ) ;
      Q_INVOKABLE bool gotoQScreen(QString ) ;

      //initialization related
      void regenOrbiter(int deviceNo);
      void regenComplete(int i);
      QString adjustPath(const QString&);

      //dce related slots
      Q_INVOKABLE void execGrp(int grp);        //for command groups
       Q_INVOKABLE void closeOrbiter();

      //random c++ related slots
      bool requestDataGrid();
   \
};

#endif // QORBITERMANAGER_H
