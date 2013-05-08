#include "hueinterface.h"
#include "QDebug"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
HueInterface::HueInterface(QObject *parent) :
    QObject(parent)
{
    comms = new QNetworkAccessManager(this);
 //   QObject::connect(comms, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadResponse(QNetworkReply*)));
    QObject::connect(comms, SIGNAL(finished(QNetworkReply*)), &waiter, SLOT(quit()));

}


void HueInterface::getDeviceDatabase()
{
    qDebug() << "Slot Activated in interface";
    QNetworkRequest test;
    test.setUrl(QUrl("http://www.google.com"));
    QNetworkReply *t= comms->get(test);
    QObject::connect(t, SIGNAL(finished()), this, SLOT(downloadResponse()));
    qDebug("Data request sent");
    waiter.exec();


}

void HueInterface::downloadResponse()
{
    qDebug() << " Got DataStore Response " ;
    QNetworkReply* r = qobject_cast<QNetworkReply*>(sender());
    qDebug() << r->readAll();
//    QJson::Parser parser;
//    bool ok;
//    QVariant p = parser.parse(r->readAll(), &ok);
//    qDebug() << p;
}
