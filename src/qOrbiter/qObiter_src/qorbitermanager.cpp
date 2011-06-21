#include "qorbitermanager.h"

qorbiterManager::qorbiterManager(QWidget *parent) :
    QWidget(parent)
{

    this->qorbiterUIwin = new QDeclarativeView;
    qorbiterUIwin->rootContext()->setContextProperty("currentDateTime", QDateTime::currentDateTime());
    qorbiterUIwin->setSource(QUrl::fromLocalFile("qml/qObiter_src/main.qml"));
    QObject *item= qorbiterUIwin->rootObject();
    //QObject::connect(item,SIGNAL(swapStyle()), this,SLOT(swapSkins()));
    gotoQScreen("Screen_1.qml");
    qorbiterUIwin->show();
}

bool qorbiterManager::gotoQScreen(QString s)
{
    QVariant screenname= s;
    QVariant returnedValue ="null";
  QObject *item = qorbiterUIwin->rootObject();
    if (QMetaObject::invokeMethod(item, "screenchange",  Q_ARG(QVariant, screenname)))
       { qDebug() << returnedValue; return true;}
                    else
        {qDebug () << "Failure!" << qPrintable(s) <<": " << returnedValue; return false;}


}
