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


#include <QtCore/QObject>
#include <contextobjects/existingorbiter.h>
#include <contextobjects/promptdata.h>
#include <QtCore/QDir>
#include <shaders/filereader.h>
#include <QThread>

#if (QT5)
#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtWidgets/QApplication>
#else
#include <qmlapplicationviewer.h>
#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/QDeclarativeEngine>
#include <QApplication>
#endif


#include <QDebug>


#include "orbiterwindow.h"

#ifdef IOS
#include "../iOS/qOrbiter/ioshelpers.h"
#endif

orbiterWindow::orbiterWindow(long deviceid, std::string routerip, QObject *parent) :
    QObject(parent)
{
    phoneSize = false;
    localPath = "";
    newOrbiter = false;
    setConnectionState(false);
    setLocalConfigState(false);
    setOrbiterConfigState(false);
    setSkinDataState(false);
    setSkinIndexState(false);
    setDeviceState(false);
    setReloadStatus(false);
    //    this->b_connectionPresent = false;
    //    this->b_localConfigReady = false;
    //    this->b_orbiterConfigReady = false;
    //    this->b_skinDataReady = false;
    //    this->b_skinIndexReady = false;
    //    this->b_devicePresent = false;

    //qDebug() << mainView.size();
    router = routerip;
    deviceno = deviceid;

    userList.append(new PromptData("No Users",0));
    roomList.append(new PromptData("No Rooms",0));

    mainView.engine()->addImportPath("imports");

    mainView.rootContext()->setContextProperty("users", QVariant::fromValue(userList));
    mainView.rootContext()->setContextProperty("rooms", QVariant::fromValue(roomList));
    mainView.rootContext()->setContextProperty("localPath", localPath);

#if defined (GLENABLED) || (QT5)
    fileReader = new FileReader();
     mainView.rootContext()->setContextProperty("fileReader", fileReader);
#endif

#if (QT5)
    mainView.setResizeMode(QQuickView::SizeRootObjectToView);
#else
    mainView.setResizeMode(QDeclarativeView::SizeRootObjectToView);
#endif
    mainView.rootContext()->setContextProperty("window", this);
    mainView.setWindowTitle("LinuxMCE Orbiter ");
  //  mainView.rootContext()->setContextProperty("orbiterList" , "");

#ifdef GLENABLED
#ifdef QT4
    glWidget = new QGLWidget();
    mainView.setViewport(glWidget);
    mainView.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
#else

//    glWidget = new QGLWidget();
//    mainView.setsetViewport(glWidget);
//    mainView.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
//#elif for_android

//#elif __ANDROID__
////    glWidget = new QGLWidget();
////    mainView.setViewport(glWidget);
//    mainView.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
#endif
#endif
    // QObject::connect(&mainView, SIGNAL(sceneResized(QSize)), this, SIGNAL(orientationChanged(QSize)));

#ifdef __ANDROID__
    mainView.rootContext()->setContextProperty("appW", (mainView.window()->width()/ 2));//this is done because android reports the desktop width as 2x what it is.at least on my phone
    mainView.rootContext()->setContextProperty("appH", mainView.window()->height());
#elif for_android
    mainView.rootContext()->setContextProperty("appW", 1280);
    mainView.rootContext()->setContextProperty("appH", 720);
#elif for_desktop
    mainView.rootContext()->setContextProperty("appW", 1280);
    mainView.rootContext()->setContextProperty("appH", 720);
#else
    mainView.rootContext()->setContextProperty("appW", 800);
    mainView.rootContext()->setContextProperty("appH", 600);
#endif

    mainView.rootContext()->setContextProperty("deviceid", int(deviceno));
    mainView.rootContext()->setContextProperty("srouterip", QString::fromStdString(router));

#ifdef for_desktop
#ifndef QT5
    buildType = "/qml/desktop";
 #else
    buildType = "/qml/rpi";
#endif
    qrcPath = buildType+"/Splash.qml";
    localPath = "desktop/";
#elif defined (for_freemantle)
    buildType = "/qml/freemantle";
    qrcPath = "qrc:freemantle/Splash.qml";
    localPath = "freemantle/";
#elif defined (WIN32)
    buildType="/qml/desktop";
    qrcPath = "qrc:desktop/Splash.qml";
    localPath = "desktop/";
#elif defined (for_harmattan)
    buildType="/qml/harmattan";
    qrcPath = "qrc:harmattan/Splash.qml";
    localPath = "harmattan/";
#elif defined (IOS)
    buildType="/qml/desktop";
    NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
    qrcPath = qStringFromNSString([resourcePath stringByAppendingPathComponent:@"qml/Splash.qml"]);
    localPath = "desktop/";
#elif defined(Q_OS_MACX)
    buildType="/qml/desktop";
    qrcPath = "qrc:osx/Splash.qml";
    localPath = "desktop/";
#elif defined __ANDROID__
    qrcPath = "qrc:android/Splash.qml";
localPath = "android/";
#elif defined for_android
    buildType = "/qml/android";
    qrcPath = "qrc:android/Splash.qml";
    localPath = "android/";
#elif defined RPI
    buildType = "/qml/rpi";
    qrcPath = "qrc:android/Splash.qml";
    localPath = "rpi/";
#else
    buildType = "/qml/desktop";
    qrcPath = "qrc:desktop/Splash.qml";

#endif
       #ifdef Q_OS_ANDROID
    mainView.addImportPath("assets:/imports/androidComponents");
    mainView.engine()->addPluginPath(QDir::homePath()+"/../lib");
    mainView.rootContext()->setBaseUrl(QUrl::fromLocalFile("/"));
    mainView.setMainQmlFile("qml/Base.qml");
#elif !for_harmattan
    mainView.setSource(QApplication::applicationDirPath().remove("/bin")+buildType+"/Splash.qml");
 #elif for_harmattan
     mainView.setSource(QApplication::applicationDirPath()+"qml/Splash.qml");
#endif

#ifdef Q_OS_SYMBIAN
    mainView.showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    mainView.showMaximized();
#elif defined(for_harmattan)
    mainView.showFullScreen();
#elif defined(for_desktop)
    mainView.show();
#elif defined(__ANDROID__)
    mainView.showFullScreen();
#elif defined(for_android)
    mainView.show();
#elif RPI
     mainView.showFullScreen();
#else
    mainView.showNormal();
    // mainView.setResizeMode(QDeclarativeView::SizeRootObjectToView);
#endif

    qDebug() << mainView.thread()->currentThread();
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

void orbiterWindow::loadSetupPage()
{
    mainView.setSource(QUrl("qrc:main/SetupNewOrbiter.qml"));
}

QString orbiterWindow::getMessage()
{
    return message;
}

void orbiterWindow::qmlSetupLmce(QString device, QString ip)
{
    router = ip.toStdString();
    deviceno = device.toLong();
//    mainView.rootContext()->setContextProperty("deviceid", int(deviceno));
//    mainView.rootContext()->setContextProperty("srouterip", QString::fromStdString(router));
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
    b_skinDataReady = b; emit skinDataLoaded();
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
    emit setupNewOrbiter();
}

void orbiterWindow::setConnectionState(bool b)
{
    b_connectionPresent = b; emit connectionChanged();
}

void orbiterWindow::setDeviceState(bool b)
{
    b_devicePresent = b; emit deviceChanged();
}

void orbiterWindow::prepareExistingOrbiters(QList<QObject*> ex_list)
{
    QList<QObject*> t = ex_list;
    mainView.rootContext()->setContextProperty("orbiterList", QVariant::fromValue(t));
  emit showList();
}

void orbiterWindow::displayPromptResponse(int type, QList<QObject*> pList)
{
#ifdef QT_DEBUG
    qDebug() << "Prompt for response" << type;
#endif
    if(type==1){
        userList.clear();
        userList = pList;
       mainView.rootContext()->setContextProperty("users", QVariant::fromValue(userList));
    }
    else
    {
        roomList.clear();
        roomList = pList;
        mainView.rootContext()->setContextProperty("rooms", QVariant::fromValue(roomList));
    }
}

void orbiterWindow::setupNewOrbiter(int user, int room, int skin, int lang, int height, int w)
{
#ifdef QT_DEBUG
    qDebug() << "Setup message recieved";
#endif
    emit newOrbiterData(user, room, skin, lang, height, w);
}



