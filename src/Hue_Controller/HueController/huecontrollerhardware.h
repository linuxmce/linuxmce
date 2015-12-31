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

    QString getIpAddress() {return m_ipAddress;}
    QString getMacAddress() {return m_macAddress;}
    QString getInternalId() {return m_phillips_id;}


    
};

#endif // HUECONTROLLERHARDWARE_H
