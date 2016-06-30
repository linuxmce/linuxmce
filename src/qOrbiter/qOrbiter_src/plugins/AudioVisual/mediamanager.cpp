/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
    Author: Langston Ball
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
*/
#include "mediamanager.h"
#include <QThread>
#include <QTcpSocket>
#include <QProcess>
#include <QDir>
#include <QQuickItem>
#include <QImage>




using namespace DCE;


/*!
 * \brief MediaManager::MediaManager
 * \param parent
 * \ingroup audio_visual
 */

MediaManager::MediaManager(QQuickItem *parent): QQuickItem(parent)

{
    serverAddress = "";
    deviceNumber = -1;
    timeCodeServer = new QTcpServer();
    flipColors = false;
    lastTick = "";
    mediaPlayer = NULL;
    setCurrentStatus("Media Manager defaults set, initializing media engines");
    totalTime=0;
    setMediaBuffer(0);
    setMediaPlaying(false);
#ifndef ANDROID
    setupDirectories();
#endif
}

void MediaManager::initializePlayer()
{

    if(deviceNumber==-1){
        return;
    }
    setCurrentStatus("Initializing Media Player "+ QString::number(deviceNumber));

    if(!mediaPlayer){
      //  mediaPlayer = new qMediaPlayer(deviceNumber, serverAddress.toStdString(), this, true, false);
    }

    initializeConnections();
}

void MediaManager::initializeConnections()
{
    /*setup*/


    /*From Dce MediaPlayer*/

    QObject::connect(mediaPlayer, SIGNAL(currentMediaUrlChanged(QString)), this, SLOT(setFileReference(QString))); //effectively play for android.
    QObject::connect(mediaPlayer, SIGNAL(stopCurrentMedia()), this, SLOT(stopPluginMedia()));
    QObject::connect(mediaPlayer, SIGNAL(pausePlayback()), this, SLOT(setPaused()));


   // QObject::connect(mediaPlayer, SIGNAL(currentMediaUrlChanged(QString)), this, SLOT(setFileReference(QString)));

    QObject::connect(mediaPlayer, SIGNAL(pluginVolumeDown()), this, SIGNAL(pluginVolumeDown()));
    QObject::connect(mediaPlayer, SIGNAL(pluginVolumeUp()), this, SIGNAL(pluginVolumeUp()));    
    QObject::connect(mediaPlayer, SIGNAL(stopCurrentMedia()), this, SLOT(stopPluginMedia()));


    QObject::connect(mediaPlayer,SIGNAL(jumpToStreamPosition(int)), this, SLOT(setMediaPosition(int)));
    QObject::connect(mediaPlayer, SIGNAL(newMediaPosition(int)), this, SLOT(setMediaPosition(int)));

    QObject::connect(mediaPlayer, SIGNAL(connectionStatusChanged(bool)), this, SLOT(setConnectionStatus(bool)));
    QObject::connect(mediaPlayer, SIGNAL(startPlayback()), this, SLOT(mediaStarted()));
    QObject::connect(mediaPlayer, SIGNAL(stopCurrentMedia()), this, SLOT(stopTimeCodeServer()));
    QObject::connect(mediaPlayer,SIGNAL(commandResponseChanged(QString)), this ,SLOT(setCurrentStatus(QString)));
    QObject::connect(mediaPlayer,SIGNAL(setZoomLevel(QString)), this, SLOT(setZoomLevel(QString)));
    QObject::connect(mediaPlayer,SIGNAL(streamIdChanged(int)), this , SLOT(setStreamId(int)));
    QObject::connect(mediaPlayer, SIGNAL(startPlayback()), this, SLOT(startTimeCodeServer()));
    setCurrentStatus("Connections initialized.");
}

