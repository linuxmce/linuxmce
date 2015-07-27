#ifndef ABSTRACTPICAMERA_H
#define ABSTRACTPICAMERA_H

#include <QObject>
#include "qurl.h"
class NvrManager;
/*!
 * \brief The AbstractPiCamera class. Base object for different version of the camera to use
 *
 */
class AbstractNvrCamera : public QObject
{
    Q_OBJECT
public:
    explicit AbstractNvrCamera(QString cameraName,
                              QString userName,
                              QString password,
                              quint16 port,
                              quint16 control_port,
                              QUrl url,
                              QObject *parent = 0);
    enum CameraType{
        PI_VLC_STREAM,
        PI_MOTION_STREAM,
        PI_JPEG_STREAM
    };
    enum AudioType{
        NO_AUDIO,
        STREAM_AUDIO,
        FILE_AUIDO
    };

    quint16 port() const;
    void setPort(const quint16 &port);

    QUrl Url() const;
    void setUrl(QUrl Url);

    QString userName() const;
    void setUserName(const QString &userName);

    QString passWord() const;
    void setPassWord(const QString &passWord);

    bool isOnline() const;
    void setIsOnline(bool isOnline);

    NvrManager *manager() const;
    void setManager(NvrManager *manager);

    QString cameraName() const;
    void setCameraName(const QString &cameraName);

    quint16 controlPort() const;
    void setControlPort(const quint16 &controlPort);

    bool usingPass() const;
    void setUsingPass(bool value);

    QUrl controlUrl() const;
    void setControlUrl(const QUrl &controlUrl);

    CameraType camType();
signals:
    void cameraNameChanged();
    void cameraTypeChanged();
    void audioTypeChanged();
    void passWordChanged();
   void userNameChanged();
   void portChanged();
   void controlPortChanged();
   void urlChanged();
   void controlUrlChanged();
   void usingPasswordChanged();
   void isOnlineChanged();
   void initialized();

public slots:
    virtual void setCameraType(CameraType t)= 0;
    virtual void setAudioType(AudioType a)=0;

protected:
    CameraType m_cameraType;
    AudioType m_audioType;
void log(QString msg);
private:
    void doConnections();


private:
    quint16 m_port;
    quint16 m_controlPort;
    QString m_cameraName;
    QUrl m_Url;
    QUrl m_controlUrl;
    QString m_userName;
    QString m_passWord;
    bool m_isOnline;
    NvrManager *m_manager;
    bool m_usingPass;
};

#endif // ABSTRACTPICAMERA_H
