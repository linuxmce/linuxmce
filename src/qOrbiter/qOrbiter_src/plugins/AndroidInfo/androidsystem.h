#ifndef ANDROIDSYSTEM_H
#define ANDROIDSYSTEM_H

#include <QObject>
#include <jni.h>
#include <QPlatformNativeInterface>


class AndroidSystem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int apiLevel READ getApiLevel NOTIFY apiLevelChanged)
    Q_PROPERTY(QString statusMessage READ getStatusMessage  NOTIFY statusMessageChanged)
    Q_PROPERTY(QString codename READ getCodeName NOTIFY codenameChanged)
    Q_PROPERTY(QString deviceName READ getDeviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(QString deviceManufacturer READ getDeviceManufacturer NOTIFY deviceManufacturerChanged)
    Q_PROPERTY(QString deviceBrand READ getDeviceBrand NOTIFY deviceBrandChanged )
public:
    explicit AndroidSystem(QObject *parent = 0);
    int apiLevel;
    QString statusMessage;
    QString codename;
    QString deviceName;
    QString deviceManufacturer;
    QString deviceBrand;

signals:
    void apiLevelChanged();
    void statusMessageChanged();
    void codenameChanged();
    void deviceNameChanged();
    void deviceManufacturerChanged();
    void deviceBrandChanged();

public slots:

    void setDeviceBrand(QString b) {deviceBrand = b; emit deviceBrandChanged();}
    QString getDeviceBrand() {return deviceBrand;}

    void setDeviceManufacturer(QString m) {deviceManufacturer = m; emit deviceManufacturerChanged();}
    QString getDeviceManufacturer() {return deviceManufacturer;}

    void setDeviceName(QString d){deviceName = d; emit deviceNameChanged();}
    QString getDeviceName(){return deviceName; }

    void setCodeName(QString c) {codename = c; emit codenameChanged();}
    QString getCodeName(){return codename;}

    void setStatusMessage(QString s) {statusMessage = s; emit statusMessageChanged();}
    QString getStatusMessage() {return statusMessage;}

    void setApiLevel(int lvl){ apiLevel = lvl; emit apiLevelChanged();}
    int getApiLevel(){ return apiLevel;}
    
private:

    void findClassIdents();



};

#endif // ANDROIDSYSTEM_H