void MediaManager::setConnectionDetails(int r, QString s)
{
    setServerAddress(s);
    setDeviceNumber(r);

    if(!serverAddress.isEmpty() && deviceNumber > 0){
        setCurrentStatus("Got address "+serverAddress+" and device number "+QString::number(deviceNumber)+", initializing");
        initializePlayer();
    }else{
        setCurrentStatus("Error in setup information");
        qDebug() << serverAddress <<"::" << deviceNumber;
    }

}

void MediaManager::newClientConnected(){

    if(timeCodeServer->hasPendingConnections()){
        QTcpSocket *lastSocket=timeCodeServer->nextPendingConnection();
        if(lastSocket->isValid()){
            setCurrentStatus("New Client Connected from"+lastSocket->peerAddress().toString());
            clientList.append(lastSocket);
        }
    }

}

void MediaManager::callBackClientConnected()
{
#ifndef QT5
    if(tcCallback->hasPendingConnections()){
        callbackClient = tcCallback->nextPendingConnection();
        if(callbackClient->isValid()){
            setCurrentStatus("Callback client connected.");
            QObject::connect(callbackClient, SIGNAL(readyRead()), this, SLOT(forwardCallbackData()),Qt::QueuedConnection);
        }
    }
#endif
}

void MediaManager::startTimeCodeServer(){

    if(timeCodeServer->isListening()){
        setCurrentStatus("TimeCodeServer already listening on port "+timeCodeServer->serverAddress().toString()+":"+QString::number(timeCodeServer->serverPort()));
        return;
    }
    setCurrentStatus("Starting timecode server on port 12000");
    timeCodeServer->listen(QHostAddress(QString::fromStdString(mediaPlayer->DATA_Get_TCP_Address()) ),12000);
    QObject::connect(timeCodeServer, SIGNAL(newConnection()), this , SLOT(newClientConnected()));

}

void MediaManager::stopTimeCodeServer()
{
    lastTick="";
    for(int d = 0; d<clientList.length(); d++){
        QTcpSocket *c = clientList.at(d);
        c->disconnectFromHost();
    }
    timeCodeServer->close();
    setMediaPlaying(false);
    incomingTime=0;
}

void MediaManager::forwardCallbackData(){

#ifndef QT5
    QString callbackTc = callbackClient->readAll();
    if(!callbackTc.isEmpty() && callbackTc.length() > 2){
        int f = callbackTc.indexOf("|", 0);
        callbackTc.remove(0,f);
        callbackTc.remove("|");
        setCurrentStatus("time recieved from ipc::"+callbackTc);
        processTimeCode(callbackTc.toInt());

    }
#endif
}

void MediaManager::setZoomLevel(QString zoom)
{
    setCurrentStatus("Implement zoom level-template::"+zoom);


}

void MediaManager::setAspectRatio(QString aspect)
{
    setCurrentStatus("Implement aspect ratio-template::"+aspect);

}


QImage MediaManager::getScreenShot(){
#if defined (QT4) && ! defined (ANDROID)
    return videoSurface->snapshot();
#else
    QImage screenShot;
    return screenShot;
#endif


}


