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
  ,interval(60000)
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
    zoomEnabled=false;
    fadeEnabled = false;
    if(useAnimation){
        m_animationTimer = startTimer(animationInterval);
    }
    incomingTransition_x=0;
    incomingTransition_y=0;
    currentTransition_y=0;
    currentTransition_x=0;


    transitionTime = 2500;
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
    requestManager = new QNetworkAccessManager();
    intervalTimer = new QTimer();
    intervalTimer->setSingleShot(false);

    QObject::connect(intervalTimer, SIGNAL(timeout()), this, SLOT(getNextImage()));
    QObject::connect(requestManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(processImageData(QNetworkReply*)));

    currentImage.fill(Qt::black);
    fadeAnimation = new QPropertyAnimation(this, "fadeOpacity");

    fadeAnimation->setDuration(transitionTime);
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(1.0);

    zoomAnimation = new QPropertyAnimation(this, "currentScale");
    zoomAnimation->setStartValue(1);
    zoomAnimation->setEndValue(1.05);
    zoomAnimation->setDuration(interval);

    surface.fill(Qt::black);
    QObject::connect(this, SIGNAL(rotationChanged()), this, SLOT(forceUpdate()));
    QObject::connect(this, SIGNAL(heightChanged()), this, SLOT(forceUpdate()));
    QObject::connect(this, SIGNAL(widthChanged()), this, SLOT(forceUpdate()));
    //  QObject::connect(zoomAnimation, SIGNAL(finished()), this, SLOT(resetPicture()));

}

DceScreenSaver::~DceScreenSaver(){
    intervalTimer->stop();
}

void DceScreenSaver::setImageList(QStringList l){
    if(l.contains("")){
        l.removeAll("");
    }
    urlList=l;

    if(!urlList.isEmpty()){
        emit urlListReady();
        pictureCount = l.count()-1;

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
    intervalTimer->stop();
    setCurrentImageName(img);
    setDebugInfo("requesting image");
    QNetworkRequest req;
    req.setUrl("http://"+requestUrl+"/lmce-admin/imdbImage.php?type=screensaver&val="+img);
    requestManager->get(req);
}

void DceScreenSaver::processImageData(QNetworkReply *r){


    setDebugInfo("handling new data");
    QByteArray p;

    if(r->bytesAvailable()){
        p = r->readAll();
    } else{
        setDebugInfo("No image!");
        r=NULL;
        getNextImage();
        return;
    }

    QPixmap t;
    t.loadFromData(p);

    if(t.isNull()){
        setDebugInfo("Null Pixmap");
        intervalTimer->start(interval);
        this->update();
        return;
    }else{
        setDebugInfo("new picture");
        currentImage= t.scaled(width(),height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        fadeOpacity=0;
    }

    imgSet=false;
    if(useAnimation){
        setDebugInfo("Animation enabled.");
        startFadeTimer();
    } else {
        setDebugInfo("Animation disabled");
        fadeOpacity=1;
        this->forceUpdate();

    }
    emit newPictureRecieved();

    r=NULL;

}

void DceScreenSaver::getNextImage()
{
    if(urlList.isEmpty()){
        return;
    }
     setDebugInfo("Getting random image from list of "+ QString::number(urlList.count()));

    int listNumber = rand()%pictureCount;
    if(listNumber!=-1 && listNumber <= pictureCount){
       requestImage(urlList.at(listNumber));
    } else {
        requestImage(urlList.at(0));
    }


}



#ifdef QT4
void DceScreenSaver::paint(QPainter *p ,const QStyleOptionGraphicsItem *option, QWidget *widget ){

    Q_UNUSED(option);
    Q_UNUSED(widget);

    //  setDebugInfo("DceScreenSaver::paint() started");
    p->setBrush(Qt::NoBrush);
    p->setPen(Qt::NoPen);
    p->setRenderHint(QPainter::HighQualityAntialiasing, 1);
    QRectF tgtRect(0,0,width(), height());

    if(!useAnimation){
        fadeOpacity=1;
    }

    if(fadeOpacity==1 && imgSet){
        p->drawPixmap(tgtRect, surface, tgtRect);
    }else{

        p->drawPixmap(tgtRect, surface, tgtRect);
        //setup and new pix map over it
        p->setOpacity(fadeOpacity);
        //create 'composed image'
        p->drawPixmap(tgtRect, currentImage, tgtRect);
    }
    //  setDebugInfo("DceScreenSaver::paint() ended.");

}
#endif

#ifdef QT5
void DceScreenSaver::paint(QPainter *painter){

    //  setDebugInfo("DceScreenSaver::paint() started");
    painter->setBrush(Qt::NoBrush);
    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::HighQualityAntialiasing, 1);
    QRectF tgtRect(0,0,width(), height());

    if(!useAnimation){
        fadeOpacity=1;
    }

    if(fadeOpacity==1){
        painter->drawPixmap(tgtRect, currentImage, tgtRect);
    }else{
        painter->drawPixmap(tgtRect, surface, tgtRect);
        //setup and new pix map over it
        painter->setOpacity(fadeOpacity);
        //create 'composed image'
        painter->drawPixmap(tgtRect, currentImage, tgtRect);
    }
    // setDebugInfo("DceScreenSaver::paint() ended.");
}
#endif



void DceScreenSaver::timerEvent(QTimerEvent *event){

    if(event->timerId()==m_animationTimer){
        if(fadeOpacity!=1 ){
            this->update();
        } else   if(fadeOpacity==1 && !imgSet){
            setDebugInfo("Transition finish, setting currentImg ==> surface");
            surface=currentImage.scaled(width(),height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            imgSet=true;
            emit transitionFinished();
            intervalTimer->start(interval);

        } else if(fadeOpacity==1 && imgSet){
            // this->update();
        }
    }

}

void DceScreenSaver::startFadeTimer(){
    fadeAnimation->setDuration(transitionTime);
    fadeAnimation->start();
}


