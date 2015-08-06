#include "hueluxbulb.h"
#include <QDebug>
#include <QVariant>
HueLuxBulb::HueLuxBulb(HueControllerHardware *hw)
{
    setController(hw);
    setBrightness(0);
}

void HueLuxBulb::proccessStateInformation(QVariantMap d)
{
    QVariantMap stateInfo = d["state"].toMap();
    setDisplayName(d["name"].toString());
    setBrightness(stateInfo["bri"].toDouble());
    setCurrentLevel(stateInfo["bri"].toUInt()/2.55);
    setOnline(stateInfo["reachable"].toBool());
    setPowerOn(stateInfo["on"].toBool());
    setAlert(stateInfo["alert"].toString());
    setEffect(stateInfo["effect"].toString());
    setSoftwareVersion(d["swversion"].toString());
    setLightModel(d["modelid"].toString());
    setLightType(d["type"].toString());
}
