#ifndef ORBITERWINDOW_H
#define ORBITERWINDOW_H

#include <QObject>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QVariant>

class orbiterWindow : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QString message READ getMessage WRITE setMessage NOTIFY messageChanged)
public:
    explicit orbiterWindow(QObject *parent = 0);
    //public members

    QString message;
    QDeclarativeView  mainView;

    QString buildType;
    QString qrcPath;

    Q_INVOKABLE void forceResponse (QString forced);

signals:
    void messageChanged();

public slots:

    void setMessage(QString imsg);
    QString getMessage() {return message;}



};
#endif



