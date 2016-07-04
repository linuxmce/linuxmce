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

void omxdbusplayerinterface::modifypath(QString file)
{

    QString outFile = file;
    qDebug() << "device to use is " << getCurrentStorageDevice();

    QString t = "/media/"+QString::number(getCurrentStorageDevice())+"/public/data/videos/";

    t.append( file.split(QString::number(getCurrentStorageDevice())+"]/").at(1) );

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

    QObject::connect(m_omxProxy, &OmxInterface::positionChanged, this, &omxdbusplayerinterface::processTimeCode);
}

