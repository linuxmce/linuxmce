#include "huemotionsensor.h"
#include "Message.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_Event.h"
#include "QVariantMap"
#include "Gen_Devices/AllCommandsRequests.h"
#include "huecontrollerhardware.h"

HueMotionSensor::HueMotionSensor(HueControllerHardware *p_controller, QObject *parent) :
    QObject(parent), m_controller(p_controller)

{

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
    m_batteryLevel = batteryLevel;
    emit batteryLevelChanged(m_batteryLevel);
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
    m_temp  = ( (temp*.01) * 9.0) / 5.0 + 32;
}

int ZLLTemp::id() const
{
    return m_id;
}

void ZLLTemp::setId(int id)
{
    m_id = id;
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

void ZLLLightLevel::setLightLevel(long lightLevel)
{
    m_lightLevel = lightLevel;
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



