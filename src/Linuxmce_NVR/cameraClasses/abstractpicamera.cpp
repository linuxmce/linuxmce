#include "abstractpicamera.h"
#include "qurl.h"
#include "../managerClasses/NvrManager.h"
#include "qdatetime.h"
#include "qdebug.h"
NvrCameraBase::NvrCameraBase(QObject *parent)
{
    Q_UNUSED(parent);
}

NvrCameraBase::NvrCameraBase(QString cameraName, QString userName, QString password, quint16 port, quint16 control_port, QUrl url, int linuxmceId, QObject *parent) : QObject(parent)
{
    m_dceDeviceId=linuxmceId;
    m_constructed=false;
    setCameraName(cameraName);
    setUserName(userName);
    setPassWord(password);
    setPort(port);
    setControlPort(control_port);
    setUrl(url);

    if(userName.isEmpty())
        setUsingPass(false);
}



quint16 NvrCameraBase::port() const
{
    return m_port;
}

void NvrCameraBase::setPort(const quint16 &port)
{
    if(m_port==port) return;

    m_port = port; emit portChanged();
}
QUrl NvrCameraBase::Url() const
{
    return m_Url;
}

void NvrCameraBase::setUrl(QUrl Url)
{
    qDebug()<<Url.toString();
    if(m_Url==Url) return;

    QUrl  u = QUrl(Url);
    u.setPort(m_port);
    if(!passWord().isEmpty()){
        u.setUserName(userName());
        u.setPassword(passWord());
    } else {
        setUsingPass(false);
    }
    u.setPath("/");

    m_Url = u;emit urlChanged();
    log(Q_FUNC_INFO+m_Url.toString());
    setControlUrl(Url);
}
QString NvrCameraBase::userName() const
{
    return m_userName;
}

void NvrCameraBase::setUserName(const QString &userName)
{
    if(m_userName == userName)return;

    m_userName = userName;emit userNameChanged();
}
QString NvrCameraBase::passWord() const
{
    return m_passWord;
}

void NvrCameraBase::setPassWord(const QString &passWord)
{
    if(m_passWord==passWord) return;
    m_passWord = passWord; emit passWordChanged();
}
bool NvrCameraBase::isOnline() const
{
    return m_isOnline;
}

void NvrCameraBase::setIsOnline(bool isOnline)
{
    if(m_isOnline==isOnline)return;
    m_isOnline = isOnline; emit isOnlineChanged();
}
NvrManager *NvrCameraBase::manager() const
{
    return m_manager;
}

void NvrCameraBase::setManager(NvrManager *manager)
{
    if(m_manager==manager) return;
    m_manager = manager;

    int pr = manager->listener()->listenPort();

    setManagerPort(QString::number(pr));
    setManagerUrl(manager->listener()->currentHost());
    doConnections();

}

void NvrCameraBase::doConnections()
{
    log(QString(Q_FUNC_INFO)+cameraName()+ "::setting connections");
    m_constructed=true;
    emit initialized();

}
int NvrCameraBase::dceDeviceId() const
{
    return m_dceDeviceId;
}

void NvrCameraBase::setDceDeviceId(int dceDeviceId)
{
    m_dceDeviceId = dceDeviceId;
}

std::string NvrCameraBase::getImage()
{
    return QString("/tmp/please_wait.png").toStdString();
}

void NvrCameraBase::getImage(char **pD, int *dataLength)
{

}

QString NvrCameraBase::managerPort() const
{
    return m_managerPort;
}

void NvrCameraBase::setManagerPort(const QString &managerPort)
{
    m_managerPort = managerPort;
}

QString NvrCameraBase::managerUrl() const
{
    return m_managerUrl;
}

void NvrCameraBase::setManagerUrl(const QString &managerUrl)
{
    m_managerUrl = managerUrl;
}

bool NvrCameraBase::constructed() const
{
    return m_constructed;
}

void NvrCameraBase::setConstructed(bool constructed)
{
    m_constructed = constructed;
}

QUrl NvrCameraBase::controlUrl() const
{
    return m_controlUrl;
}

void NvrCameraBase::setControlUrl(const QUrl &controlUrl)
{

    if(m_controlUrl==controlUrl)return;

    QUrl  u = QUrl(controlUrl);
    u.setPort(m_controlPort);
    if(!passWord().isEmpty()){
        u.setUserName(userName());
        u.setPassword(passWord());
    } else {
        setUsingPass(false);
    }
    u.setPath("/");
    m_controlUrl = u;    emit controlUrlChanged();
    log(Q_FUNC_INFO+m_controlUrl.toString());

}

NvrCameraBase::CameraType NvrCameraBase::camType()
{
    return m_cameraType;
}

void NvrCameraBase::setCameraType(NvrCameraBase::CameraType t)
{
    if(m_cameraType==t) return;
    m_cameraName=t;
    emit cameraTypeChanged();
}

void NvrCameraBase::setAudioType(NvrCameraBase::AudioType a)
{
    if(m_audioType==a) return;
    m_audioType = a;
    emit audioTypeChanged();
}

bool NvrCameraBase::usingPass() const
{
    return m_usingPass;
}

void NvrCameraBase::setUsingPass(bool value)
{
    if(m_usingPass==value)return;
   m_usingPass = value; emit usingPasswordChanged();
}

quint16 NvrCameraBase::controlPort() const
{
    return m_controlPort;
}

void NvrCameraBase::setControlPort(const quint16 &controlPort)
{
    if(m_controlPort==controlPort) return;
    m_controlPort = controlPort; emit controlPortChanged();
}


void NvrCameraBase::log(QString msg)
{
    qDebug() <<QDateTime::currentDateTime().toLocalTime().toString() << msg;
}
QString NvrCameraBase::cameraName() const
{
    return m_cameraName;
}

void NvrCameraBase::setCameraName(const QString &cameraName)
{
    if(m_cameraName==cameraName)return;
    m_cameraName = cameraName; emit cameraNameChanged();
}








