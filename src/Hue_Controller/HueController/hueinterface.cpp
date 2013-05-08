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
   // qDebug() << r->readAll();
    QJson::Parser parser;
    bool ok;
    QVariantMap p = parser.parse(r->readAll(), &ok).toMap();
    QVariantMap lightNest = p["lights"].toMap();

    qDebug() << "Light Count"<< lightNest.count();
   // qDebug() << lightNest;
    QMap<QString, QVariant>::const_iterator i;

    for(i=lightNest.begin(); i!= lightNest.constEnd(); i++ )
    {
        qDebug() << i.key() ;// << " :: " << i.value();
        QVariantMap light = i.value().toMap();

        QVariantMap state = light["state"].toMap();
        qDebug() << state["on"].toString();
    }


}
