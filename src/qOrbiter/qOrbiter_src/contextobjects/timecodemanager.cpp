#include "timecodemanager.h"


TimeCodeManager::TimeCodeManager(QObject *parent) :
    QObject(parent)
{
    dceMediaSocket = new QTcpSocket();
    playbackSpeed = 0;
    tcCurrentTime = 0;
    tcTotalTime = 0;
    socketData = NULL;
    mediaPlayerIp = "";
}

void TimeCodeManager::start(QString server, int iport)
{
    port = iport;
    mediaPlayerIp = server;

    if(!dceMediaSocket->isOpen())
    {
        // qDebug() <<"opening connection to " << port;

        dceMediaSocket->connectToHost(mediaPlayerIp, port, QFile::ReadOnly );
        if ( dceMediaSocket->isValid() )
        {
            // setDceResponse("Time Code Socket connected! " + QString::fromStdString(qs_routerip.toStdString()));
            QObject::connect(dceMediaSocket,SIGNAL(readyRead()), this, SLOT(updateTimeCode()));
        }
        else
        {   //setDceResponse("couldnt start timecode");
            //  qDebug() << timeCodeSocket->errorString();
        }
    }
    else
    {

    }
}

void TimeCodeManager::stop()
{
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
    QString tcData = QString::fromAscii(socketData.data(), socketData.size());
   // qDebug() << tcData;
    if (tcData.length() > 0)
    {

        QStringList tcVars = tcData.split(",");
        QString tcClean = tcVars.at(1);
        tcClean.remove(QRegExp(".\\d\\d\\d|00:0|0:0|00:"));
        //  nowPlayingButton->setTimeCode(tcClean);


        QString playbackSpeed = tcVars.at(0);
        playbackSpeed.remove(QRegExp("000"));
        // nowPlayingButton->setStringSpeed(playbackSpeed+"x");
        //  nowPlayingButton->setMediaSpeed(playbackSpeed.toInt());

        QString duration = tcVars.at(2);
        duration.remove(QRegExp(".\\d\\d\\d|00:0|0:0|00:"));
        //nowPlayingButton->setDuration(duration);
        QTime total = QTime::fromString(tcVars.at(2), "hh:mm:ss:sss" );
        //qDebug() << total.toString();
       // setSpeed(tcVars.at(0).remove(",000").toInt());


    }
}
