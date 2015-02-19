#include "screeninfo.h"
#include "QtGui/qguiapplication.h"
#include "QDebug"
ScreenInfo::ScreenInfo(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<ScreenObject*>("ScreenObject");

    m_primaryScreen = new ScreenObject();
    foreach (QScreen *screen, QGuiApplication::screens()){
        ScreenObject *newScreen = new ScreenObject(
                    screen->name(),
                    screen->logicalDotsPerInch(),
                    screen->physicalDotsPerInch(),
                    screen->orientation(),  screen->primaryOrientation(), screen->nativeOrientation(),
                    screen->physicalSize().height(), screen->physicalSize().width(),
                    screen->availableSize().height(), screen->availableSize().width()
                    );

        connect(screen, SIGNAL(refreshRateChanged(qreal)), newScreen, SLOT(setRefreshRate(qreal)));
        connect(screen, SIGNAL(orientationChanged(Qt::ScreenOrientation)), newScreen, SLOT(setOrientation(Qt::ScreenOrientation)));
        connect(screen, SIGNAL(geometryChanged(QRect)), newScreen, SLOT(setScreenGeometry(QRect)));

        newScreen->setRefreshRate(screen->refreshRate());
        newScreen->setScreenGeometry(screen->geometry());

        m_screenList.insert(screen->name(), newScreen);
        qDebug() << Q_FUNC_INFO << " Processed screen" << newScreen->screenName();
    }

    if(!m_screenList.isEmpty()){
        m_primaryScreen=m_screenList.first();
        emit primaryScreenChanged();
    }

}
