#ifndef OMXINTERFACE_H
#define OMXINTERFACE_H

#include <QObject>
#include "qprocess.h"
#include "OmxDbusProxy.h"

class omxdbusplayerinterface;

class OmxInterface : public QObject
{
    Q_OBJECT
public:
    explicit OmxInterface(QObject *parent = 0);
    Q_PROPERTY(bool omxConnected READ omxConnected WRITE setOmxConnected NOTIFY omxConnectedChanged)
    Q_PROPERTY(QString playbackStatus READ getPlaybackStatus WRITE setPlaybackStatus NOTIFY playbackStatusChanged)
    Q_PROPERTY(qlonglong duration READ duration  WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(qlonglong position READ getPosition WRITE setPosition NOTIFY positionChanged)

signals:
    void dbusAddressChanged();
    void dbusPidChanged();
    void omxConnectedChanged(bool isConnected);
    void playbackStatusChanged(QString status);
    void durationChanged(quint64 totalTime);
    void positionChanged(quint64 time);
    void volumeChanged(double vol);

    void mediaEnded(bool withError);
    void mediaStarted();


public slots:
    void runOmxWithFile(QString file ="/media/84/public/data/videos/JAWS.mp4");
    void stopPlayer();
    void seekToPosition(int position);
    void handleTimecodeTick(qlonglong tick);
    void handleStateChanged(QString s, QDBusVariant v);
    void setOmxVolume(double vol);
    void handleMediaStopped();

public:
    QString dbusAddress() const;
    void setDbusAddress(const QString &dbusAddress);

    bool omxConnected() const;
    void setOmxConnected(bool omxConnected);

    QString dbusOmxPid() const;
    void setDbusOmxPid(const QString &dbusOmxPid);

    QString getPlaybackStatus() const;
    void setPlaybackStatus(const QString &playbackStatus);

    long duration() const;
    void setDuration(qlonglong duration);

    long getPosition() const;
    void setPosition(qlonglong position);

    void pause();

    void handleMute(bool mute);

    void handleVolumeUp();
    void handleVolumeDown();
    void requestVolume();

    void fileIdentity(QString identity);

private: //controller members
    bool                                m_omxConnected;
    OmxDbusProxy *                      dbusOmxProperties;
    OmxDbusProxyPlayer *                dbusOmxPlayer;
    OmxDbusProxyRoot *                  dbusOmxRoot;
    QString                             m_dbusAddress;
    QString                             m_dbusOmxPid;
    QProcess *                          m_omxProcess;
    static const QString                m_dbusName;

private slots:
    void handleOmxProcessChanged(QProcess::ProcessState s);

private://members for Omx properties
    bool m_canControl;
    bool m_canGoNext;
    bool m_canGoPrevious;
    bool m_canPause;
    bool m_canPlay;
    bool m_canQuit;
    bool m_canRaise;
    bool m_canSeek;
    bool m_canSetFullScreen;
    double m_omxVolume;

    QList<QVariant> m_subtitleList;
    QList<QVariant> m_audioTrackList;
    QList<QVariant> m_videoTrackList;

    bool m_isFullscreen;
    QList<QVariant> m_hasTrackList;
    QString m_playbackStatus;
    long m_position;
    long m_duration;

    QString m_currentMediaLink;

private: //private functions
    void timerEvent(QTimerEvent *event);
    bool getOmxDbusInfo();
    void doSetup();

    void runOmxOnce();

    int m_checkTimer;
    bool interrupt;

    omxdbusplayerinterface *m_parentInterface;



};

#endif // OMXINTERFACE_H
