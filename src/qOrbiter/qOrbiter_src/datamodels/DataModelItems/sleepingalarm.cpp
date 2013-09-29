#include "sleepingalarm.h"
#include <QByteArray>
#include <QHash>
#include <QVariant>

SleepingAlarm::SleepingAlarm(int i, QString n, QString f, bool u, QString c, QString k):eventHandler(i), name(n), alarmTime(f), b_state(u), timeLeft(c), activeDays(k) {

}

QHash<int, QByteArray> SleepingAlarm::roleNames() const
{
    QHash<int, QByteArray> names;
    names[NameRole] = "name";
    names[DescriptionRole] = "desc";
    names[TimeRole] = "alarmtime";
    names[DayRole] = "active";
    names[StatusRole] = "status";
    names[EventHandlerRole] = "handler";
    names[RemainingRole] = "remaining";
    return names;
}

void SleepingAlarm::updateStatus(bool s)
{
    b_state=s;
    emit dataChanged();
}

QVariant SleepingAlarm::data(int role) const
{
    switch(role) {
    case NameRole:
      return id();
    case EventHandlerRole:
        return handler();
    case RemainingRole:
      return remaining();
    case DayRole:
      return active();
    case TimeRole:
        return time();
    case StatusRole:
        return currentState();
    default:
      return QVariant();
    }
}



