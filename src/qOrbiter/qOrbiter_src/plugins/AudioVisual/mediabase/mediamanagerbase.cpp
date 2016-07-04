#include "mediamanagerbase.h"
#include <QThread>
#include <QTcpSocket>
#include <QProcess>
#include <QDir>
#include <QQuickItem>
#include <QImage>


MediaManagerBase::MediaManagerBase()
{
    usingExternalMediaPlayer = false;
    m_serverAddress = "";
    m_deviceNumber = -1;
    timeCodeServer = new QTcpServer();
    m_lastTick = "";
    mediaPlayer = NULL;
    setCurrentStatus("Media Manager defaults set, initializing media engines");
    m_totalTimeNumeric=0;
    setMediaBuffer(0);
    setMediaPlaying(false);
}
int MediaManagerBase::deviceNumber() const
{
    return m_deviceNumber;
}

void MediaManagerBase::setDeviceNumber(int deviceNumber)
{
    qDebug() << Q_FUNC_INFO << deviceNumber;
    m_deviceNumber = deviceNumber;
    emit deviceNumberChanged();
}
int MediaManagerBase::streamId() const
{
    return m_streamId;
}

void MediaManagerBase::setStreamId(int streamId)
{
    m_streamId = streamId;
    emit streamIdChanged();
}
int MediaManagerBase::fileNo() const
{
    return m_fileNo;
}

void MediaManagerBase::setFileNo(int fileNo)
{
    m_fileNo = fileNo;
    emit fileNoChanged();
}
QString MediaManagerBase::fileReference() const
{
    return m_fileReference;
}

void MediaManagerBase::stopPluginMedia()
{
    setMediaPlaying(false);
}

void MediaManagerBase::mediaStarted()
{
    setMediaPlaying(true);
}

void MediaManagerBase::setFileReference(const QString &fileReference)
{
    m_fileReference = fileReference;
    emit fileReferenceChanged();

}

void MediaManagerBase::togglePause()
{
    m_paused = !m_paused;
    emit pauseChanged();

}
QString MediaManagerBase::filePath() const
{
    return m_filePath;
}

void MediaManagerBase::setFilePath(const QString &filePath)
{
    m_filePath = filePath;
    emit filePathChanged();
}
int MediaManagerBase::playbackSpeed() const
{
    return m_playbackSpeed;
}

void MediaManagerBase::setPlaybackSpeed(int playbackSpeed)
{
    m_playbackSpeed = playbackSpeed;
    emit playbackSpeedChanged();
}
int MediaManagerBase::mediaBuffer() const
{
    return m_mediaBuffer;
}

void MediaManagerBase::setMediaBuffer(int mediaBuffer)
{
    m_mediaBuffer = mediaBuffer;
    emit mediaBufferChanged();
}
bool MediaManagerBase::mediaPlaying() const
{
    return m_mediaPlaying;
}

void MediaManagerBase::setMediaPlaying(bool mediaPlaying)
{
    m_mediaPlaying = mediaPlaying;
    emit mediaPlayingChanged();
}
QString MediaManagerBase::lastTick() const
{
    return m_lastTick;
}

void MediaManagerBase::setLastTick(const QString &lastTick)
{
    m_lastTick = lastTick;
    emit lastTickChanged();
}
qreal MediaManagerBase::volume() const
{
    return m_volume;
}

void MediaManagerBase::setVolume(const qreal &volume)
{
    m_volume = volume;
    emit volumeChanged();
}
qreal MediaManagerBase::displayVolume() const
{
    return m_displayVolume;
}

void MediaManagerBase::setDisplayVolume(const qreal &displayVolume)
{
    m_displayVolume = displayVolume;
    emit displayVolumeChanged();
}
bool MediaManagerBase::muted() const
{
    return m_muted;
}

void MediaManagerBase::setMuted(bool muted)
{
    m_muted = muted;
    emit mutedChanged();
}
QString MediaManagerBase::currentStatus() const
{
    return m_currentStatus;
}

void MediaManagerBase::setCurrentStatus(const QString &currentStatus)
{
    m_currentStatus = currentStatus;
    emit currentStatusChanged();
}
bool MediaManagerBase::hasError() const
{
    return m_hasError;
}

