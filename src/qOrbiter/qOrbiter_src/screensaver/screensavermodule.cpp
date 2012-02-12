#include "screensavermodule.h"

ScreenSaverModule::ScreenSaverModule(QObject *parent) :
    QObject(parent)
{
    imageList.append("../img/lmcesplash.png");
    imageList.append("../img/icons/bedroom.jpg");
    imageList.append("../img/icons/mediatime.png");

    setImage(QUrl(imageList.at(0)));
    testtext = "ack!";
    timerInterval = 10000;
    setTImage(QUrl(imageList.at(getNext())));

    picChanger.setInterval(timerInterval);
    connect(&picChanger, SIGNAL(timeout()), this, SLOT(nextImage()));
    picChanger.start();

    QTimer::singleShot(5000,this,SLOT(startAlternate()));

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

void ScreenSaverModule::nextTransition()
{
    setTImage(QUrl(imageList.at(getNext())));
}

void ScreenSaverModule::startAlternate()
{
    picChanger2.setInterval(timerInterval);
    connect(&picChanger2, SIGNAL(timeout()), this, SLOT(nextTransition()));
    picChanger2.start();

}


