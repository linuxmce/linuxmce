#include "NvrManager.h"
#include "../cameraClasses/abstractpicamera.h"
#include "qlist.h"
#include "qdebug.h"
#include <qdatetime.h>
#include "../cameraClasses/motionpicamera.h"
#include "../Linuxmce_NVR.h"
#include "qdebug.h"
#include "qdir.h"
#include "qimage.h"
NvrManager::NvrManager(QObject *parent) : QObject(parent)
{
    verbose=false;
    log(QString(Q_FUNC_INFO)+"::Started");



}

NvrManager::NvrManager(int listenPort, QObject *parent) :QObject(parent)
{

    startUp(listenPort);
    QImage waitImage;
    waitImage.load(":/resources/please_wait.png");
    if(waitImage.isNull())
        qDebug() << "failed to load wait image";
    else
        waitImage.save("/tmp/please_wait.png", "PNG");
}

int NvrManager::cameraCount()
{
    return cam_list.count();
}

void NvrManager::addCamera(NvrCameraBase *c)
{

    cam_list.append(c);
    c->setManager(this);
    emit cameraCountChanged();

}

void NvrManager::log(QString logMsg)
{
    QString outMsg = QDateTime::currentDateTime().toLocalTime().toString()+logMsg;
    //mp_Linuxmce_NVR->handleManagerMessage(outMsg);
    emit newManagerMessage(outMsg);

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
    Q_UNUSED(listenPort);
}

std::string NvrManager::getImageFrame(int camId)
{

    for(int t=0;t<cam_list.length();t++){
        if(cam_list.at(t)->dceDeviceId()==camId){
            NvrCameraBase * camera = cam_list.at(t);
            return camera->getImage();
        }
    }
}



void NvrManager::setDetectionStatus(bool enabled, int camId)
{
    for(int t=0;t<cam_list.length();t++){
        if(cam_list.at(t)->dceDeviceId()==camId){
            NvrCameraBase * camera = cam_list.at(t);
            camera->setMotionEnabled(enabled);
        }
    }
}

