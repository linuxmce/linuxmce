#ifndef HUEINTERFACE_H
#define HUEINTERFACE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QUrl>

class HueInterface : public QObject
{
    Q_OBJECT
public:
    explicit HueInterface(QObject *parent = 0);
    QNetworkAccessManager *comms;
    QEventLoop waiter;
signals:
    void downloadFinished(QString);
    void newDataStore(const QByteArray b);

public slots:
    void getDeviceDatabase(QUrl target);
    void downloadResponse();
    
};

#endif // HUEINTERFACE_H
