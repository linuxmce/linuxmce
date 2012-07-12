

#include <QPalette>
#if (QT_VERSION >= 0x050000)
#include <QtQuick/QQuickView>
#include <QQmlContext>
#else
#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/QDeclarativeContext>
#endif

#include "threadedsplash.h"



ThreadedSplash::ThreadedSplash(QObject *parent)
{

    qs_progress="Loading...";

         #ifdef for_desktop

             qrcPath = QUrl("qrc:desktop/Splash.qml");
    #elif defined (for_freemantle)

             qrcPath = "qrc:freemantle/Splash.qml";
         #elif defined (for_harmattan)

             qrcPath = "qrc:harmattan/Splash.qml";
         #elif defined (Q_OS_MACX)

             qrcPath = "qrc:osx/Splash.qml";
         #elif defined (__ANDROID__)

             qrcPath = "qrc:android-phone/Splash.qml";
         #elif defined (for_droid)

             qrcPath = "qrc:android-tablet/Splash.qml";
         #endif

     QPalette palette;
     palette.setColor(QPalette::Base, Qt::transparent);

#if (QT_VERSION >= 0x050000)
     splashView = new QQuickView; //qdeclarativeview for splash.qml
#else
     splashView = new QDeclarativeView; //qdeclarativeview for splash.qml
     splashView->setAttribute(Qt::WA_TranslucentBackground, true);     //doing the same for qdeclarativeview
#endif

     splashView->rootContext()->setContextProperty("dcemessage", qs_progress); //sets connection to messaging variable in splash window

     splashView->setSource((qrcPath));        //using the QRC splash as a kickstart

 #ifdef Q_OS_SYMBIAN
     splashView->showFullScreen();
 #elif defined(Q_WS_MAEMO_5)
     splashView->showMaximized();
 #elif defined(for_harmattan)
     splashView->showFullScreen();
 #elif defined(for_desktop)
     splashView->show();
 #else
     splashView->show();
#endif

     setMessage("Splash Loaded");
}

void ThreadedSplash::setMessage(QString s)
{
    qs_progress = s;
     emit qsProgress();
    //qDebug() << "Cross thread message:" << qs_progress;

}

QString ThreadedSplash::getMessage()
{
    return qs_progress;
}

void ThreadedSplash::setProgess(int p)
{
    mi_progress = p;
    emit iProgress();
}

int ThreadedSplash::getProgress()
{
    return mi_progress;
}

void ThreadedSplash::closeSplash(bool)
{
    //qDebug("Recieved command to close!");
    splashView->close();
}

void ThreadedSplash::raiseSplash()
{

    //qDebug("Recieved command to open!");
#ifdef Q_OS_SYMBIAN
    splashView->showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    splashView->showMaximized();
#elif defined(for_harmattan)
    splashView->showFullScreen();
#elif defined(for_desktop)
    splashView->show();
#else
    splashView->show();
#endif
}



