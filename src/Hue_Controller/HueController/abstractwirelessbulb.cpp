#include "abstractwirelessbulb.h"
#include "qdebug.h"
#include "Message.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_Event.h"

AbstractWirelessBulb::AbstractWirelessBulb(HueControllerHardware *p_controller, QObject *parent) :
    QObject(parent),
    mp_controller(p_controller),
    m_CurrentLevel(0.0),
    m_powerOn(false),
    m_brightness(0),
    m_room(-1)
{

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
    emit dceMessage(EVENT_State_Changed_CONST);
}
bool AbstractWirelessBulb::powerOn() const
{
    return m_powerOn;
}

void AbstractWirelessBulb::setPowerOn(bool powerOn)
{

    bool stateChanged=false;
    if(m_powerOn != powerOn) stateChanged=true;

    m_powerOn = powerOn;
    emit powerOnChanged();

    if(linuxmceId()==0)
        return;

    if(stateChanged){
        emit dceMessage(EVENT_Device_OnOff_CONST);
        qDebug() << QString("%1 Light Changed, sending event").arg(displayName());
    }


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
    Q_UNUSED(d);
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

QVariant AbstractWirelessBulb::getColorMap() const
{
    return m_colorMap;
}

void AbstractWirelessBulb::setColorMap(const QVariant &colorMap)
{
    m_colorMap = colorMap;
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
}

int AbstractWirelessBulb::getBlueLevel() const
{
    return m_blueLevel;
}

int AbstractWirelessBulb::getGreenLevel() const
{
    return m_greenLevel;
}

int AbstractWirelessBulb::getRedLevel() const
{
    return m_redLevel;
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
    bool changed=false;
    if(m_redLevel!=r) {
        m_redLevel=r;
        changed=true;
    }

    if(m_greenLevel!=g){
        m_greenLevel = g;
        changed=true;
    }

    if(m_blueLevel!=b){
        m_blueLevel=b;
        changed=true;
    }

    if(changed){
        emit rgbColorChanged();
    }
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









