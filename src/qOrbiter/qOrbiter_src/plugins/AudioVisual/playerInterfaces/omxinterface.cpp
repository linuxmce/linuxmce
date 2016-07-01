#include "omxinterface.h"
#include "qdebug.h"

const QString OmxInterface::m_dbusName="org.mpris.MediaPlayer2.omxplayer";

OmxInterface::OmxInterface(QObject *parent) : QObject(parent)
{

    dbusOmxPlayer = NULL;
    dbusOmxProperties = NULL;
    dbusOmxRoot = NULL;
    m_omxProcess = NULL;
    qDebug() << Q_FUNC_INFO;

    if(getOmxDbusInfo())
        doSetup();

}

QString OmxInterface::dbusAddress() const
{
    return m_dbusAddress;
}

void OmxInterface::setDbusAddress(const QString &dbusAddress)
{
    if(m_dbusAddress == dbusAddress) return;
    m_dbusAddress = dbusAddress;
    emit dbusAddressChanged();
}

bool OmxInterface::omxConnected() const
{
    return m_omxConnected;
}

void OmxInterface::setOmxConnected(bool omxConnected)
{


    if(m_omxConnected && omxConnected){
        QMetaObject::invokeMethod(dbusOmxProperties, "PlaybackStatus", Qt::QueuedConnection);
      //  QMetaObject::invokeMethod(dbusOmxProperties, "Position", Qt::QueuedConnection);
    }

    if(m_omxConnected == omxConnected) return;

    if(!m_omxConnected && omxConnected){
        qDebug()<< Q_FUNC_INFO << "registering callbacks";
        QMetaObject::invokeMethod(dbusOmxProperties, "Duration", Qt::QueuedConnection);
    }

    if(m_omxConnected){
        dbusOmxProperties->Position();
    }

    m_omxConnected = omxConnected;
    emit omxConnectedChanged();

}

QString OmxInterface::dbusOmxPid() const
{
    return m_dbusOmxPid;
}

void OmxInterface::setDbusOmxPid(const QString &dbusOmxPid)
{
    if(m_dbusOmxPid == dbusOmxPid) return;
    m_dbusOmxPid = dbusOmxPid;
    emit dbusPidChanged();
}

void OmxInterface::handleOmxProcessChanged(QProcess::ProcessState s)
{
    qDebug() << Q_FUNC_INFO << s;
    if(s == QProcess::Running){
          qDebug() << Q_FUNC_INFO << dbusOmxPlayer->connection().interface()->registeredServiceNames().value();
    }

}
long OmxInterface::getPosition() const
{
    return m_position;
}

void OmxInterface::setPosition(qlonglong position)
{
    m_position = position;
    emit positionChanged(m_position/1000);
}

void OmxInterface::pause()
{
    dbusOmxPlayer->Pause();
}

long OmxInterface::duration() const
{
    return m_duration;
}

void OmxInterface::setDuration(qlonglong duration)
{
    m_duration = duration;
    emit durationChanged();
}

QString OmxInterface::getPlaybackStatus() const
{
    return m_playbackStatus;
}

void OmxInterface::setPlaybackStatus(const QString &playbackStatus)
{
    m_playbackStatus = playbackStatus;
    emit playbackStatusChanged();
}


void OmxInterface::timerEvent(QTimerEvent *event)
{
    if(!dbusOmxPlayer)
        return;

    Q_UNUSED(event);
   // qDebug() << Q_FUNC_INFO << dbusOmxProperties->connection().interface()->registeredServiceNames().value();
    setOmxConnected(dbusOmxPlayer->connection().isConnected());

}

