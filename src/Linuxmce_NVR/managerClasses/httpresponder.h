#ifndef HTTPRESPONDER_H
#define HTTPRESPONDER_H
#include "qscopedpointer.h"
#include <QObject>
class QHttpRequest;
class QHttpResponse;
class HttpResponder : public QObject
{
    Q_OBJECT
public:
    explicit HttpResponder(QHttpRequest* req, QHttpResponse* resp, QObject *parent = 0);


signals:
    void done();

public slots:

private slots:
    void accumulate(const QByteArray &data);
    void reply();

private:
    QHttpRequest* m_req;
    QHttpResponse *m_resp;
    QByteArray respData;
};

#endif // HTTPRESPONDER_H
