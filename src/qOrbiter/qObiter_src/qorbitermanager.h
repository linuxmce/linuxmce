#ifndef QORBITERMANAGER_H
#define QORBITERMANAGER_H

#include <QWidget>
#include <listModel.h>
#include <QDeclarativeView>
#include <qdeclarativecontext.h>
#include <QDeclarativeItem>
#include <qOrbiter/qOrbiter.h>
#include <uiclasses/uicontroller.h>
#include <Command_Impl.h>
#include <Gen_Devices/AllCommandsRequests.h>
#include <QStringList>


namespace DCE
{
class qOrbiter;
}

class qorbiterManager : public QWidget
{
    Q_OBJECT
public:
     qorbiterManager(QWidget *parent = 0);  //constructor


    //ui variables
    QString currentSkin;
    QString currentSkinURL;
    QDeclarativeView  *qorbiterUIwin;               //Qml declarativeview
    QObject *item;                                  //qObject reference to UI
    bool refreshUI();
    void swapSkins();
    ListModel *model;

    //ui functions
    Q_INVOKABLE QDateTime getCurrentDateTime() const { return QDateTime::currentDateTimeUtc();}

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

    string s_RouterIP;               // string of the router ip
    bool dceBool;                   //
    bool bLocalMode;                //local running flag, for running without router.


    string sEntertainArea;          //current entertain area int
    int iPK_User;                    //current user
    int iFK_Room;                    //current room
    QString s_onOFF;

    long iOrbiterPluginID;           //the orbiter plugin id for future use
    QStringList *sRoomList;          //Linked list of rooms in house
    QStringList *sUserList;          //linked list of users in house
    QStringList *sCurr_Room_Devices; //linked list of current devices (experimental)

    int CurrentScreen();

signals:
      void orbiterReady();
      void orbiterClosing();

public slots:
      //ui related slots
   Q_INVOKABLE bool gotoQScreen(QString s) ;
        //qt c++ related slots
    bool requestDataGrid(QString s);
    //dce related slots
    Q_INVOKABLE void closeOrbiter();
};

#endif // QORBITERMANAGER_H
