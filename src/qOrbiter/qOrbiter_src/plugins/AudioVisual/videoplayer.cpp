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
    VideoPlayerBase *videoPlayer = new VideoPlayerBase();

    proxy->setWidget(videoPlayer);
    proxy->show();
    QObject::connect(this, SIGNAL(stop()), videoPlayer, SLOT(stopPlayback()));
    QObject::connect(this, SIGNAL(play()), videoPlayer, SLOT(startPlayback()));
    QObject::connect(this, SIGNAL(setPlayerSource(QString)), videoPlayer, SLOT(setSource(QString)));

    //QObject::connect(this, SIGNAL(stop()), audioPlayer->audioObject, SLOT(stop()));


}




void VideoPlayer::setupPlayer()
{


}

void VideoPlayer::switchTypes()
{

}


void VideoPlayer::stopMedia()
{
    emit stop();
}

void VideoPlayer::rwMedia(int spd)
{
}

void VideoPlayer::ffMedia(int spd)
{
}

void VideoPlayer::seekToPosition(int position)
{
}

void VideoPlayer::setSource(QString source)
{

    mediaSource = source;

        emit setPlayerSource(mediaSource);

    emit sourceChanged();
}

QString VideoPlayer::getSource()
{
    return mediaSource;
}


void VideoPlayer::playItem(QString track)
{
    emit setPlayerSource(track);
    emit play();
}
#endif
