#include "abstractwirelessbulb.h"
#include "qdebug.h"
#include "Message.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_Event.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "QJsonDocument"
#include "QJsonObject"
#include <math.h>

AbstractWirelessBulb::AbstractWirelessBulb(HueControllerHardware *p_controller, QObject *parent) :
    QObject(parent),
    mp_controller(p_controller),
    m_CurrentLevel(0.0),
    m_powerOn(false),
    m_brightness(0),
    m_room(-1),
    m_implementsColor(false),
    m_implementsColorTemp(false)
{
   conversionVar = ceil(65280 / 360);
    m_rgbColor.setRgb(0,0,0);
    m_hslColor = m_rgbColor.toHsl();

    if(mp_controller){
        //qDebug() << Q_FUNC_INFO << "contoller IP::" << mp_controller->getIpAddress();
    } else {
        //qDebug() << Q_FUNC_INFO << "invalid controller!";
    }

}

QString AbstractWirelessBulb::displayName() const
{
    return ms_displayName;
}

void AbstractWirelessBulb::setDisplayName(const QString &value)
{
    if(ms_displayName==value) return;
    ms_displayName= value;
    emit displayNameChanged();
}

QString AbstractWirelessBulb::serialNumber() const
{
    return ms_serialNumber;
}

void AbstractWirelessBulb::setSerialNumber(const QString &s)
{
    if(ms_serialNumber== s)return;

    ms_serialNumber=s;
    emit serialNumberChanged();

}
QColor AbstractWirelessBulb::color() const
{
    return m_color;
}

void AbstractWirelessBulb::setColor(const QColor &color)
{
    if(m_color==color) return;
    m_color = color;
    emit colorChanged();
}
int AbstractWirelessBulb::id()
{
    return m_id;
}

void AbstractWirelessBulb::setId( int id)
{
    if(m_id==id) return;

    m_id = id;
    emit idChanged();
}
double AbstractWirelessBulb::CurrentLevel() const
{
    return m_CurrentLevel;
}

void AbstractWirelessBulb::setCurrentLevel(double CurrentLevel)
{

   if(m_CurrentLevel == CurrentLevel) return;

    m_CurrentLevel = CurrentLevel;
    emit currentLevelChanged();
    qDebug() << Q_FUNC_INFO;
    emit dceMessage(EVENT_State_Changed_CONST);
}
bool AbstractWirelessBulb::powerOn() const
{
    return m_powerOn;
}

void AbstractWirelessBulb::setPowerOn(bool powerOn)
{
    if(m_powerOn == powerOn) return;

    m_powerOn = powerOn;
    emit powerOnChanged();

    if(linuxmceId()==0)
        return;    
    qDebug() << Q_FUNC_INFO;
        emit dceMessage(EVENT_Device_OnOff_CONST);
}
int AbstractWirelessBulb::bulbType() const
{
    return m_bulbType;
}

void AbstractWirelessBulb::setBulbType(int bulbType)
{
    if(m_bulbType==bulbType) return;

    m_bulbType = bulbType;
    emit bulbTypeChanged();
}
QString AbstractWirelessBulb::effect() const
{
    return m_effect;
}

void AbstractWirelessBulb::setEffect(const QString &effect)
{
    if(m_effect==effect);return;

    m_effect = effect;
    emit effectChanged();
}
QString AbstractWirelessBulb::alert() const
{
    return m_alert;
}

void AbstractWirelessBulb::setAlert(const QString &alert)
{
    if(m_alert==alert) return;

    m_alert = alert;
    emit alertChanged();
}
bool AbstractWirelessBulb::online() const
{
    return m_online;
}

void AbstractWirelessBulb::setOnline(bool online)
{
    if(m_online==online)return;

    m_online = online;
    emit onlineChanged();
}

HueControllerHardware *AbstractWirelessBulb::getController()
{
    if(mp_controller)
        return mp_controller;
    else
        return new HueControllerHardware();
}

void AbstractWirelessBulb::setController(HueControllerHardware *c)
{
    mp_controller=c;
}

void AbstractWirelessBulb::proccessStateInformation(QVariantMap d)
{

    QVariantMap stateInfo = d["state"].toMap();

    setDisplayName(d["name"].toString());
    setOnline(stateInfo["reachable"].toBool());
    setPowerOn(stateInfo["on"].toBool());
    setSoftwareVersion(d["swversion"].toString());
    setLightModel(d["modelid"].toString());
    setLightType(d["type"].toString());
    setAlert(stateInfo["alert"].toString());
    setEffect(stateInfo["effect"].toString());
    setCurrentLevel(stateInfo["bri"].toUInt()/2.55);

    switch (m_deviceTemplate) {
    case DEVICETEMPLATE_Hue_Lightbulb_CONST :
        setCurrentColor(stateInfo);
        break;
    case DEVICETEMPLATE_Hue_Light_Strips_CONST :
        setCurrentColor(stateInfo);

        break;
    case DEVICETEMPLATE_Hue_Lux_Bulb_CONST :
        setCurrentColor(stateInfo);

        break;
    case DEVICETEMPLATE_Connected_A19_60w_Equivalent_CONST :
        setBrightness(stateInfo["bri"].toDouble());
        break;
    default:
        break;
    }
}

