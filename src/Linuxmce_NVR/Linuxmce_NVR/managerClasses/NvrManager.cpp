#include "NvrManager.h"
#include "../cameraClasses/abstractpicamera.h"
#include "qlist.h"
#include "qdebug.h"
#include <qdatetime.h>
#include "../cameraClasses/motionpicamera.h"
#include "../Linuxmce_NVR.h"

NvrManager::NvrManager(DCE::Linuxmce_NVR *dceObject, QObject *parent) : QObject(parent) , mp_Linuxmce_NVR(dceObject)
{
    verbose=false;

    log(QString(Q_FUNC_INFO)+"::Started");
    listener = new MotionEventListener();
}

int NvrManager::cameraCount()
{
    return cam_list.count();
}

void NvrManager::addCamera(AbstractNvrCamera *c)
{
    cam_list.append(c);
    c->setManager(this);
    emit cameraCountChanged();

}

void NvrManager::log(QString logMsg)
{
    QString outMsg = QDateTime::currentDateTime().toLocalTime().toString()+logMsg;
    mp_Linuxmce_NVR->handleManagerMessage(outMsg);

    if(verbose)
        qDebug()<< logMsg;
}

void NvrManager::shutDown()
{
    log(QString(Q_FUNC_INFO)+"::shutting down");
    exit(0);
}

void NvrManager::startUp(int listenPort)
{
    listener->setListenPort(listenPort);
}

