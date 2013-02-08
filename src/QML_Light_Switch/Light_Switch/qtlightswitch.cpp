#include "qtlightswitch.h"
#include <QDebug>

QTlightSwitch::QTlightSwitch(QObject *parent) :
    QObject(parent)
{
}

void QTlightSwitch::dce_on()
    {
    qDebug() << "Emitting On signal";


}

void QTlightSwitch::dce_off()
    {
    qDebug() << "Emitting On signal";

}

void QTlightSwitch::dce_setLevel(int)
{

}

bool QTlightSwitch::update_deviceStatus(QString)
{
return true;
}

