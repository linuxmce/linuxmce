#ifndef ABSTRACTPICAMERA_H
#define ABSTRACTPICAMERA_H

#include <QObject>
#include "qurl.h"
class NvrManager;
/*!
 * \brief The NvrCameraBase class. Base object for different version of the camera to use
 *
 */
class NvrCameraBase : public QObject
{
    Q_OBJECT
public:
    explicit NvrCameraBase(QObject*parent=0);
     NvrCameraBase(QString cameraName,
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

    void setCameraType(CameraType t) ;
    CameraType camType();


    void setAudioType(AudioType a) ;


    bool constructed() const;
    void setConstructed(bool constructed);

    QString managerUrl() const;
    void setManagerUrl(const QString &managerUrl);

    QString managerPort() const;
    void setManagerPort(const QString &managerPort);

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
    bool m_constructed;
    QString m_managerUrl;
    QString m_managerPort;
};
//Q_DECLARE_METATYPE(NvrCameraBase)

#endif // ABSTRACTPICAMERA_H
