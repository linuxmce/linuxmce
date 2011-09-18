#ifndef SLEEPINGALARM_H
#define SLEEPINGALARM_H

#include <QObject>

class SleepingAlarm : public QObject
{
    Q_OBJECT
    Q_PROPERTY (int eventHandler WRITE setHandler READ getHandler NOTIFY eventHandlerChanged)
    Q_PROPERTY (QString alarmTime WRITE setAlarmTime READ getAlarmTime NOTIFY alarmTimeChanged)
    Q_PROPERTY (bool state READ getState WRITE setState NOTIFY stateChanged)
    Q_PROPERTY (QString timeLeft READ getTimeLeft WRITE setTimeLeft NOTIFY countDownChanged)
    Q_PROPERTY (QString activeDays READ getDaysActive WRITE setDaysActive NOTIFY daysChanged)
public:
     SleepingAlarm(int i, QString f, bool u, QString c, QString k) {
     setHandler(i);
     setState(u);
     setAlarmTime(f);
     setTimeLeft(c);
     setDaysActive(k);

     }

 int eventHandler;
 QString alarmTime;
 bool state;
 QString timeLeft;
 QString activeDays;


signals:
 void eventHandlerChanged();
 void stateChanged();
 void alarmTimeChanged();
 void countDownChanged();
 void daysChanged();

public slots:
 void setHandler(int iHandler) {eventHandler = iHandler; emit eventHandlerChanged();}
 int getHandler () {return eventHandler;}

 void setState (bool iState) { state = iState; emit stateChanged();}
 bool getState () {return state;}

 void setAlarmTime (QString iAlarmTime) {alarmTime = iAlarmTime; emit alarmTimeChanged();}
 QString getAlarmTime () {return alarmTime;}

 void setTimeLeft(QString iTimeLeft) {timeLeft = iTimeLeft; emit countDownChanged();}
 QString getTimeLeft () {return timeLeft;}

 void setDaysActive(QString iDaysActive) {activeDays = iDaysActive; emit daysChanged();}
 QString getDaysActive () {return activeDays;}


};

#endif // SLEEPINGALARM_H
