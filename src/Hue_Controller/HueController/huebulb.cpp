#include "huebulb.h"
#include <QDebug>
#include <QVariant>

HueBulb::HueBulb(HueControllerHardware *hw): resetColor("#90C638")
{
    setController(hw);


}

void HueBulb::proccessStateInformation(QVariantMap d)
{
  // qDebug() << Q_FUNC_INFO << QString("Device %1").arg(linuxmceId());
  // qDebug() << d["name"];
    QVariantMap stateInfo = d["state"].toMap();    
    setDisplayName(d["name"].toString());
    setOnline(stateInfo["reachable"].toBool());
    setPowerOn(stateInfo["on"].toBool());
    setSoftwareVersion(d["swversion"].toString());
    setLightModel(d["modelid"].toString());
    setLightType(d["type"].toString());

    if(powerOn()){
        setBrightness(stateInfo["bri"].toDouble());
       setCurrentLevel(stateInfo["bri"].toUInt()/2.55);
       setAlert(stateInfo["alert"].toString());
       setEffect(stateInfo["effect"].toString());
    }
   // qDebug() << "\n\r";
}


