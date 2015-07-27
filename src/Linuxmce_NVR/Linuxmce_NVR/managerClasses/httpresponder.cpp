#include "httpresponder.h"
#include "qhttprequest.h"
#include "qhttpresponse.h"
#include "qdebug.h"
HttpResponder::HttpResponder(QHttpRequest *req, QHttpResponse *resp, QObject *parent) : QObject(parent), m_req(req), m_resp(resp)
{
    m_resp->setHeader("Content-Type", "text/html");
       m_resp->writeHead(200);

       connect(req, SIGNAL(data(const QByteArray&)), this, SLOT(accumulate(const QByteArray&)));
          connect(req, SIGNAL(end()), this, SLOT(reply()));
          connect(m_resp, SIGNAL(done()), this, SLOT(deleteLater()));
}

void HttpResponder::accumulate(const QByteArray &data)
{
 respData.append(data);
 qDebug() << respData;
}

void HttpResponder::reply()
{
 m_resp->end(QByteArray("</p></body></html>"));
}

