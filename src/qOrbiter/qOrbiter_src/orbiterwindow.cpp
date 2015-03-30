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
#include <QtOpenGL/QGLFormat>
#if (QT5)
#include <QtQml/QtQml>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtWidgets/QApplication>
#include <contextobjects/screeninfo.h>


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

orbiterWindow::orbiterWindow(int deviceid, std::string routerip, bool fullScreen, bool frameless, int simScreenSize, QObject *parent) :
    QObject(parent),
    deviceno(deviceid), router(QString::fromStdString(routerip)), internalIp("0.0.0.0"), externalIp("0.0.0.0"),
    usingExternal(false),
    phoneSize(false),  localPath(""),  newOrbiter(false),
    b_connectionPresent(false), b_localConfigReady(false),
    b_orbiterConfigReady(false), b_devicePresent(false),
    b_skinDataReady(false), b_skinIndexReady(false),
    b_reloadStatus(false), fullScreenOrbiter(fullScreen)
{
#ifdef ANDROID
    mainView.engine()->addImportPath("assets:/imports/androidComponents");
    mainView.engine()->addPluginPath(QDir::homePath()+"/../lib");
    mainView.engine()->addPluginPath("assets:/lib");
#endif
    mainView.engine()->addPluginPath("lib");
    mainView.engine()->addPluginPath("imports");

    mainView.engine()->addImportPath(QApplication::applicationDirPath()+"/imports");
    mainView.engine()->addImportPath("lib");
    mainView.engine()->addImportPath("qml");

    qDebug() << "Qt Import Paths::"<<mainView.engine()->importPathList();
#ifdef QT4_8
#ifndef ANDROID
    mainView.setAttribute(Qt::WA_TranslucentBackground);
    QPalette palette;
    palette.setColor(QPalette::Base, Qt::transparent);
    mainView.setPalette(palette);
    mainView.setAutoFillBackground(false);
#endif
    if(frameless==true){
        mainView.window()->setWindowFlags(Qt::FramelessWindowHint);
    }
#elif QT5
#ifndef  ANDROID
#ifndef Q_OS_IOS
//    mainView.setSurfaceType(QSurface::OpenGLSurface);

//    QSurfaceFormat format;
//    format.setAlphaBufferSize(8);
//    format.setRenderableType(QSurfaceFormat::OpenGL);

//    mainView.setFormat(format);
//    mainView.setColor(QColor(Qt::transparent));
//    mainView.setClearBeforeRendering(true);
#endif
#endif
    if(frameless == true){
        mainView.setFlags(Qt::FramelessWindowHint);
    }
#endif

    mainView.setResizeMode(QQuickView::SizeRootObjectToView);
    QObject::connect(&mainView, SIGNAL(sceneResized(QSize)), this, SIGNAL(orientationChanged(QSize)));
    mainView.rootContext()->setContextProperty("window", this);
    qrcPath="qrc:/qml/qml/Index.qml";
    mainView.setSource(qrcPath); /* Sets the initial qml file based on all the above switching */
    if(simScreenSize!=-1){
        switch (simScreenSize) {
        case ScreenData::Device_Small:
            mainView.setWidth(480);
            mainView.setHeight(854);
            break;
        case ScreenData::Device_Medium:
            mainView.setWidth(1280);
            mainView.setHeight(720);
            break;
        case ScreenData::Device_Large:
            mainView.setWidth(1600);
            mainView.setHeight(900);
        default:
            mainView.setWidth(800);
            mainView.setHeight(600);
            break;
        }
        mainView.showNormal();
    } else {

#ifdef ANDROID
        mainView.showFullScreen();
#elif defined(Q_OS_IOS)
        mainView.showFullScreen();
#else
        mainView.setWidth(800);
        mainView.setHeight(600);
        mainView.showNormal();
#endif
    }
    mainView.rootContext()->setContextProperty("appW", mainView.width());
    mainView.rootContext()->setContextProperty("appH", mainView.height());

    userList.append(new PromptData("No Users",0));
    roomList.append(new PromptData("No Rooms",0));
    mainView.rootContext()->setContextProperty("users", QVariant::fromValue(userList));
    mainView.rootContext()->setContextProperty("rooms", QVariant::fromValue(roomList));
    mainView.rootContext()->setContextProperty("localPath", localPath);
#if defined (GLENABLED) || (QT5)
    fileReader = new FileReader();
    mainView.rootContext()->setContextProperty("fileReader", fileReader);
#endif


#ifndef QT5
    mainView.setWindowTitle("LinuxMCE QOrbiter ");
#else
    mainView.setTitle("LinuxMCE QOrbiter");
#endif
    //  mainView.rootContext()->setContextProperty("orbiterList" , "");

#ifdef GLENABLED
#ifndef Q_OS_ANDROID
#ifndef QT5

    //        QGLFormat format= QGLFormat::defaultFormat();
    //        format.setAlpha(true);
    //        glWidget = new QGLWidget(format);
    //        glWidget->setStyleSheet("background:transparent;");
    //        glWidget->setAutoFillBackground(false);
    //        mainView.setViewport(glWidget);
    //        glWidget->setAttribute(Qt::WA_TranslucentBackground, true);
    //        glWidget->setBackgroundRole(QPalette::Base);
    //        mainView.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
#endif

#endif
#endif

#ifdef RPI
   // mainView.setSurfaceType(QSurface::OpenGLSurface);

    qDebug() << "is opengl? " << mainView.openglContext();
    qDebug() << "surface type " << mainView.surfaceType();
    //mainView.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
#endif
    //    #if defined (QT4) && defined (ANDROID)
    //        QGLFormat format= QGLFormat::defaultFormat();
    //        glWidget = new QGLWidget();
    //        glWidget->setAutoFillBackground(false);
    //        mainView.setViewport(glWidget);
    //        mainView.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    //    #endif

    //window sizing
}

