#include "huebulb.h"
#include <QDebug>
#include <QVariant>

HueBulb::HueBulb(HueControllerHardware *hw)
{
    setController(hw);
}

void HueBulb::proccessStateInformation(QVariantMap d)
{
   // qDebug() << Q_FUNC_INFO << QString("Device %1").arg(linuxmceId());
   // qDebug() << d["name"];
    QVariantMap stateInfo = d["state"].toMap();
    setBrightness(stateInfo["bri"].toDouble());
    setCurrentLevel(stateInfo["bri"].toUInt()/2.55);
    setOnline(stateInfo["reachable"].toBool());
    setPowerOn(stateInfo["on"].toBool());
    setAlert(stateInfo["alert"].toString());
    setAlert(stateInfo["effect"].toString());
    setSoftwareVersion(d["swversion"].toString());
    setLightModel(d["modelid"].toString());
    setLightType(d["type"].toString());
   // qDebug() << "\n\r";
}


