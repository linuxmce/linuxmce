#include "screensaverclass.h"
#include <QTimer>


ScreenSaverClass::ScreenSaverClass(QObject *parent) :
    QObject(parent)
{

}

void ScreenSaverClass::setImageList(QStringList imgList)
{
    images = imgList;
    if(!images.empty())
    {
        pickImage();
        setImage(images.at(0));
        emit screenSaverReady();
    }
    else
    {
        //("Screensaver image list error!");
    }
}

void ScreenSaverClass::clearImageList()
{
    images.clear();
}

void ScreenSaverClass::setImageData(const uchar *data, int iData_size)
{
    QImage img;
    qi_currentImage = img;
    qi_currentImage.loadFromData(data, iData_size);
    emit imageChanged();
}

void ScreenSaverClass::setActive(bool state)
{
   active = state;
   emit stateChanged();
}

bool ScreenSaverClass::getStatus()
{
    return isReady;
}

void ScreenSaverClass::setStatus(bool status)
{
    isReady = status;
    emit statusChanged();

}

void ScreenSaverClass::pickImage()
{

    QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(selectNew()));
        timer->start(10000);


}

void ScreenSaverClass::selectNew()
{
    int index = images.lastIndexOf(currentImage);

    int count = images.count();

    if (count > index+1)
    {
        setImage(images.at(index+1));
    }
    else
    {
        setImage(images.at(0));

    }
}

QString ScreenSaverClass::getImage()
{
    return currentImage;
}

void ScreenSaverClass::setImage(QString url)
{
    currentImage =  url;
    emit requestNewImage(currentImage);

}
