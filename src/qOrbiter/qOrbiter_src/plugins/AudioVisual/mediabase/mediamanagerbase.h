#ifndef MEDIAMANAGERBASE_H
#define MEDIAMANAGERBASE_H

#include <QQuickItem>
#include <QImage>
#include <QKeyEvent>
#include <QObject>
#include <qMediaPlayer/qMediaPlayer.h>
#include <QTime>
#include <QTcpServer>

class NavigationMenu;

using namespace DCE;
using namespace Qt;

/*!
 * \brief The MediaManagerBase class. Base class for abstracting other media players in place of the default QML one.
 */
class MediaManagerBase : public QQuickItem
{
    Q_PROPERTY(int deviceNumber READ deviceNumber WRITE setDeviceNumber NOTIFY deviceNumberChanged)
    Q_PROPERTY(bool connected READ connected WRITE setConnected NOTIFY connectedChanged)
    Q_PROPERTY(QString pluginUrl READ pluginUrl WRITE setPluginUrl NOTIFY pluginUrlChanged)
    Q_PROPERTY(QString currentStatus READ currentStatus WRITE setCurrentStatus NOTIFY currentStatusChanged)
    Q_PROPERTY(QString fileReference READ fileReference WRITE setFileReference NOTIFY fileReferenceChanged)
    Q_PROPERTY(QString serverAddress READ serverAddress WRITE setServerAddress NOTIFY serverAddressChanged)

    Q_PROPERTY(bool hasError READ hasError WRITE setHasError NOTIFY hasErrorChanged)
    Q_PROPERTY(QString lastError READ lastError WRITE setLastError NOTIFY lastErrorChanged)

    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(qreal displayVolume READ displayVolume WRITE setDisplayVolume NOTIFY displayVolumeChanged)

    Q_PROPERTY(int mediaBuffer READ mediaBuffer WRITE setMediaBuffer NOTIFY mediaBufferChanged)
    Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(bool mediaPlaying READ mediaPlaying WRITE setMediaPlaying NOTIFY mediaPlayingChanged)
    Q_PROPERTY(bool hasVideoStream READ hasVideoStream WRITE setHasVideoStream NOTIFY hasVideoStreamChanged)
    Q_PROPERTY(bool pause READ pause WRITE setPause NOTIFY pauseChanged)

    Q_OBJECT
public:
    MediaManagerBase();

    int deviceNumber() const;
    void setDeviceNumber(int deviceNumber);

    int streamId() const;


    int fileNo() const;
    void setFileNo(int fileNo);

    QString filePath() const;
    void setFilePath(const QString &filePath);

    int playbackSpeed() const;
    void setPlaybackSpeed(int playbackSpeed);

    int mediaBuffer() const;
    void setMediaBuffer(int mediaBuffer);

    bool mediaPlaying() const;
    void setMediaPlaying(bool mediaPlaying);

    QString lastTick() const;
    void setLastTick(const QString &lastTick);

    qreal volume() const;
    void setVolume(const qreal &volume);

    qreal displayVolume() const;
    void setDisplayVolume(const qreal &displayVolume);

    bool muted() const;


    QString currentStatus() const;


    bool hasError() const;
    void setHasError(bool hasError);

    QString lastError() const;
    void setLastError(const QString &lastError);

    QString totalTime() const;
    void setTotalTime(const QString &totalTime);

    QString currentPosition() const;
    void setCurrentPosition(const QString &currentPosition);

    int currentPositionNumeric() const;
    void setCurrentPositionNumeric(int currentPositionNumeric);

    int currentTimeNumeric() const;
    void setCurrentTimeNumeric(int currentTimeNumeric);

    quint64 totalTimeNumeric() const;
    void setTotalTimeNumeric(const quint64 &totalTimeNumeric);

    QString serverAddress() const;
    void setServerAddress(const QString &serverAddress);

    QString pluginUrl() const;


    bool connected() const;


    int videoHeight() const;
    void setVideoHeight(int videoHeight);

    int videoWidth() const;
    void setVideoWidth(int videoWidth);

