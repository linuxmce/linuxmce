#include "existingorbiter.h"
#include "qdebug.h"

ExistingOrbiter::ExistingOrbiter(int deviceno, QString title, QObject *parent) :
    QObject(parent), i_device_number(deviceno), label("")
{
 QStringList metaData;
 metaData = title.split(QRegExp("\\d\\d\\d "));


 if(metaData.length()==1){
     label=metaData.at(0).split("(").at(0);
     location =metaData.at(0).split("(").at(1);
     location.remove(")");
 } else {
     label=metaData.at(1).split("(").at(0);
     location =metaData.at(1).split("(").at(1);
     location.remove(")");
 }

qDebug() << Q_FUNC_INFO << metaData;
}

QVariant ExistingOrbiter::data(int role) const
{
    switch(role) {
    case LabelRole:
      return id();
    case LocationRole:
      return locationData();
    case DeviceRole:
      return deviceData();
    default:
      return QVariant();
    }
}

QHash<int, QByteArray> ExistingOrbiter::roleNames() const
{
    QHash<int, QByteArray> names;
    names[LabelRole] = "label";
    names[LocationRole] = "location";
    names[DeviceRole] = "device";
    return names;
}



void ExistingOrbiter::set_title(QString t)
{
    label = t;
    emit titleChanged();
}

QString ExistingOrbiter::get_title()
{
    return label;
}

void ExistingOrbiter::set_device_number(int dev)
{
    i_device_number = dev;
    emit deviceChanged();
}

int ExistingOrbiter::get_device_number()
{
    return i_device_number;
}

void ExistingOrbiter::set_label(QString lab)
{
    location = lab;
    emit locationChanged();
}

QString ExistingOrbiter::get_label()
{
    return location;
}
