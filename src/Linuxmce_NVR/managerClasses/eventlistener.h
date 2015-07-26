#ifndef MOTIONEVENTLISTENER_H
#define MOTIONEVENTLISTENER_H

#include <QObject>
#include "qtcpserver.h"
#include "qhttpserver.h"

class MotionEventListener : public QObject
{
    Q_OBJECT
public:
    explicit MotionEventListener(quint16 listen_port=8001 , QObject *parent = 0);

    bool isReady() const;
    void setIsReady(bool isReady);

    quint16 listenPort() const;
    void setListenPort(const quint16 &listenPort);


signals:
    void newEvent(QString event);
    void listenPortChanged();
    void isReadyChanged();

public slots:

    void log(QString msg);

private slots:
    void handleEvent(QHttpRequest*r, QHttpResponse *a );
    void spitFire(QByteArray d);
    void handleImageData(QByteArray imgData);
    void replyEnded();

private:
    QHttpServer *server;
    quint16 m_listenPort;
    bool m_isReady;
    QList<QPair<QHttpRequest*, QHttpResponse*> > qued_http_requests;

};

#endif // MOTIONEVENTLISTENER_H
