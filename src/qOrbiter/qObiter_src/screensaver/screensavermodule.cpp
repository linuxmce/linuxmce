#include "screensavermodule.h"

ScreenSaverModule::ScreenSaverModule(QObject *parent) :
    QObject(parent)
{
    QUrl *m_imgUrl = new QUrl("../../img/lmcesplash.jpg");
}
