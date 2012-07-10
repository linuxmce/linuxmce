#include "videowidgetplayer.h"
#include <phonon/Phonon>
#include "QVBoxLayout"
#include <QUrl>
#include <phonon/VideoWidget>

#include "QVBoxLayout"
#include <QDebug>
videoWidgetPlayer::videoWidgetPlayer(QWidget *parent) :
    QWidget(parent)
{

    Phonon::MediaObject *mediaObject = new Phonon::MediaObject(this);

         Phonon::VideoWidget *videoWidget = new Phonon::VideoWidget(this);
         Phonon::createPath(mediaObject, videoWidget);

         Phonon::AudioOutput *audioOutput =
             new Phonon::AudioOutput(Phonon::VideoCategory, this);
         Phonon::createPath(mediaObject, audioOutput);
         qDebug() << audioOutput->isValid();
         qDebug() << videoWidget->isValid();
         mediaObject->setCurrentSource(Phonon::MediaSource(QUrl::fromLocalFile(":/video/vid.3gp")));
         mediaObject->play();

         qDebug() << mediaObject->state();
         qDebug() << mediaObject->totalTime();
         qDebug() << mediaObject->isSeekable();
         qDebug() << mediaObject->errorType();


   Phonon::VideoPlayer *player =
             new Phonon::VideoPlayer(Phonon::VideoCategory, parent);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
      mainLayout->setMargin(0);
      mainLayout->addWidget( player);
      player->play(QUrl::fromLocalFile("file goes here"));
      }





