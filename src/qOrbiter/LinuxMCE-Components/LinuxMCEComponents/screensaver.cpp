#include "screensaver.h"
#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QtNetwork/QNetworkAccessManager>
#include <QEventLoop>
#include <QThread>
ScreenSaver::ScreenSaver(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
    setFlag(ItemHasNoContents, false);
    setActive(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);
    qDebug() << acceptedMouseButtons();
    qDebug() << acceptTouchEvents();
    qi_currentImage = QImage();
  connect(&picChanger, SIGNAL(timeout()), this, SLOT(selectNew()));

}


void ScreenSaver::setImageData( QByteArray buffer)
{
    //    qi_currentImage = QImage();
    //    qi_currentImage.loadFromData(buffer);
    //  emit imageChanged();
    //buffer.clear();
}

void ScreenSaver::setActive(bool state)
{
    if(state==true){
        active = true;
       picChanger.start();
    }
    else{
        active=false;
        picChanger.stop();

    }
    emit activatedChanged();
    setStatus(state);
}

bool ScreenSaver::getStatus()
{
    return isReady;
}

void ScreenSaver::setStatus(bool status)
{
    isReady = status;
    emit statusChanged();

}

void ScreenSaver::pickImage()
{

    picChanger.start(timerInterval);
}

void ScreenSaver::selectNew()
{
    currentImage = newImageUrl;
    emit requestNewImage();
}

void ScreenSaver::fetchImageData()
{
    qDebug() << newImageUrl;
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkRequest req;
    req.setUrl(QUrl(newImageUrl));
    QNetworkReply * r = manager->get(req);

    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(processImageData(QNetworkReply*)));
    //    QEventLoop e;
    //    QObject::connect(r,SIGNAL(finished()), &e, SLOT(quit()));
    //            e.exec();
}

void ScreenSaver::processImageData(QNetworkReply *imgData)
{
    qDebug()<<"Reply Recieved" << imgData->size();
    qi_currentImage.loadFromData(imgData->readAll());
    this->update();
}

void ScreenSaver::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{

    QRect plane;
    plane.setSize(QSize( (int)this->width(),(int)this->height()) );
//    QPen pen;
//    QBrush(color);
//    pen.setWidthF(1.0);
    //pen.setColor(color);
    painter->drawImage(plane, qi_currentImage.scaled(QSize(this->width(), this->height()), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
   sleep(.045);
   qDebug() << "Thread Sleeping";
}

QString ScreenSaver::getImage()
{
    return currentImage;
}

void ScreenSaver::setImage(QString url)
{
    if(!url.isEmpty()){
        currentImage =  url;
        //  emit requestNewImage(currentImage);
    }
    else{
        currentImage="";

    }
}
