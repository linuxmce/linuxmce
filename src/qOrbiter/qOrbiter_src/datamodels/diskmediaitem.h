#ifndef DISKMEDIAITEM_H
#define DISKMEDIAITEM_H

#include <QObject>
#include <QVariant>
#include <QHash>
#ifdef debug
#include <QDebug>

#endif
#include "genericmodelitem.h"

class DiskMediaItem : public GenericModelItem
{
    Q_OBJECT

    enum Roles {
        DiscRole = Qt::UserRole+1,
        FileRole= Qt::UserRole+2,
        RippingRole= Qt::UserRole+3,
        NumberRole= Qt::UserRole+4,
        StatusRole= Qt::UserRole+5,
        IdRole= Qt::UserRole+6,
        DescriptionRole = Qt::UserRole+7,
        DiscInfoRole = Qt::UserRole+8,
        LocationRole = Qt::UserRole+9

    };

public:
    explicit DiskMediaItem(QObject *parent = 0);
    virtual bool setData(int role, const QVariant &value);
    void setDiscData(int deviceId, int fileId, bool ripping, int discNumber, bool status, int discId, QString desc, QString displayLocation);

signals:
    void dataChanged();

public slots:
};

#endif // DISKMEDIAITEM_H
