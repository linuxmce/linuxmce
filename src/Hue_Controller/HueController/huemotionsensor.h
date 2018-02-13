#ifndef HUEMOTIONSENSOR_H
#define HUEMOTIONSENSOR_H

#include <QObject>
#include <QVariantMap>

class HueControllerHardware;

namespace DCE {
class Message;
}

class ZLLTemp: public QObject{
    Q_OBJECT
public:
    ZLLTemp(){}
    ~ZLLTemp(){}

    int temp() const;
    void setTemp(int temp);

    int id() const;
    void setId(int id);

private:
    int m_temp;
    int m_id;
};


class ZLLLightLevel: public QObject{
    Q_OBJECT

public:
    ZLLLightLevel(){}
    ~ZLLLightLevel(){}

    int id() const;
    void setId(int id);

    int darkThreshold() const;
    void setDarkThreshold(long darkThreshold);

    int thresholdOffset() const;
    void setThresholdOffset(long thresholdOffset);

    int lightLevel() const;
    void setLightLevel(long lightLevel);

    bool dark() const;
    void setDark(bool dark);

    bool daylight() const;
    void setDaylight(bool daylight);

private:
    int m_id;
    int m_darkThreshold;
    int m_thresholdOffset;
    int m_lightLevel;
    bool m_dark;
    bool m_daylight;
};


class HueMotionSensor : public QObject
{
    Q_OBJECT
public:
    explicit HueMotionSensor(HueControllerHardware *p_controller, QObject * parent=0);

    int hueId() const;
    void setHueId(int hueId);

    HueControllerHardware *controller() const;
    void setController(HueControllerHardware *controller);

signals:
    void batteryLevelChanged(int batteryLevel);
    void enabledChanged(bool enabled);
    void presenceDetectedChanged(bool presenceDetected);
    void notifyEvent(DCE::Message * m);


public slots:

    int linuxmceId() const;
    void setLinuxmceId(int linuxmceId);

    QString status() const;
    void setStatus(const QString &status);

    QString description() const;
    void setDescription(const QString &description);

    QString name() const;
    void setName(const QString &name);

    QString type() const;
    void setType(const QString &type);

    QString modelId() const;
    void setModelId(const QString &modelId);

    QString manufacturer() const;
    void setManufacturer(const QString &manufacturer);

    QString productId() const;
    void setProductId(const QString &productId);

    QString uniqueId() const;
    void setUniqueId(const QString &uniqueId);

    QString swConfigId() const;
    void setSwConfigId(const QString &swConfigId);

    bool presenceDetected() const;
    void setPresenceDetected(bool presenceDetected);

    bool enabled() const;
    void setEnabled(bool enabled);

    int batteryLevel() const;
    void setBatteryLevel(int batteryLevel);

    QString alert() const;
    void setAlert(const QString &alert);

    bool reachable() const;
    void setReachable(bool reachable);

    int sensitivity() const;
    void setSensitivity(int sensitivity);

    QString lastUpdated() const;
    void setLastUpdated(const QString &lastUpdated);

    void setTempSensor(int id, QVariantMap obj);
    void setLightSensor(int id, QVariantMap obj);

protected:

    QString m_status;
    QString m_description;
    QString m_name;
    QString m_type;
    QString m_modelId;
    QString m_manufacturer;
    QString m_productId;
    QString m_uniqueId;
    QString m_swConfigId;
    bool m_presenceDetected;
    bool m_enabled;
    int m_batteryLevel;
    QString m_alert;
    bool m_reachable;
    int m_sensitivity;
    int m_sensitivityMax;
    QString m_lastUpdated;
    int m_linuxmceId;
    int m_hueId;

    ZLLLightLevel lightSensor;
    ZLLTemp       tempSensor;
    HueControllerHardware * m_controller;


};

#endif // HUEMOTIONSENSOR_H
