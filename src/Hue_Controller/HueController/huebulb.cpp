#include "huebulb.h"
#include <QDebug>
#include <QVariant>
HueBulb::HueBulb(HueControllerHardware *hw)
{
    setController(hw);
}

void HueBulb::proccessStateInformation(QVariantMap d)
{
    qDebug() << Q_FUNC_INFO << QString("Device %d").arg(linuxmceId());
    qDebug() << d;
}


