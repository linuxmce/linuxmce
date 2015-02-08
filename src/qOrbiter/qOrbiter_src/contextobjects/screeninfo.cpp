#include "screeninfo.h"
#include "QGuiApplication"
#include "QDebug"
ScreenInfo::ScreenInfo(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<ScreenObject*>("ScreenObject");
m_primaryScreen = new ScreenObject();
    foreach (QScreen *screen, QGuiApplication::screens()){
        ScreenObject *newScreen = new ScreenObject(
                    screen->logicalDotsPerInch(),
                    screen->physicalDotsPerInch(),
                    screen->orientation(),
                    screen->name()
                );

        m_screenList.insert(screen->name(), newScreen);
        qDebug() << Q_FUNC_INFO << " Processed screen" << newScreen->screenName();
    }

    if(!m_screenList.isEmpty()){
        m_primaryScreen=m_screenList.first();
        emit primaryScreenChanged();
    }

}
