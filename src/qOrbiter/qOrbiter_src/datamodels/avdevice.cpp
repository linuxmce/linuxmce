#include "avdevice.h"

AvDevice::AvDevice(int inc_deviceNo, QString device_name, QString qs_controller, QObject *parent) :
    QObject(parent)
{
    setDeviceNo(inc_deviceNo);
    setDeviceName(device_name);
    setController(qs_controller);
}

void AvDevice::setDeviceNo(int d)
{
    i_deviceNo = d;
    emit deviceSet();
}

int AvDevice::getDeviceNo()
{
    return i_deviceNo;
}

void AvDevice::setController(QString controller)
{
    qs_controlledBy = controller;
    emit controllerSet();
}

QString AvDevice::getController()
{

    return qs_controlledBy;
}

void AvDevice::setDeviceName(QString d)
{
    qs_device = d;
    emit deviceNameSet();
}

QString AvDevice::getDeviceName()
{
    return qs_device;
}
