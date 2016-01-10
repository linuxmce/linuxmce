#ifndef HTTPSNAPSHOTCAMERA_H
#define HTTPSNAPSHOTCAMERA_H
#include "abstractpicamera.h"
#include "qnetworkaccessmanager.h"

class HttpSnapshotCamera : public NvrCameraBase
{
    Q_OBJECT
public:
      explicit HttpSnapshotCamera(
            QString cameraName,
            QString userName,
            QString password,
            quint16 port,
            quint16 control_port,
            QUrl url,
            int dceId,
            QString camPath,
            CameraType t=STREAM_TYPE_JPEG_SNAP ,
            AudioType a =NO_AUDIO,
            QObject *parent=0);

signals:
    void newImage();
    void motionDetected(bool detected);

public slots:
    std::string getImage();

private slots:
    bool checkMotionStatus();

private:
    QNetworkAccessManager *httpManager;
    QByteArray mp_buffer;
};

#endif // HTTPSNAPSHOTCAMERA_H
