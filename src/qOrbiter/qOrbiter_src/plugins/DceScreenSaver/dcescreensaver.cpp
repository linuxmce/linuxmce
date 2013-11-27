#include "dcescreensaver.h"
#include <qdeclarative.h>
#include <qtimer.h>
#include <QDebug>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QPainter>
#include <QTimerEvent>


const int animationInterval = 30; // should be 60fps

DceScreenSaver::DceScreenSaver(QDeclarativeItem *parent):
    QDeclarativeItem(parent)
{
    // By default, QDeclarativeItem does not draw anything. If you subclass
    // QDeclarativeItem to create a visual item, you will need to uncomment the
    // following line:
    m_animationTimer = startTimer(animationInterval);

    fadeOpacity = 1;
    currentScale = 1;
    endSize.setHeight(0);
    endSize.setWidth(0);
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

    currentImage.fill(Qt::black);
    fadeAnimation = new QPropertyAnimation(this, "fadeOpacity");

    fadeAnimation->setDuration(2500);
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(1.0);

    zoomAnimation = new QPropertyAnimation(this, "currentScale");
    zoomAnimation->setStartValue(1);
    zoomAnimation->setEndValue(1.05);
    zoomAnimation->setDuration(interval);

    surface.fill(Qt::black);
    QObject::connect(this, SIGNAL(heightChanged()), this, SLOT(forceUpdate()));
    QObject::connect(this, SIGNAL(widthChanged()), this, SLOT(forceUpdate()));
  //  QObject::connect(zoomAnimation, SIGNAL(finished()), this, SLOT(resetPicture()));
 qDebug() << "Screensaver ctor";
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
    }    else{
        return;
    }

    QPixmap t;
    t.loadFromData(p);

    if(t.isNull()){
        intervalTimer->start(interval);
    }else{
        surface =currentImage;
        currentImage= t.scaled(width(),height(), Qt::KeepAspectRatioByExpanding, Qt::FastTransformation);
    }

    startFadeTimer(2500);
  //  beginZoom();

}

void DceScreenSaver::getNextImage()
{
    if(urlList.isEmpty()){
        return;
    }

#ifdef ANDROID
    requestImage(urlList.at(rand()%urlList.count()));
#else
    requestImage(urlList.at(rand()%urlList.count()));
#endif
}




void DceScreenSaver::paint(QPainter *p ,const QStyleOptionGraphicsItem *option, QWidget *widget )
{

    Q_UNUSED(option); //mark unused options
    Q_UNUSED(widget);
    p->setBrush(Qt::NoBrush);
    p->setPen(Qt::NoPen);
#ifdef ANDROID
    p->setRenderHint(QPainter::HighQualityAntialiasing, 1);
#endif

    //draw old frame first
    QRectF tgtRect(0,0,width(), height());
    QSize scaledSize;
    scaledSize.setHeight(height()*currentScale);
    scaledSize.setWidth(width()*currentScale);

    p->drawPixmap(tgtRect, surface, tgtRect);

    //setup and new pix map over it
    p->setOpacity(fadeOpacity);

    //create 'composed image'
    p->drawPixmap(tgtRect, currentImage, tgtRect);
}



void DceScreenSaver::timerEvent(QTimerEvent *event)
{
    if(event->timerId()==m_animationTimer){
        if(fadeOpacity!=1 && !currentImage.isNull() ){
            this->update();
        }
    }
}



void DceScreenSaver::beginZoom()
{
//    zoomAnimation->setDuration(interval);
//    zoomAnimation->start();
}

void DceScreenSaver::startFadeTimer(int time)
{
    fadeAnimation->setDuration(time);
    fadeAnimation->start();
}
