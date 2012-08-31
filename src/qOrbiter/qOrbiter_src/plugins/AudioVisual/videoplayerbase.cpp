#include "videoplayerbase.h"
#ifndef ANDROID
#ifdef QT5
//#include <QtMultimedia/QtMultimedia>
#include <QtWidgets/qwidget.h>
#else
#include <phonon/Phonon>
#include "QVBoxLayout"
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <phonon/VideoWidget>
#include <QWidget>
#include <QApplication>
#endif
#include <QUrl>
#include <QDebug>


VideoPlayerBase::VideoPlayerBase(QWidget *parent) :
    QWidget(parent)
{
#ifdef QT5

#else

    QVBoxLayout *l = new QVBoxLayout(this);
    l->setMargin(0);

    //  Phonon::VideoWidget * w = new Phonon::VideoWidget(this);

    videoObject = new Phonon::MediaObject();
    videoSurface = new Phonon::VideoWidget();
    videoAudio = new Phonon::AudioOutput();

    Phonon::createPath(videoObject, videoSurface);    
    Phonon::createPath(videoObject, videoAudio);

    videoSurface->setScaleMode(Phonon::VideoWidget::FitInView);

    l->addWidget(videoSurface);    
    videoSurface->setFixedSize(480,320);

    QWidget * w = new QWidget(this);
    w->setLayout(l);

//videoObject->play();
    QObject::connect(this, SIGNAL(play()), videoObject,SLOT(play()));
    QObject::connect(this, SIGNAL(stop()), videoObject, SLOT(stop()));
#endif

}



void VideoPlayerBase::startPlayback()
{
     videoObject->setCurrentSource(Phonon::MediaSource(currentSource));
     videoObject->play();
}

#endif
