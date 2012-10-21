#include "existingorbiter.h"

ExistingOrbiter::ExistingOrbiter(int deviceno, QString title, QObject *parent) :
    QObject(parent)
{

    set_device_number(deviceno);
    set_title(title);
    set_label(title);

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
