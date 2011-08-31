#include "screensavermodule.h"

ScreenSaverModule::ScreenSaverModule(QObject *parent) :
    QObject(parent)
{
    imageList.append("../../../img/lmcesplash.jpg");
   imageList.append("../../../img/icons/bedroom.jpeg");
   imageList.append("../../../img/icons/mediatime.png");

   setImage(QUrl(imageList.at(2)));
   testtext = "ack!";

}

void ScreenSaverModule::nextImage()
{
    setImage(QUrl(imageList.at(getNext())));
}

int ScreenSaverModule::getNext()
{
    int listcount = imageList.count();
    int pos = imageList.indexOf(current_image.toString());

    if ((pos+1) < listcount )
    {
        return 0;
    }
    else
    {
        return pos+1;
    }
}


