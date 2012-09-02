#include "mediamanager.h"

using namespace DCE;

MediaManager::MediaManager(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
    setFlag(ItemHasNoContents, false);
    serverAddress = "";
    deviceNumber = -1;
    setCurrentStatus("Media Manager defaults set, initializing media engines");

    audioOutput = new AudioPlayerBase();
    setCurrentStatus("Audio Player Initialized");

    filterProxy = new ColorFilterProxyWidget(this);
    videoOutput = new VideoPlayerBase();
    filterProxy->setWidget(videoOutput);
    filterProxy->show();
    setCurrentStatus("Video Player Initialized");

}

void MediaManager::initializePlayer()
{
    mediaPlayer = new qMediaPlayer(deviceNumber, serverAddress.toStdString(), true, false);
    if (mediaPlayer->GetConfig() && mediaPlayer->Connect(DEVICETEMPLATE_qMediaPlayer_CONST))
    {
        setCurrentStatus("Connected to router, device running");
        setConnectionStatus(mediaPlayer->m_bRunning);
    }
    else
    {
        setCurrentStatus("Could not connect!");
        setConnectionStatus(false);
    }
}

void MediaManager::setConnectionDetails(int t, QString r)
{
    serverAddress = r;
    deviceNumber = t;
    setCurrentStatus("Got address "+serverAddress+" and device number "+deviceNumber+", initializing");
    initializePlayer();
}
