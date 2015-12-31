#include "sleepingalarm.h"
#include <QByteArray>
#include <QHash>
#include <QVariant>
#include "DCE/Logger.h"
#include "../../qorbitermanager.h"

using namespace DCE;

SleepingAlarm::SleepingAlarm(QObject* parent) : GenericModelItem(parent) {

    QHash<int, QByteArray> names;
    names[NameRole] = "name";
    names[DescriptionRole] = "desc";
    names[TimeRole] = "alarmtime";
    names[DayRole] = "active";
    names[StatusRole] = "status";
    names[EventHandlerRole] = "handler";
    names[RemainingRole] = "remaining";
    setRoleNames(names);
}

void SleepingAlarm::setAlarmData(int i, QString n, QString f, bool u, QString c, QString k) {
    m_data.insert(NameRole, n);
    m_data.insert(EventHandlerRole, i);
    m_data.insert(TimeRole, f);
    m_data.insert(DayRole, k);
    m_data.insert(StatusRole, u);
    m_data.insert(RemainingRole, c);
}

bool SleepingAlarm::setData(int role, const QVariant &value)
{
    LoggerWrapper::GetInstance()->Write(LV_DEBUG, "SleepingAlarm.setData");
    QObject* p = parent();
    GenericFlatListModel* pModel = static_cast<GenericFlatListModel*>(p);
    QObject* p2 = p->parent();
    qorbiterManager* pManager = static_cast<qorbiterManager*>(p2);
    if (role == StatusRole)
    {
        bool b = value.toBool();
	int handler = m_data[EventHandlerRole].toInt();
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SleepingAlarm.setData model id = %s, handler = %d, Status = %d", pModel->modelName.toStdString().c_str(), handler, b);
	emit pManager->updateAlarm(pModel->modelName, pModel->indexFromItem(this).row(), role, b, handler);
    }
    LoggerWrapper::GetInstance()->Write(LV_DEBUG, "SleepingAlarm.setData end");
    return true;
}



