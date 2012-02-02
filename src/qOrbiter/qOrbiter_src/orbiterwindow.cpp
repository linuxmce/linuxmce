#include "orbiterwindow.h"
#include <QObject>
#include <QDeclarativeView>
#include <qdeclarative.h>
#include <QDebug>
#include <QApplication>

#ifdef IOS
#include "../iOS/qOrbiter/ioshelpers.h"
#endif

orbiterWindow::orbiterWindow(QObject *parent) :
    QObject(parent)
{
    newOrbiter = false;

    mainView.rootContext()->setContextProperty("window", this);
    mainView.setWindowTitle("LinuxMCE Orbiter ");
    mainView.setResizeMode(QDeclarativeView::SizeViewToRootObject);


#ifdef for_desktop
    buildType = "/qml/desktop";
    qrcPath = "qrc:desktop/Splash.qml";
#elif defined (for_freemantle)
    buildType = "/qml/freemantle";
    qrcPath = "qrc:freemantle/Splash.qml";
#elif defined (for_harmattan)
    buildType="/qml/harmattan";
    qrcPath = "qrc:harmattan/Splash.qml";
#elif defined (IOS)
    buildType="/qml/desktop";
    NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
    qrcPath = qStringFromNSString([resourcePath stringByAppendingPathComponent:@"qml/Splash.qml"]);
#elif defined (Q_OS_MACX)
    buildType="/qml/desktop";
    qrcPath = "qrc:osx/Splash.qml";
#elif defined (ANDROID)
    buildType = "/qml/android";
    qrcPath = "qrc:android/Splash.qml";
#elif defined (for_droid)
    buildType = "/qml/android/phone";
    qrcPath = "qrc:android/Splash.qml";
#else
    buildType = "/qml/desktop";
    qrcPath = "qrc:desktop/Splash.qml";
#endif

    mainView.setSource(QUrl(qrcPath));

#ifdef Q_OS_SYMBIAN
    mainView.showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    mainView.showMaximized();
#elif defined(for_harmattan)
    mainView.showFullScreen();
#elif defined(for_desktop)
    mainView.showMaximized();
#elif defined(ANDROID)
    mainView.showFullScreen();
#else
    mainView.show();
#endif


}

void orbiterWindow::setMessage(QString imsg)
{
    // QApplication::processEvents(QEventLoop::AllEvents);
    message = imsg; emit MessageChanged();
    qDebug() << "Orbiter window output:" << imsg;
}

void orbiterWindow::forceResponse(QString forced)
{

}

QString orbiterWindow::getMessage()
{
    return message;
}

void orbiterWindow::qmlSetupLmce(QString device, QString ip)
{
    emit setupLmce(device, ip);

}

bool orbiterWindow::getOrbiterState()
{
    return newOrbiter;
}

void orbiterWindow::setOrbiterState(bool state)
{
    newOrbiter = state;
    emit StatusChanged();
}


