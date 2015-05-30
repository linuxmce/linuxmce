#include "abstractwirelessbulb.h"
#include "qdebug.h"

AbstractWirelessBulb::AbstractWirelessBulb(HueControllerHardware *p_controller, QObject *parent) : QObject(parent), mp_controller(p_controller)
{
    qDebug() << Q_FUNC_INFO << "ctor";
    if(mp_controller){
        qDebug() << Q_FUNC_INFO << mp_controller->getIpAddress();
    } else {
        qDebug() << Q_FUNC_INFO << "invalid controller!";
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
    return mp_controller;
}

void AbstractWirelessBulb::setController(HueControllerHardware *c)
{
    mp_controller=c;
}
QString AbstractWirelessBulb::getUniqueId() const
{
    return m_uniqueId;
}

void AbstractWirelessBulb::setUniqueId(const QString &uniqueId)
{
    m_uniqueId = uniqueId;
    emit uniqueIdChanged();
}

QString AbstractWirelessBulb::getManufacturerName() const
{
    return m_manufacturerName;
}

void AbstractWirelessBulb::setManufacturerName(const QString &manufacturerName)
{
    m_manufacturerName = manufacturerName;
    emit manufacturerChanged();
}

QString AbstractWirelessBulb::getSoftwareVersion() const
{
    return m_softwareVersion;
}

void AbstractWirelessBulb::setSoftwareVersion(const QString &softwareVersion)
{
    m_softwareVersion = softwareVersion;
    emit softwareVersionChanged();
}

QString AbstractWirelessBulb::getLightModel() const
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