void MediaManager::setMediaUrl(QString url)
{

    setCurrentStatus("Got New Media Url::"+url);
    filepath=url;
    fileUrl=url;
    emit fileUrlChanged();

#if defined (QT4) && ! defined (ANDROID) //only for phonon

    QString mediaPath;
    if(url.toLower().endsWith(".iso",Qt::CaseInsensitive)||url.toLower().endsWith(".dvd", Qt::CaseInsensitive)){
        mediaPath="/mnt/remote/dvd";
    } else {
        mediaPath = "/mnt/remote/"+url;
    }

    qDebug() << "Current media path::"<< mediaPath;
    //    QString mountProg = "gksudo";
    //    QStringList args;

    //    args.append(QString("mount -t nfs -o loop "+url+" /mnt/remote/dvd/"));
    //    QProcess *mountProcess = new QProcess(this);
    //    mountProcess->start(mountProg, args);
    //    mountProcess->waitForFinished(10000);
    //    qDebug() << "Process Status ::" <<mountProcess->state();
    //    if(mountProcess->state()== QProcess::FailedToStart){
    //        qWarning() << "command failed to start!";
    //        qDebug() << mountProcess->readAllStandardError();
    //        qDebug() << mountProcess->errorString();
    //    }

    if(url.toLower().endsWith(".iso",Qt::CaseInsensitive)||url.toLower().endsWith(".dvd", Qt::CaseInsensitive)){
        mediaObject->setCurrentSource(Phonon::MediaSource(Phonon::Dvd, url));
        qDebug() << discController->currentTitle();
    }else{
        mediaObject->setCurrentSource(Phonon::MediaSource(url));

    }

    setCurrentStatus(QString("MediaObject Source::"+mediaObject->currentSource().fileName()));
    qDebug() <<"Media Object Source::"<< mediaObject->currentSource().type();
    qDebug() <<"Media Object Source::"<< mediaObject->currentSource().fileName();

    mediaObject->play();
    if(mediaObject->hasVideo()){
        setVideoStream(true);
    } else {
        setVideoStream(false);
    }
    qDebug() <<"Item is playing? " << mediaObject->state();
    qDebug() << "Errors " << mediaObject->errorString();
    discController->setAutoplayTitles(true);

#endif

}


void MediaManager::processTimeCode(qint64 f)
{

    if(!f ){
        f=0;
        return;
    }
    /*
     *
     *socket template - comma dilineated
     *"1000,00:01:05.899,00:22:17.668,1007,0,0,F,147209,/home/public/data/videos/MediaDrive [121]/musicVideos/electronic/Eminence - E65 Podcast (Trance Edition).mp4
     * param 1) speed, x 1000 ? ie 1000 2000 4000 16000 32000 etc
     * param 2) curent time in this breakdown- HH:MM:SS:MSEC
     * param 3) total time - breakdown- HH:MM:SS:MSEC
     * param 4) stream id
     * param 5) ? number, example was 0
     * param 6) ? number, provided example was 0
     * param 7) F in the case of stored media? investigation is required
     * param 8) correlates to 7 in some sense as its the linuxmce file number for the file in param 9
     * param 9) the full path of the file, including the linuxmce sub path (home/public/data)
     */
    int seconds = f / 1000;
    int displayHours = seconds / (3600);
    int remainder = seconds % 3600;
    int minutes = remainder / 60;
    int forseconds = remainder % 60;

    QString hrs = QString::number(displayHours);
    if(hrs.length()==1)
        hrs.prepend("0");

    QString min = QString::number(minutes);
    if(min.length()==1)
        min.prepend("0");

    QString sec = QString::number(forseconds);
    if(sec.length()==1)
        sec.prepend("0");

    currentTime =f;

    QString t = hrs + ":" + min + ":" +sec;

    if(t=="::"){
        qWarning() << Q_FUNC_INFO << " Has no time set, adjusting to 00:00:00";
        t="00:00:00";
    }
    QString timeCodeTick = "0/"+QString::number(1000)+","+t+","+qs_totalTime+","+QString::number(streamId)+",0,0,"+fileReference+","+QString::number(fileno)+","+filepath;

    transmit(timeCodeTick);
    current_position=timeCodeTick;
   mediaPlayer->positionChanged(qs_totalTime, t);

    //  setCurrentStatus("Current position::" +QString::number(displayHours) + ":" + QString::number(minutes) + ":" +QString::number(forseconds));
}


void MediaManager::playbackInfoUpdated(QString mediaTitle, QString mediaSubTitle, QString name, int screen)
{
    qDebug() << Q_FUNC_INFO ;
    //mediaPlayer->updateMetadata(playbackData);
    mediaPlayer->updateMetadata(mediaTitle, mediaSubTitle, name, screen);
}


