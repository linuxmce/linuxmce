#ifndef ABSTRACTWIRELESSBULB_H
#define ABSTRACTWIRELESSBULB_H

#include <QObject>
#include <qcolor.h>
#include <qmap.h>
#include "huecontrollerhardware.h"

class AbstractWirelessBulb : public QObject
{
    Q_OBJECT
public:
    explicit AbstractWirelessBulb(HueControllerHardware* p_controller,  QObject *parent = 0);
    AbstractWirelessBulb() {}
    enum BulbType{
        PHILLIPS_HUE,
        PHILLIPS_HUE_LUX,
        PHILLIPS_HUE_LIGHTSTRIPS,
        PHILLIPS_HUE_BLOOM,
        CREE_WIRELESS_LED
    };

    enum alert {
        NOALERT,BLINK,FLASH
    };

    enum effect {
        NOEFFECT,COLORLOOP
    };

    enum colormode{
        HUESAT,COLORTEMP,XY
    };


    QString displayName() const;
    void setDisplayName(const QString &value);

    QString serialNumber() const;
    void setSerialNumber(const QString &s);

    QColor color() const;
    void setColor(const QColor &color);

    int id();
    void setId( int id);

    double CurrentLevel() const;
    void setCurrentLevel(double CurrentLevel);

    bool powerOn() const;
    void setPowerOn(bool powerOn);

    int bulbType() const;
    void setBulbType(int bulbType);

    QString effect() const;
    void setEffect(const QString &effect);

    QString alert() const;
    void setAlert(const QString &alert);

    bool online() const;
    void setOnline(bool online);

    int linuxmceId() const;
    void setLinuxmceId(int value);

    QString getLightModel() const;
    void setLightModel(const QString &lightModel);

    QString getSoftwareVersion() const;
    void setSoftwareVersion(const QString &softwareVersion);

    QString getManufacturerName() const;
    void setManufacturerName(const QString &manufacturerName);

    QString getUniqueId() const;
    void setUniqueId(const QString &uniqueId);

signals:
    void serialNumberChanged();
    void displayNameChanged();
    void colorChanged();
    void idChanged();
    void powerOnChanged();
    void alertChanged();
    void effectChanged();
    void bulbTypeChanged();
    void onlineChanged();
    void currentLevelChanged();
    void linuxmceIdChanged();
    void lightModelChanged();
    void softwareVersionChanged();
    void manufacturerChanged();
    void uniqueIdChanged();

public slots:
     HueControllerHardware * getController();
     void setController(HueControllerHardware * c);

private:
    QString m_lightModel;
    QString ms_serialNumber;
    QString ms_displayName;
    QString m_softwareVersion;
    QString m_manufacturerName;
    QString m_uniqueId;
    quint8 m_brightness;
    quint16 m_hue;
    quint8 m_saturation;
    QMap<QString, float>xy;
    QColor m_color;
    int m_id;
    int m_linuxmceId;
    double m_CurrentLevel;
    bool m_powerOn;
    int m_bulbType;
    QString m_effect;
    QString m_alert;
    bool m_online;

    HueControllerHardware *mp_controller;

     QMap<QString, int> deviceMap;

};

#endif // ABSTRACTWIRELESSBULB_H
