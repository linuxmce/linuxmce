#include "videoplayer.h"
#ifndef ANDROID
#include <QUrl>
#include <QFile>
#include <QPainter>

#ifdef QT5
#include <QtWidgets/QApplication>
#include <QtQuick/QQuickItem>
#include <QtWidgets/QGraphicsProxyWidget>
//#include <QtMultimedia/QtMultimedia>
#else
#include <QApplication>
#include <phonon/VideoPlayer>
#include <QDeclarativeItem>
#include <QGraphicsProxyWidget>
#include <colorfilterproxywidget.h>
#endif
#include <QTime>
#ifdef debug
#include <QDebug>
#endif

//there will be a lot of commented out code as i work this into a proper plugin. the commented out code
//is essentially prototype methods and things i dont feel like looking up again
#ifdef QT5
VideoPlayer::VideoPlayer(QQuickItem *parent) :
    QQuickItem(parent)
  #else
VideoPlayer::VideoPlayer(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
  #endif
{
    setFlag(ItemHasNoContents, false);
    playerType = -1;

    ColorFilterProxyWidget * proxy = new ColorFilterProxyWidget(this);
    videoPlayer = new VideoPlayerBase();

    proxy->setWidget(videoPlayer);
    proxy->show();
    QObject::connect(this, SIGNAL(stop()), videoPlayer, SLOT(stopPlayback()));
    QObject::connect(this, SIGNAL(play()), videoPlayer, SLOT(startPlayback()));
    QObject::connect(this, SIGNAL(setPlayerSource(QString)), videoPlayer, SLOT(setSource(QString)));
    QObject::connect(videoPlayer, SIGNAL(mediaSourceError()), this, SLOT(getError()));
    QObject::connect(videoPlayer->videoObject, SIGNAL(tick(qint64)), this, SLOT(setTimeCode(qint64)));
    QObject::connect(videoPlayer->videoObject, SIGNAL(finished()), this, SIGNAL(mediaEnded()));
    QObject::connect(videoPlayer->videoObject, SIGNAL(aboutToFinish()), this, SIGNAL(mediaAboutToEnd()));
    QObject::connect(videoPlayer->videoObject, SIGNAL(bufferStatus(int)), this, SLOT(setBufferStatus(int)));

    //QObject::connect(videoPlayer->videoObject, SIGNAL())

    //QObject::connect(this, SIGNAL(stop()), audioPlayer->audioObject, SLOT(stop()));
    setStatusMessage("Video player initialized");
}




void VideoPlayer::setupPlayer()
{


}

void VideoPlayer::switchTypes()
{

}


void VideoPlayer::stopMedia()
{
  videoPlayer->videoObject->stop();
}

void VideoPlayer::rwMedia(int spd)
{

}

void VideoPlayer::ffMedia(int spd)
{

}

void VideoPlayer::seekToPosition(qint64 position)
{
    videoPlayer->videoObject->seek(position);
}

void VideoPlayer::setSource(QString source)
{

    mediaSource = source;

    videoPlayer->videoObject->setCurrentSource(Phonon::MediaSource(mediaSource));
    setStatusMessage("Video player Set Source::"+mediaSource);
    emit sourceChanged();
}

QString VideoPlayer::getSource()
{
    return mediaSource;
}


void VideoPlayer::playItem(QString track)
{
    setStatusMessage("Playing item::"+track);
    videoPlayer->setSource(track);
    videoPlayer->videoObject->play();
}



void VideoPlayer::setTimeCode(qint64 t)
{
    currentPosition = t;
    QString currentPos = QString::number(t);
    setCurrentPosition(currentPos);
    setStatusMessage("Processing timecode");


}
#endif
