#ifndef QTLIGHTSWITCH_H
#define QTLIGHTSWITCH_H


#include <QObject>


class QTlightSwitch : public QObject
{
    Q_OBJECT
    Q_PROPERTY( NOTIFY DCEon)
    Q_PROPERTY( NOTIFY DCEoff)
    Q_PROPERTY( NOTIFY StatusChanged)

public:
    explicit QTlightSwitch(QObject *parent = 0);

signals:
    //signals to pass from the qt side to the dce side
void DceOff();
void DceOn();
void statusChanged();

public slots:
    //implementing dce commands as slots since they will be
    //recieved. We can then pass the parameters to the qml obect

    Q_INVOKABLE  void dce_on();
    Q_INVOKABLE  void dce_off();
    Q_INVOKABLE  void dce_setLevel(int);

    bool update_deviceStatus(QString);

};

#endif // QTLIGHTSWITCH_H
