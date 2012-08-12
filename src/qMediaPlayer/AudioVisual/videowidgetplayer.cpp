#include "videowidgetplayer.h"
#ifndef ANDROID
#ifdef QT5
//#include <QtMultimedia/QtMultimedia>
#include <QtWidgets/qwidget.h>
#else
#include <phonon/Phonon>
#include "QVBoxLayout"
#endif
#include <QUrl>
#include <QDebug>

videoWidgetPlayer::videoWidgetPlayer(QWidget *parent) :
    QWidget(parent)
{
#ifdef QT5

#else
    Phonon::MediaObject *qPlayer = new Phonon::MediaObject(this);
    Phonon::VideoWidget *videoOutput = new Phonon::VideoWidget(this);

    Phonon::createPath(qPlayer, videoOutput);
#endif

}
#endif




