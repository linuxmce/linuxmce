#ifndef PIMANAGER_H
#define PIMANAGER_H

#include <QObject>
#include "qlist.h"
#include "eventlistener.h"
#include "../Linuxmce_NVR.h"
class NvrCameraBase;

class NvrManager : public QObject
{
    Q_OBJECT
public:
    explicit NvrManager( QObject *parent = 0);
    NvrManager(int listenPort, QObject*parent=0);
    int cameraCount();

   // MotionEventListener *listener() const;
   // void setListener(MotionEventListener *listener);

signals:
    void cameraCountChanged();
    void cameraAdded(QString cameraName);
    void cameraRemoved(QString cameraName);
    void cameraFailed(QString cameraName);
    void newManagerMessage(QString msg);



public slots:
    void addCamera(NvrCameraBase* c);
    void shutDown();
    void startUp(int listenPort);
    string getImageFrame(int camId);

    void setDetectionStatus(bool enabled, int camId);


private:
    void log(QString logMsg);

    bool verbose; //turn on to enable qDebug messages


private:

    //MotionEventListener *m_listener;
    DCE::Linuxmce_NVR *mp_Linuxmce_NVR;
    QList<NvrCameraBase*> cam_list;
};

#endif // PIMANAGER_H
