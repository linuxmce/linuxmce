#ifndef AUDIOVISUAL_H
#define AUDIOVISUAL_H


#ifdef QT5
#include <QtWidgets/QApplication>
#include <QtQuick/QQuickItem>
//#include <QtMultimedia/QtMultimedia>
#else
#include <QApplication>
#include <phonon/VideoPlayer>
#include <QDeclarativeItem>
#include <QGraphicsProxyWidget>
#include <QDebug>
#endif

#include <QString>
#include <QUrl>
#include "videowidgetplayer.h"
#ifdef QT5
class AudioVisual : public QQuickItem
        #else
class AudioVisual : public QDeclarativeItem
        #endif
{
    Q_OBJECT
public:
#ifdef QT5
    explicit AudioVisual(QQuickItem *parent=0);
#else
    explicit AudioVisual(QDeclarativeItem *parent = 0);
#endif
    bool playMedia(const QString& media);
    bool playVideo(const QString& video);
    bool playAudio(const QString& audio);



signals:
    
public slots:
    
};



#endif // AUDIOVISUAL_H
