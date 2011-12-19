#ifndef ORBITERWINDOW_H
#define ORBITERWINDOW_H

#include <QObject>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QVariant>

class orbiterWindow : public QObject
{
    Q_OBJECT

    Q_PROPERTY (QString message READ getMessage WRITE setMessage NOTIFY MessageChanged)
    Q_PROPERTY (bool newOrbiter READ getOrbiterState WRITE setOrbiterState NOTIFY StatusChanged)
public:
    explicit orbiterWindow(QObject *parent = 0);
    //public members

    QString message;
    QDeclarativeView  mainView;

    QString buildType;
    QString qrcPath;

    Q_INVOKABLE void forceResponse (QString forced);

    bool newOrbiter;


signals:
    void MessageChanged();
    void setupLmce(QString device, QString routerIp);
    void StatusChanged();

public slots:
    void qmlSetupLmce(QString device, QString ip);
    void setMessage(QString imsg);
    QString getMessage();

    void setOrbiterState(bool state);
    bool getOrbiterState();


};
#endif



