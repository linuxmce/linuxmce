#ifndef SLEEPINGALARM_H
#define SLEEPINGALARM_H

#include <QObject>
#include <QVariant>
#include <QHash>
#ifdef debug
#include <QDebug>

#endif
#include "../genericmodelitem.h"
class SleepingAlarm : public GenericModelItem
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

    explicit SleepingAlarm(QObject* parent = NULL);
    void setAlarmData(int i, QString n, QString f, bool u, QString c, QString k);
    virtual bool setData(int role, const QVariant &value);

signals:  
    void dataChanged();

};

#endif // SLEEPINGALARM_H
