#ifndef SLEEPINGALARM_H
#define SLEEPINGALARM_H

#include <QObject>
#include <QVariant>
#include <QHash>
#ifdef debug
#include <QDebug>

#endif
class SleepingAlarm : public QObject
{
    Q_OBJECT

    enum Roles {
        NameRole = Qt::UserRole+1,
        DescriptionRole= Qt::UserRole+2,
        TimeRole= Qt::UserRole+3,
        DayRole= Qt::UserRole+4,
        StatusRole= Qt::UserRole+5,
        EventHandlerRole= Qt::UserRole+6,
        RemainingRole= Qt::UserRole+7
    };
public:

   explicit SleepingAlarm(int i, QString n, QString f, bool u, QString c, QString k);

    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;
    inline QString id() const { return name; }
    inline int handler() const { return eventHandler; }
    inline QString remaining() const { return timeLeft; }
    inline QString active() const { return activeDays; }
    inline bool currentState() const {return b_state;}
    inline QString time() const {return alarmTime;}

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
