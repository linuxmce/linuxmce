#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

#ifdef QT5
#include <qdeclarative.h>
#include <QDeclarativeItem>
#include <QtWidgets/QBoxLayout>
#include <QtMultimedia>

#else
#include <QBoxLayout>
#include <phonon/phonon>
#include <QDeclarativeItem>
#include <QKeyEvent>

#endif

#include <QObject>
#include <QProcess>

#include <qMediaPlayer/qMediaPlayer.h>
#include <colorfilterproxywidget.h>
#include <QTime>
#include <QTcpServer>


using namespace DCE;
using namespace Qt;
/*!
 * \brief The MediaManager class provides Video and Audio playback for DCE related video streams.
 * This class represents a network media player embedded into the application. It runs as a plugin
 * as opposed to being directly instantiated by the Application to allow for modularity.
 */
class MediaManager : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ getConnectionStatus WRITE setConnectionStatus NOTIFY connectedChanged)
    Q_PROPERTY(QString currentStatus READ getCurrentStatus WRITE setCurrentStatus NOTIFY currentStatusChanged)
    Q_PROPERTY(bool mediaPlaying READ getMediaPlaying WRITE setMediaPlaying NOTIFY mediaPlayingChanged)
    Q_PROPERTY(bool hasError READ getErrorStatus WRITE setErrorStatus NOTIFY hasErrorChanged)
    Q_PROPERTY(QString lastError READ getMediaError WRITE setMediaError NOTIFY lastErrorChanged)
    Q_PROPERTY(int mediaBuffer READ getMediaBuffer WRITE setMediaBuffer NOTIFY mediaBufferChanged)

    Q_PROPERTY(QString serverAddress READ getServerAddress WRITE setServerAddress NOTIFY serverAddressChanged)
    Q_PROPERTY(int deviceNumber READ getDeviceNumber WRITE setDeviceNumber NOTIFY deviceNumberChanged)

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

    int mediaBuffer;

    bool mediaPlaying;
    bool hasError;
    QString lastError;

    //--------------------------


    QString currentStatus;
    QString qs_totalTime;


    QTcpServer *timeCodeServer;
    QString current_position;
    int iCurrent_Position;

    QWidget *window;
    QVBoxLayout *layout;
    qMediaPlayer *mediaPlayer;

    ColorFilterProxyWidget *filterProxy;
    QList<QTcpSocket*> clientList;
    QProcess *mountProcess;

    QString serverAddress;
    int deviceNumber;

    bool connected;

    int videoHeight;
    int videoWidth;

#ifdef QT4
    Phonon::VideoWidget *videoSurface;
    Phonon::AudioOutput *audioSink;
    Phonon::MediaObject *mediaObject;
#else
    QAbstractAudioOutput *audioSink;
    QMediaObject *mediaObject;
    QAbstractVideoSurface * videoSurface;
#endif

signals:
    void connectedChanged();
    void currentStatusChanged();
    void totalTimeChanged();

    void newData(QByteArray b);

    void mediaPlayingChanged();
    void hasErrorChanged();
    void lastErrorChanged();
    void mediaBufferChanged();
    void mediaAboutToFinish();
    void prefinishMark();

    void serverAddressChanged();
    void deviceNumberChanged();

public slots:

    void mediaStarted(){setMediaPlaying(true);  }

    void setServerAddress(QString a) {if(serverAddress !=a) {serverAddress = a;emit serverAddressChanged();}}
    QString getServerAddress(){return serverAddress;}

    void setDeviceNumber(int d) { if(deviceNumber !=d) {deviceNumber = d; emit deviceNumberChanged();}}
    int getDeviceNumber() {return deviceNumber;}

    void updateMetaData(){}

    void setPrefinishMarkHit(qint32 t ) {}

    void setMediaBuffer(int b) {mediaBuffer = b; emit mediaBufferChanged();}
    int getMediaBuffer () {return mediaBuffer;}

    void setMediaError(QString e) {lastError = e; emit lastErrorChanged();}
    QString getMediaError() {return lastError;}

    void setErrorStatus(bool e) {hasError = e; emit hasErrorChanged();}
    bool getErrorStatus() {return hasError;}

    void setMediaPlaying(bool s) {mediaPlaying = s;
                                  qDebug() << "media playback changed in plugin!" << s;
                                  if (mediaPlaying==false)
                                  { filterProxy->hide();}
                                  else
                                  {filterProxy->show();}
                                emit mediaPlayingChanged();
                                 }
    bool getMediaPlaying() {return mediaPlaying;}

    void setFileReference(QString f){fileReference = f.at(0); setFileNumber(f.remove(0,1).toInt()); }
    QString getFileReference() {return fileReference; }

    void setFileNumber(int n) {fileno = n;}

    void transmit(QString d);
    void setStreamId(int id) {streamId = id; }

    void setMediaUrl(QString url);

    void setCurrentStatus(QString s) {currentStatus = QTime::currentTime().toString()+"::"+s; emit currentStatusChanged(); qDebug() <<currentStatus; }
    QString getCurrentStatus() {return currentStatus;}

    void setConnectionDetails(int r, QString s);

    void setConnectionStatus(bool stat){connected = stat; emit connectedChanged();}
    bool getConnectionStatus(){return connected;}

    void setWindowSize(int h, int w) {

#ifdef QT4
        videoSurface->setFixedSize(w, h);
#elif QT5

#endif
    }

    void newClientConnected();
    void startTimeCodeServer();
    void stopTimeCodeServer();


    void setMediaPosition(int msec) {
#ifdef QT4
        mediaObject->seek((qint64)msec);
#elif QT5

#endif

    }
    void setZoomLevel(QString zoom);
    void setAspectRatio(QString aspect);
    void getScreenShot();

    void setVideoSize(int h, int w) {
#ifdef QT4
        videoSurface->setFixedSize(w, h);
#elif QT5

#endif
    }

    void setTotalTime(qint64 t) {
        int seconds = t / 1000;
        int displayHours = seconds / (3600);
        int remainder = seconds % 3600;
        int minutes = remainder / 60;
        int forseconds = remainder % 60;
        QString hrs = QString::number(displayHours);
        if(hrs.length()==1)
            hrs.prepend("0");

        QString min = QString::number(minutes);
        if(min.length()==1)
            min.prepend("0");

        QString sec = QString::number(forseconds);
        if(sec.length()==1)
            sec.prepend("0");

        qs_totalTime =hrs + ":" + min + ":" +sec;
        totalTime = t;
        emit totalTimeChanged();}


    QString getTotalTime() {return qs_totalTime;}

    void processTimeCode(qint64 f);

private:
    void initializePlayer();
    void initializeConnections();
    void shutdownDevice();
    void mountDrive(int device);


private slots:


};

#endif // MEDIAMANAGER_H