void MediaManagerBase::setHasError(bool hasError)
{
    m_hasError = hasError;
    emit hasErrorChanged();
}
QString MediaManagerBase::lastError() const
{
    return m_lastError;
}

void MediaManagerBase::setLastError(const QString &lastError)
{
    m_lastError = lastError;
    emit lastErrorChanged();
}
QString MediaManagerBase::totalTime() const
{
    return m_totalTime;
}

void MediaManagerBase::setTotalTime(const QString &totalTime)
{
    m_totalTime = totalTime;
    emit totalTimeChanged();
}

QString MediaManagerBase::currentPosition() const
{
    return m_currentPosition;
}

void MediaManagerBase::setCurrentPosition(const QString &currentPosition)
{
    m_currentPosition = currentPosition;
    emit currentPositionChanged();
}
int MediaManagerBase::currentPositionNumeric() const
{
    return m_currentPositionNumeric;
}

void MediaManagerBase::setCurrentPositionNumeric(int currentPositionNumeric)
{
    m_currentPositionNumeric = currentPositionNumeric;
    emit currentPositionNumericChanged();
}
int MediaManagerBase::currentTimeNumeric() const
{
    return m_currentTimeNumeric;
}

void MediaManagerBase::setCurrentTimeNumeric(int currentTimeNumeric)
{
    m_currentTimeNumeric = currentTimeNumeric;
    emit currentPositionNumericChanged();
}
quint64 MediaManagerBase::totalTimeNumeric() const
{
    return m_totalTimeNumeric;
}

void MediaManagerBase::setTotalTimeNumeric(const quint64 &totalTimeNumeric)
{
    m_totalTimeNumeric = totalTimeNumeric;
    emit totalTimeNumericChanged();
}
QString MediaManagerBase::serverAddress() const
{
    return m_serverAddress;
}

void MediaManagerBase::setServerAddress(const QString &serverAddress)
{
    m_serverAddress = serverAddress;
    emit serverAddressChanged();
}
QString MediaManagerBase::pluginUrl() const
{
    return m_pluginUrl;
}

void MediaManagerBase::setPluginUrl(const QString &pluginUrl)
{   
    m_pluginUrl = pluginUrl;
    emit pluginUrlChanged();
}
bool MediaManagerBase::connected() const
{
    return m_connected;
}

void MediaManagerBase::setConnected(bool connected)
{
    m_connected = connected;
    emit connectedChanged();
}
int MediaManagerBase::videoHeight() const
{
    return m_videoHeight;
}

void MediaManagerBase::setVideoHeight(int videoHeight)
{
    m_videoHeight = videoHeight;
    emit videoHeightChanged();
}
int MediaManagerBase::videoWidth() const
{
    return m_videoWidth;
}

void MediaManagerBase::setVideoWidth(int videoWidth)
{
    m_videoWidth = videoWidth;
    emit videoWidthChanged();
}
bool MediaManagerBase::hasVideoStream() const
{
    return m_hasVideoStream;
}

void MediaManagerBase::setHasVideoStream(bool hasVideoStream)
{
    m_hasVideoStream = hasVideoStream;
    emit hasVideoStreamChanged();
}

void MediaManagerBase::setMediaPosition(int msec)
{
    updatePluginSeek(msec);
}

void MediaManagerBase::setZoomLevel(QString zoom)
{
    m_zoomLevel = zoom;
    emit zoomLevelChanged();
}

void MediaManagerBase::setConnectionDetails(int device, QString routerAdd)
{
    setServerAddress(routerAdd);
    setDeviceNumber(device);

    if(!m_serverAddress.isEmpty() && m_deviceNumber > 0){
        setCurrentStatus("Got address "+m_serverAddress+" and device number "+QString::number(m_deviceNumber)+", initializing");
        initializePlayer();
    }else{
        setCurrentStatus("Error in setup information");
        qDebug()<<Q_FUNC_INFO << m_serverAddress <<"::" << m_deviceNumber << "::INVALID";
    }
}

void MediaManagerBase::qmlPlaybackEnded(bool ended)
{
    mediaPlayer->EVENT_Playback_Completed(mediaPlayer->getInternalMediaUrl().toStdString(),mediaPlayer->getStreamID(), ended);
}

