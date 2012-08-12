#ifndef AUDIOVISUAL_H
#define AUDIOVISUAL_H

#ifndef ANDROID

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
#include "audiowidget.h"
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
    videoWidgetPlayer * videoPlayer;
    AudioWidget * audioPlayer;


    int mediaType;
    bool mediaInProgress;
    QString qs_currentPosition;
    QString qs_totalDuration;
    int currentPosition;
    int totalDuration;
    QString playerStatus;
    QString mediaSource;

signals:

    void play(QString file);
    void ff(int spd);
    void rw (int spd);
    void nextTrackReady();

    void currentTrackChanged();
    void ffSpeedChanged();
    void rwSpeedChanged();


    void playerStatusChanged();
    
public slots:
    void playItem(QString track);
   void stopMedia();
    void rwMedia(int spd);
    void ffMedia(int spd);
    void seekToPosition(int position);

    void setStatusMessage(QString msg) {playerStatus = msg;emit playerStatusChanged();}
    QString getPlayerStatus() {return playerStatus;}

    void setSource(QString source);
    QString getSource();

private slots:
    void setupPlayer();

    
};
#endif


#endif // AUDIOVISUAL_H
