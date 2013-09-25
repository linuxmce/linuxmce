#include "mediamanager.h"
#include <QThread>
#include <QTcpSocket>
#include <QProcess>
#include <QGraphicsScene>
#include <QtOpenGL/QGLWidget>
#include <QGraphicsView>
using namespace DCE;


/*!
 * \brief MediaManager::MediaManager
 * \param parent
 * \ingroup audio_visual
 */
MediaManager::MediaManager(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
    setFlag(ItemHasNoContents, false);
    serverAddress = "";
    deviceNumber = -1;
    timeCodeServer = new QTcpServer();
    flipColors = false;

    setCurrentStatus("Media Manager defaults set, initializing media engines");
#ifdef QT4

    setAvailibleOutputs(Phonon::BackendCapabilities::availableAudioOutputDevices());
    qDebug() << outputs;
    qDebug() << Phonon::BackendCapabilities::availableMimeTypes();

    mediaObject = new Phonon::MediaObject();
    videoSurface = new Phonon::VideoWidget();
    audioSink = new Phonon::AudioOutput();
    discController = new Phonon::MediaController(mediaObject);


    Phonon::createPath(mediaObject, audioSink);
    setCurrentStatus("Audio Sink Initialized");
    Phonon::createPath(mediaObject, videoSurface);
    setCurrentStatus("Video Player Initialized");

    videoSurface->setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);
    videoSurface->setScaleMode(Phonon::VideoWidget::FitInView);


    if(initViews(true))
        setCurrentStatus("Color Filter Applied.");

#endif
    setCurrentStatus("Window Initialized");
    totalTime=0;
    setMediaBuffer(0);
    setMediaPlaying(false);
}

void MediaManager::initializePlayer()
{
    setCurrentStatus("Initializing Media Player");
    if(!mediaPlayer){
        mediaPlayer = new qMediaPlayer(deviceNumber, serverAddress.toStdString(), this, true, false);
    }

    initializeConnections();
}

void MediaManager::initializeConnections()
{
    /*setup*/


    /*From Dce MediaPlayer*/
    QObject::connect(mediaPlayer,SIGNAL(currentMediaUrlChanged(QString)), this, SLOT(setMediaUrl(QString)));
    QObject::connect(mediaPlayer,SIGNAL(startPlayback()), mediaObject, SLOT(play()));
    QObject::connect(mediaPlayer, SIGNAL(startPlayback()), this, SLOT(mediaStarted()));
    QObject::connect(mediaPlayer, SIGNAL(stopCurrentMedia()), mediaObject, SLOT(stop()));
    QObject::connect(mediaPlayer, SIGNAL(stopCurrentMedia()), this, SLOT(stopTimeCodeServer()));
    QObject::connect(mediaPlayer, SIGNAL(pausePlayback()), mediaObject, SLOT(pause()));
    QObject::connect(mediaPlayer,SIGNAL(commandResponseChanged(QString)), this ,SLOT(setCurrentStatus(QString)));
    QObject::connect(mediaPlayer,SIGNAL(setZoomLevel(QString)), this, SLOT(setZoomLevel(QString)));
    QObject::connect(mediaPlayer,SIGNAL(streamIdChanged(int)), this , SLOT(setStreamId(int)));
    QObject::connect(mediaPlayer, SIGNAL(mediaIdChanged(QString)), this, SLOT(setFileReference(QString)));
    QObject::connect(mediaPlayer, SIGNAL(startPlayback()), this, SLOT(startTimeCodeServer()));
    QObject::connect(mediaPlayer, SIGNAL(startPlayback()), videoSurface, SLOT(showFullScreen()));



    QObject::connect(mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(setState(Phonon::State,Phonon::State)));

    QObject::connect(mediaPlayer, SIGNAL(connectionStatusChanged(bool)), this, SLOT(setConnectionStatus(bool)));
    QObject::connect(mediaPlayer,SIGNAL(jumpToStreamPosition(int)), this, SLOT(setMediaPosition(int)));

    /*From internal plugin*/
    QObject::connect(mediaObject, SIGNAL(finished()), mediaPlayer, SLOT(mediaEnded()));
    QObject::connect(mediaObject, SIGNAL(tick(qint64)), this, SLOT(processTimeCode(qint64)));
    QObject::connect(mediaObject, SIGNAL(finished()), videoSurface, SLOT(lower()));
    QObject::connect(mediaObject, SIGNAL(totalTimeChanged(qint64)), this, SLOT(setTotalTime(qint64)));
    QObject::connect(mediaObject, SIGNAL(bufferStatus(int)), this, SLOT(setMediaBuffer(int)));
    QObject::connect(mediaObject, SIGNAL(aboutToFinish()), this, SIGNAL(mediaAboutToFinish()));
    QObject::connect(mediaObject, SIGNAL(prefinishMarkReached(qint32)), this ,SLOT(setPrefinishMarkHit(qint32)));
    QObject::connect(mediaObject,SIGNAL(metaDataChanged()), this, SLOT(updateMetaData()));



    /*internals*/
    QObject::connect(audioSink, SIGNAL(volumeChanged(qreal)), this, SLOT(setVolume(qreal)));
    QObject::connect(audioSink, SIGNAL(mutedChanged(bool)), this, SLOT(setMuted(bool)));

#ifdef QT4
    mediaObject->setTickInterval(quint32(1000));
#elif QT5

#endif

}

