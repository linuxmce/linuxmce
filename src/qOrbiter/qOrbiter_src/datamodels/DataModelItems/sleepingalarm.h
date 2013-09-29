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
    void dataChanged();
public slots:
    void updateStatus(bool s);

};

#endif // SLEEPINGALARM_H
