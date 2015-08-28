#include "avdevice.h"

AvDevice::AvDevice(int inc_deviceNo, QString device_name, QString qs_controller, QString loc, int type, bool state, QObject *parent) :
    QObject(parent) , i_deviceNo(inc_deviceNo), qs_device(device_name),  qs_controlledBy(qs_controller), location(loc), deviceType(type), active(state)
{

}

QHash<int, QByteArray> AvDevice::roleNames() const
{

  QHash<int, QByteArray> names;
  names[NameRole] = "name";
  names[ControllerRole] = "controller";
  names[DeviceNumberRole] = "devicenumber";
  names[ActiveRole] = "status";
  names[TypeRole] = "type";
  names[LocationRole]= "location";
  return names;
}

QVariant AvDevice::data(int role) const
{
  switch(role) {
  case NameRole:
    return id();
  case ControllerRole:
    return controlled();
  case DeviceNumberRole:
    return deviceNumber();
  case ActiveRole:
    return activity();
  case LocationRole:
    return located();
  case TypeRole:
  return device_Type();
  default:
    return QVariant();
  }
}
