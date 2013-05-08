#ifndef HUEINTERFACE_H
#define HUEINTERFACE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QEventLoop>

class HueInterface : public QObject
{
    Q_OBJECT
public:
    explicit HueInterface(QObject *parent = 0);
    QNetworkAccessManager *comms;
    QEventLoop waiter;
signals:
    void downloadFinished(QString);

public slots:
    void getDeviceDatabase();
    void downloadResponse();
    
};

#endif // HUEINTERFACE_H
