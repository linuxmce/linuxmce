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

QImage AbstractImageProvider::floorplanProvider()
{

  QImage  key= managerreference->floorplans->getFloorPlanImage();

    if (key.isNull())
    {
        key.load(":/icons/icon.png");
    }

    //aspect checking
    if(key.height() > key.width())
    {
        managerreference->nowPlayingButton->setImageAspect("poster");
    }
    else
    {
        managerreference->nowPlayingButton->setImageAspect("wide");
    }
    return key;
}

QImage AbstractImageProvider::securityProvider(int cam)
{
   QImage key = managerreference->SecurityVideo->getCameraImage(cam);

   if (key.isNull())
   {
       key.load(":/icons/icon.png");

   }

   return key;

}

QImage AbstractImageProvider::updateObjectProvider()
{
   QImage key = managerreference->nowPlayingButton->getImage();
   //qDebug()<< "!!!!!!!!!!" <<key.size();
    if (key.isNull())
    {
        key.load(":/icons/icon.png");
        qDebug("Error getting image!");

    }
    //aspect checking
    if(key.height() > key.width())
    {
        managerreference->nowPlayingButton->setImageAspect("poster");
    }
    else
    {
        managerreference->nowPlayingButton->setImageAspect("wide");
    }
    return key;
}


QImage AbstractImageProvider::screenShot()
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

QImage AbstractImageProvider::Stream()
{
    QImage key = managerreference->nowPlayingButton->streamImage;

     if (key.isNull())
     {
          key.load(":/icons/video.png");
     }

     return key;
}

