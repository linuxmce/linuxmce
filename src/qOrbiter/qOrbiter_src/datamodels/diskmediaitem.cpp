#include "diskmediaitem.h"
#include <QByteArray>
#include <QHash>
#include <QVariant>
#include "DCE/Logger.h"
#include "../qorbitermanager.h"

DiskMediaItem::DiskMediaItem(QObject *parent) : GenericModelItem(parent)
{
    QHash<int, QByteArray> names;
    names[DiscRole] = "drive";
    names[DescriptionRole] = "desc";
    names[FileRole] = "pkFile";
    names[RippingRole] = "isRipping";
    names[StatusRole] = "status";
    names[IdRole] = "discId";
    names[NumberRole] = "discNumber";
    names[DiscInfoRole] = "discName";
    names[LocationRole] = "displayLocation";
    setRoleNames(names);
}

bool DiskMediaItem::setData(int role, const QVariant &value)
{
    LoggerWrapper::GetInstance()->Write(LV_DEBUG, "DiscMediaItem.setData");
    QObject* p = parent();
    GenericFlatListModel* pModel = static_cast<GenericFlatListModel*>(p);
    QObject* p2 = p->parent();
    qorbiterManager* pManager = static_cast<qorbiterManager*>(p2);
    if (role == StatusRole)
    {
        bool b = value.toBool();
    int handler = m_data[role].toInt();
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "DiscMediaItem.setData model id = %s, handler = %d, Status = %d", pModel->modelName.toStdString().c_str(), handler, b);
    //todo add signal for updating ripping progress.
    //emit pManager->updateAlarm(pModel->modelName, pModel->indexFromItem(this).row(), role, b, handler);
    }
    LoggerWrapper::GetInstance()->Write(LV_DEBUG, "DiscMediaItem.setData end");
    return true;
}

void DiskMediaItem::setDiscData(int deviceId, int fileId, bool ripping, int discNumber, bool status, int discId, QString desc, QString displayLocation)
{
    m_data.insert(DiscRole, deviceId);
    m_data.insert(FileRole, fileId);
    m_data.insert(RippingRole, ripping);
    m_data.insert(NumberRole, discNumber);
    m_data.insert(StatusRole, status);
    m_data.insert(IdRole, discId);
    m_data.insert(DescriptionRole, desc);
    m_data.insert(LocationRole, displayLocation);
}

