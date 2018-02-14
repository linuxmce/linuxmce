#include "huemotionsensor.h"
#include "Message.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_Event.h"
#include "QVariantMap"
#include "Gen_Devices/AllCommandsRequests.h"
#include "huecontrollerhardware.h"
#include <qdebug.h>

HueMotionSensor::HueMotionSensor(HueControllerHardware *p_controller, QObject *parent) :
    QObject(parent), m_controller(p_controller), m_linuxmceId(0), useCelsius(false)

{
        connect(&tempSensor, SIGNAL(notifyEvent(DCE::Message*)), this, SIGNAL(notifyEvent(DCE::Message*)));
        connect(&lightSensor, SIGNAL(notifyEvent(DCE::Message*)), this, SIGNAL(notifyEvent(DCE::Message*)));
}

QString HueMotionSensor::status() const
{
    return m_status;
}

void HueMotionSensor::setStatus(const QString &status)
{
    m_status = status;
}

QString HueMotionSensor::description() const
{
    return m_description;
}

void HueMotionSensor::setDescription(const QString &description)
{
    m_description = description;
}

QString HueMotionSensor::name() const
{
    return m_name;
}

void HueMotionSensor::setName(const QString &name)
{
    m_name = name;
}

QString HueMotionSensor::type() const
{
    return m_type;
}

void HueMotionSensor::setType(const QString &type)
{
    m_type = type;
}

QString HueMotionSensor::modelId() const
{
    return m_modelId;
}

void HueMotionSensor::setModelId(const QString &modelId)
{
    m_modelId = modelId;
}

QString HueMotionSensor::manufacturer() const
{
    return m_manufacturer;
}

void HueMotionSensor::setManufacturer(const QString &manufacturer)
{
    m_manufacturer = manufacturer;
}

QString HueMotionSensor::productId() const
{
    return m_productId;
}

void HueMotionSensor::setProductId(const QString &productId)
{
    m_productId = productId;
}

QString HueMotionSensor::uniqueId() const
{
    return m_uniqueId;
}

void HueMotionSensor::setUniqueId(const QString &uniqueId)
{
    m_uniqueId = uniqueId;
}

QString HueMotionSensor::swConfigId() const
{
    return m_swConfigId;
}

void HueMotionSensor::setSwConfigId(const QString &swConfigId)
{
    m_swConfigId = swConfigId;
}

bool HueMotionSensor::presenceDetected() const
{
    return m_presenceDetected;
}

void HueMotionSensor::setPresenceDetected(bool presenceDetected)
{
    if(m_presenceDetected == presenceDetected) return;

    m_presenceDetected = presenceDetected;
    emit presenceDetectedChanged(m_presenceDetected);
    DCE::Message * m = new DCE::Message(
                linuxmceId(),
                DEVICEID_EVENTMANAGER,
                DCE::PRIORITY_NORMAL,
                DCE::MESSAGETYPE_EVENT,
                EVENT_Sensor_Tripped_CONST,
                1,
                EVENTPARAMETER_Tripped_CONST,
                m_presenceDetected ? "1" : "0"
                );

    emit notifyEvent(m);
}

bool HueMotionSensor::enabled() const
{
    return m_enabled;
}

void HueMotionSensor::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

int HueMotionSensor::batteryLevel() const
{
    return m_batteryLevel;
}

void HueMotionSensor::setBatteryLevel(int batteryLevel)
{
    if(m_batteryLevel == batteryLevel) return;

    m_batteryLevel = batteryLevel;
    emit batteryLevelChanged(m_batteryLevel);

    DCE::Message *status = new DCE::Message(
                linuxmceId(),
                DEVICEID_EVENTMANAGER,
                DCE::PRIORITY_NORMAL,
                DCE::MESSAGETYPE_EVENT,
                EVENT_Battery_Level_Changed_CONST,
                1 /* number of parameter's pairs (id, value) */,
                EVENTPARAMETER_Value_CONST,
                QString(m_batteryLevel).toStdString().c_str()
                );

    emit notifyEvent(status);


}

