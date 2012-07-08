#ifndef TIMECODEMANAGER_H
#define TIMECODEMANAGER_H

#include <QtGlobal>
#include <QtDeclarative/QDeclarativeItem>
#include <QtNetwork/QTcpSocket>
#include <QFile>
#include <QStringList>
#include <QRegExp>
#include <QTime>

class TimeCodeManager : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY (int tcTotalTime READ getTotalTime WRITE setTotalTime NOTIFY totalTimeChanged)
    Q_PROPERTY (int tcCurrentTime READ getTime WRITE setTime NOTIFY timeChanged)
    Q_PROPERTY (double runningTimer READ getProgressBar WRITE setProgressBar NOTIFY positionChanged)
    Q_PROPERTY (QString dragTime READ getDragTime WRITE setDragTime NOTIFY dragtimeChanged)

    Q_PROPERTY (QString qsTotalTime READ getStringTotalTime WRITE setStringTotalTime NOTIFY totalStringTimeChanged)
    Q_PROPERTY (QString qsCurrentTime READ getStringTime WRITE setStringTime NOTIFY stringTimeChanged)

    Q_PROPERTY (QString stringPlaybackSpeed READ getStringPlaybackSpeed WRITE setStringPlaybackSpeed NOTIFY sPlaybackSpeedChanged)
    Q_PROPERTY (int playbackSpeed READ getSpeed WRITE setSpeed NOTIFY playbackSpeedChanged)

public:
    explicit TimeCodeManager(QDeclarativeItem *parent = 0);

    //these ints are used to represent the beginning and end of media in a format for consumption by a scroller
    //it needs to report the positon of the marker, and provide the time readout converted back to the proper HH::MM:SS format
    int tcTotalTime;
    int tcCurrentTime;
    double runningTimer;

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

    QString dragTime;
    int i_dragTime;

    bool reverse;

    QString ObjectToString(const QObject * obj);
signals:
    void totalStringTimeChanged();
    void stringTimeChanged();
    void totalTimeChanged();
    void timeChanged();
    void positionChanged();
    void playbackSpeedChanged();
    void sPlaybackSpeedChanged();
    void tcMessage();
    void seekToTime(QString time);
    void dragtimeChanged();

    
public slots:
    void start(QString server, int iport);
    void stop();
    void restart();
    void updateTimeCode();
    void convertToSeconds();
    void setPosition();

    void showDragTime(int seconds);


    void setDragTime(QString dtime) {dragTime = dtime; emit dragtimeChanged();}
    QString getDragTime () {return dragTime;}

    void setStringTotalTime(QString qstTime) {qsTotalTime = qstTime; emit totalStringTimeChanged(); }
     QString getStringTotalTime(){return qsTotalTime;}

     void setStringTime(QString qstime) { qsCurrentTime = qstime; setPosition(); emit stringTimeChanged(); }
     QString getStringTime () {return qsCurrentTime;}

    void setTotalTime(int tTime) {tcTotalTime = tTime; emit totalTimeChanged();}
    int getTotalTime(){return tcTotalTime;}

    void setTime(int time) { tcCurrentTime = time; emit timeChanged();}
    int getTime () {return tcCurrentTime;}

    void setSpeed (int speed) { playbackSpeed = speed; emit playbackSpeedChanged();}
    int getSpeed () { return playbackSpeed; }

    void setStringPlaybackSpeed(QString pbs ) {stringPlaybackSpeed = pbs; emit sPlaybackSpeedChanged();}
    QString getStringPlaybackSpeed() {return stringPlaybackSpeed ; }

    void setProgressBar(int progress) {runningTimer = progress; emit positionChanged();}
    int getProgressBar() {return runningTimer;}

    void finishDragging() {

      //  QString t = dragTime;
      //  t.replace(QRegExp("0:(?!/d)"), "00:");
      //  t.replace(QRegExp("0:(?![/d/d[1-9][1-9]:)"), "0:0");
                emit seekToTime(QString::number(i_dragTime));

    }
    
};

#endif // TIMECODEMANAGER_H
