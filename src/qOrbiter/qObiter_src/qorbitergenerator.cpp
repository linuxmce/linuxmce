#include "qorbitergenerator.h"
#include <QDebug>
#include <QTime>

qOrbiterGenerator::qOrbiterGenerator(int piDevice, int piRoom , int piUser , QWidget *parent) :
    QWidget(parent)
{
    //attempting to create a splash screen while its working
    splashScreen = new QDeclarativeView;
    splashScreen->setSource(QUrl::fromLocalFile("qml/qObiter_src/Splash.qml"));
    splashScreen->activateWindow();
    qDebug() << splashScreen->status();

}

bool qOrbiterGenerator::initializeRegen()
{


    QTime dieTime = QTime::currentTime().addSecs(2);
    while( QTime::currentTime() < dieTime )
    {
        //qDebug() << "Waiting for:" << QTime::currentTime() ; just a timer to test things

    }
   splashScreen->close();
    delete splashScreen;
    return true;
}
