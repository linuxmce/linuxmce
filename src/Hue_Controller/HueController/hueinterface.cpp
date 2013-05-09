#include "hueinterface.h"
#include "QDebug"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <qjson/parser.h>

HueInterface::HueInterface(QObject *parent) :
    QObject(parent)
{
    comms = new QNetworkAccessManager(this);
    //  QObject::connect(comms, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadResponse(QNetworkReply*)));
    QObject::connect(comms, SIGNAL(finished(QNetworkReply*)), &waiter, SLOT(quit()));

}


void HueInterface::getDeviceDatabase(QUrl target)
{
    qDebug() << "Slot Activated in interface";
    QNetworkRequest test;
    test.setUrl(target);
    QNetworkReply *t= comms->get(test);
    QObject::connect(t, SIGNAL(finished()), this, SLOT(downloadResponse()));
    qDebug("Data request sent");
    waiter.exec();
}

void HueInterface::downloadResponse()
{
    qDebug() << " Got DataStore Response " ;
    QNetworkReply* r = qobject_cast<QNetworkReply*>(sender());
    QByteArray dataStore = r->readAll();
    emit newDataStore(dataStore);
    r->deleteLater();
}
