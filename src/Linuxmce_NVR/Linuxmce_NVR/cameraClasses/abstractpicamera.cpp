#include "abstractpicamera.h"
#include "qurl.h"
#include "../managerClasses/NvrManager.h"
#include "qdatetime.h"
#include "qdebug.h"
AbstractNvrCamera::AbstractNvrCamera(QString cameraName, QString userName, QString password, quint16 port, quint16 control_port, QUrl url, QObject *parent) : QObject(parent)
{
    setCameraName(cameraName);
    setUserName(userName);
    setPassWord(password);
    setPort(port);
    setControlPort(control_port);
    setUrl(url);

    if(userName.isEmpty())
        setUsingPass(false);
}
quint16 AbstractNvrCamera::port() const
{
    return m_port;
}

void AbstractNvrCamera::setPort(const quint16 &port)
{
    if(m_port==port) return;

    m_port = port; emit portChanged();
}
QUrl AbstractNvrCamera::Url() const
{
    return m_Url;
}

void AbstractNvrCamera::setUrl(QUrl Url)
{
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
QString AbstractNvrCamera::userName() const
{
    return m_userName;
}

void AbstractNvrCamera::setUserName(const QString &userName)
{
    if(m_userName == userName)return;

    m_userName = userName;emit userNameChanged();
}
QString AbstractNvrCamera::passWord() const
{
    return m_passWord;
}

void AbstractNvrCamera::setPassWord(const QString &passWord)
{
    if(m_passWord==passWord) return;
    m_passWord = passWord; emit passWordChanged();
}
bool AbstractNvrCamera::isOnline() const
{
    return m_isOnline;
}

void AbstractNvrCamera::setIsOnline(bool isOnline)
{
    if(m_isOnline==isOnline)return;
    m_isOnline = isOnline; emit isOnlineChanged();
}
NvrManager *AbstractNvrCamera::manager() const
{
    return m_manager;
}

void AbstractNvrCamera::setManager(NvrManager *manager)
{
    if(m_manager==manager) return;
    m_manager = manager;
    doConnections();

}

void AbstractNvrCamera::doConnections()
{
    log(QString(Q_FUNC_INFO)+cameraName()+ "::setting connections");
    emit initialized();

}
QUrl AbstractNvrCamera::controlUrl() const
{
    return m_controlUrl;
}

void AbstractNvrCamera::setControlUrl(const QUrl &controlUrl)
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

AbstractNvrCamera::CameraType AbstractNvrCamera::camType()
{
    return m_cameraType;
}

bool AbstractNvrCamera::usingPass() const
{
    return m_usingPass;
}

void AbstractNvrCamera::setUsingPass(bool value)
{
    if(m_usingPass==value)return;
   m_usingPass = value; emit usingPasswordChanged();
}

quint16 AbstractNvrCamera::controlPort() const
{
    return m_controlPort;
}

void AbstractNvrCamera::setControlPort(const quint16 &controlPort)
{
    if(m_controlPort==controlPort) return;
    m_controlPort = controlPort; emit controlPortChanged();
}


void AbstractNvrCamera::log(QString msg)
{
    qDebug() <<QDateTime::currentDateTime().toLocalTime().toString() << msg;
}
QString AbstractNvrCamera::cameraName() const
{
    return m_cameraName;
}

void AbstractNvrCamera::setCameraName(const QString &cameraName)
{
    if(m_cameraName==cameraName)return;
    m_cameraName = cameraName; emit cameraNameChanged();
}








