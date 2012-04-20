#include "securityvideoclass.h"

#include <QUrl>

SecurityVideoClass::SecurityVideoClass(QObject *parent) :
    QObject(parent)
{
    currentFrame.load(":/icons/security.png");
    if (currentFrame.isNull())
    {

    }
}

