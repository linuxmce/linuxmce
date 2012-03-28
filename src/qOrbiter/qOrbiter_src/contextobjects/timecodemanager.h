#ifndef TIMECODEMANAGER_H
#define TIMECODEMANAGER_H

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QFile>
#include <QStringList>
#include <QRegExp>
#include <QTime>

class TimeCodeManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY (int tcTotalTime READ getTotalTime WRITE setTotalTime NOTIFY totalTimeChanged)
    Q_PROPERTY (int tcCurrentTime READ getTime WRITE setTime NOTIFY timeChanged)

    Q_PROPERTY (QString qsTotalTime READ getStringTotalTime WRITE setStringTotalTime NOTIFY totalStringTimeChanged)
    Q_PROPERTY (QString qsCurrentTime READ getStringTime WRITE setStringTime NOTIFY stringTimeChanged)

    Q_PROPERTY (QString stringPlaybackSpeed READ getStringPlaybackSpeed WRITE setStringPlaybackSpeed NOTIFY sPlaybackSpeedChanged)
    Q_PROPERTY (int playbackSpeed READ getSpeed WRITE setSpeed NOTIFY playbackSpeedChanged)

public:
    explicit TimeCodeManager();

    //these ints are used to represent the beginning and end of media in a format for consumption by a scroller
    //it needs to report the positon of the marker, and provide the time readout converted back to the proper HH::MM:SS format
    int tcTotalTime;
    int tcCurrentTime;

    QString qsTotalTime;
    QString qsCurrentTime;

    int playbackSpeed;
    QString stringPlaybackSpeed;
    bool paused;

    QByteArray socketData;

    QString mediaPlayerIp;
    QTcpSocket *dceMediaSocket;
    int port;

    QString qsTitle;
    QString qsChapter;

    
signals:
    void totalStringTimeChanged();
    void stringTimeChanged();
    void totalTimeChanged();
    void timeChanged();
    void positionChanged();
    void playbackSpeedChanged();
    void sPlaybackSpeedChanged();
    void tcMessage();
    
public slots:
    void start(QString server, int iport);
    void stop();
    void restart();
    void updateTimeCode();
    void convertToSeconds();
    void setPosition();

    void setStringTotalTime(QString qstTime) {qsTotalTime = qstTime; emit totalStringTimeChanged(); }
     QString getStringTotalTime(){return qsTotalTime;}

     void setStringTime(QString qstime) { qsCurrentTime = qstime; emit stringTimeChanged(); }
     QString getStringTime () {return qsCurrentTime;}

    void setTotalTime(int tTime) {tcTotalTime = tTime; emit totalTimeChanged();}
    int getTotalTime(){return tcTotalTime;}

    void setTime(int time) { tcCurrentTime = time; emit timeChanged();}
    int getTime () {return tcCurrentTime;}

    void setSpeed (int speed) { playbackSpeed = speed; emit playbackSpeedChanged();}
    int getSpeed () { return playbackSpeed; }

    void setStringPlaybackSpeed(QString pbs ) {stringPlaybackSpeed = pbs; emit sPlaybackSpeedChanged();}
    QString getStringPlaybackSpeed() {return stringPlaybackSpeed ; }
    
};

#endif // TIMECODEMANAGER_H
