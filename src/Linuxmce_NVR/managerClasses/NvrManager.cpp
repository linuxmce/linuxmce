#include "NvrManager.h"
#include "../cameraClasses/abstractpicamera.h"
#include "qlist.h"
#include "qdebug.h"
#include <qdatetime.h>
#include "../cameraClasses/motionpicamera.h"


NvrManager::NvrManager(QObject *parent) : QObject(parent)
{
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
    qDebug() <<QDateTime::currentDateTime().toLocalTime().toString() << logMsg;
}

void NvrManager::shutDown()
{
    log(QString(Q_FUNC_INFO)+"::shutting down");
    exit(0);
}

