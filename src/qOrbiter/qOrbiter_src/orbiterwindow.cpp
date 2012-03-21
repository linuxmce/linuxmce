/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
   Langston Ball  golgoj4@gmail.com
    QOrbiter is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QOrbiter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QOrbiter.  If not, see <http://www.gnu.org/licenses/>.

    orbiterwindow.h/cpp is specifically for spawning the qml window independant of the manager, allowing us to update it and operatate
    upon it indepentant of the manager object. It can recieves status signals and other information before all the subsequent objects are in place,
    removing the lag time from the user starting the application and the app appearing on screen.
*/

#include "orbiterwindow.h"
#include <QObject>
#include <QDeclarativeView>
#include <qdeclarative.h>
#include <contextobjects/existingorbiter.h>
#include <QApplication>

#ifdef IOS
#include "../iOS/qOrbiter/ioshelpers.h"
#endif

orbiterWindow::orbiterWindow(long deviceid, std::string routerip, QObject *parent) :
    QObject(parent)
{

    newOrbiter = false;
    this->b_connectionPresent = false;
    this->b_localConfigReady = false;
    this->b_orbiterConfigReady = false;
    this->b_skinDataReady = false;
    this->b_skinIndexReady = false;
    this->b_devicePresent = false;

    //qDebug() << mainView.size();
    router = routerip;
    deviceno = deviceid;

    mainView.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    mainView.rootContext()->setContextProperty("window", this);
    mainView.setWindowTitle("LinuxMCE Orbiter ");
    mainView.rootContext()->setContextProperty("orbiterList" , "");
    mainView.setViewport(glWidget);
    mainView.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);


    //QObject::connect(&mainView, SIGNAL(sceneResized(QSize)), this, SIGNAL(orientationChanged(QSize)));


#ifdef ANDROID
    mainView.rootContext()->setContextProperty("appW", (mainView.window()->width()/ 2));//this is done because android reports the desktop width as 2x what it is.at least on my phone
    mainView.rootContext()->setContextProperty("appH", mainView.window()->height());
#elif for_android

    mainView.rootContext()->setContextProperty("appW", 1280);
    mainView.rootContext()->setContextProperty("appH", 720);
#elif for_desktop
    mainView.rootContext()->setContextProperty("appW", 1280);
    mainView.rootContext()->setContextProperty("appH", 720);
#else
    mainView.rootContext()->setContextProperty("appW", 1280);
    mainView.rootContext()->setContextProperty("appH", 720);
#endif

    mainView.rootContext()->setContextProperty("deviceid", int(deviceno));
    mainView.rootContext()->setContextProperty("srouterip", QString::fromStdString(router));


#ifdef for_desktop
    buildType = "/qml/desktop";
    qrcPath = "qrc:desktop/Splash.qml";
#elif defined (for_freemantle)
    buildType = "/qml/freemantle";
    qrcPath = "qrc:freemantle/Splash.qml";
#elif defined (WIN32)
    buildType="/qml/desktop";
    qrcPath = "qrc:desktop/Splash.qml";
#elif defined (for_harmattan)
    buildType="/qml/harmattan";
    qrcPath = "qrc:harmattan/Splash.qml";
#elif defined (IOS)
    buildType="/qml/desktop";
    NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
    qrcPath = qStringFromNSString([resourcePath stringByAppendingPathComponent:@"qml/Splash.qml"]);
#elif defined(Q_OS_MACX)
    buildType="/qml/desktop";
    qrcPath = "qrc:osx/Splash.qml";
#elif defined ANDROID
    qrcPath = "qrc:android/Splash.qml";
    mainView.window()->setAttribute(Qt::WA_LockPortraitOrientation);
#elif defined for_android
    buildType = "/qml/android";
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
    mainView.showNormal();
#elif defined(ANDROID)
    mainView.showMaximized();
#elif defined(for_android)
    mainView.show();
#else
    mainView.showNormal();
    // mainView.setResizeMode(QDeclarativeView::SizeRootObjectToView);
#endif

}

void orbiterWindow::setMessage(QString imsg)
{
    // QApplication::processEvents(QEventLoop::AllEvents);
    message = imsg; emit MessageChanged();
    //qDebug() << "Orbiter window output:" << imsg;
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
    router = ip.toStdString();
    deviceno = device.toLong();
    mainView.rootContext()->setContextProperty("deviceid", int(deviceno));
    mainView.rootContext()->setContextProperty("srouterip", QString::fromStdString(router));
    emit setupLmce(device, ip);
}

bool orbiterWindow::getOrbiterState()
{
    return newOrbiter;
}

void orbiterWindow::showSplash()
{
  mainView.setSource(QUrl(qrcPath));
}

void orbiterWindow::setSkinDataState(bool b)
{
     b_skinDataReady = b; emit connectionChanged();
}

void orbiterWindow::setSkinIndexState(bool b)
{
     b_skinIndexReady = b; emit skinIndexStatus();
}

void orbiterWindow::setOrbiterConfigState(bool b)
{
     b_orbiterConfigReady = b; emit orbiterConfigStatus();
}

void orbiterWindow::setLocalConfigState(bool b)
{
     b_localConfigReady = b; emit configStatus();
}

void orbiterWindow::setOrbiterState(bool state)
{
    newOrbiter = state;
    emit StatusChanged();
}

void orbiterWindow::showSetup()
{
    mainView.setSource(QUrl(":main/SetupNewOrbiter.qml"));
}

void orbiterWindow::setConnectionState(bool b)
{
    b_connectionPresent = b; emit connectionChanged();
}

void orbiterWindow::setDeviceState(bool b)
{
    b_devicePresent = b; emit deviceChanged();
}

void orbiterWindow::prepareExistingOrbiters(QList<QObject *> ex_list)
{
    orbiterList = ex_list;
    mainView.rootContext()->setContextProperty("orbiterList", QVariant::fromValue(orbiterList));
    emit showList();
}


