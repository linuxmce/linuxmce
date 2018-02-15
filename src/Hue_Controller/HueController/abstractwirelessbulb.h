#ifndef ABSTRACTWIRELESSBULB_H
#define ABSTRACTWIRELESSBULB_H

#include <QObject>
#include <qcolor.h>
#include <qmap.h>
#include "huecontrollerhardware.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include <QVariantMap>
#include <Message.h>

class AbstractWirelessBulb : public QObject
{
    Q_OBJECT
public:
    explicit AbstractWirelessBulb(HueControllerHardware* p_controller,  QObject *parent = 0);
    AbstractWirelessBulb() {}
    enum BulbType{
        PHILLIPS_HUE,
        PHILLIPS_HUE_LUX,
        PHILLIPS_HUE_WHITE,
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

    QString lightModel() const;
    void setLightModel(const QString &lightModel);

    QString softwareVersion() const;
    void setSoftwareVersion(const QString &softwareVersion);

    QString manufacturerName() const;
    void setManufacturerName(const QString &manufacturerName);

    QString uniqueId() const;
    void setUniqueId(const QString &uniqueId);

    void setRgb(int r, int g, int b);

    int getRedLevel() const;

    int getGreenLevel() const;

    int getBlueLevel() const;

    quint8 getBrightness() const;
    void setBrightness(const quint8 &brightness);

    QVariantMap getColorMap() const;
    void setColorMap(const QVariantMap &colorMap);

    QString getLightType() const;
    void setLightType(const QString &lightType);

    int getRoom() const;
    void setRoom(int room);

    long getDeviceTemplate() const;
    void setDeviceTemplate(long deviceTemplate);

    QVariantMap getCurrentColor() const;
    void setCurrentColor(const QVariantMap &value);

    bool getImplementsColor() const;
    void setImplementsColor(bool implementsColor);

    bool getImplementsColorTemp() const;
    void setImplementsColorTemp(bool implementsColorTemp);

    int getCurrentColorTemp() const;
    void setCurrentColorTemp(int value);

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
    void rgbColorChanged();
    void brightnessChanged();
    void lightTypeChanged();
    void roomChanged();
    void dceMessage(int t);
    void dataEvent(DCE::PreformedCommand cmd );

public slots:
     HueControllerHardware * getController();
     void setController(HueControllerHardware * c);

     virtual void proccessStateInformation(QVariantMap d);
     virtual void resetColor() { setColor(QColor("#90C638")); }


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
    int m_room;
    int m_currentColorTemp;
    QColor m_rgbColor;
    QColor m_hslColor;
    long m_deviceTemplate;

    bool m_implementsColor;
    bool m_implementsColorTemp;

    int currentColorTemp;

    QString m_lightType;

    QVariantMap currentColor;
    QVariantMap alertColor;
    QVariantMap defaultColor;


    HueControllerHardware *mp_controller;

    QMap<QString, int> deviceMap;

    double conversionVar;

};

#endif // ABSTRACTWIRELESSBULB_H
