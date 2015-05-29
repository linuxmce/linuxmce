#include "abstractimageprovider.h"
/*
  the functions herein correlate to the different list models that we have. this allows us to keep
  1 image provider as opposed to multiple.
  */
#if (QT5)
AbstractImageProvider::AbstractImageProvider(qorbiterManager *manager):QQuickImageProvider(QQuickImageProvider::Image), managerreference(manager)
  #else
AbstractImageProvider::AbstractImageProvider(qorbiterManager *manager):QDeclarativeImageProvider(QDeclarativeImageProvider::Image), managerreference(manager)
  #endif
{

}

AbstractImageProvider::~AbstractImageProvider()
{
}

QImage AbstractImageProvider::floorplanProvider(QSize &requestedSize)
{

    QImage  key= managerreference->floorplans->getFloorPlanImage();

    if (key.isNull())
    {
        key.load(":/icons/icon.png");
    }
     return key;



}

QImage AbstractImageProvider::securityProvider(QSize &requestedSize,int cam)
{
    QImage key = managerreference->mp_securityVideo->getCameraImage(cam);

    if (key.isNull())
    {
        key.load(":/icons/icon.png");

    }

    return key;

}

QImage AbstractImageProvider::updateObjectProvider(QSize &requestedSize)
{
    QImage key = managerreference->nowPlayingButton->getImage();
    //qDebug()<< "!!!!!!!!!!" <<key.size();
    if (key.isNull())
    {
        key.load(":/icons/icon.png");
        qDebug("Error getting image!");

    }
    //aspect checking
    double aRatio = (double)key.height() / (double)key.width();
    qDebug() << "Aspect Ratio Check    ==> Height / Width"<<key.height()<<"::"<<key.width();
    qDebug() << "Aspect RatioResult  ==> "<< aRatio;

    if( key.height() > key.width())
    {
        managerreference->nowPlayingButton->setImageAspect("poster");
    }
    else if (key.height() < key.width())
    {
        managerreference->nowPlayingButton->setImageAspect("wide");
    }
    else if(key.height() == key.width()){
        managerreference->nowPlayingButton->setImageAspect("album");
    }


    if(requestedSize.isValid() && requestedSize.width() >0 && requestedSize.height() >0){
        managerreference->setDceResponse("Abstract Image provider returned requestSize based on requested size. Image aspect" +QString::number(aRatio));
        return key.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );

    } else if(requestedSize.height() > 0 ){
        QSize fallback( requestedSize.height(),(int)(requestedSize.height()*aRatio) );
           qDebug() << "Val" << fallback;
        managerreference->setDceResponse("Abstract Image provider returned requestSize based on height() aspect " +QString::number(aRatio));
       QImage t = key.scaled(QSize(fallback), Qt::KeepAspectRatio, Qt::SmoothTransformation );
       qDebug() << t.isNull();
        return t;

    } else if (requestedSize.width() > 0){
        QSize fallback((int)(requestedSize.height()*aRatio), requestedSize.height() );
           qDebug() << "Val" << fallback;
        managerreference->setDceResponse("Abstract Image provider returned requestSize based on width() aspect " +QString::number(aRatio));
        return key.scaled( fallback, Qt::KeepAspectRatio, Qt::SmoothTransformation );
    } else {
        managerreference->setDceResponse("Abstract Image provider returned NO requestSize.  Aspect " +QString::number(aRatio));
        return key;
    }
}


QImage AbstractImageProvider::screenShot(QSize &requestedSize)
{
    QImage key = managerreference->mediaScreenShot;

    if (key.isNull())
    {
        key.load(":/icons/video.png");
    }

    return key;
}

QImage AbstractImageProvider::ScreenSaver()
{
    QImage key = managerreference->ScreenSaver->getImageData();

    if (key.isNull())
    {
        key.load(":/icons/icon.png");
    }

    return key;
}

QImage AbstractImageProvider::Stream(QSize &requestedSize)
{
    QImage key = managerreference->nowPlayingButton->streamImage;

    if (key.isNull())
    {
        key.load(":/icons/video.png");
    }

    return key;
}

