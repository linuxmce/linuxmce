#include "qorbiterlogger.h"
#include <QDebug>
#include <QTime>

QOrbiterLogger::QOrbiterLogger(QObject *parent) :
    QObject(parent)
{
    logLocation = "~/QOrbiter"+QT_VERSION;

    if (initializeCommandFile()){

        logCommandMessage(QTime::currentTime().toString() + " - CMD - Command file initialized");
        logCommandMessage("-----------------------"+QTime::currentTime().toString()+"-----------------------------");
    }
    else
    {
        qDebug() << "---------------------------Cant open logfile!";
    }

    if(initializeGuiFile()){

    }

    if (initializeSkinFile()){

    }

}

void QOrbiterLogger::logCommandMessage(QString message)
{
    commandMsg = message;
    emit commandMessageRecieved(message);
}

void QOrbiterLogger::logGuiMessage(QString message)
{
    guiMsg = message;
    emit guiMessageRecieved(guiMsg);

}

void QOrbiterLogger::logQtMessage(QString message)
{
    qtMsg = message;
    emit qtMessageRecieved(qtMsg);
}


void QOrbiterLogger::logSkinMessage(QString message)
{
    skinMsg = message;
    emit skinMessageRecieved(message);
}

void QOrbiterLogger::logMediaMessage(QString message)
{
}


bool QOrbiterLogger::writeToFile(QString msg)
{
}

bool QOrbiterLogger::initializeCommandFile()
{
    commandFile.setFileName(logLocation+"qorbiter-command.log");
    if(commandFile.exists()){

        if(commandFile.open(QFile::ReadWrite)){
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {

    }
}

bool QOrbiterLogger::initializeGuiFile()
{
}

bool QOrbiterLogger::initializeSkinFile()
{
}


