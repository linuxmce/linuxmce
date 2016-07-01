#include "omxdbusplayerinterface.h"
omxdbusplayerinterface::omxdbusplayerinterface()
{
    qDebug() << Q_FUNC_INFO;

    setUsingExternalMediaPlayer(true);
    connect(this, &omxdbusplayerinterface::readyForConnections, this, &omxdbusplayerinterface::setConnections);
#ifdef RPI
    m_omxProxy = new OmxInterface();
#endif

}

QImage omxdbusplayerinterface::getScreenShot()
{
    return QImage();
}

void omxdbusplayerinterface::setConnections()
{
    QObject::connect(mediaPlayer, &qMediaPlayer::currentMediaFileChanged, m_omxProxy, &OmxInterface::runOmxWithFile);
    QObject::connect(mediaPlayer, &qMediaPlayer::stopCurrentMedia, m_omxProxy, &OmxInterface::stopPlayer);
    QObject::connect(mediaPlayer, &qMediaPlayer::pausePlayback,  m_omxProxy, &OmxInterface::pause);
    QObject::connect(mediaPlayer, &qMediaPlayer::jumpToStreamPosition, m_omxProxy, &OmxInterface::seekToPosition);
}

