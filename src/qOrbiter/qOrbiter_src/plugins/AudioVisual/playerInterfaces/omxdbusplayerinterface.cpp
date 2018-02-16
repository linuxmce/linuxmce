#include "omxdbusplayerinterface.h"

omxdbusplayerinterface::omxdbusplayerinterface()
{
    qDebug() << Q_FUNC_INFO;
#ifdef RPI
    setUsingExternalMediaPlayer(true);
    connect(this, &omxdbusplayerinterface::readyForConnections, this, &omxdbusplayerinterface::setConnections);
    m_omxProxy = new OmxInterface(this);
#endif

}

QImage omxdbusplayerinterface::getScreenShot()
{
    return QImage();
}

void omxdbusplayerinterface::modifypath(QString file)
{

    QString outFile = file;
    qDebug() << "device to use is " << getCurrentStorageDevice();
    QString t = "/media/"+QString::number(getCurrentStorageDevice());
    switch (mediaPlayer->i_pkMediaType) {
    case 4:
    case 2:
    case 19:
        t.append("/public/data/audio/");
        break;
    case 5:
        t.append("/public/data/videos/");
        break;
    case 7:
        t.append("/public/data/pictures/");
        break;
    case 43:
    case 44:
    case 64:
        t= file;
        break;
    default:
        t=file;
        break;
    }

    if(!t.contains("http://")){
        t.append( file.split(QString::number(getCurrentStorageDevice())+"]/").at(1) );
    }

    qDebug() << "path is " << t;
    outFile = t;
    m_omxProxy->runOmxWithFile(outFile);
}

void omxdbusplayerinterface::setConnections()
{

    QObject::connect(mediaPlayer, &qMediaPlayer::currentStorageDeviceChanged, this, &omxdbusplayerinterface::setCurrentStorageDevice);
    QObject::connect(mediaPlayer, &qMediaPlayer::currentMediaFileChanged, this, &omxdbusplayerinterface::modifypath);
    //QObject::connect(mediaPlayer, &qMediaPlayer::currentMediaFileChanged, m_omxProxy, &OmxInterface::runOmxWithFile);
    QObject::connect(mediaPlayer, &qMediaPlayer::stopCurrentMedia, m_omxProxy, &OmxInterface::stopPlayer);
    QObject::connect(mediaPlayer, &qMediaPlayer::pausePlayback,  m_omxProxy, &OmxInterface::pause);
    QObject::connect(mediaPlayer, &qMediaPlayer::jumpToStreamPosition, m_omxProxy, &OmxInterface::seekToPosition);
    QObject::connect(mediaPlayer, &qMediaPlayer::mutedChanged, this, &omxdbusplayerinterface::setMuted);
    QObject::connect(this, &omxdbusplayerinterface::mutedChanged, m_omxProxy, &OmxInterface::handleMute);
    connect(mediaPlayer, &qMediaPlayer::pluginVolumeDown, m_omxProxy, &OmxInterface::handleVolumeDown);
    connect(mediaPlayer, &qMediaPlayer::pluginVolumeUp, m_omxProxy, &OmxInterface::handleVolumeUp);

    QObject::connect(m_omxProxy, &OmxInterface::positionChanged, this, &omxdbusplayerinterface::processTimeCode);
    QObject::connect(m_omxProxy, &OmxInterface::durationChanged, this, &omxdbusplayerinterface::setTotalTimeNumeric);
    QObject::connect(m_omxProxy, &OmxInterface::mediaEnded, mediaPlayer, &qMediaPlayer::mediaEnded);

}


