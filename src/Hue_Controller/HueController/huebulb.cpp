#include "huebulb.h"

HueBulb::HueBulb(QObject *parent) :
    QObject(parent)
{
    powerState="";
    int lightId =0 ;
    int linuxmceId =0;
    QString powerState = "";
    quint8 bri = 0;
    quint16 hue =0;
    quint8 sat = 0;
    QMap<QString*, float> xy;
    quint16 colorTemp = 0;
    QString alertStatus = "";
    QString effectStatus ="";
    QString currentColorMode = "";
    bool reachable = true;

    QString lightType = "";
    QString lightName = "";
    QString modelId = "";
    QString swVersion = "";
    QString controller= "";
}
