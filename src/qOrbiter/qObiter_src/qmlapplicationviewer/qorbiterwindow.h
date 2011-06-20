#ifndef QORBITERWINDOW_H
#define QORBITERWINDOW_H

#include <QMainWindow>
#include <qOrbiter/qOrbiter.h>
#include <uiclasses/uicontroller.h>
#include <Command_Impl.h>
#include <Gen_Devices/AllCommandsRequests.h>
#include <qorbitermanager.h>


namespace DCE
{
class qOrbiter;
}

class qOrbiterWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit qOrbiterWindow(QWidget *parent = 0);
    ~qOrbiterWindow();
    Q_INVOKABLE QDateTime getCurrentDateTime() const { return QDateTime::currentDateTimeUtc();}

    QString currentSkin;
    QString currentSkinURL;

    //class objects
    DCE::qOrbiter * pqOrbiter;
    qorbiterManager *qmlManager;


    //QT Functions to initialize lmce data
    bool initialize(int dev_id);
    bool setupLmce(int PK_Device, string sRouterIP, bool, bool bLocalMode);
    bool getConf(int pPK_Device);
    bool OrbiterGen();

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

    long iPK_Device;                //this orbiters device number, passed in from command line
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


public slots:
    void sendMessage(QString Qmsg);
     void swapSkins();

private:

    bool refreshUI();

};

#endif // QORBITERWINDOW_H
