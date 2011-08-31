#include "screensavermodule.h"

ScreenSaverModule::ScreenSaverModule(QObject *parent) :
    QObject(parent)
{
    imageList.append("../../../img/lmcesplash.jpg");
   imageList.append("../../../img/icons/bedroom.jpg");
   imageList.append("../../../img/icons/mediatime.png");

   setImage(QUrl(imageList.at(0)));
   testtext = "ack!";
   timerInterval = 10000;

   picChanger.setInterval(timerInterval);
   connect(&picChanger, SIGNAL(timeout()), this, SLOT(nextImage()));
   picChanger.start();

}

void ScreenSaverModule::nextImage()
{
    setImage(QUrl(imageList.at(getNext())));

}

int ScreenSaverModule::getNext()
{
    int listcount = (imageList.count()) - (1);
    int pos = imageList.indexOf(current_image.toString());

   // qDebug() << pos;

    if ((pos+1) > listcount )
    {
        return 0;
    }
    else
    {
        return pos+1;
    }
}


