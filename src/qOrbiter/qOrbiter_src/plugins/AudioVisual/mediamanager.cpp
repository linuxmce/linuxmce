#include "mediamanager.h"

using namespace DCE;

MediaManager::MediaManager(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
    setFlag(ItemHasNoContents, false);
    serverAddress = "";
    deviceNumber = -1;

    setCurrentStatus("Media Manager defaults set, initializing media engines");

    audioSink = new Phonon::AudioOutput();
    videoSurface = new Phonon::VideoWidget();
    mediaObject = new Phonon::MediaObject();

    Phonon::createPath(mediaObject, audioSink);
    setCurrentStatus("Audio Sink Initialized");

    Phonon::createPath(mediaObject, videoSurface);
    setCurrentStatus("Video Player Initialized");

    videoSurface->setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);
    videoSurface->setScaleMode(Phonon::VideoWidget::FitInView);

    // layout = new QVBoxLayout();
    // layout->addWidget(videoSurface);
    //  window->setLayout(layout);
    filterProxy = new ColorFilterProxyWidget(this);
    filterProxy->setWidget(videoSurface);
    filterProxy->setAutoFillBackground(false);
    setCurrentStatus("Window Initialized");

}

void MediaManager::initializePlayer()
{
    mediaPlayer = new qMediaPlayer(deviceNumber, serverAddress.toStdString(), true, false);
    if (mediaPlayer->GetConfig() && mediaPlayer->Connect(DEVICETEMPLATE_qMediaPlayer_CONST))
    {
        setCurrentStatus("Connected to router, device running");
        setConnectionStatus(mediaPlayer->m_bRunning);
        initializeConnections();
        setCurrentStatus("Initializing inter-device connections");
    }
    else
    {
        setCurrentStatus("Could not connect!");
        setConnectionStatus(false);
    }
}

void MediaManager::initializeConnections()
{
    QObject::connect(mediaPlayer,SIGNAL(currentMediaUrlChanged(QString)), this, SLOT(setMediaUrl(QString)));
    QObject::connect(mediaPlayer,SIGNAL(startPlayback()), mediaObject, SLOT(play()));
    QObject::connect(mediaPlayer, SIGNAL(stopCurrentMedia()), mediaObject, SLOT(stop()));
    QObject::connect(mediaPlayer, SIGNAL(pausePlayback()), mediaObject, SLOT(pause()));
    QObject::connect(mediaPlayer,SIGNAL(commandResponseChanged(QString)), this ,SLOT(setCurrentStatus(QString)));
    QObject::connect(mediaPlayer,SIGNAL(setZoomLevel(QString)), this, SLOT(setZoomLevel(QString)));
    QObject::connect(mediaObject, SIGNAL(finished()), mediaPlayer, SLOT(mediaEnded()));
    QObject::connect(mediaPlayer, SIGNAL(startPlayback()), this, SLOT(startTimeCodeServer()));
    QObject::connect(mediaObject, SIGNAL(tick(qint64)), this, SLOT(processTimeCode(qint64)));
    QObject::connect(mediaObject, SIGNAL(finished()), videoSurface, SLOT(lower()));
    QObject::connect(mediaPlayer, SIGNAL(startPlayback()), videoSurface, SLOT(raise()));
    mediaObject->setTickInterval(quint32(1000));
    //mediaObject->setCurrentSource(Phonon::MediaSource("/mnt/remote/121/public/data/videos/The Venture Brothers/TVB Season 1/The.Venture.Brothers.S01E12.The.Trial.of.the.Monarch.avi"));
    //mediaObject->play();
}

void MediaManager::setConnectionDetails(int t, QString r)
{
    serverAddress = r;
    deviceNumber = t;
    setCurrentStatus("Got address "+serverAddress+" and device number "+deviceNumber+", initializing");
    initializePlayer();
}

void MediaManager::newClientConnected()
{
    setCurrentStatus("New Client Connected::");

}

void MediaManager::startTimeCodeServer()
{
    setCurrentStatus("Staring timecode server on port 12000");
    timeCodeServer = new QTcpServer();
    timeCodeServer->listen(QHostAddress::Any,12000);
    QObject::connect(timeCodeServer, SIGNAL(newConnection()), this , SLOT(newClientConnected()));

    if(mediaObject->errorString() =="")
    {

    }
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
    mediaObject->setCurrentSource(Phonon::MediaSource(url));
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
    setCurrentStatus("Current position::" +QString::number(displayHours) + ":" + QString::number(minutes) + ":" +QString::number(forseconds));
}