QString HueMotionSensor::alert() const
{
    return m_alert;
}

void HueMotionSensor::setAlert(const QString &alert)
{
    m_alert = alert;
}

bool HueMotionSensor::reachable() const
{
    return m_reachable;
}

void HueMotionSensor::setReachable(bool reachable)
{
    m_reachable = reachable;
}

int HueMotionSensor::sensitivity() const
{
    return m_sensitivity;
}

void HueMotionSensor::setSensitivity(int sensitivity)
{
    m_sensitivity = sensitivity;
}

QString HueMotionSensor::lastUpdated() const
{
    return m_lastUpdated;
}

void HueMotionSensor::setLastUpdated(const QString &lastUpdated)
{
    m_lastUpdated = lastUpdated;
}

void HueMotionSensor::setTempSensor(int id, QVariantMap obj)
{
    tempSensor.setId(id);
    tempSensor.setTemp(obj.value("state").toMap().value("temperature").toInt());
}

void HueMotionSensor::setLightSensor(int id, QVariantMap obj)
{
    lightSensor.setId(id);
    lightSensor.setDark(obj.value("state").toMap().value("dark").toBool());
    lightSensor.setDaylight(obj.value("state").toMap().value("daylight").toBool());
    lightSensor.setLightLevel(obj.value("state").toMap().value("lightlevel").toInt());
    lightSensor.setDarkThreshold(obj.value("config").toMap().value("tholddark").toInt());
    lightSensor.setThresholdOffset(obj.value("config").toMap().value("tholdoffset").toInt());
}

void HueMotionSensor::setPresenceData(QVariantMap data)
{
    setPresenceDetected(data.value("state").toMap().value("presence").toBool());
    setBatteryLevel(data.value("config").toMap().value("battery").toInt());
}

void HueMotionSensor::setTempData(QVariantMap data)
{

    int t = data.value("state").toMap().value("temperature").toInt()*.01;

    if(!useCelsius) {
        t = (t *9.0) / 5.0+32;
    }

    tempsensor()->setTemp(t );
}

void HueMotionSensor::setLightLevelData(QVariantMap data)
{
    lightSensor.setLightLevel(data["state"].toMap().value("lightlevel").toInt());
    lightSensor.setDark(data["state"].toMap().value("dark").toBool());
    lightSensor.setDaylight(data["state"].toMap().value("daylight").toBool());

}

bool HueMotionSensor::getUseCelsius() const
{
    return useCelsius;
}

void HueMotionSensor::setUseCelsius(bool value)
{
    useCelsius = value;
}

void HueMotionSensor::initValues()
{

    DCE::Message * m = new DCE::Message(
                tempsensor()->linuxmceId(),
                DEVICEID_EVENTMANAGER,
                DCE::PRIORITY_NORMAL,
                DCE::MESSAGETYPE_EVENT,
                EVENT_Temperature_Changed_CONST,
                1,
                EVENTPARAMETER_Value_CONST,
                StringUtils::itos(tempSensor.temp()).c_str() );

    emit notifyEvent(m);

    DCE::Message *status = new DCE::Message(
                lightsensor()->linuxmceId(),
                DEVICEID_EVENTMANAGER,
                DCE::PRIORITY_NORMAL,
                DCE::MESSAGETYPE_EVENT,
                EVENT_Brightness_Changed_CONST,
                1 /* number of parameter's pairs (id, value) */,
                EVENTPARAMETER_Value_CONST,
                StringUtils::itos(lightsensor()->lightLevel()).c_str()
                );

    emit notifyEvent(status);

    status = new DCE::Message(
                linuxmceId(),
                DEVICEID_EVENTMANAGER,
                DCE::PRIORITY_NORMAL,
                DCE::MESSAGETYPE_EVENT,
                EVENT_Battery_Level_Changed_CONST,
                1 /* number of parameter's pairs (id, value) */,
                EVENTPARAMETER_Value_CONST,
                QString(m_batteryLevel).toStdString().c_str()
                );

    emit notifyEvent(status);

}

