#include "abstractcameraevent.h"

CameraEvent::CameraEvent(QString host, QString name, QString url, QString status, CameraEvent::SensorState stat, int deviceId, QObject *parent) :
    QObject(parent),
    m_host(host), m_name(name), m_url(url), m_status(status), m_sensorState(stat), m_deviceId(deviceId)
{
    setEventTime(QDateTime::currentDateTime().toLocalTime());
    if(m_status=="detected"){
        setSensorState(CameraEvent::MOTION_DETECTED);
    }
}
QString CameraEvent::host() const
{
    return m_host;
}

void CameraEvent::setHost(const QString &host)
{
    m_host = host;
}
QString CameraEvent::name() const
{
    return m_name;
}

void CameraEvent::setName(const QString &name)
{
    m_name = name;
}
QString CameraEvent::url() const
{
    return m_url;
}

void CameraEvent::setUrl(const QString &url)
{
    m_url = url;
}
QString CameraEvent::status() const
{
    return m_status;
}

void CameraEvent::setStatus(const QString &status)
{
    m_status = status;
}
CameraEvent::NotificationStatus CameraEvent::notificationStatus() const
{
    return m_notificationStatus;
}

void CameraEvent::setNotificationStatus(const NotificationStatus &notificationStatus)
{
    m_notificationStatus = notificationStatus;
}

CameraEvent::SensorState CameraEvent::sensorState() const
{
    return m_sensorState;
}

void CameraEvent::setSensorState(const SensorState &sensorState)
{
    m_sensorState = sensorState;
}
QDateTime CameraEvent::eventTime() const
{
    return m_eventTime;
}

void CameraEvent::setEventTime(const QDateTime &eventTime)
{
    m_eventTime = eventTime;
}
int CameraEvent::deviceId() const
{
    return m_deviceId;
}

void CameraEvent::setDeviceId(int deviceId)
{
    m_deviceId = deviceId;
}









