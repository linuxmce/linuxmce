#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

#include <QObject>
#include <qdeclarative.h>
#include <QDeclarativeItem>
#include <Phonon>
#include <qMediaPlayer/qMediaPlayer.h>
#include "../qOrbiter/qOrbiter_src/plugins/AudioVisual/videoplayerbase.h"
#include "../qOrbiter/qOrbiter_src/plugins/AudioVisual/audioplayerbase.h"
#include <colorfilterproxywidget.h>
#include <QTime>
#include <QBoxLayout>
#include <QTcpServer>


using namespace DCE;
using namespace Qt;

class MediaManager : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ getConnectionStatus WRITE setConnectionStatus NOTIFY connectedChanged)
    Q_PROPERTY(QString currentStatus READ getCurrentStatus WRITE setCurrentStatus NOTIFY currentStatusChanged)
public:
    explicit MediaManager(QDeclarativeItem *parent = 0);

    QString currentStatus;
    QString qs_totalTime;


    QTcpServer *timeCodeServer;
    QString current_position;
    int iCurrent_Position;

    QWidget *window;
    QVBoxLayout *layout;
    qMediaPlayer *mediaPlayer;
    Phonon::VideoWidget *videoSurface;
    Phonon::AudioOutput *audioSink;
    Phonon::MediaObject *mediaObject;
    ColorFilterProxyWidget *filterProxy;

    QString serverAddress;
    int deviceNumber;
    bool connected;

    int videoHeight;
    int videoWidth;

signals:
    void connectedChanged();
    void currentStatusChanged();
    void totalTimeChanged();


public slots:
    void setMediaUrl(QString url);

    void setCurrentStatus(QString s) {currentStatus = QTime::currentTime().toString()+"::"+s; emit currentStatusChanged();}
    QString getCurrentStatus() {return currentStatus;}

    void setConnectionDetails(int t, QString r);

    void setConnectionStatus(bool stat){connected = stat; emit connectedChanged();}
    bool getConnectionStatus(){return connected;}

    void setWindowSize(int h, int w) {videoSurface->setFixedSize(w, h);}

    void newClientConnected();
    void startTimeCodeServer();

    void setMediaPosition(int msec) {mediaObject->seek(msec); }
    void setZoomLevel(QString zoom);
    void setAspectRatio(QString aspect);
    void getScreenShot();

    void setVideoSize(int h, int w) { videoSurface->setFixedSize(w, h);  }

    void setTotalTime(QString t) { qs_totalTime = t; emit totalTimeChanged();}
    QString getTotalTime() {return qs_totalTime;}

    void processTimeCode(qint64 f);

private:
    void initializePlayer();
    void initializeConnections();
    void shutdownDevice();


private slots:

};

#endif // MEDIAMANAGER_H