HueControllerHardware *HueMotionSensor::controller() const
{
    return m_controller;
}

void HueMotionSensor::setController(HueControllerHardware *controller)
{
    m_controller = controller;
}

int HueMotionSensor::hueId() const
{
    return m_hueId;
}

void HueMotionSensor::setHueId(int hueId)
{
    m_hueId = hueId;
}

int HueMotionSensor::linuxmceId() const
{
    return m_linuxmceId;
}

void HueMotionSensor::setLinuxmceId(int linuxmceId)
{
    m_linuxmceId = linuxmceId;
}


int ZLLTemp::temp() const
{
    return m_temp;
}

void ZLLTemp::setTemp(int temp)
{
    if(m_temp == temp) return;
    m_temp  = temp;


    DCE::Message * m = new DCE::Message(
                linuxmceId(),
                DEVICEID_EVENTMANAGER,
                DCE::PRIORITY_NORMAL,
                DCE::MESSAGETYPE_EVENT,
                EVENT_Temperature_Changed_CONST,
                1,
                EVENTPARAMETER_Value_CONST,
                StringUtils::itos(m_temp).c_str() );
 if(linuxmceId() != 0)
    emit notifyEvent(m);

}

int ZLLTemp::id() const
{
    return m_id;
}

void ZLLTemp::setId(int id)
{
    m_id = id;
}

int ZLLTemp::linuxmceId() const
{
    return m_linuxmceId;
}

void ZLLTemp::setLinuxmceId(int linuxmceId)
{
    m_linuxmceId = linuxmceId;
}

int ZLLLightLevel::id() const
{
    return m_id;
}

void ZLLLightLevel::setId(int id)
{
    m_id = id;
}

int ZLLLightLevel::darkThreshold() const
{
    return m_darkThreshold;
}

void ZLLLightLevel::setDarkThreshold(long darkThreshold)
{
    m_darkThreshold = darkThreshold;
}

int ZLLLightLevel::thresholdOffset() const
{
    return m_thresholdOffset;
}

void ZLLLightLevel::setThresholdOffset(long thresholdOffset)
{
    m_thresholdOffset = thresholdOffset;
}

int ZLLLightLevel::lightLevel() const
{
    return m_lightLevel;
}

void ZLLLightLevel::setLightLevel(int lightLevel)
{
    if(m_lightLevel== lightLevel) return;
    m_lightLevel = lightLevel;
    qDebug() << Q_FUNC_INFO;
    DCE::Message *status = new DCE::Message(
                linuxmceId(),
                DEVICEID_EVENTMANAGER,
                DCE::PRIORITY_NORMAL,
                DCE::MESSAGETYPE_EVENT,
                EVENT_Brightness_Changed_CONST,
                1 /* number of parameter's pairs (id, value) */,
                EVENTPARAMETER_Value_CONST,
                StringUtils::itos(m_lightLevel).c_str()
                );

    if(linuxmceId() != 0)
    emit notifyEvent(status);

}

bool ZLLLightLevel::dark() const
{
    return m_dark;
}

void ZLLLightLevel::setDark(bool dark)
{
    m_dark = dark;
}

bool ZLLLightLevel::daylight() const
{
    return m_daylight;
}

void ZLLLightLevel::setDaylight(bool daylight)
{
    m_daylight = daylight;
}

int ZLLLightLevel::linuxmceId() const
{
    return m_linuxmceId;
}

void ZLLLightLevel::setLinuxmceId(int linuxmceId)
{
    m_linuxmceId = linuxmceId;
    qDebug() << Q_FUNC_INFO << linuxmceId;
}



