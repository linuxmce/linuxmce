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

    if (key.isNull())
    {
         key.load(":/icons/playlist.png");

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

   return key;
}

