#include "abstractimageprovider.h"
/*
  the functions herein correlate to the different list models that we have. this allows us to keep
  1 image provider as opposed to multiple.
  */

AbstractImageProvider::AbstractImageProvider(qorbiterManager *manager):QDeclarativeImageProvider(QDeclarativeImageProvider::Image), managerreference(manager)
{

}

QImage AbstractImageProvider::floorplanProvider()
{

    key= managerreference->floorplans->getPageImage(managerreference->floorplans->currentPage);


    if (key.isNull())
    {
        key.load(":/icons/playlist.png");

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
       key.load(":/icons/playlist.png");

   }

   return key;

}

QImage AbstractImageProvider::updateObjectProvider()
{
   key = managerreference->updatedObjectImage;
   //qDebug()<< "!!!!!!!!!!" <<key.size();
    if (key.isNull())
    {
         key.load(":/icons/playlist.png");

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
    key= managerreference->filedetailsclass->getScreenshotimage();

    if (key.isNull())
    {
         key.load(":/icons/playlist.png");

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
          key.load(":/icons/playlist.png");

     }

     return key;
}

QImage AbstractImageProvider::ScreenSaver()
{
    key = managerreference->pqOrbiter->getfileForDG(managerreference->ScreenSaver.currentImage.toStdString());

     if (key.isNull())
     {
          key.load(":/icons/playlist.png");
     }

     return key;
}

