#ifndef SLEEPINGALARM_H
#define SLEEPINGALARM_H

#include <QObject>
#include <QDebug>

class SleepingAlarm : public QObject
{
    Q_OBJECT
    Q_PROPERTY (int eventHandler WRITE setHandler READ getHandler NOTIFY eventHandlerChanged)
    Q_PROPERTY (QString alarmTime WRITE setAlarmTime READ getAlarmTime NOTIFY alarmTimeChanged)
    Q_PROPERTY (bool state READ getState WRITE setState NOTIFY stateChanged)
    Q_PROPERTY (QString timeLeft READ getTimeLeft WRITE setTimeLeft NOTIFY countDownChanged)
    Q_PROPERTY (QString activeDays READ getDaysActive WRITE setDaysActive NOTIFY daysChanged)
    Q_PROPERTY (QString status READ getStatus WRITE setStatus NOTIFY statusChanged)
public:

    SleepingAlarm(int i, QString f, bool u, QString c, QString k):eventHandler(i), alarmTime(f), state(u), timeLeft(c), activeDays(k) {

        qDebug() << state;
        if (state == false)
        {
            status = "Off";
        }
        else
        {
            status = "On";
        }
     }

 int eventHandler;
 QString alarmTime;
 bool state;
 QString timeLeft;
 QString activeDays;
 QString status;


signals:
 void eventHandlerChanged();
 void stateChanged();
 void alarmTimeChanged();
 void countDownChanged();
 void daysChanged();
 void statusChanged();

public slots:
 void setHandler(int iHandler) {eventHandler = iHandler; emit eventHandlerChanged();}
 int getHandler () {return eventHandler;}

 void setState (bool iState) { state = iState; emit stateChanged();}
 bool getState () {return state;}

 void setStatus (QString iStatus) {alarmTime = iStatus; emit statusChanged();}
 QString getStatus () {return status;}

 void setAlarmTime (QString iAlarmTime) {alarmTime = iAlarmTime; emit alarmTimeChanged();}
 QString getAlarmTime () {return alarmTime;}

 void setTimeLeft(QString iTimeLeft) {timeLeft = iTimeLeft; emit countDownChanged();}
 QString getTimeLeft () {return timeLeft;}

 void setDaysActive(QString iDaysActive) {activeDays = iDaysActive; emit daysChanged();}
 QString getDaysActive () {return activeDays;}


};

#endif // SLEEPINGALARM_H
