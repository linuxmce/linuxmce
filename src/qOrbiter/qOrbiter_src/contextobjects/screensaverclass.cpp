#include "screensaverclass.h"
#include <QTimer>
#include <QDebug>


ScreenSaverClass::ScreenSaverClass(QObject *parent) :
    QObject(parent)
{
    qi_currentImage = QImage();
    primary = true;
    transitionDuration = 15000;
    connect(&picChanger, SIGNAL(timeout()), this, SLOT(selectNew()));
}

void ScreenSaverClass::setImageList(QStringList imgList)
{
    images = imgList;
    if(images.contains("")){
        qDebug() <<"Invalid image url found in list, removing.";

        int d = images.indexOf("");
        images.removeAt(d);
        qDebug() << "Bad Data removed from list at entry " << d;
    }
    if(!images.empty())
    {
        setImage(images.at(0));
        picChanger.start();
        qDebug() << "Screensaver starting";
        emit screenSaverReady();
    }
    else
    {
        setActive(false);
       qDebug("Screensaver image list error!");
       picChanger.stop();
    }
}

void ScreenSaverClass::clearImageList()
{
    images.clear();
}

void ScreenSaverClass::setImageData( QByteArray buffer)
{
//    qi_currentImage = QImage();
//    qi_currentImage.loadFromData(buffer);
  //  emit imageChanged();
    //buffer.clear();
}

void ScreenSaverClass::setActive(bool state)
{
    if(active == state){

    }else
    {
        if(state==true){
      //  picChanger.start(transitionDuration);
        } else {
     //   picChanger.stop();

        }
        active = state;
        emit activatedChanged();
    }

    if(active = true){

    }
    setStatus(state);
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
    connect(&picChanger, SIGNAL(timeout()), this, SLOT(selectNew()));
    picChanger.start(transitionDuration);
}

void ScreenSaverClass::selectNew()
{
    if(images.isEmpty())
    {
        return;
    }
    int index = images.lastIndexOf(currentImage);

    if (index!= images.count())
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
    if(!url.isEmpty()){
    currentImage =  url;
    qDebug() << url;   
    emit imageChanged();
 //  emit requestNewImage(currentImage);
    }
    else{
        currentImage="";
        selectNew();
    }
}
