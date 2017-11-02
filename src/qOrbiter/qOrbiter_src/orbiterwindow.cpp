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
#include <qquickitem.h>
#include <QtQml/QQmlEngine>
#include <QtWidgets/QApplication>
#include <contextobjects/screeninfo.h>
#include "QQmlApplicationEngine"
#include "qqmlfileselector.h"

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

orbiterWindow::orbiterWindow(int deviceid, std::string routerip, bool fullScreen, bool frameless, int simScreenSize, QQmlApplicationEngine *engine, bool isOsd, QObject *parent) :
    QObject(parent),
    deviceno(deviceid), router(QString::fromStdString(routerip)), internalIp("0.0.0.0"), externalIp("0.0.0.0"),
    usingExternal(false),
    phoneSize(false),  localPath(""),  newOrbiter(false),
    b_connectionPresent(false), b_localConfigReady(false),
    b_orbiterConfigReady(false), b_devicePresent(false),
    b_skinDataReady(false), b_skinIndexReady(false),
    b_reloadStatus(false), fullScreenOrbiter(fullScreen),
    m_bIsOsd(isOsd),
    m_appEngine(engine)
{
    m_appEngine->rootContext()->setContextProperty("orbiterWindow", this);
#ifdef ANDROID
    m_appEngine->addImportPath("assets:/imports/androidComponents");
    m_appEngine->addPluginPath(QDir::homePath()+"/../lib");
    m_appEngine->addPluginPath("assets:/lib");
#endif
    m_appEngine->addPluginPath("lib");
    m_appEngine->addPluginPath("imports");

    m_appEngine->addImportPath(QApplication::applicationDirPath()+"/imports");
    m_appEngine->addImportPath("lib");
    m_appEngine->addImportPath("qml");

    QQmlFileSelector *winSelector = new QQmlFileSelector(m_appEngine->rootContext()->engine());
    if(isOsd){
        winSelector->setExtraSelectors(QStringList() << "md" );
    }

    m_appEngine->rootContext()->setContextProperty("window", this);
#ifdef NOQRC
    qrcPath="../qOrbiter_src/qml/Index.qml";
#else
    qrcPath="qrc:/qml/Index.qml";
#endif
    // mainView.setSource( qrcPath ); /* Sets the initial qml file based on all the above switching */
    int testH =600;
    int testW = 800;

    if(simScreenSize!=-1){
        qDebug() << "Using screen size " << simScreenSize;
        switch (simScreenSize) {
        case ScreenData::Device_Small:
            testW=480;
            testH=854;
            mainView.setGeometry(0,0,480*.75, 854*.75);
             winSelector->setExtraSelectors(QStringList() << "small");
            break;
        case ScreenData::Device_Medium:
            testW=1280;
            testH=720;
            mainView.setGeometry(0,0,1280*.65, 720*.65);
             winSelector->setExtraSelectors(QStringList() << "medium");
            break;
        case ScreenData::Device_Large:
            testW=1980;
            testH=1080;
            mainView.setGeometry(0,0,1920*.65, 1080*.65);
             winSelector->setExtraSelectors(QStringList() << "large");
            break;
        default:
            testW=800;
            testH=600;
            mainView.setGeometry(0,0,800*.65, 600*.65);
            break;
        }

    } else {
#ifdef ANDROID

#elif defined(Q_OS_IOS)

#else
        if(fullScreen){

        } else {
            testW=800;
            testH=600;
          mainView.setGeometry(0,0,800*.65, 600*.65);
        }
#endif
    }

    mainView.rootContext()->setContextProperty("appW", mainView.width());
    mainView.rootContext()->setContextProperty("appH", mainView.height());

    userList.append(new PromptData("No Users",0));
    roomList.append(new PromptData("No Rooms",0));

    mainView.rootContext()->setContextProperty("users", QVariant::fromValue(userList));
    mainView.rootContext()->setContextProperty("rooms", QVariant::fromValue(roomList));
    mainView.rootContext()->setContextProperty("localPath", localPath);

    m_appEngine->rootContext()->setContextProperty("users", QVariant::fromValue(userList));
    m_appEngine->rootContext()->setContextProperty("rooms", QVariant::fromValue(roomList));
    m_appEngine->rootContext()->setContextProperty("localPath", localPath);

#if defined (GLENABLED) || (QT5)
    fileReader = new FileReader();
    m_appEngine->rootContext()->setContextProperty("fileReader", fileReader);
#endif


#ifndef QT5
    mainView.setWindowTitle("LinuxMCE QOrbiter ");
#else
    mainView.setTitle("LinuxMCE QOrbiter");
#endif

}

/*!
 * \brief orbiterWindow::initView This function is used to provide any special handling needed for window setup on start.
 */
void orbiterWindow::initView(){


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
void orbiterWindow::displayPromptResponse(int type, QList<PromptData*> *pList)
{
#ifdef QT_DEBUG
    qDebug() << "Prompt for response" << type;
#endif
    userList.clear();
    if(type==1){
        userList.clear();
        for(int i=0; i < pList->length(); i++){
            PromptData *p = pList->at(i);
            userList.append( new PromptData(p->dataTitle,p->data_id) );
        }

        m_appEngine->rootContext()->setContextProperty("users", QVariant::fromValue(userList));
    }
    else
    {
        roomList.clear();
        for(int i=0; i < pList->length(); i++){
            PromptData *p = pList->at(i);
            roomList.append( new PromptData(p->dataTitle,p->data_id) );
        }

        m_appEngine->rootContext()->setContextProperty("rooms", QVariant::fromValue(roomList));
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
void orbiterWindow::setupNewOrbiter(int user, int room, int skin, int lang, int height, int w, QString deviceName)
{
#ifdef QT_DEBUG
    qDebug() << "Setup message recieved";
#endif
    emit newOrbiterData(user, room, skin, lang, height, w, deviceName);
}



