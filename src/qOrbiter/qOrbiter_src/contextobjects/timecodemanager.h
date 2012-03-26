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
    Q_PROPERTY (int playbackSpeed READ getSpeed WRITE setSpeed NOTIFY playbackSpeedChanged)
public:
    explicit TimeCodeManager(QObject *parent = 0);

    int tcTotalTime;
    int tcCurrentTime;
    int playbackSpeed;
    bool paused;

    QByteArray socketData;

    QString mediaPlayerIp;
    QTcpSocket *dceMediaSocket;
    int port;

    
signals:
    void totalTimeChanged();
    void timeChanged();
    void playbackSpeedChanged();
    void tcMessage();
    
public slots:
    void start(QString server, int iport);
    void stop();
    void restart();
    void updateTimeCode();

    void setTotalTime(int tTime) {tcTotalTime = tTime; emit totalTimeChanged();}
    int getTotalTime(){return tcTotalTime;}

    void setTime(int time) { tcCurrentTime = time; emit timeChanged();}
    int getTime () {return tcCurrentTime;}

    void setSpeed (int speed) { playbackSpeed = speed; emit playbackSpeedChanged();}
    int getSpeed () { return playbackSpeed; }

    
};

#endif // TIMECODEMANAGER_H