/*!
 * \brief orbiterWindow::initView This function is used to provide any special handling needed for window setup on start.
 */
void orbiterWindow::initView(){
    //mainView.showMaximized();

}

void orbiterWindow::setMessage(QString imsg){
    // QApplication::processEvents(QEventLoop::AllEvents);
    message = imsg; emit MessageChanged();
    //qDebug() << "Orbiter window output:" << imsg;
}

void orbiterWindow::forceResponse(QString forced)
{

}

void orbiterWindow::loadSetupPage(){
    mainView.setSource(QUrl("qrc:main/SetupNewOrbiter.qml"));
}

QString orbiterWindow::getMessage(){
    return message;
}

void orbiterWindow::qmlSetupLmce(int device, QString ip){
    qDebug() <<"Orbiter Window Settings.\n Device::"<<device<<"\n Router::"<<ip;
    emit setupLmce(device, ip);
    router=ip;
}

bool orbiterWindow::getOrbiterState(){
    return newOrbiter;
}

/*!
 * \brief orbiterWindow::showSplash Shows the initial loading page, which then loads the splash.
 */
void orbiterWindow::showSplash(){
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
    emit beginSetupNewOrbiter();
    emit showList();
}

void orbiterWindow::setConnectionState(bool b)
{
    b_connectionPresent = b; emit connectionChanged();
}

void orbiterWindow::setDeviceState(bool b)
{
    b_devicePresent = b; emit deviceChanged();
}

void orbiterWindow::prepareExistingOrbiters(){
    qDebug()<< "showing List";
    emit showList();
}

/*!
 * \brief orbiterWindow::displayPromptResponse Used for initial setup. Works differently than original implementation in Orbiter classic.
 * \param type
 * \param pList
 */
void orbiterWindow::displayPromptResponse(int type, QList<QObject*> pList)
{
#ifdef QT_DEBUG
    qDebug() << "Prompt for response" << type;
#endif
    userList.clear();
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
    emit showList();
}

/*!
 * \brief orbiterWindow::setupNewOrbiter Sets up a new orbiter
 * \param user
 * \param room
 * \param skin
 * \param lang
 * \param height
 * \param w
 */
void orbiterWindow::setupNewOrbiter(int user, int room, int skin, int lang, int height, int w)
{
#ifdef QT_DEBUG
    qDebug() << "Setup message recieved";
#endif
    emit newOrbiterData(user, room, skin, lang, height, w);
}