void MediaManagerBase::setQmlTotalTime(int inSeconds)
{
    setCurrentStatus("Android Time call in Audio Visual Plugin::"+QString::number(inSeconds));
    int s = inSeconds ;

    int seconds = s / 1000;
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

    m_totalTime =hrs + ":" + min + ":" +sec;
    m_totalTimeNumeric = inSeconds;
    qDebug() << Q_FUNC_INFO << m_totalTime;
    emit totalTimeChanged();
}

void MediaManagerBase::setTotalTimeFromPlugin(quint64 inSeconds)
{
    setCurrentStatus("Android Time call in Audio Visual Plugin::"+QString::number(inSeconds));
    int s = inSeconds ;

    int seconds = s / 1000;
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

    m_totalTime =hrs + ":" + min + ":" +sec;
    m_totalTimeNumeric = s;
    qDebug() << Q_FUNC_INFO << m_totalTime;
    emit totalTimeChanged();

}

void MediaManagerBase::reInit()
{
    setConnected(false);
    mediaPlayer->deleteLater();
    mediaPlayer=NULL;
    initializePlayer();
}

void MediaManagerBase::pluginNotifyStart(QString title, QString audioOptions, QString videoOptions)
{
    startTimeCodeServer();
    mediaPlayer->EVENT_Playback_Started(
                mediaPlayer->getInternalMediaUrl().toStdString(),
                mediaPlayer->i_StreamId,
                title.toStdString(),
                audioOptions.toStdString(),
                videoOptions.toStdString());
}

void MediaManagerBase::pluginNotifyEnd(bool withError)
{
    mediaPlayer->EVENT_Playback_Completed(mediaPlayer->getInternalMediaUrl().toStdString(), mediaPlayer->i_StreamId, withError);
}

void MediaManagerBase::playbackInfoUpdated(QString mediaTitle, QString mediaSubTitle, QString name, int screen)
{
    mediaPlayer->updateMetadata(mediaTitle, mediaSubTitle, name, screen);
}

void MediaManagerBase::stopTimeCodeServer()
{
    m_lastTick="";
    for(int d = 0; d<clientList.length(); d++){
        QTcpSocket *c = clientList.at(d);
        c->disconnectFromHost();
    }
    timeCodeServer->close();
    setMediaPlaying(false);
    // incomingTime=0;
}

void MediaManagerBase::startTimeCodeServer()
{
    if(timeCodeServer->isListening()){
        setCurrentStatus("TimeCodeServer already listening on port "+timeCodeServer->serverAddress().toString()+":"+QString::number(timeCodeServer->serverPort()));
        return;
    }
    setCurrentStatus("Starting timecode server on port 12000");
    timeCodeServer->listen(QHostAddress(QString::fromStdString(mediaPlayer->DATA_Get_TCP_Address()) ),12000);
    QObject::connect(timeCodeServer, SIGNAL(newConnection()), this , SLOT(onNewClientConnected()));
}

