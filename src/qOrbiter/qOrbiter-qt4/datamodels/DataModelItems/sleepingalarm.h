#ifndef SLEEPINGALARM_H
#define SLEEPINGALARM_H

#include <QObject>
#ifdef debug
#include <QDebug>
#endif
class SleepingAlarm : public QObject
{

    Q_PROPERTY (int eventHandler WRITE setHandler READ getHandler NOTIFY eventHandlerChanged)
    Q_PROPERTY (QString alarmTime WRITE setAlarmTime READ getAlarmTime NOTIFY alarmTimeChanged)
    Q_PROPERTY (bool b_state READ getState WRITE setState NOTIFY stateChanged)
    Q_PROPERTY (QString timeLeft READ getTimeLeft WRITE setTimeLeft NOTIFY countDownChanged)
    Q_PROPERTY (QString activeDays READ getDaysActive WRITE setDaysActive NOTIFY daysChanged)
    Q_PROPERTY (QString status READ getStatus WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY (QString name READ getName WRITE setName NOTIFY nameChanged)
     Q_OBJECT
public:

    SleepingAlarm(int i, QString n, QString f, bool u, QString c, QString k):eventHandler(i), name(n), alarmTime(f), b_state(u), timeLeft(c), activeDays(k) {


        if (b_state == false)
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
 bool b_state;
 QString timeLeft;
 QString activeDays;
 QString status;
 QString name;


signals:
 void eventHandlerChanged();
 void stateChanged();
 void alarmTimeChanged();
 void countDownChanged();
 void daysChanged();
 void statusChanged();
 void nameChanged();
public slots:
 void setHandler(int iHandler) {eventHandler = iHandler; emit eventHandlerChanged();}
 int getHandler () {return eventHandler;}

 void setState (bool iState) { b_state = iState; emit stateChanged();}
 bool getState () {return b_state;}

 void setStatus (QString iStatus) {alarmTime = iStatus; emit statusChanged();}
 QString getStatus () {return status;}

 void setAlarmTime (QString iAlarmTime) {alarmTime = iAlarmTime; emit alarmTimeChanged();}
 QString getAlarmTime () {return alarmTime;}

 void setTimeLeft(QString iTimeLeft) {timeLeft = iTimeLeft; emit countDownChanged();}
 QString getTimeLeft () {return timeLeft;}

 void setDaysActive(QString iDaysActive) {activeDays = iDaysActive; emit daysChanged();}
 QString getDaysActive () {return activeDays;}

 void setName(QString iName) {name = iName; emit nameChanged();}
 QString getName () {return name;}


};

#endif // SLEEPINGALARM_H
