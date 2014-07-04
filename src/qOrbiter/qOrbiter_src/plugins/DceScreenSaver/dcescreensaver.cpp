#include "dcescreensaver.h"
#ifdef QT5
#include <qqml.h>
#include <QPainter>
#else
#include <qdeclarative.h>
#include <QPainter>
#endif


#include <qtimer.h>
#include <QDebug>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QTimerEvent>


const int animationInterval = 30; // should be 60fps

#ifdef QT5
DceScreenSaver::DceScreenSaver(QQuickPaintedItem *parent):
    QQuickPaintedItem(parent)
  #else
DceScreenSaver::DceScreenSaver(QDeclarativeItem *parent):
    QDeclarativeItem(parent)
  #endif
{
#ifdef QT5
    setFlag(QQuickItem::ItemHasContents, true);
#else
    // By default, QDeclarativeItem does not draw anything. If you subclass
    // QDeclarativeItem to create a visual item, you will need to uncomment the
    // following line:
    setFlag(ItemHasNoContents, false);
#endif
    enableDebug=false;
    useAnimation=true;
    if(useAnimation){
         m_animationTimer = startTimer(animationInterval);
    }
    fadeOpacity = 1;
    currentScale = 1;
    endSize.setHeight(0);
    endSize.setWidth(0);
    imgSet = false;
    currentImageName="none";

    currentUrl = "";
    active = true;
    running = false;
    ready = false;
    interval = 60000;
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

}

DceScreenSaver::~DceScreenSaver()
{
    intervalTimer->stop();

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

        if(active){
            getNextImage();
            setRunning(true);
        } else {
          setDebugInfo("Screen Saver inactive");
            setRunning(false);
           setDebugInfo("Screen Saver images loaded, but screensaver disabled by option.");
        }

        setReady(true);
       setDebugInfo("Screen Saver images loaded.");
    }  else {
        setRunning(false);
        setReady(false);
        setScreenSaverActive(false);
    }

}

void DceScreenSaver::requestImage(QString img){
    QNetworkRequest req;
    setCurrentImageName(img);
    req.setUrl("http://"+requestUrl+"/lmce-admin/imdbImage.php?type=screensaver&val="+img);
    QObject::connect(requestManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(processImageData(QNetworkReply*)));
    requestManager->get(req);
}

void DceScreenSaver::processImageData(QNetworkReply *r){

    QByteArray p;


    if(r->bytesAvailable()){
        p = r->readAll();
    } else{
        return;
    }

    QPixmap t;
    t.loadFromData(p);


    if(t.isNull()){
        intervalTimer->start(interval);
        this->update();
    }else{
        setDebugInfo("new picture");
        surface =currentImage.copy();
        currentImage= t.scaled(width(),height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }

    imgSet=false;
    if(useAnimation){
       setDebugInfo("Animation enabled.");
        startFadeTimer(2500);
    } else {
        setDebugInfo("Animation disabled");
        fadeOpacity=1;
        this->forceUpdate();
    }



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



#ifdef QT4
void DceScreenSaver::paint(QPainter *p ,const QStyleOptionGraphicsItem *option, QWidget *widget ){

    Q_UNUSED(option);
    Q_UNUSED(widget);

    p->setBrush(Qt::NoBrush);
    p->setPen(Qt::NoPen);
    p->setRenderHint(QPainter::HighQualityAntialiasing, 1);


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
#endif

#ifdef QT5
void DceScreenSaver::paint(QPainter *painter){

    setDebugInfo("DceScreenSaver::paint() started");
    painter->setBrush(Qt::NoBrush);
    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::HighQualityAntialiasing, 1);
    QRectF tgtRect(0,0,width(), height());

    if(!useAnimation){
        fadeOpacity=1;
    }

    if(fadeOpacity==1){
        painter->drawPixmap(tgtRect, surface.scaled(width(),height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation), tgtRect);
    }else{
        painter->drawPixmap(tgtRect, surface.scaled(width(),height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation), tgtRect);
        //setup and new pix map over it
        painter->setOpacity(fadeOpacity);
        //create 'composed image'
        painter->drawPixmap(tgtRect, currentImage.scaled(width(),height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation), tgtRect);
    }
     setDebugInfo("DceScreenSaver::paint() ended.");
}
#endif



void DceScreenSaver::timerEvent(QTimerEvent *event){

    if(event->timerId()==m_animationTimer){
        if(fadeOpacity!=1 && !currentImage.isNull() ){
            this->update();
        } else   if(fadeOpacity==1 && !imgSet){
            //  qWarning() << "Transition finish, setting currentImg ==> surface";
            surface=currentImage.copy();
            imgSet=true;

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


