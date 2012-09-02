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

    videoSurface->setFixedSize(480,320);
    window = new QWidget();

    layout = new QVBoxLayout(window);
    layout->addWidget(videoSurface);
    window->setLayout(layout);
    filterProxy = new ColorFilterProxyWidget(this);
    filterProxy->setWidget(window);
    filterProxy->show();
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


void MediaManager::setMediaUrl(QString url)
{
    setCurrentStatus("Got New Media Url::"+url);
    mediaObject->setCurrentSource(Phonon::MediaSource(url));
}