void MediaManager::transmit(QString d)
{
    lastTick =d;
    QByteArray chunk;
    QDataStream out(&chunk, QIODevice::WriteOnly);
    //out << (quint16)0;
    out << d.toLocal8Bit(); //.toStdString().c_str();
    out.device()->reset();
    out << (quint16)(chunk.size() -sizeof(quint16));

    if(!clientList.isEmpty()){
        for(int s = 0; s< clientList.length(); s++)
        {

            QTcpSocket *tr = clientList.at(s);
            tr->write(chunk);
        }
    }
}


bool MediaManager::mountDrive(long device){
    return false;
    /*
    qDebug() << "Mounting Media storage device==>" <<device;

    QString dirCmd = "";
#ifndef RPI
    dirCmd  = "gksudo";
#endif

    QDir mntDir;
    mntDir.setPath("/mnt/remote/"+QString::number(device));
    qDebug() << "Mount directory exists? " << mntDir.exists();
    if(!mntDir.exists()){
        qDebug() << "Attempting to create dir " << mntDir.path();
        QProcess *mkPath = new QProcess();
        QStringList dArgs;
        dArgs.append("mkdir -p /mnt/remote/"+QString::number(device)+"");
        mkPath->start(dirCmd, dArgs);
        mkPath->waitForFinished(10000);
        while(mkPath->state()==QProcess::Running){
            qDebug() << "Response to command::" << mkPath->readAllStandardOutput();
            qDebug() << "Error Response to command::" << mkPath->readAllStandardError();
        }
    }

    if(!mntDir.exists()){
        qWarning() << "Couldnt create path " << mntDir.path();
        return false;
    }

    qWarning() << mntDir.entryList().count();
    if(mntDir.exists() && mntDir.entryList().length() ==2){
        qWarning() << mntDir.path() << " is not mounted, mounting";

        QString mountProg = "gksudo";
#ifdef RPI
        mountProg="";
#endif
        qWarning() << "Current Device " << currentDevice;
        QString mntString = "mount -t nfs "+serverAddress+":/mnt/device/"+QString::number(currentDevice)+" "+mntDir.path()+" -o vers=3";
        QStringList args;
        args.append( mntString);
        QProcess *mountProcess = new QProcess();
        mountProcess->start(mountProg, args);
        mountProcess->waitForFinished(10000);

        qDebug() << "Process Status ::" <<mountProcess->state();
        if(mountProcess->error() == QProcess::FailedToStart){
            qWarning() << "command failed to start!";
            qDebug() << mountProcess->readAllStandardError();
            qDebug() << mountProcess->errorString();
        }
        qWarning() << "QProcess Exiting, state is :"<< mountProcess->state();
        qWarning() << "Process exited with::"<< mountProcess->exitCode();
        return true;
    }
    else{
        qWarning() <<  mntDir.path() << " is mounted. done";
    }
    */
}


void MediaManager::setupDirectories()
{
    /*
    QString dirCmd = "gksudo";
    QDir dvdDir;
    dvdDir.setPath("/mnt/remote/dvd");
    qDebug() << dvdDir.exists();
#ifdef RPI
    dirCmd="";
#endif
    if(!dvdDir.exists()){
        QProcess *mkPath = new QProcess(this);
        QStringList dArgs;
        dArgs.append("mkdir -p /mnt/remote/dvd");
        mkPath->start(dirCmd, dArgs);
        mkPath->waitForFinished(10000);
    }

    dirCmd = "";
#ifndef RPI
    dirCmd  = "gksudo";
#endif
    QDir mntDir;
    mntDir.setPath("/mnt/remote/");
    qDebug() << "Mount directory exists? " << mntDir.exists();
    if(!mntDir.exists()){
        QProcess *mkPath = new QProcess(this);
        QStringList dArgs;
        dArgs.append("mkdir -p /mnt/remote/");
        mkPath->start(dirCmd, dArgs);
        mkPath->waitForFinished(10000);
        while(mkPath->state()==QProcess::Running){
            qDebug() << mkPath->readAllStandardOutput();
        }
    }
    */
}



