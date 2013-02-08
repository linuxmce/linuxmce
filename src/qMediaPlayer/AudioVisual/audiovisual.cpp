#include "audiovisual.h"
#ifndef ANDROID
#include <QUrl>
#include <QFile>
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
#endif
#ifdef debug
#include <QDebug>
#endif

//there will be a lot of commented out code as i work this into a proper plugin. the commented out code
//is essentially prototype methods and things i dont feel like looking up again
#ifdef QT5
AudioVisual::AudioVisual(QQuickItem *parent) :
    QQuickItem(parent)
  #else
AudioVisual::AudioVisual(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
  #endif
{
    setFlag(ItemHasNoContents, false);
    playerType = -1;

    ColorFilterProxyWidget * proxy = new ColorFilterProxyWidget(this);
    videoWidgetPlayer *videoPlayer = new videoWidgetPlayer();

    proxy->setWidget(videoPlayer);
    proxy->show();
    QObject::connect(this, SIGNAL(stop()), videoPlayer, SLOT(stopPlayback()));
    QObject::connect(this, SIGNAL(play()), videoPlayer, SLOT(startPlayback()));
    QObject::connect(this, SIGNAL(setPlayerSource(QString)), videoPlayer, SLOT(setSource(QString)));

    AudioWidget* audioPlayer = new AudioWidget();
    //QObject::connect(this, SIGNAL(stop()), audioPlayer->audioObject, SLOT(stop()));


}




void AudioVisual::setupPlayer()
{


}

void AudioVisual::switchTypes()
{

}


void AudioVisual::stopMedia()
{
    emit stop();
}

void AudioVisual::rwMedia(int spd)
{
}

void AudioVisual::ffMedia(int spd)
{
}

void AudioVisual::seekToPosition(int position)
{
}

void AudioVisual::setSource(QString source)
{

    mediaSource = source;
    switch(playerType){
    case 5 :
        emit setPlayerSource(mediaSource);
        break;
    case 4:
        emit setPlayerSource(mediaSource);
        break;
    }
    emit sourceChanged();
}

QString AudioVisual::getSource()
{
    return mediaSource;
}


void AudioVisual::playItem(QString track)
{
    emit setPlayerSource(track);
    emit play();
}
#endif
