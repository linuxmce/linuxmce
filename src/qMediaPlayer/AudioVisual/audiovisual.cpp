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
    videoWidgetPlayer *player = new videoWidgetPlayer;
    QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget();
    proxy->setWidget(player);

}
//public function to take the url and then decide which source to send it to?
bool AudioVisual::playMedia(const QString &media)
{
#ifdef debug
    qDebug("Handed off to play function");
#endif
    return true;
}

bool AudioVisual::playVideo(const QString &video)
{
    return true;
}

bool AudioVisual::playAudio(const QString &audio)
{
#ifdef QT5

#else
    Phonon::MediaObject *music =
            Phonon::createPlayer(Phonon::MusicCategory,
                                 Phonon::MediaSource(":/audio/epic.mp3"));
    music->play();
#endif
    return true;
}
#endif
