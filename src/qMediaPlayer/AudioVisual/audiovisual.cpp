#include "audiovisual.h"
#include <QUrl>
#include <QFile>
#include <QPainter>
#include <phonon/VideoPlayer>
#include <QDeclarativeItem>

#include <QGraphicsProxyWidget>
#include <QDebug>

//there will be a lot of commented out code as i work this into a proper plugin. the commented out code
//is essentially prototype methods and things i dont feel like looking up again

AudioVisual::AudioVisual(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{

    videoWidgetPlayer *player = new videoWidgetPlayer;
    QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget(this);
    proxy->setWidget(player);

}
//public function to take the url and then decide which source to send it to?
bool AudioVisual::playMedia(const QString &media)
{
    qDebug("Handed off to play function");
    return true;
}

bool AudioVisual::playVideo(const QString &video)
{
    return true;
}

bool AudioVisual::playAudio(const QString &audio)
{
    Phonon::MediaObject *music =
             Phonon::createPlayer(Phonon::MusicCategory,
                                  Phonon::MediaSource(":/audio/epic.mp3"));
         music->play();
    return true;
}

