#include "omxinterface.h"
#include "omxdbusplayerinterface.h"
#include "qdebug.h"

const QString OmxInterface::m_dbusName="org.mpris.MediaPlayer2.omxplayer";

OmxInterface::OmxInterface(QObject *parent) : QObject(parent)
{
    m_parentInterface = qobject_cast<omxdbusplayerinterface*>(parent);
    if(!m_parentInterface){
        qWarning() << "Something is very wrong";
    }

    interrupt = false;
    m_omxVolume=1.0;
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

        //  QMetaObject::invokeMethod(dbusOmxProperties, "Position", Qt::QueuedConnection);
    }
    if(m_omxConnected){
        QMetaObject::invokeMethod(dbusOmxProperties, "Position", Qt::QueuedConnection);
        QMetaObject::invokeMethod(dbusOmxProperties, "Duration", Qt::QueuedConnection);
        QMetaObject::invokeMethod(dbusOmxProperties, "PlaybackStatus", Qt::QueuedConnection);
    }

    if(m_omxConnected == omxConnected) return;

    if(!m_omxConnected && omxConnected){
        qDebug()<< Q_FUNC_INFO << "registering callbacks";
    }

    m_omxConnected = omxConnected;
    emit omxConnectedChanged(m_omxConnected);

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

    switch (s) {
    case QProcess::Running:
        qDebug() << Q_FUNC_INFO << dbusOmxPlayer->connection().interface()->registeredServiceNames().value();
        break;
    case QProcess::NotRunning:
        setPlaybackStatus("Stopped");
        if(m_omxProcess->exitCode() != 0){

            if(!interrupt){
                emit mediaEnded(true);
            } else {
                interrupt = false;
            }

        } else {

            if(!interrupt){
                emit mediaEnded(false);
            } else {//this from tapping an item on a playlist while its playing
                m_omxProcess->deleteLater();
                m_omxProcess = new QProcess();
                m_omxProcess->setProgram("/usr/bin/omxplayer");

                interrupt = false;
                runOmxWithFile(m_currentMediaLink);
            }
        }
        if(m_checkTimer !=-1){
            killTimer(m_checkTimer);
            m_checkTimer=-1;
        }
        break;
    default:
        if(m_checkTimer !=-1){
            killTimer(m_checkTimer);
            m_checkTimer=-1;
        }
        break;
    }

}
long OmxInterface::getPosition() const
{
    return m_position;
}

void OmxInterface::setPosition(qlonglong position)
{
    m_position = position;
    emit positionChanged(m_position);
}

void OmxInterface::pause()
{
    dbusOmxPlayer->Pause();
}

void OmxInterface::handleMute(bool mute)
{
    qDebug() << Q_FUNC_INFO;
    if(mute)
        dbusOmxProperties->Mute();
    else
        dbusOmxProperties->Unmute();

}

void OmxInterface::handleVolumeUp()
{
    m_omxVolume+=0.1;
    dbusOmxProperties->Volume(m_omxVolume);
}

void OmxInterface::handleVolumeDown()
{
    m_omxVolume-=0.1;
    dbusOmxProperties->Volume(m_omxVolume);
}

void OmxInterface::requestVolume()
{
    dbusOmxProperties->Volume();
}

long OmxInterface::duration() const
{
    return m_duration;
}

void OmxInterface::setDuration(qlonglong duration)
{
    m_duration = duration;
    emit durationChanged(m_duration);
}

QString OmxInterface::getPlaybackStatus() const
{
    return m_playbackStatus;
}

void OmxInterface::setPlaybackStatus(const QString &playbackStatus)
{
    if(m_playbackStatus == playbackStatus) return;

    if(m_playbackStatus == "Stopped" && playbackStatus == "Playing"){
        dbusOmxProperties->Identity();
    }

    m_playbackStatus = playbackStatus;

    if(m_playbackStatus.contains("The name org.mpris.MediaPlayer2.omxplayer was not provided by any .service files")){
        m_omxProcess->write("q");

        m_omxProcess->terminate();
    }
    emit playbackStatusChanged(m_playbackStatus);
    qDebug() << Q_FUNC_INFO << m_playbackStatus;
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
    m_omxProcess->setArguments(QStringList() << "-b " << "--no-osd");
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
    connect(dbusOmxProperties, &OmxDbusProxy::volumeChanged, this, &OmxInterface::setOmxVolume);

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

        if(
                m_omxProcess->state()== QProcess::Running ||
                m_omxProcess->state() == QProcess::Starting
                ){
            qDebug() << "Process running ";
            interrupt = true;
            m_currentMediaLink = file;
            dbusOmxPlayer->StopWithConfirmation();
            m_omxProcess->write("q");
            handleOmxProcessChanged(QProcess::NotRunning);
            return;
        } else {
            m_omxProcess->arguments().clear();

            m_omxProcess->setArguments(QStringList()<<"-b" << "--no-osd" <<file);
            qDebug() << m_omxProcess->arguments();
            m_omxProcess->start();
            m_checkTimer = startTimer(1500);
            m_currentMediaLink = file;
        }
    }
}

void OmxInterface::stopPlayer()
{
    qDebug() << Q_FUNC_INFO;
    dbusOmxPlayer->Stop();
    killTimer(m_checkTimer);
    m_checkTimer = -1;
    m_currentMediaLink = "";
}

void OmxInterface::seekToPosition(int position)
{
    int ct = m_parentInterface->currentPositionNumeric();
    int tt = position *1000;
  //  qDebug() << Q_FUNC_INFO << "Current Position " << ct;
  //  qDebug() << Q_FUNC_INFO << "Seek target:"<<tt;

    qlonglong delta =(tt - ct)*1000;
  // qDebug() << "Delta seek value " << delta;
    dbusOmxPlayer->Seek(delta);
}

void OmxInterface::handleTimecodeTick(qlonglong tick)
{
    qDebug() << Q_FUNC_INFO << tick;
}

void OmxInterface::handleStateChanged(QString s, QDBusVariant v)
{
    qDebug() << Q_FUNC_INFO << s;
}

void OmxInterface::setOmxVolume(double vol)
{
    m_omxVolume = vol;
    emit volumeChanged(m_omxVolume);
}

void OmxInterface::handleMediaStopped()
{
    qDebug() << "Process ended , current media status is " << getPlaybackStatus();
    if(m_omxProcess->state() == QProcess::NotRunning ){

    }
}



