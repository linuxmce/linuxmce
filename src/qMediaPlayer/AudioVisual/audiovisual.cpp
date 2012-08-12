#include "audiovisual.h"
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
#endif
#include <QDeclarativeItem>
#include <QGraphicsProxyWidget>
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


}

#endif


void AudioVisual::setupPlayer()
{
    if(mediaType == 5)
    {
        videoWidgetPlayer *videoPlayer = new videoWidgetPlayer();

    }
    else
        if (mediaType ==4)
        {
            AudioWidget* audioPlayer = new AudioWidget();
        }

}


void AudioVisual::stopMedia()
{
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
    switch(mediaType){
    case 5 :
        videoPlayer->qPlayer->setCurrentSource(Phonon::MediaSource(source));
        break;
    case 4:
        audioPlayer->audioObject->setCurrentSource(Phonon::MediaSource(source));
        break;
    }

}

QString AudioVisual::getSource()
{
    return mediaSource;
}


void AudioVisual::playItem(QString track)
{
}

