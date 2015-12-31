#ifndef MOTIONEVENTLISTENER_H
#define MOTIONEVENTLISTENER_H

#include <QObject>
#include "qtcpserver.h"
#include "qhttpserver.h"
class CameraEvent;

class MotionEventListener : public QObject
{
    Q_OBJECT
public:
    explicit MotionEventListener(int dceId,int listen_port=8001 ,  QObject *parent = 0);

    bool isReady() const;
    void setIsReady(bool isReady);

    int listenPort() const;
    void setListenPort(const int &listenPort);


signals:
    void motionEvent(int device, bool motionDetected);
    void motionEvent(CameraEvent * e);
    void listenPortChanged();
    void isReadyChanged();

public slots:
    QString currentHost();
    void log(QString msg);

private slots:
    void handleEvent(QHttpRequest*r, QHttpResponse *a );
    void spitFire(QByteArray d);
    void handleImageData(QByteArray imgData, int device);
    void replyEnded();

private:
    QHttpServer *server;
    int m_listenPort;
    bool m_isReady;
    QList<QPair<QHttpRequest*, QHttpResponse*> > qued_http_requests;
    int dceDevice;

};

#endif // MOTIONEVENTLISTENER_H
