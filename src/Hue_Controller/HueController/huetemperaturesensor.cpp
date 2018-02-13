#include "huetemperaturesensor.h"

HueTemperatureSensor::HueTemperatureSensor(QObject *parent) : QObject(parent)
{

}

int HueTemperatureSensor::getMi_temp() const
{
    return mi_temp;
}

void HueTemperatureSensor::setMi_temp(int value)
{
    mi_temp = value;
}