void MediaManagerBase::processTimeCode(quint64 time)
{

    if(!time ){
        time=0;
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
    int seconds = time / 1000;
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

    m_currentPositionNumeric =time;

    QString t = hrs + ":" + min + ":" +sec;

    if(t=="::"){
        qWarning() << Q_FUNC_INFO << " Has no time set, adjusting to 00:00:00";
        t="00:00:00";
    }
    QString timeCodeTick = "0/"+QString::number(1000)+","+t+","+m_totalTime+","+QString::number(m_streamId)+",0,0,"+m_fileReference+","+QString::number(m_fileNo)+","+m_filePath;

    transmit(timeCodeTick);
    m_currentPosition=timeCodeTick;
    mediaPlayer->positionChanged(m_totalTime, t);

    //  setCurrentStatus("Current position::" +QString::number(displayHours) + ":" + QString::number(minutes) + ":" +QString::number(forseconds));
}

void MediaManagerBase::initializePlayer()
{
    if(m_deviceNumber==-1){
        return;
    }
    setCurrentStatus("Initializing Media Player "+ QString::number(m_deviceNumber));

    if(!mediaPlayer){
        mediaPlayer = new qMediaPlayer(m_deviceNumber, m_serverAddress.toStdString(), this, true, false);
    }

    initializeConnections();
}

void MediaManagerBase::initializeConnections()
{
    /*setup*/

    if(usingExternalMediaPlayer){
     emit readyForConnections();
        return;
    }

    /*From Dce MediaPlayer*/

    QObject::connect(mediaPlayer, SIGNAL(currentMediaUrlChanged(QString)), this, SLOT(setPluginUrl(QString))); //effectively play for android.
    QObject::connect(mediaPlayer, &qMediaPlayer::currentMediaFileChanged, this, &MediaManagerBase::setFileReference );
    QObject::connect(mediaPlayer, SIGNAL(stopCurrentMedia()), this, SLOT(stopPluginMedia()));
    QObject::connect(mediaPlayer, SIGNAL(pausePlayback()), this, SLOT(setPause(bool)));


    // QObject::connect(mediaPlayer, SIGNAL(currentMediaUrlChanged(QString)), this, SLOT(setFileReference(QString)));

    QObject::connect(mediaPlayer, SIGNAL(pluginVolumeDown()), this, SIGNAL(pluginVolumeDown()));
    QObject::connect(mediaPlayer, SIGNAL(pluginVolumeUp()), this, SIGNAL(pluginVolumeUp()));
    QObject::connect(mediaPlayer, SIGNAL(stopCurrentMedia()), this, SLOT(stopPluginMedia()));


    QObject::connect(mediaPlayer,SIGNAL(jumpToStreamPosition(int)), this, SLOT(setMediaPosition(int)));
    QObject::connect(mediaPlayer, SIGNAL(newMediaPosition(int)), this, SLOT(setMediaPosition(int)));

    QObject::connect(mediaPlayer, SIGNAL(connectionStatusChanged(bool)), this, SLOT(setConnected(bool)));
    QObject::connect(mediaPlayer, SIGNAL(startPlayback()), this, SLOT(mediaStarted()));
    QObject::connect(mediaPlayer, SIGNAL(stopCurrentMedia()), this, SLOT(stopTimeCodeServer()));
    QObject::connect(mediaPlayer,SIGNAL(commandResponseChanged(QString)), this ,SLOT(setCurrentStatus(QString)));
    QObject::connect(mediaPlayer,SIGNAL(setZoomLevel(QString)), this, SLOT(setZoomLevel(QString)));
    QObject::connect(mediaPlayer,SIGNAL(streamIdChanged(int)), this , SLOT(setStreamId(int)));
    QObject::connect(mediaPlayer, SIGNAL(startPlayback()), this, SLOT(startTimeCodeServer()));
    setCurrentStatus("Connections initialized.");
}

void MediaManagerBase::breakConnections()
{

}

void MediaManagerBase::transmit(QString msg)
{
    m_lastTick =msg;
    QByteArray chunk;
    QDataStream out(&chunk, QIODevice::WriteOnly);
    //out << (quint16)0;
    out << msg.toLocal8Bit(); //.toStdString().c_str();
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
bool MediaManagerBase::getUsingExternalMediaPlayer() const
{
    return usingExternalMediaPlayer;
}

void MediaManagerBase::setUsingExternalMediaPlayer(bool value)
{
    usingExternalMediaPlayer = value;
    emit usingExternalMediaPlayerChanged();
}


void MediaManagerBase::onNewClientConnected()
{
    if(timeCodeServer->hasPendingConnections()){
        QTcpSocket *lastSocket=timeCodeServer->nextPendingConnection();
        if(lastSocket->isValid()){
            setCurrentStatus("New Client Connected from"+lastSocket->peerAddress().toString());
            clientList.append(lastSocket);
        }
    }
}
bool MediaManagerBase::pause() const
{
    return m_paused;
}

void MediaManagerBase::setPause(bool paused)
{
    if(m_paused = true)
        m_paused = false;
    else
        m_paused = true;
    emit pauseChanged();
}

long MediaManagerBase::getCurrentStorageDevice() const
{
    return m_currentStorageDevice;
}

void MediaManagerBase::setCurrentStorageDevice(long currentStorageDevice)
{
    qDebug() << Q_FUNC_INFO << currentStorageDevice;
    m_currentStorageDevice = currentStorageDevice;
    emit currentStorageDeviceChanged();
}




























