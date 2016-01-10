/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
    Author: Langston Ball
   Langston Ball  golgoj4@gmail.com
    QOrbiter is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QOrbiter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QOrbiter.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H




#ifdef QT4
#include <qdeclarative.h>
#include <QDeclarativeItem>
#include <QGLWidget>

#ifndef Q_OS_ANDROID && !defined(Q_OS_IOS)
#include <QBoxLayout>
#include <phonon>
#include <QHBoxLayout>
#include <colorfilterproxywidget.h>
#include <phonon/mediacontroller.h>
#endif

#elif QT5
#include <QQuickItem>
#include <QImage>

#endif

#include <QKeyEvent>
#include <QObject>

#include <qMediaPlayer/qMediaPlayer.h>

#include <QTime>
#include <QTcpServer>

class NavigationMenu;

using namespace DCE;
using namespace Qt;
/*!
 * \brief The MediaManager class provides Video and Audio playback for DCE related video streams.
 * This class represents a network media player embedded into the application. It runs as a plugin
 * as opposed to being directly instantiated by the Application to allow for modularity.
 */
class MediaManager :
        #ifdef QT4
        public QDeclarativeItem
        #elif QT5
        public QQuickItem
        #endif
{


    Q_PROPERTY(bool connected READ getConnectionStatus WRITE setConnectionStatus NOTIFY connectedChanged)
    Q_PROPERTY(QString pluginUrl READ getPluginUrl NOTIFY pluginUrlUpdated)
    Q_PROPERTY(QString currentStatus READ getCurrentStatus WRITE setCurrentStatus NOTIFY currentStatusChanged)
    Q_PROPERTY(bool mediaPlaying READ getMediaPlaying WRITE setMediaPlaying NOTIFY mediaPlayingChanged)
    Q_PROPERTY(bool hasError READ getErrorStatus WRITE setErrorStatus NOTIFY hasErrorChanged)
    Q_PROPERTY(QString lastError READ getMediaError WRITE setMediaError NOTIFY lastErrorChanged)
    Q_PROPERTY(int mediaBuffer READ getMediaBuffer WRITE setMediaBuffer NOTIFY mediaBufferChanged)
    Q_PROPERTY(QString fileReference READ getFileReference NOTIFY fileReferenceChanged)
    Q_PROPERTY(qreal volume READ getVolume NOTIFY volumeChanged)
    Q_PROPERTY(qreal displayVolume READ getDisplayVolume() WRITE setDisplayVolume NOTIFY displayVolumeChanged)
    Q_PROPERTY(bool muted READ getMuted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(QString fileUrl READ getMediaUrl NOTIFY fileUrlChanged)
    Q_PROPERTY(int incomingTime READ getIncomingTime WRITE setIncomingTime NOTIFY incomingTimeChanged)
    Q_PROPERTY(bool flipColors READ getColorFlip WRITE setColorFlip NOTIFY colorFlipChanged)
#ifdef QT4
#ifndef ANDROID
    Q_PROPERTY(bool useInvertTrick READ getInvertTrick WRITE setInvertTrick  NOTIFY useInvertTrickChanged)
#endif
#endif
    Q_PROPERTY(bool videoStream READ getVideoStream WRITE setVideoStream NOTIFY videoStreamChanged)
    Q_PROPERTY(QString serverAddress READ getServerAddress WRITE setServerAddress NOTIFY serverAddressChanged)
    Q_PROPERTY(int deviceNumber READ getDeviceNumber WRITE setDeviceNumber NOTIFY deviceNumberChanged)

#ifndef Q_OS_ANDROID

#ifndef QT5
    Q_PROPERTY(QList <Phonon::AudioOutputDevice> outputs READ getAvailibleOutputs NOTIFY availibleAudioOutputsChanged())
#endif

#endif

    Q_OBJECT

public:
#ifdef QT4
    explicit MediaManager(QDeclarativeItem *parent = 0);
#elif QT5
    explicit MediaManager(QQuickItem *parent = 0);
#endif

    //media info
    int currentTime;
    qint64 totalTime;
    int playBackSpeed;
    int streamId;
    QString fileReference;
    int fileno;
    QString filepath;
    QString fileUrl;
    int mediaBuffer;

    bool mediaPlaying;
    bool hasError;
    bool useInvertTrick;
    QString lastError;
    QString lastTick;

    //--------------------------
    qreal volume;
    qreal displayVolume;
    bool muted;

#ifdef NECESSITAS

    long callbackAddress;
#endif

#ifdef __ANDROID__
#ifdef QT4

#endif
#endif

    QString currentStatus;
    QString qs_totalTime;


    QTcpServer *timeCodeServer;
#ifndef QT5
    QTcpServer *infoSocket;
    QTcpServer *tcCallback;
#endif

    QString current_position;
    int iCurrent_Position;



#ifndef Q_OS_ANDROID
#ifdef QT4
    QWidget *window;
    QList <Phonon::AudioOutputDevice> outputs;
    QVBoxLayout *layout;
    ColorFilterProxyWidget *filterProxy;
#elif QT5
    /*
     *In Qt5
     *We dont need to do any painting of the operations normally
     *This may change with raspberry pi
     */
#endif
#endif

    qMediaPlayer *mediaPlayer;

    QList<QTcpSocket*> clientList;
    QTcpSocket*lastClient;
    QTcpSocket*callbackClient;



    QString serverAddress;
    int deviceNumber;

    QString pluginUrl;

    bool connected;

    int videoHeight;
    int videoWidth;

    bool flipColors;
    quint64 tempTime;

    int incomingTime;
    long currentDevice;
    bool videoStream;

#ifdef QT4

#ifndef ANDROID
    Phonon::VideoWidget *videoSurface;
    Phonon::AudioOutput *audioSink;
    Phonon::MediaObject *mediaObject;
    Phonon::MediaController * discController;
    QGLWidget *accel;
    QWidget *surf;
#endif

#endif

signals:
    void useInvertTrickChanged();
    void videoStreamChanged();
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

    void pluginUrlUpdated();
    void asyncStop();
    void asyncPositionRequest(int r);
    void callbackChanged();
    void fileReferenceChanged();
    void fileUrlChanged();
    void fileNumberChanged();
    void incomingTimeChanged();
    void incomingTick(quint64);
    void updatePluginSeek(int pos);

    void setPluginVolume(double d);
    void pauseMedia();
    void requestPosition();
    void requestDuration();
    void displayVolumeChanged();
    void pluginVolumeUp();
    void pluginVolumeDown();

public slots:
#ifdef QT4
#ifndef ANDROID
    void setInvertTrick(bool i){ if(i!=useInvertTrick){useInvertTrick=i; if(filterProxy){filterProxy->enable=useInvertTrick;} emit useInvertTrickChanged();} }
    bool getInvertTrick() {return useInvertTrick;}
#endif
#endif
    void setVideoStream(bool b ){
#ifdef QT4
#ifndef ANDROID

        videoStream = mediaObject->hasVideo(); emit videoStreamChanged();
        qDebug() << "qMediaPlayer::setVideoStream("<<videoStream<<")";

#endif
#else
        if(videoStream != b){
            videoStream = b;
            emit videoStreamChanged();
        }
        qDebug() << "Item has video::"<<videoStream;
#endif

    }
    bool getVideoStream(){return videoStream;}

    void setCurrentDevice(long d){currentDevice = d;mountDrive(currentDevice);}

    void setIncomingTime(int i){ incomingTime = i; emit incomingTimeChanged();}
    int getIncomingTime() { return incomingTime;}

    QString getPluginUrl(){ return pluginUrl;}

    void stopPluginMedia(){
        setMediaPlaying(false);
    }

    void qmlPlaybackEnded(bool ended){        
        mediaPlayer->EVENT_Playback_Completed(mediaPlayer->getCurrentMediaUrl().toStdString(),mediaPlayer->getStreamID(), ended);
    }


    void setColorFlip(bool f){
        flipColors = f;
#ifndef Q_OS_ANDROID
#ifndef QT5

        if(filterProxy)
            filterProxy->invert = f;

#endif
#endif
        emit colorFlipChanged();
    }
    bool getColorFlip() { return flipColors;}

#ifndef Q_OS_ANDROID
#ifndef QT5
    void setAvailibleOutputs(QList<Phonon::AudioOutputDevice> l){outputs.clear(); outputs = l; emit availibleAudioOutputsChanged(); }
    QList <Phonon::AudioOutputDevice> getAvailibleOutputs(){ return outputs;}
#endif
#endif

    void setMuted(bool m){muted = m; emit mutedChanged();}
    bool getMuted(){ return muted;}

    void triggerVolumeChange(){

    }

    void setPaused(){
        qWarning() << "mediaManager::Setting pause";
        emit pauseMedia();
    }

    void setVolume(qreal vol){
#ifdef QT4
#ifndef Q_OS_ANDROID
        audioSink->setVolume(vol);
#else
        setPluginVolume((double)vol);
#endif
#endif
        volume = vol;
        displayVolume=vol;
        qWarning() << "Vol level :: " << vol;
        emit displayVolumeChanged();
        emit volumeChanged();
    }

    qreal getVolume(){
        return volume;
    }

    void setDisplayVolume(qreal lvl){if(displayVolume !=lvl){ displayVolume=lvl; emit displayVolumeChanged();} }
    qreal getDisplayVolume(){return displayVolume;}

    void mediaStarted(){
        setMediaPlaying(true);
    }

    void setServerAddress(QString a) {if(serverAddress !=a) {serverAddress = a;emit serverAddressChanged();}}
    QString getServerAddress(){return serverAddress;}

    void setDeviceNumber(int d) { if(deviceNumber !=d) {deviceNumber = d; emit deviceNumberChanged();}}
    int getDeviceNumber() {return deviceNumber;}

    void updateMetaData(){
#ifndef __ANDROID__
#ifndef QT5
        qDebug() <<"Artist::"<< mediaObject->metaData(Phonon::ArtistMetaData);
        qDebug() <<"Album::"<< mediaObject->metaData(Phonon::AlbumMetaData);
        qDebug() <<"Title::"<< mediaObject->metaData(Phonon::TitleMetaData);
        qDebug() <<"Date::"<< mediaObject->metaData(Phonon::DateMetaData);
        qDebug() <<"Genre::"<< mediaObject->metaData(Phonon::GenreMetaData);
        qDebug() <<"Track::"<< mediaObject->metaData(Phonon::TracknumberMetaData);
        qDebug() <<"Description::"<< mediaObject->metaData(Phonon::DescriptionMetaData);
#endif
#endif

    }

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
        setVideoStream(false);

        if (mediaPlaying==false)
        {
#ifndef Q_OS_ANDROID
#ifndef QT5
            if(filterProxy)
                filterProxy->hide();
#endif
#endif
        }
        else
        {
#ifndef Q_OS_ANDROID
#ifdef QT4
            if(filterProxy)
                filterProxy->show();
#endif
#endif
        }
        emit mediaPlayingChanged();

    }
    bool getMediaPlaying() {return mediaPlaying;}

    void setFileReference(QString f){
qDebug() << "Updating file url.";
        fileReference = f;
        pluginUrl = fileReference;
#if defined(ANDROID) || defined(Q_OS_IOS) || defined(QT5)
        emit pluginUrlUpdated();
        connectInfoSocket();
        startTimeCodeServer();
        qDebug() << "CPP Url Updated";

#endif
        emit fileReferenceChanged();
    }
    QString getFileReference() {return fileReference; }

    void setFileNumber(int n) {fileno = n; fileNumberChanged();}

    void transmit(QString d);
    void setStreamId(int id) {streamId = id; }

    void setMediaUrl(QString url);
    QString getMediaUrl(){ return fileUrl;}

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
    void callBackClientConnected();
    void startTimeCodeServer();
    void stopTimeCodeServer();
    void handleError(){
    }
    void forwardCallbackData();

    void setState(){
#ifndef __ANDROID__
#ifdef QT4
        qDebug() << mediaObject->state();
        int i =  mediaObject->errorType();
        if(i==0){
            setCurrentStatus("Media Player Loading");
#ifndef __ANDROID__
#ifdef QT4
            qDebug() << "Titles: \t" << discController->availableTitles();
            qDebug() << "Chapters: \t" << discController->availableChapters();
            qDebug() << "SubTitles: \t" << discController->availableSubtitles();
            qDebug() << "Angles: \t" << discController->availableAngles();
            qDebug() << "Audio Channels: \t" << discController->availableAudioChannels();
            qDebug() << "Features: \t" << discController->supportedFeatures();


#endif
#endif
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
#endif
    }

    void setMediaPosition(int msec) {
        qDebug() << "Seeking to " << msec << " msec";
#ifdef QT4

#ifndef __ANDROID__
        mediaObject->seek((qint64)msec);
#endif

#if defined ANDROID
        updatePluginSeek(msec);
#endif


#elif QT5
        updatePluginSeek(msec);
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

    void setQmlTotalTime(int inSec){
        setCurrentStatus("Android Time call in Audio Visual Plugin::"+QString::number(inSec));
        int s = inSec ;

        int seconds = s / 1000;
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
        totalTime = s;
        qDebug() << qs_totalTime;
        emit totalTimeChanged();
        processTimeCode(0);
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
        emit totalTimeChanged();
    }


    QString getTotalTime() {return qs_totalTime;}

    void processTimeCode(qint64 f);
    void processSocketdata();

    void reInit(){
        setConnectionStatus(false);
        mediaPlayer=NULL;
        initializePlayer();
    }

    void pluginNotifyStart(){startTimeCodeServer(); mediaPlayer->EVENT_Playback_Started(this->fileReference.toStdString(), mediaPlayer->i_StreamId, "", "", "");}
    void pluginNotifyEnd(bool withError){mediaPlayer->EVENT_Playback_Completed(this->fileReference.toStdString(), mediaPlayer->i_StreamId, withError);}

#ifndef __ANDROID__
#ifdef QT4

#endif
#endif

    //Event slots
    void playbackInfoUpdated(QVariant playbackData);


private:
    void initializePlayer();
    void initializeConnections();
    void shutdownDevice();
    bool mountDrive(long device);




private slots:
    void infoConnectHandler();
    void connectInfoSocket();
    void disconnectInfoSocket();


    bool initViews(bool flipped);
    void setupDirectories();
#ifdef QT4
#ifndef Q_OS_ANDROID
    void setSubtitles(){      qDebug() << "SubTitles: \t" << discController->availableSubtitles(); }
    void setAvailTitles(int t){ qDebug() << "Titles: \t" << discController->availableTitles(); }
    void setChapters(int c){qDebug() << "Chapters: \t" << discController->availableChapters(); }
    void setAngles(int a) {qDebug() << "Angles: \t" << discController->availableAngles(); }
    void setAudioChannels(){ qDebug() << "Audio Channels: \t" << discController->availableAudioChannels();}
    void setMenus(QList<NavigationMenu> *menus){ qDebug()<< "Menus\t" << menus;}

#endif
#endif
};


QML_DECLARE_TYPE(MediaManager)

#endif // MEDIAMANAGER_H