bool OmxInterface::getOmxDbusInfo()
{
    QFile tempAddr("/tmp/omxplayerdbus.pi");
    QFile tempPid("/tmp/omxplayerdbus.pi.pid");

    if(tempAddr.exists() && tempAddr.open(QFile::ReadOnly)){
        QString data = tempAddr.readAll();
        setDbusAddress(data.trimmed());
    } else {
       runOmxOnce();
       getOmxDbusInfo();
       return true;
    }

    if(tempPid.exists() && tempPid.open(QFile::ReadOnly)){
        QString pid = tempPid.readAll();
        setDbusOmxPid(pid.trimmed());
    } else{
      //  return false;
    }




//    qDebug() << Q_FUNC_INFO;
//    QProcess dbusStart;
//    dbusStart.setProcessChannelMode(QProcess::MergedChannels);
//    dbusStart.start("dbus-launch", QStringList()<< "--auto-syntax" );
//    QString sessionInformation;
//    while(dbusStart.waitForFinished(10000)){
//           sessionInformation.append(dbusStart.readAll());
//    }

//    QString dsa = sessionInformation.mid(sessionInformation.indexOf("DBUS_SESSION_BUS_ADDRESS='"), sessionInformation.indexOf("';")-sessionInformation.indexOf("DBUS_SESSION_BUS_ADDRESS=")).remove(";").remove("DBUS_SESSION_BUS_ADDRESS=").simplified().remove("'");
//    QString dpid = sessionInformation.mid(sessionInformation.indexOf("DBUS_SESSION_BUS_PID="), sessionInformation.length()-sessionInformation.indexOf("DBUS_SESSION_BUS_PID=")).remove(";").remove("DBUS_SESSION_BUS_PID=").simplified();
//    qDebug() << "Session pid?" << dpid;
//    qDebug() << " session addr " << dsa;

  //  setDbusOmxPid(dpid);
   // setDbusAddress(dsa);




    m_omxProcess = new QProcess();
    m_omxProcess->setProgram("/usr/bin/omxplayer");
    connect(m_omxProcess, &QProcess::stateChanged , this, &OmxInterface::handleOmxProcessChanged);

    return true;
}

void OmxInterface::doSetup()
{
    qDebug() << Q_FUNC_INFO;
    setenv("DBUS_SESSION_BUS_ADDRESS", dbusAddress().toLocal8Bit(),0);
    setenv("DBUS_SESSION_BUS_PID", dbusOmxPid().toLocal8Bit(), 0);
    qDebug() << "OMX_DBUS_SESSION_BUS_ADDRESS::" << dbusAddress();
    qDebug() << "OMX_DBUS_SESSION_BUS_PID::" << dbusOmxPid();

    dbusOmxPlayer = new OmxDbusProxyPlayer(m_dbusName, "/org/mpris/MediaPlayer2", QDBusConnection::sessionBus(), this);
    dbusOmxProperties = new OmxDbusProxy(m_dbusName, "/org/mpris/MediaPlayer2", QDBusConnection::sessionBus(), this);
    dbusOmxRoot = new OmxDbusProxyRoot(m_dbusName, "/org/mpris/MediaPlayer2", QDBusConnection::sessionBus(), this);

    connect(dbusOmxProperties, &OmxDbusProxy::playbackStatusChanged, this, &OmxInterface::setPlaybackStatus);
    connect(dbusOmxProperties, &OmxDbusProxy::positionChanged, this, &OmxInterface::setPosition);
    connect(dbusOmxProperties, &OmxDbusProxy::durationChanged, this, &OmxInterface::setDuration);

    qDebug() <<  " Omx is setup";
    qDebug() << Q_FUNC_INFO << dbusOmxPlayer->connection().interface()->registeredServiceNames().value();
}

void OmxInterface::runOmxOnce()
{
    if(m_omxProcess){
        if(m_omxProcess->state()==QProcess::Running){
            return;
        } else {
            m_omxProcess->start();
        }
    }

}

void OmxInterface::runOmxWithFile(QString file)
{
    qDebug() << Q_FUNC_INFO;
    if(m_omxProcess){
        if(m_omxProcess->state()==QProcess::Running){
            return;
        } else {

            qDebug() << Q_FUNC_INFO << "arg " << file;
            m_omxProcess->setArguments(QStringList()<<file);
            m_omxProcess->start();
            startTimer(1000);

        }
    }
}

void OmxInterface::stopPlayer()
{
    qDebug() << Q_FUNC_INFO;   
    dbusOmxPlayer->Stop();
}

void OmxInterface::seekToPosition(int position)
{
    dbusOmxPlayer->SetPosition("", position*1000);
}

void OmxInterface::handleTimecodeTick(qlonglong tick)
{
    qDebug() << Q_FUNC_INFO << tick;
}

void OmxInterface::handleStateChanged(QString s, QDBusVariant v)
{
    qDebug() << Q_FUNC_INFO << s;
}



