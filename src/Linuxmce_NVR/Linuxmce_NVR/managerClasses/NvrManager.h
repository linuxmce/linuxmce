#ifndef PIMANAGER_H
#define PIMANAGER_H

#include <QObject>
#include "qlist.h"
#include "eventlistener.h"
#include "Linuxmce_NVR.h"
class AbstractNvrCamera;

class NvrManager : public QObject
{
    Q_OBJECT
public:
    explicit NvrManager(DCE::Linuxmce_NVR *dceObject, QObject *parent = 0);

    int cameraCount();
signals:
    void cameraCountChanged();
    void cameraAdded(QString cameraName);
    void cameraRemoved(QString cameraName);
    void cameraFailed(QString cameraName);


public slots:
    void addCamera(AbstractNvrCamera* c);
    void shutDown();
    void startUp(int listenPort);


private:
    void log(QString logMsg);

    bool verbose; //turn on to enable qDebug messages


private:
    QList<AbstractNvrCamera*> cam_list;
    MotionEventListener *listener;
    DCE::Linuxmce_NVR *mp_Linuxmce_NVR;

};

#endif // PIMANAGER_H
