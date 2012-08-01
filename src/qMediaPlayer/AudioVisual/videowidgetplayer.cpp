#include "videowidgetplayer.h"
#ifndef ANDROID
#ifdef QT5
//#include <QtMultimedia/QtMultimedia>
#include <QtWidgets/qwidget.h>
#else
#include <QtMultimedia/QtMultimedia>
#include <phonon/Phonon>
#include <phonon/VideoWidget>
#endif

#include "QVBoxLayout"
#include <phonon/VideoWidget>
#include "QVBoxLayout"
#include <QUrl>
#include <QDebug>

videoWidgetPlayer::videoWidgetPlayer(QWidget *parent) :
    QWidget(parent)
{
#ifdef QT5

#else

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
#endif

}
#endif




