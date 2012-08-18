#include "audiowidget.h"

#ifndef ANDROID
#ifdef QT5
//#include <QtMultimedia/QtMultimedia>
#include <QtWidgets/qwidget.h>
#else
#include <phonon/Phonon>
#include <phonon/AudioOutput>
#endif

#include <QUrl>
#include <QDebug>

AudioWidget::AudioWidget(QWidget *parent) :
    QWidget(parent)
{
#ifdef QT5

#else

    Phonon::MediaObject *audioObject = new Phonon::MediaObject(this);
    Phonon::AudioOutput *audioPath = new Phonon::AudioOutput(this);
    Phonon::createPath(audioObject, audioPath);
#endif
}

void AudioWidget::start(QString fileToPlay)
{
}

void AudioWidget::restart()
{
}

void AudioWidget::setNextTrack(QString f)
{
}

void AudioWidget::stop()
{
}

void AudioWidget::seek(int msec)
{
}

void AudioWidget::seekForward(int speed)
{
}

void AudioWidget::seekBackward(int speed)
{
}

#endif
