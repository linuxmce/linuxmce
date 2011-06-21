#ifndef QORBITERMANAGER_H
#define QORBITERMANAGER_H

#include <QWidget>
#include <QDeclarativeView>
#include <qdeclarativecontext.h>
#include <QDeclarativeItem>

class qorbiterManager : public QWidget
{
    Q_OBJECT
public:
    explicit qorbiterManager(QWidget *parent = 0);
    QDeclarativeView  *qorbiterUIwin;               //Qml declarativeview
    QObject *item;

    bool gotoQScreen(QString s) ;

signals:

public slots:

};

#endif // QORBITERMANAGER_H
