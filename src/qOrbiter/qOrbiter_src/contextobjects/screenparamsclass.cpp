#include "screenparamsclass.h"

ScreenParamsClass::ScreenParamsClass(QObject *parent) :
    QObject(parent)
{
}

void ScreenParamsClass::clear()
{
    paramDevices.clear();
}
