#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

#ifdef QT5
#include <qdeclarative.h>
#include <QDeclarativeItem>
#include <QtWidgets/QBoxLayout>
#else
#include <QBoxLayout>
#include <Phonon>
#endif

#include <QObject>

#include <qMediaPlayer/qMediaPlayer.h>
#include "../qOrbiter/qOrbiter_src/plugins/AudioVisual/videoplayerbase.h"
#include "../qOrbiter/qOrbiter_src/plugins/AudioVisual/audioplayerbase.h"
#include <colorfilterproxywidget.h>
#include <QTime>
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

    //media info
    int currentTime;
    qint64 totalTime;
    int playBackSpeed;
    int streamId;
    QString fileReference;
    int fileno;
    QString filepath;

    //--------------------------

    QString currentStatus;
    QString qs_totalTime;
    QThread *dcethread;

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
    QList<QTcpSocket*> clientList;

    QString serverAddress;
    int deviceNumber;
    bool connected;

    int videoHeight;
    int videoWidth;

signals:
    void connectedChanged();
    void currentStatusChanged();
    void totalTimeChanged();
    void newData(QByteArray b);


public slots:
    void setFileReference(QString f){fileReference = f.at(0); setFileNumber(f.remove(0,1).toInt()); }
    QString getFileReference() {return fileReference; }

    void setFileNumber(int n) {fileno = n;}

    void transmit(QString d);
    void setStreamId(int id) {streamId = id; }

    void setMediaUrl(QString url);

    void setCurrentStatus(QString s) {currentStatus = QTime::currentTime().toString()+"::"+s; emit currentStatusChanged(); }
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

    void setTotalTime(qint64 t) {
        int seconds = t / 1000;
        int displayHours = seconds / (3600);
        int remainder = seconds % 3600;
        int minutes = remainder / 60;
        int forseconds = remainder % 60;
        qs_totalTime =QString::number(displayHours) + ":" + QString::number(minutes) + ":" +QString::number(forseconds);
        totalTime = t;
        emit totalTimeChanged();}


    QString getTotalTime() {return qs_totalTime;}

    void processTimeCode(qint64 f);

private:
    void initializePlayer();
    void initializeConnections();
    void shutdownDevice();


private slots:

};

#endif // MEDIAMANAGER_H
