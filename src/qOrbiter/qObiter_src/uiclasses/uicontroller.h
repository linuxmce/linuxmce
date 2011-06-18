#ifndef UICONTROLLER_H
#define UICONTROLLER_H
/*
  the purpose of this class is to be the interface for all of the c++ ui components.
  */
#include <QObject>
#include <uiclasses/lightingui.h>

class uiController : public QObject
{
    Q_OBJECT
public:
    explicit uiController(QObject *parent = 0);
    lightingUI * lightingRow;

signals:

public slots:

};

#endif // UICONTROLLER_H
