#ifndef HUECONTROLLERHARDWARE_H
#define HUECONTROLLERHARDWARE_H

#include <QObject>

class HueControllerHardware : public QObject
{
    Q_OBJECT
public:
    explicit HueControllerHardware(QObject *parent = 0);
    QString m_ipAddress;
    QString m_macAddress;
    QString m_name;
    QString m_phillips_id;
    QString m_userName;
    bool linkButtonState;
    bool portalEnabled;

signals:
    
public slots:


    
};

#endif // HUECONTROLLERHARDWARE_H
