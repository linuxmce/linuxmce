#include "dcescreensaver.h"
#include <QtOpenGL>
#include <qdeclarative.h>
#include <qtimer.h>
#include <QDebug>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QImage>
#include <QPainter>

DceScreenSaver::DceScreenSaver(QDeclarativeItem *parent):
    QDeclarativeItem(parent)
{
    // By default, QDeclarativeItem does not draw anything. If you subclass
    // QDeclarativeItem to create a visual item, you will need to uncomment the
    // following line:
    glEnable(GL_MULTISAMPLE);
    setFlag(ItemHasNoContents, false);
    currentUrl = "";
    active = false;
    running = false;
    ready = false;
    interval = 5000;
    requestManager = new QNetworkAccessManager(this);
    intervalTimer = new QTimer();
    intervalTimer->setSingleShot(false);
    intervalTimer->stop();
    QObject::connect(intervalTimer, SIGNAL(timeout()), this, SLOT(getNextImage()));
    qDebug() << "Screensaver ctor";
    currentImage = QImage();

}

DceScreenSaver::~DceScreenSaver()
{

}

void DceScreenSaver::setImageList(QStringList l)
{
    if(l.contains("")){
        l.removeAll("");
    }
    urlList=l;
    if(!urlList.isEmpty()){
        emit urlListReady();
        pictureCount = l.count();
        getNextImage();
        setRunning(true);
        setReady(true);
        setActive(true);
        qWarning() << "Screen Saver images loaded.";
    }
    else {

    }

}

void DceScreenSaver::requestImage(QString img)
{
    QNetworkRequest req;
    req.setUrl("http://"+requestUrl+"/lmce-admin/imdbImage.php?type=screensaver&val="+img);
    QObject::connect(requestManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(processImageData(QNetworkReply*)));
    requestManager->get(req);

}

void DceScreenSaver::processImageData(QNetworkReply *r)
{
    QByteArray p;

    if(r->bytesAvailable()){
        p = r->readAll();
    }
    else{
        return;
    }

    QImage t;
    t.loadFromData(p);

    if(t.isNull()){
        intervalTimer->start(interval);
    }
    else{
        currentImage = t;
    }


}

void DceScreenSaver::getNextImage()
{
    requestImage(urlList.at(rand()%urlList.count()));
}




void DceScreenSaver::paint(QPainter *p ,const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    p->setBrush(Qt::NoBrush);
    p->setPen(Qt::NoPen);
    p->setRenderHint(QPainter::Antialiasing);
    p->drawRoundedRect(0, 0, boundingRect().width(), boundingRect().height() - 10, 10,10);
    p->drawImage(boundingRect(), currentImage );

}

void DceScreenSaver::update()
{

}

