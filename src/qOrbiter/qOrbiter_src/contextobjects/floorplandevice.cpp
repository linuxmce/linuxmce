#include "floorplandevice.h"

FloorplanDevice::FloorplanDevice(QString &name, int &deviceNo, int &floorplan_device_type, int i_flooplanType, QString &position, QImage &icon,  QObject *parent) :
    mQS_name(name), mI_deviceNo(deviceNo), mI_floorplan_device_type(floorplan_device_type), mI_floorplanType(i_flooplanType), mQS_position(position), mIM_icon(icon)

{
}

QHash<int, QByteArray> FloorplanDevice::roleNames() const
{

  QHash<int, QByteArray> names;
  names[NameRole] = "name";
  names[DeviceNoRole] = "deviceno";
  names[FloorPlanTypeRole] = "floorplantype";
  names[DeviceTypeRole] = "type";
  names[PositionRole] = "position";
  names[IconImageRole]= "iconimage";
  return names;
}

QVariant FloorplanDevice::data(int role) const
{

  switch(role) {
  case NameRole:
    return id();
  case DeviceNoRole:
    return deviceNum();
  case DeviceTypeRole:
    return deviceType();
  case PositionRole:
    return pagePosition();
  case IconImageRole:
    return deviceImage();
  case FloorPlanTypeRole:
      return floorplanType();
  default:
    return QVariant();
  }
}