    bool hasVideoStream() const;
    void setHasVideoStream(bool hasVideoStream);



    long getCurrentStorageDevice() const;


    bool pause() const;


    bool getUsingExternalMediaPlayer() const;


signals:
    void readyForConnections();

    void usingExternalMediaPlayerChanged();
    void deviceNumberChanged();
    void streamIdChanged();
    void fileNoChanged();
    void fileReferenceChanged();
    void filePathChanged();

    void playbackSpeedChanged();
    void mediaBufferChanged();
    void mediaPlayingChanged();
    void lastTickChanged();
    void volumeChanged();
    void displayVolumeChanged();
    void mutedChanged(bool mute);
    void currentStatusChanged();
    void hasErrorChanged();
    void lastErrorChanged();
    void totalTimeChanged();
    void currentPositionChanged();
    void currentPositionNumericChanged();
    void totalTimeNumericChanged();
    void pauseChanged();
    void zoomLevelChanged();

    void serverAddressChanged();
    void pluginUrlChanged();
    void connectedChanged();

    void videoHeightChanged();
    void videoWidthChanged();
    void hasVideoStreamChanged();

    void pluginVolumeUp();
    void pluginVolumeDown();
    void pauseMedia();
    void updatePluginSeek(int pos);
    void currentStorageDeviceChanged();

public slots:

    void setMuted(bool muted);
    void setCurrentStorageDevice(long currentStorageDevice);
    void setPluginUrl(const QString &pluginUrl);
    QString fileReference() const;
    void setCurrentStatus(const QString &currentStatus);
    void stopPluginMedia();
    void mediaStarted();
    void setConnected(bool connected);
    void setStreamId(int streamId);
    void setFileReference(const QString &fileReference);

    void togglePause();
    void setPause(bool paused);
    void setMediaPosition(int msec);
    void setZoomLevel(QString zoom);


    void setConnectionDetails(int device, QString routerAdd);
    void qmlPlaybackEnded(bool ended) ;
    virtual QImage getScreenShot() = 0;

    void setQmlTotalTime(int inSeconds);
    void setTotalTimeFromPlugin(quint64 inSeconds);
    void reInit();

    void pluginNotifyStart(QString title, QString audioOptions="", QString videoOptions="");
    void pluginNotifyEnd(bool withError);
    void playbackInfoUpdated(QString mediaTitle, QString mediaSubTitle, QString name, int screen);

    void stopTimeCodeServer();
    void startTimeCodeServer();

    void onNewClientConnected();
    void processTimeCode(quint64 time);

protected:

    void initializePlayer();
    void initializeConnections();
    void breakConnections();

      qMediaPlayer *mediaPlayer;

protected slots:
    void setUsingExternalMediaPlayer(bool value);

private:
    void transmit(QString msg);




private:
    int     m_deviceNumber;
    int     m_streamId;
    int     m_fileNo;
    QString m_fileReference;
    QString m_filePath;

    int     m_playbackSpeed;
    int     m_mediaBuffer;
    bool    m_mediaPlaying;
    bool    m_paused;
    QString m_lastTick;
    qreal   m_volume;
    qreal   m_displayVolume;
    bool    m_muted;

    QString m_zoomLevel;
    QString m_aspectRatio;


    QString m_currentStatus;
    bool    m_hasError;
    QString m_lastError;

    QString m_totalTime;
    QString m_currentPosition;
    int     m_currentPositionNumeric;
    int     m_currentTimeNumeric;
    quint64 m_totalTimeNumeric;



    QTcpServer *timeCodeServer;
    QList<QTcpSocket*> clientList;
    QTcpSocket*lastClient;
    QTcpSocket*callbackClient;

    QString m_serverAddress;
    QString m_pluginUrl;

    bool m_connected;

    int m_videoHeight;
    int m_videoWidth;
    bool m_hasVideoStream;

    long m_currentStorageDevice;

    bool usingExternalMediaPlayer;

};

#endif // MEDIAMANAGERBASE_H
