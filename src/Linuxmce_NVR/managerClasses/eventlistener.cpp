#include "eventlistener.h"
#include "qdebug.h"
#include "qdatetime.h"
#include "qhttprequest.h"
#include "qhttpresponse.h"
#include "qurl.h"
#include "qurlquery.h"
#include "qjsondocument.h"
#include "qjsonarray.h"
#include "../cameraClasses/abstractcameraevent.h"
#include "qimage.h"
#include "qfile.h";
#include "../cameraClasses/abstractcameraevent.h"
MotionEventListener::MotionEventListener(int dceId, int listen_port, QObject *parent) : m_listenPort(0), dceDevice(dceId)
{
    server = new QHttpServer();
    connect(server, &QHttpServer::newRequest, this, &MotionEventListener::handleEvent);
  setListenPort(listen_port);
}

bool MotionEventListener::isReady() const
{
    return m_isReady;
}

void MotionEventListener::setIsReady(bool isReady)
{
    if(m_isReady==isReady)return;
    m_isReady = isReady; emit isReadyChanged();
}
int MotionEventListener::listenPort() const
{
    return m_listenPort;
}

void MotionEventListener::setListenPort(const int &listenPort)
{

    if(m_listenPort==listenPort)return;
    m_listenPort = listenPort; emit listenPortChanged();

    if(server->listen(QHostAddress::Any, m_listenPort) ){
        setIsReady(true);

        log( QString(Q_FUNC_INFO)+"::listening on " +QString::number(m_listenPort));
    } else {
        setIsReady(false);
        log( QString(Q_FUNC_INFO)+"::failed to start");

    }

}

QString MotionEventListener::currentHost()
{

  return QString("192.168.80.194");
}

void MotionEventListener::log(QString msg)
{
    qDebug() << QDateTime::currentDateTime().toLocalTime().toString() << msg;
}

void MotionEventListener::handleEvent(QHttpRequest *r, QHttpResponse *a)
{
    int hdrLength= r->header("content-length").toInt();
    if(hdrLength > 1000){
      int device = r->header("X-dcedevice").toInt();
        r->storeBody();      
        QByteArray standardResponse("OK");
        // a->setHeader("Content-Length", QString::number(standardResponse.length()));
        connect(r, &QHttpRequest::end, [=](){
            a->writeHead(200);
            a->end(standardResponse);
            handleImageData(r->body(), device);
        } );

    } else {
        connect(r, &QHttpRequest::data, this, &MotionEventListener::spitFire);
        QByteArray standardResponse("Command Recieved");
        a->setHeader("Content-Length", QString::number(standardResponse.length()));
        a->writeHead(200);
        a->end(standardResponse);
        QObject::disconnect(r, SIGNAL(data(QByteArray)), this, SLOT(spitFire(QByteArray)));
    }
}

void MotionEventListener::spitFire(QByteArray d)
{

    d.replace("'", "\"");
    QJsonDocument eventDoc=QJsonDocument::fromJson(d);
    if(eventDoc.isNull()){
        qDebug() << "Invalid Event";
        qDebug() << d;
        return;
    }

    QVariant data = eventDoc.toVariant();
    QVariantMap eventData = data.value<QVariantMap>();
    QVariantMap eventCode = eventData["event"].toMap();

    CameraEvent *evt = new CameraEvent(
                eventData["ip"].toString(),
            eventData["host"].toString(),
            eventData["ip"].toString(),
            eventCode["status"].toString(),
            eventCode["status"].toString()=="stopped" ? CameraEvent::MOTION_ENDED : CameraEvent::MOTION_STARTED,
            eventData["deviceId"].toInt()
            );

  // qDebug() << data;
   // emit motionEvent( eventData["deviceId"].toInt(), eventCode["status"].toString()=="stopped" ? false : true );
    emit motionEvent(evt);

}

void MotionEventListener::handleImageData(QByteArray imgData, int device)
{
   // qDebug() << Q_FUNC_INFO;
   // qDebug() << imgData;
    int brk = imgData.indexOf("\xFF\xD8\xFF");

    QString parts=imgData.mid(0, brk);
    int fName = parts.indexOf("filename=\"")+9;
    int fEnd = parts.indexOf("jpg")+3;


    QString fileName = parts.mid(fName ,fEnd-fName).remove("\"");   
    imgData.remove(0,brk);
    QImage t;

    if(t.loadFromData(imgData)){

        if(t.save("/tmp/"+QString::number(dceDevice)+"/event-"+fileName, "JPEG"))
            qDebug() << QString("Image saved for device %1 using filename %2").arg(device).arg(fileName);
        return;
    } else {
         qDebug() << QString("Image saved for device %1 using filename %2 failed.").arg(device).arg(fileName);
    }
}

void MotionEventListener::replyEnded()
{
    qDebug() << " reply in ether ";
}




