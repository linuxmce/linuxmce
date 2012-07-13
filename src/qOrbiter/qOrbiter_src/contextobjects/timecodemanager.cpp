#include "timecodemanager.h"
#include "PlutoUtils/StringUtils.h"
#include <QStringList>
#include <QMetaType>
#ifdef debug
#include <QDebug>
#endif
#include <QMetaProperty>

#ifdef QT5
TimeCodeManager::TimeCodeManager(QQuickItem *parent) :
    QQuickItem(parent)
#else
TimeCodeManager::TimeCodeManager(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
#endif
{

    dceMediaSocket = new QTcpSocket();
    playbackSpeed = 0;
    tcCurrentTime = 0;
    tcTotalTime = 0;
    socketData = NULL;
    mediaPlayerIp = "";
    qDebug("Setup Timecode Manager");
}

QString TimeCodeManager::ObjectToString(const QObject *obj)
{
        //setup list rof results
        QStringList result;
       const QMetaObject * meta = obj->metaObject();
       result += QString("class %1 : public %2 {")
               .arg(meta->className())
               .arg(meta->superClass()->className());
       for (int i=0; i < meta->propertyCount(); ++i)
       {
           const QMetaProperty qmp = meta->property(i);
           QVariant val = obj->property(qmp.name());
           result += QString(" %1 %2 = %3")
                   .arg(qmp.typeName())
                   .arg(qmp.name())
                   .arg(val.toString()) ;
       }
       result +="};";
       return result.join("\n");
}

void TimeCodeManager::start(QString server, int iport)
{
    qDebug("Starting timecode...");
    port = iport;
    mediaPlayerIp = server;

    if(!dceMediaSocket->isOpen())
    {
         qDebug() <<"opening connection to " << port;

        dceMediaSocket->connectToHost(mediaPlayerIp, port, QFile::ReadOnly );
        if ( dceMediaSocket->isValid() )
        {
            // setDceResponse("Time Code Socket connected! " + QString::fromStdString(qs_routerip.toStdString()));
            QObject::connect(dceMediaSocket,SIGNAL(readyRead()), this, SLOT(updateTimeCode()));
        }
        else
        {   //setDceResponse("couldnt start timecode");
             qDebug() << dceMediaSocket->errorString();
        }
    }
    else
    {
        qDebug("Time code running, moving on.");
    }
}

void TimeCodeManager::stop()
{
    setTotalTime(0);
    setStringTotalTime("0");
    setTime(0);
    setStringTime("0");
    setSpeed(0);
    dceMediaSocket->close();
}

void TimeCodeManager::restart()
{
    setTotalTime(0);
    setTime(0);
    setSpeed(0);
    dceMediaSocket->close();
}

void TimeCodeManager::updateTimeCode()
{
    socketData = dceMediaSocket->readLine();
    //qDebug() << socketData;
    std::string sLine = QString::fromAscii(socketData.data(), socketData.size()).toStdString();
   //qDebug() << QString::fromStdString(sLine);
    if (sLine.length() > 0)
    {
        std::string::size_type pos=0;
        int iSpeed = atoi(StringUtils::Tokenize( sLine,",",pos ).c_str());
        std::string m_sTime = StringUtils::Tokenize( sLine,",",pos );
        std::string m_sTotalTime = StringUtils::Tokenize( sLine,",",pos );
        int iStreamID = atoi(StringUtils::Tokenize( sLine,",",pos ).c_str());
        std::string sTitle = StringUtils::Tokenize( sLine,",",pos );
        std::string sChapter = StringUtils::Tokenize( sLine,",",pos );

        //in case of a dvd or other media supporting chapters and such - still need Q_Property accessors
        qsChapter = QString::fromStdString(sChapter);
        qsTitle = QString::fromStdString(sTitle);
        //-----------------------------------------------------------/

        // Strip fractions of a second
        if( (pos=m_sTime.find('.'))!=string::npos )
            m_sTime = m_sTime.substr(0,pos);

        if( (pos=m_sTotalTime.find('.'))!=string::npos )
            m_sTotalTime = m_sTotalTime.substr(0,pos);

        if (qsTotalTime != QString::fromStdString(m_sTotalTime))
        {
            setStringTotalTime(QString::fromStdString(m_sTotalTime));
            convertToSeconds();
        }

        setStringTime(QString::fromStdString(m_sTime));

        std::string sSpeed;
        if( iSpeed!=1000 ) // normal playback
        {
            if( iSpeed>=1000 || iSpeed<=-1000 )
                sSpeed = StringUtils::itos( iSpeed / 1000 ) + "x";  // A normal speed
            else if( iSpeed==250 )
                sSpeed = "1/4x";
            else if( iSpeed==-250 )
                sSpeed = "-1/4x";
            else if( iSpeed==500 )
                sSpeed = "1/2x";
            else if( iSpeed==-500 )
                sSpeed = "-1/2x";
            else
                sSpeed = "." + StringUtils::itos(iSpeed) + "x";
        }
        setSpeed(iSpeed);
    }
}

void TimeCodeManager::convertToSeconds()
{

    QStringList times = qsTotalTime.split(":");
    int hoursToSec = times.at(0).toInt() * 3600 ;
    int minuteToSec = (times.at(1).toInt() * 60);
    int seconds = times.at(2).toInt();
    int totalSeconds = hoursToSec + minuteToSec + seconds;
    setTotalTime(totalSeconds);
}

void TimeCodeManager::setPosition()
{
    QStringList current = qsCurrentTime.split(":");
    int hoursToSec = current.at(0).toInt() * 3600 ;
    int minuteToSec = (current.at(1).toInt() * 60);
    int seconds = current.at(2).toInt();
    int currentPosition = hoursToSec + minuteToSec + seconds;

    setTime(currentPosition);
    setProgressBar((currentPosition));
}

void TimeCodeManager::showDragTime(int seconds)
{
    int displayHours = seconds / (3600);
    int remainder = seconds % 3600;
    int minutes = remainder / 60;
    int forseconds = remainder % 60;
    i_dragTime = seconds;
    if (i_dragTime < tcCurrentTime)
    {
        reverse = true;
    }
    else
    {
        reverse = false;
    }
    setDragTime(QString::number(displayHours)+":"+QString::number(minutes)+":"+QString::number(forseconds) );

}
