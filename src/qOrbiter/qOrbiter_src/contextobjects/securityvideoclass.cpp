#include "securityvideoclass.h"

#include <QUrl>
#include <qdebug.h>
#include <qOrbiter/qOrbiter.h>

SecurityVideoClass::SecurityVideoClass(QObject *qOrbiter_ptr, QObject *parent) :
    QObject(parent)
{
    qOrbiter * ptr = qobject_cast<qOrbiter*>(qOrbiter_ptr);

    QObject::connect(ptr, &qOrbiter::securityImageReady,this, &SecurityVideoClass::setCameraImage,Qt::QueuedConnection);

    currentFrame.load(":/icons/security.png");
    if (currentFrame.isNull())
    {
        currentFrame=QImage();
    }
}

QString SecurityVideoClass::getTimeStamp()
{
    return QTime::currentTime().toString();
}

void SecurityVideoClass::setTimeStamp(QString t)
{
    timestamp = QTime::currentTime().toString();
    emit imageUpdated();

}

void SecurityVideoClass::setCameraImage(int cam, QImage img)
{
    qDebug() << Q_FUNC_INFO << getTimeStamp();
    if(cameras.find(cam).key()==cam)
    {
        cameras.find(cam).value() = img;
    }
    else{
        cameras.insert(cam, img);
    }
    currentFrame = img;
    emit imageUpdated();

}

QImage SecurityVideoClass::getCameraImage(int cam)
{
    {
        if(cameras.find(cam).key()==cam)
        {
            return cameras.find(cam).value();
        }
        else{
            return QImage();
        }

    }
}

