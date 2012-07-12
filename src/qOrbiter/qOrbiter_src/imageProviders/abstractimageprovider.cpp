#include "abstractimageprovider.h"
/*
  the functions herein correlate to the different list models that we have. this allows us to keep
  1 image provider as opposed to multiple.
  */
#if (QT_VERSION >= 0x050000)
AbstractImageProvider::AbstractImageProvider(qorbiterManager *manager):QQmlImageProvider(QQmlImageProvider::Image), managerreference(manager)
  #else
AbstractImageProvider::AbstractImageProvider(qorbiterManager *manager):QDeclarativeImageProvider(QDeclarativeImageProvider::Image), managerreference(manager)
#endif
{

}

QImage AbstractImageProvider::floorplanProvider()
{

    key= managerreference->floorplans->getFloorPlanImage();

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

QImage AbstractImageProvider::securityProvider()
{
   key = managerreference->SecurityVideo->currentFrame;

   if (key.isNull())
   {
       key.load(":/icons/icon.png");

   }

   return key;

}

QImage AbstractImageProvider::updateObjectProvider()
{
   key = managerreference->nowPlayingButton->getImage();
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

QImage AbstractImageProvider::fileDetails()
{
    key= managerreference->filedetailsclass->getTitleImage();

    if (key.isNull())
    {
         key.load(":/icons/icon.png");

    }
    managerreference->filedetailsclass->setAspectH(key.height()/ 100);
    managerreference->filedetailsclass->setAspectW(key.width() / 100);
    //qDebug()<< key.width()/key.height();

    return key;
}

QImage AbstractImageProvider::screenShot()
{
    key = managerreference->mediaScreenShot;

     if (key.isNull())
     {
          key.load(":/icons/video.png");
     }

     return key;
}

QImage AbstractImageProvider::ScreenSaver()
{
    key = managerreference->ScreenSaver->qi_currentImage;

     if (key.isNull())
     {
          key.load(":/icons/icon.png");
     }

     return key;
}

QImage AbstractImageProvider::Stream()
{
    key = managerreference->nowPlayingButton->streamImage;

     if (key.isNull())
     {
          key.load(":/icons/video.png");
     }

     return key;
}