int AbstractWirelessBulb::getCurrentColorTemp() const
{
    return currentColorTemp;
}

void AbstractWirelessBulb::setCurrentColorTemp(int value)
{
    currentColorTemp = value;
}

bool AbstractWirelessBulb::getImplementsColorTemp() const
{
    return m_implementsColorTemp;
}

void AbstractWirelessBulb::setImplementsColorTemp(bool implementsColorTemp)
{
    m_implementsColorTemp = implementsColorTemp;
}

bool AbstractWirelessBulb::getImplementsColor() const
{
    return m_implementsColor;
}

void AbstractWirelessBulb::setImplementsColor(bool implementsColor)
{
    m_implementsColor = implementsColor;
}

QVariantMap AbstractWirelessBulb::getCurrentColor() const
{
    return currentColor;
}

void AbstractWirelessBulb::setCurrentColor(const QVariantMap &value)
{

    m_hslColor = QColor::fromHsl( int(value["hue"].toInt() / conversionVar), value["sat"].toInt(), value["bri"].toInt() );
    m_rgbColor = m_hslColor.toRgb();
    currentColorTemp = value["ct"].toInt();
    QVariantMap out;
    out.insert("red", QVariant::fromValue(m_rgbColor.red()));
    out.insert("blue", m_rgbColor.blue());
    out.insert("green", m_rgbColor.blue());    

}

long AbstractWirelessBulb::getDeviceTemplate() const
{
    return m_deviceTemplate;
}

void AbstractWirelessBulb::setDeviceTemplate(long deviceTemplate)
{
    m_deviceTemplate = deviceTemplate;
    switch (m_deviceTemplate) {
    case DEVICETEMPLATE_Hue_Lightbulb_CONST :
    case DEVICETEMPLATE_Hue_Light_Strips_CONST :
        setImplementsColor(true);
        setImplementsColorTemp(true);
        break;
    case DEVICETEMPLATE_Hue_Lux_Bulb_CONST :
       setImplementsColorTemp(true);
        break;
    default:
        break;
    }
}
int AbstractWirelessBulb::getRoom() const
{
    return m_room;
}

void AbstractWirelessBulb::setRoom(int room)
{
    m_room = room;
    emit roomChanged();
}


QString AbstractWirelessBulb::getLightType() const
{
    return m_lightType;
}

void AbstractWirelessBulb::setLightType(const QString &lightType)
{
    if(m_lightType==lightType) return;

    m_lightType = lightType;
    emit lightTypeChanged();
}

QVariantMap AbstractWirelessBulb::getColorMap() const
{
    QVariantMap ret;
    ret.insert("defaultColor", defaultColor);
    ret.insert("alertColor", alertColor);
    ret.insert("currentColor", currentColor);
    return ret;

}

void AbstractWirelessBulb::setColorMap(const QVariantMap &colorMap)
{
    defaultColor = colorMap.value("defaultColor").toMap();
    alertColor = colorMap.value("alertColor").toMap();
}

quint8 AbstractWirelessBulb::getBrightness() const
{
    return m_brightness;
}

void AbstractWirelessBulb::setBrightness(const quint8 &brightness)
{

    if(m_brightness==brightness)
        return;

    m_brightness = brightness;
    emit brightnessChanged();

    if(linuxmceId()==0); return;

    emit dceMessage(EVENT_Brightness_Changed_CONST);
    emit dceMessage(EVENT_State_Changed_CONST);
    qDebug() << Q_FUNC_INFO;
}

QString AbstractWirelessBulb::uniqueId() const
{
    return m_uniqueId;
}

void AbstractWirelessBulb::setUniqueId(const QString &uniqueId)
{
    m_uniqueId = uniqueId;
    emit uniqueIdChanged();
}

void AbstractWirelessBulb::setRgb(int r, int g, int b)
{
    m_rgbColor = QColor::fromRgb(r,g,b);
        emit rgbColorChanged();     

}

QString AbstractWirelessBulb::manufacturerName() const
{
    return m_manufacturerName;
}

void AbstractWirelessBulb::setManufacturerName(const QString &manufacturerName)
{
    m_manufacturerName = manufacturerName;
    emit manufacturerChanged();
}

QString AbstractWirelessBulb::softwareVersion() const
{
    return m_softwareVersion;
}

void AbstractWirelessBulb::setSoftwareVersion(const QString &softwareVersion)
{
    m_softwareVersion = softwareVersion;
    emit softwareVersionChanged();
}

QString AbstractWirelessBulb::lightModel() const
{
    return m_lightModel;
}

void AbstractWirelessBulb::setLightModel(const QString &lightModel)
{
    m_lightModel = lightModel;
    emit lightModelChanged();
}

int AbstractWirelessBulb::linuxmceId() const
{
    return m_linuxmceId;
}

void AbstractWirelessBulb::setLinuxmceId(int value)
{
    m_linuxmceId = value;
    emit linuxmceIdChanged();
}









