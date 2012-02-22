#ifndef AVDEVICE_H
#define AVDEVICE_H

#include <QObject>

class AvDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QString qs_device READ getDeviceName WRITE setDeviceName NOTIFY deviceNameSet )
    Q_PROPERTY (QString qs_controlledBy READ getController WRITE setController NOTIFY controllerSet)
    Q_PROPERTY (int i_deviceNo READ getDeviceNo WRITE setDeviceNo NOTIFY deviceSet)
public:
    explicit AvDevice(int inc_deviceNo, QString device_name, QString qs_controller, QObject *parent = 0);
    int i_deviceNo;
    QString qs_controlledBy;
    QString qs_device;
    
signals:
    void deviceSet();
    void controllerSet();
    void deviceNameSet();
    
public slots:
    void setDeviceNo(int d);
    int getDeviceNo();

    void setController(QString controller);
    QString getController();

    void setDeviceName(QString d);
    QString getDeviceName();
    
};

#endif // AVDEVICE_H