void MediaManager::setConnectionDetails(int r, QString s)
{
    setServerAddress(s);
    setDeviceNumber(r);


    if(!serverAddress.isEmpty() && deviceNumber > 1)
    {
        if(!mediaPlayer){
            setCurrentStatus("Got address "+serverAddress+" and device number "+QString::number(deviceNumber)+", initializing");
            initializePlayer();
        }

    }else
    {
        setCurrentStatus("Error in setup information");
        qDebug() << serverAddress <<"::" << deviceNumber;
    }

}

void MediaManager::newClientConnected()
{
    setCurrentStatus("New Client Connected::");
    if(timeCodeServer->hasPendingConnections()){
        QTcpSocket *lastSocket=timeCodeServer->nextPendingConnection();
        if(lastSocket->isValid()){
            clientList.append(lastSocket);
        }
    }

}

void MediaManager::startTimeCodeServer()
{

    setCurrentStatus("Staring timecode server on port 12000");
    timeCodeServer->listen(QHostAddress::Any,12000);
    QObject::connect(timeCodeServer, SIGNAL(newConnection()), this , SLOT(newClientConnected()));

    if(mediaObject->errorString() =="")
    {

    }

}

void MediaManager::stopTimeCodeServer()
{
    for(int d = 0; d<clientList.length(); d++){
        QTcpSocket *c = clientList.at(d);
        c->disconnectFromHost();
    }
    timeCodeServer->close();
    setMediaPlaying(false);
}

void MediaManager::setZoomLevel(QString zoom)
{
    setCurrentStatus("Implement zoom level-template::"+zoom);
}

void MediaManager::setAspectRatio(QString aspect)
{
    setCurrentStatus("Implement aspect ratio-template::"+aspect);

}

void MediaManager::getScreenShot()
{

    QImage screenShot =  videoSurface->snapshot();


}


void MediaManager::setMediaUrl(QString url)
{
    setCurrentStatus("Got New Media Url::"+url);
    filepath=url;

    if(url.endsWith(".ISO")){

        QString mountProg = "gksudo";
        QStringList args;

        args.append(QString("mount -o loop "+url+" /mnt/remote/dvd"));
        QProcess *mountProcess = new QProcess(this);
        mountProcess->start(mountProg, args);
        mountProcess->waitForFinished(10000);
        qDebug() << "Process Status ::" <<mountProcess->state();
        if(mountProcess->state()== QProcess::FailedToStart){
            qWarning() << "command failed to start!";
            qDebug() << mountProcess->readAllStandardError();
            qDebug() << mountProcess->errorString();
        }

        mediaObject->setCurrentSource(Phonon::MediaSource(Phonon::Dvd, "/mnt/remote/dvd/VIDEO_TS"));
    }else{
          mediaObject->setCurrentSource(Phonon::MediaSource(url));
    }


    setCurrentStatus(QString("MediaObject Source::"+mediaObject->currentSource().fileName()));
    qDebug() <<"Media Object Source::"<< mediaObject->currentSource().type();
    qDebug() <<"Media Object Source::"<< mediaObject->currentSource().fileName();


    mediaObject->play();
    qDebug() <<"Item is playing? " << mediaObject->state();
    qDebug() << "Errors " << mediaObject->errorString();
    qDebug() << discController->currentTitle();

}


void MediaManager::processTimeCode(qint64 f)
{
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

    QString timeCodeTick = "0/"+QString::number(1000)+","+t+","+qs_totalTime+","+QString::number(streamId)+",0,0,"+fileReference+","+QString::number(fileno)+","+filepath;
    transmit(timeCodeTick);

    //  setCurrentStatus("Current position::" +QString::number(displayHours) + ":" + QString::number(minutes) + ":" +QString::number(forseconds));
}


void MediaManager::transmit(QString d)
{
    QByteArray chunk;
    QDataStream out(&chunk, QIODevice::WriteOnly);
    //out << (quint16)0;
    out << d.toAscii(); //.toStdString().c_str();
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


void MediaManager::mountDrive(int device)
{
    mountProcess = new QProcess();
    QObject::connect(mountProcess, SIGNAL(readyRead()), this, SLOT(setCurrentStatus(QString)));
    mountProcess->execute("gksudo  mount 192.168.80.1:/mnt/device/121 /mnt/device/121");
    qDebug() << mountProcess->state();
}

bool MediaManager::initViews(bool flipped)
{
    filterProxy = new ColorFilterProxyWidget(this);

    if(flipColors){
        filterProxy->invert = true;
    }
    else{
        filterProxy->invert = false;
    }

    filterProxy->setWidget(videoSurface);
    filterProxy->setAutoFillBackground(false);
    filterProxy->hide();
    return true;

}
