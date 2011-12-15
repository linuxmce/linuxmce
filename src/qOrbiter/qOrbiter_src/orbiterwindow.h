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
    void setupLmce(QString device, QString routerIp);

public slots:
    void qmlSetupLmce(QString device, QString ip);
    void setMessage(QString imsg);
    QString getMessage();



};
#endif



