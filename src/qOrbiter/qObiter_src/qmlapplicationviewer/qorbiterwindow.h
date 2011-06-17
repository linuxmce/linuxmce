#ifndef QORBITERWINDOW_H
#define QORBITERWINDOW_H

#include <QMainWindow>
#include <QDeclarativeView>
#include <qOrbiter/qOrbiter.h>
#include <Gen_Devices/AllCommandsRequests.h>


namespace DCE
{
class qOrbiter;
}

class qOrbiterWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit qOrbiterWindow(QWidget *parent = 0);
    bool initialize(int dev_id);

    DCE::qOrbiter * pqOrbiter;

    char *pData;
    int *iSize;
    bool bAppError;
    bool bReload;
    long iPK_Device;
    string srouterIP;
    bool dceBool;
    bool bLocalMode;

    int i_EA;                        //current entertain area int
    string sCurr_user;                   //current user
    string s_Room;                      //current room
    long iOrbiterPluginID;          //the orbiter plugin id for future use


    bool setupLmce(int PK_Device, string sRouterIP, bool, bool bLocalMode);

signals:
    void orbiterReady();

public slots:
    void sendMessage(QString Qmsg);

private:
    QDeclarativeView  * qorbiterUI;


    bool refreshUI();

};

#endif // QORBITERWINDOW_H
