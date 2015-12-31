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

class ColorFilterProxyWidget : public QGraphicsProxyWidget
{
public:
    ColorFilterProxyWidget(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0):
        QGraphicsProxyWidget(parent,wFlags)
    {
    }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
        QPainter *painter_inverted = new QPainter();
        QImage invertedColor(boundingRect().width(),boundingRect().height(),QImage::Format_RGB32);
        painter_inverted->begin(&invertedColor);
        QGraphicsProxyWidget::paint(painter_inverted,option,widget);
        painter_inverted->end();
        painter->drawImage(0,0,invertedColor.rgbSwapped());
    }

};


#ifdef QT5
class AudioVisual : public QQuickItem
        #else
class AudioVisual : public QDeclarativeItem
        #endif
{
    Q_PROPERTY(int mediaType READ getMediaType WRITE setMediaType NOTIFY mediaTypeChanged)
    Q_PROPERTY(QString mediaSource READ getSource WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY (int playerType READ getPlayerType WRITE setPlayerType NOTIFY playerTypeChanged)
    Q_PROPERTY (int height READ getHeight WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(int width READ getWidth WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(QSize videoSize READ getVideoSize WRITE setVideoSize NOTIFY videoSizeChanged)

    Q_OBJECT
public:
#ifdef QT5
    explicit AudioVisual(QQuickItem *parent=0);
#else
    explicit AudioVisual(QDeclarativeItem *parent = 0);
#endif
    videoWidgetPlayer  videoPlayer;
    AudioWidget  audioPlayer;

    int mediaType;
    bool mediaInProgress;
    QString qs_currentPosition;
    QString qs_totalDuration;
    int currentPosition;
    int totalDuration;
    QString playerStatus;
    QString  mediaSource;
    int playerType;

    int height;
    int width;
    QSize videoSize;

signals:
    void heightChanged();
    void widthChanged();
    void videoSizeChanged();

    void play();
    void ff(int spd);
    void rw (int spd);
    void nextTrackReady();

    void currentTrackChanged();
    void ffSpeedChanged();
    void rwSpeedChanged();
    void setPlayerSource(QString g);

    void mediaTypeChanged();
    void playerTypeChanged();

    void playerStatusChanged();
    void sourceChanged();
    void stop();
    
public slots:

    void setVideoSize(QSize size) {videoSize = size; emit videoSizeChanged();}
    QSize getVideoSize(){return videoSize;}

    void setHeight(int h) {}
    int getHeight() {return height;}

    void setWidth(int w)  {}
    int getWidth () {return width;}

    void playItem(QString track);
    void stopMedia();
    void rwMedia(int spd);
    void ffMedia(int spd);
    void seekToPosition(int position);

    int getMediaType() {return mediaType;}
    void setMediaType(int t) {
        if (mediaType==t)
        {
            return;
        }
        else{
            switchTypes();
            mediaType = t;
            emit mediaTypeChanged();
        }

    }

    void setStatusMessage(QString msg) {playerStatus = msg;emit playerStatusChanged();}
    QString getPlayerStatus() {return playerStatus;}

    void setSource(QString source);
    QString getSource();

    void setPlayerType(int t) {
        if(playerType == -1){

            playerType = t;emit mediaTypeChanged();
            setupPlayer();
        }
    }
    int getPlayerType()  { return playerType;}

private slots:
    void setupPlayer();
    void switchTypes();

    
};
#endif


#endif // AUDIOVISUAL_H
