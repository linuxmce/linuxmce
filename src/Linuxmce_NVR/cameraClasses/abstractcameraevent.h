#ifndef ABSTRACTCAMERAEVENT_H
#define ABSTRACTCAMERAEVENT_H

#include <QObject>
#include <QDateTime>

class CameraEvent : public QObject
{
    Q_OBJECT
public:
    enum SensorState{
        MOTION_DETECTED,
        MOTION_STARTED,
        MOTION_ENDED
    };
    Q_ENUMS(SensorState)

    enum NotificationStatus{
        NOTIFIED_EMAIL,
        NOTIFIED_SMS,
        NOTIFIED_DRY_CONTACT,
        NOTIFY_SOCIAL
    };
    Q_ENUMS(NotificationStatus)

     CameraEvent(QString host, QString name, QString url, QString status, CameraEvent::SensorState stat, int deviceId,  QObject *parent = 0);

    QString host() const;
    void setHost(const QString &host);

    QString name() const;
    void setName(const QString &name);

    QString url() const;
    void setUrl(const QString &url);

    QString status() const;
    void setStatus(const QString &status);

    NotificationStatus notificationStatus() const;
    void setNotificationStatus(const NotificationStatus &notificationStatus);

    SensorState sensorState() const;
    void setSensorState(const SensorState &sensorState);

    QDateTime eventTime() const;
    void setEventTime(const QDateTime &eventTime);

    int deviceId() const;
    void setDeviceId(int deviceId);

signals:


public slots:

private:
    QString m_host;
    QString m_name;
    QString m_url;
    QString m_status;
    QDateTime m_eventTime;
    int m_deviceId;

    NotificationStatus m_notificationStatus;
    SensorState m_sensorState;
};



#endif // ABSTRACTCAMERAEVENT_H
