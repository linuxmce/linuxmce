#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

#ifdef QT5
#include <qdeclarative.h>
#include <QDeclarativeItem>
#include <QtWidgets/QBoxLayout>
#include <QtMultimedia>

#else

#ifndef __ANDROID__
#include <QBoxLayout>
#include <phonon>
#include <QHBoxLayout>
#include <colorfilterproxywidget.h>
#endif

#include <QDeclarativeItem>
#include <QKeyEvent>
#include <QGLWidget>
#endif

#include <QObject>
#include <QProcess>
#include <qMediaPlayer/qMediaPlayer.h>

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

    Q_PROPERTY(bool connected READ getConnectionStatus WRITE setConnectionStatus NOTIFY connectedChanged)
    Q_PROPERTY(QString androidUrl READ getAndroidUrl NOTIFY androidUrlUpdated)
    Q_PROPERTY(QString currentStatus READ getCurrentStatus WRITE setCurrentStatus NOTIFY currentStatusChanged)
    Q_PROPERTY(bool mediaPlaying READ getMediaPlaying WRITE setMediaPlaying NOTIFY mediaPlayingChanged)
    Q_PROPERTY(bool hasError READ getErrorStatus WRITE setErrorStatus NOTIFY hasErrorChanged)
    Q_PROPERTY(QString lastError READ getMediaError WRITE setMediaError NOTIFY lastErrorChanged)
    Q_PROPERTY(int mediaBuffer READ getMediaBuffer WRITE setMediaBuffer NOTIFY mediaBufferChanged)
    Q_PROPERTY(qreal volume READ getVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ getMuted WRITE setMuted NOTIFY mutedChanged)

    Q_PROPERTY(bool flipColors READ getColorFlip WRITE setColorFlip NOTIFY colorFlipChanged)
  #ifndef __ANDROID__
  //  Q_PROPERTY(QList <Phonon::AudioOutputDevice> outputs READ getAvailibleOutputs NOTIFY availibleAudioOutputsChanged())
#endif
    Q_PROPERTY(QString serverAddress READ getServerAddress WRITE setServerAddress NOTIFY serverAddressChanged)
    Q_PROPERTY(int deviceNumber READ getDeviceNumber WRITE setDeviceNumber NOTIFY deviceNumberChanged)
    Q_OBJECT

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
    qreal volume;
    bool muted;


    QString currentStatus;
    QString qs_totalTime;


    QTcpServer *timeCodeServer;
    QString current_position;
    int iCurrent_Position;



#ifndef __ANDROID__
    QWidget *window;
    QList <Phonon::AudioOutputDevice> outputs;
    QVBoxLayout *layout;
    ColorFilterProxyWidget *filterProxy;
#endif

    qMediaPlayer *mediaPlayer;

    QList<QTcpSocket*> clientList;
    QProcess *mountProcess;

    QString serverAddress;
    int deviceNumber;

    QString androidUrl;

    bool connected;

    int videoHeight;
    int videoWidth;

    bool flipColors;
    quint64 tempTime;

#ifdef QT4

#ifndef __ANDROID__
    Phonon::VideoWidget *videoSurface;
    Phonon::AudioOutput *audioSink;
    Phonon::MediaObject *mediaObject;
    Phonon::MediaController * discController;
    QGLWidget *accel;
#endif
#else
    QAbstractAudioOutput *audioSink;
    QMediaObject *mediaObject;
    QAbstractVideoSurface * videoSurface;
#endif

signals:
    void connectedChanged();
    void currentStatusChanged();
    void totalTimeChanged();

    void colorFlipChanged();

    void newData(QByteArray b);

    void mediaPlayingChanged();
    void hasErrorChanged();
    void lastErrorChanged();
    void mediaBufferChanged();
    void mediaAboutToFinish();
    void prefinishMark();

    void serverAddressChanged();
    void deviceNumberChanged();

    void volumeChanged();
    void mutedChanged();

    void availibleAudioOutputsChanged();

    void androidUrlUpdated();
    void asyncStop();
    void asyncPositionRequest(int r);

public slots:

    QString getAndroidUrl(){ return androidUrl;}

    void stopAndroidMedia(){
        setMediaPlaying(false);
    }

    void setColorFlip(bool f){
        flipColors = f;
#ifndef __ANDROID__
        filterProxy->invert = f;
#endif
        emit colorFlipChanged();
    }
    bool getColorFlip() { return flipColors;}

#ifndef __ANDROID__
  //  void setAvailibleOutputs(QList<Phonon::AudioOutputDevice> l){outputs.clear(); outputs = l; emit availibleAudioOutputsChanged(); }
  //  QList <Phonon::AudioOutputDevice> getAvailibleOutputs(){ return outputs;}
#endif

    void setMuted(bool m){muted = m; emit mutedChanged();}
    bool getMuted(){ return muted;}

    void setVolume(qreal vol){
        qDebug() << vol;
        volume = vol;
        emit volumeChanged();
    }

    qreal getVolume(){
        return volume;
    }

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

    void setMediaPlaying(bool s) {
        mediaPlaying = s;
        qDebug() << "media playback changed in plugin!" << s;
        if (mediaPlaying==false)
        {
#ifndef __ANDROID__
            filterProxy->hide();
#endif
        }
        else
        {
#ifndef __ANDROID__
            filterProxy->show();
#endif
        }
        emit mediaPlayingChanged();
#ifndef __ANDROID__
        qDebug() << "Titles ==>" << discController->availableTitles();
        qDebug() << "subTitles==>" << discController->availableSubtitles();
        qDebug() << "Availible titles ==>" << discController->availableTitles();
#endif

    }
    bool getMediaPlaying() {return mediaPlaying;}

    void setFileReference(QString f){
            fileReference = f;
            androidUrl = fileReference;
            emit androidUrlUpdated();
    }
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
#ifndef __ANDROID__
        videoSurface->setFixedSize(w, h);
#endif
#elif QT5

#endif
    }

    void newClientConnected();
    void startTimeCodeServer();
    void stopTimeCodeServer();
    void handleError(){


    }


    void setState(){
      #ifndef __ANDROID__

        qDebug() << mediaObject->state();
        int i =  mediaObject->errorType();
        if(i==0){
            setCurrentStatus("Media Player Loading");
        } else if( i== 1){
            setCurrentStatus("Media Player Stopped");
        } else if( i ==2) {
            setCurrentStatus("Media is playing.");
        } else if (i == 3) {
            setCurrentStatus("Media is buffering");
        } else if ( i == 4 ){
            setCurrentStatus("Media is paused.");
        }
        else if (i == 5 ){
            setCurrentStatus("Media Error.");
            mediaPlayer->EVENT_Playback_Completed(mediaPlayer->currentMediaUrl.toStdString(), mediaPlayer->i_StreamId, false);
            qWarning("Media could not start.");
        }
     #endif
    }



    void setMediaPosition(int msec) {
        qDebug() << msec;
#ifdef QT4

#ifndef __ANDROID__
        mediaObject->seek((qint64)msec);
#endif

#elif QT5

#endif

    }
    void setZoomLevel(QString zoom);
    void setAspectRatio(QString aspect);

    QImage getScreenShot();

    void setVideoSize(int h, int w) {
#ifdef QT4
#ifndef __ANDROID__
        videoSurface->setFixedSize(w, h);
#endif
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

    bool initViews(bool flipped);

};

#endif // MEDIAMANAGER_H
