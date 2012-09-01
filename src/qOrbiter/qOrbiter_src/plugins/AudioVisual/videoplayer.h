#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H
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
#include <QTime>
#include <QString>
#include <QUrl>
#include "videoplayerbase.h"

#ifdef QT5
class VideoPlayer : public QQuickItem
        #else
class VideoPlayer : public QDeclarativeItem
        #endif
{
    Q_PROPERTY(int mediaType READ getMediaType WRITE setMediaType NOTIFY mediaTypeChanged)
    Q_PROPERTY(QString mediaSource READ getSource WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY (int playerType READ getPlayerType WRITE setPlayerType NOTIFY playerTypeChanged)
    Q_PROPERTY (int height READ getHeight WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(int width READ getWidth WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(QSize videoSize READ getVideoSize WRITE setVideoSize NOTIFY videoSizeChanged)
    Q_PROPERTY(QString playerStatus READ getPlayerStatus WRITE setStatusMessage NOTIFY playerStatusChanged)
    Q_PROPERTY(QString qs_currentPosition READ getCurrentPosition WRITE setCurrentPosition NOTIFY currentPositionChanged)
    Q_PROPERTY(int buffer READ getBufferStatus WRITE setBufferStatus NOTIFY loadingStatusChanged)

    Q_OBJECT
public:
#ifdef QT5
    explicit VideoPlayer(QQuickItem *parent=0);
#else
    explicit VideoPlayer(QDeclarativeItem *parent = 0);
#endif
    VideoPlayerBase  *videoPlayer;


    int mediaType;
    bool mediaInProgress;
    QString qs_currentPosition;
    QString qs_totalDuration;
    int currentPosition;
    int totalDuration;
    QString playerStatus;
    QString  mediaSource;
    int playerType;

    int buffer;

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
    void mediaEnded();
    void mediaAboutToEnd();
    void currentPositionChanged();
    void loadingStatusChanged();


public slots:
    void setTimeCode(qint64 t);
    void pause () {videoPlayer->videoObject->pause();}

    void setBufferStatus(int l) {buffer = l; emit loadingStatusChanged();}
    int getBufferStatus(){return buffer;}

    void setCurrentPosition(QString p) {qs_currentPosition = p; emit currentPositionChanged(); }
    QString getCurrentPosition() {return qs_currentPosition;}

    void getError() {setStatusMessage(videoPlayer->getSourceErrorMsg());}

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
    void pauseMedia() {videoPlayer->videoObject->pause();}
    void seekToPosition(qint64 position);

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

    void setStatusMessage(QString msg) {playerStatus = QTime::currentTime().toString()+msg;emit playerStatusChanged();}
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



#endif // VIDEOPLAYER_H
