#include "securityvideoclass.h"
#include <QDebug>
#include <QUrl>

SecurityVideoClass::SecurityVideoClass(QObject *parent) :
    QObject(parent)
{
    currentFrame.load(":/icons/security.png");
    if (currentFrame.isNull())
    {
        //qDebug() << "default security failed to load";
    }
}
