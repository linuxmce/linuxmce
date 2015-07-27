#ifndef PIMANAGER_H
#define PIMANAGER_H

#include <QObject>
#include "qlist.h"
#include "eventlistener.h"
class AbstractNvrCamera;

class NvrManager : public QObject
{
    Q_OBJECT
public:
    explicit NvrManager(QObject *parent = 0);

    int cameraCount();
signals:
    void cameraCountChanged();
    void cameraAdded(QString cameraName);
    void cameraRemoved(QString cameraName);
    void cameraFailed(QString cameraName);


public slots:
 void addCamera(AbstractNvrCamera* c);
void shutDown();
private:
    void log(QString logMsg);



private:
    QList<AbstractNvrCamera*> cam_list;
    MotionEventListener *listener;

};

#endif // PIMANAGER_H
