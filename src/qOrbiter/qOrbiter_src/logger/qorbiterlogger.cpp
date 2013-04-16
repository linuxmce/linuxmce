#include "qorbiterlogger.h"
#include <QDebug>
#include <QTime>
#include <QDir>
#include <QApplication>

QOrbiterLogger::QOrbiterLogger(QObject *parent) :
    QObject(parent)
{
    loggingEnabled = false;
#ifndef ANDROID
    logLocation = QDir::homePath()+"/QOrbiter-log/";
    setLogLocation(logLocation);
#else
    logLocation="";
#endif
}


void QOrbiterLogger::setLogLocation(QString l)
{
    if(l != ""){
        logLocation = l;
        qDebug() << "New Log Location! " << logLocation;

        QDir fileLocation;
        fileLocation.setPath(logLocation);

        if(!fileLocation.exists())
        {
            qDebug() << "-----------------log directory doesnt exit, setting up.";
            if(!fileLocation.mkpath(logLocation)){
                qDebug() << "---------------------------Cant create logfile directory!";
                return;
            }
            else{
                qDebug() << "Log Location sucessfully created. " << fileLocation.exists();
                loggingEnabled = true;
            }
        }
        else
        {
            qDebug() << "Log Location sucessfully set. ";
            loggingEnabled = true;

        }

        emit logLocationChanged(logLocation);
        if(loggingEnabled)
            initLogs();
    }
    else
    {
        qDebug() << "Bad Location " << logLocation;
    }
}



void QOrbiterLogger::logCommandMessage(QString message)
{
    commandMsg = QTime::currentTime().toString()+QString(" - CMD - ")+message+"\n";
    emit commandMessageRecieved(message);
    if(loggingEnabled)
        writeCommandMessage(commandMsg);
}

void QOrbiterLogger::logGuiMessage(QString message)
{
    guiMsg =  QTime::currentTime().toString()+QString(" - GUI - ")+message+"\n";
    emit guiMessageRecieved(guiMsg);
    if(loggingEnabled)
        writeGuiMessage(guiMsg);

}

void QOrbiterLogger::logQtMessage(QString message)
{
    qtMsg = QTime::currentTime().toString()+QString(" - QT - ")+message+"\n";
    emit qtMessageRecieved(qtMsg);
    if(loggingEnabled)
        writeGuiMessage(qtMsg);
}


void QOrbiterLogger::logSkinMessage(QString message)
{
    skinMsg = QTime::currentTime().toString()+QString(" - SKIN - ")+message+"\n";
    emit skinMessageRecieved(message);
    if(loggingEnabled)
        writeSkinMessage(skinMsg);
}

void QOrbiterLogger::logMediaMessage(QString message)
{
    mediaMsg = QTime::currentTime().toString()+QString(" - MEDIA - ")+message+"\n";
    emit mediaMessageRecieved(message);
    if(loggingEnabled)
        writeCommandMessage(skinMsg);
}

#ifdef QT5
void QOrbiterLogger::logQmlErrors(QList<QQmlError> e)
{
    if(loggingEnabled){
        for(int i = 0; i < e.count(); i++){
            QQmlError b = e.at(i);
            writeGuiMessage(QTime::currentTime().toString()+" - QML Error - "+b.toString()+"\n");
        }
    }
}
#endif

#ifndef QT5
void QOrbiterLogger::logQmlErrors(QList<QDeclarativeError> e)
{
    if(loggingEnabled){
        for(int i = 0; i < e.count(); i++){
            QDeclarativeError b = e.at(i);
            writeGuiMessage(QTime::currentTime().toString()+" - QML Error - "+b.toString()+"\n");
        }
    }
}
#endif

void QOrbiterLogger::logUserMessage(QString message)
{
    writeSkinMessage(message);
}



bool QOrbiterLogger::writeToFile(QString msg)
{
}

bool QOrbiterLogger::initializeCommandFile()
{
    commandFile.setFileName(logLocation+"qorbiter-command.log");
    qDebug()<< "-----------------------Attempting to open logfile " << commandFile.fileName()  ;
    if(!commandFile.exists()){



        if(commandFile.open(QFile::Append)){
            commandFile.seek(commandFile.size());
            return true;
        }
        else
        {
            qDebug() << commandFile.errorString();
            return false;
        }
    }
    else
    {
        if(commandFile.size() > 10000)
        {
            QDir t;
            t.setPath(logLocation);
            t.remove(commandFile.fileName());
            initializeCommandFile();
            return false;
        }else{
            return true;
        }
    }
}

bool QOrbiterLogger::initializeGuiFile()
{
    guiFile.setFileName(logLocation+"qorbiter-gui.log");
    qDebug()<< "-----------------------Attempting to open logfile " << guiFile.fileName()  ;
    if(!guiFile.exists()){
        if(guiFile.open(QFile::Append)){
            guiFile.seek(guiFile.size());
            return true;
        }
        else
        {
            qDebug() << guiFile.errorString();
            return false;
        }
    }
    else
    {
        if(guiFile.size() > 10000)
        {
            QDir t;
            t.setPath(logLocation);
            t.remove(guiFile.fileName());
            initializeGuiFile();
            return false;
        }
        else {
            return true;
        }
    }
}

bool QOrbiterLogger::initializeSkinFile()
{
    skinFile.setFileName(logLocation+"qorbiter-qml.log");
    qDebug()<< "-----------------------Attempting to open logfile " << skinFile.fileName()  ;
    if(!skinFile.exists()){

        if(skinFile.open(QFile::Append)){
            skinFile.seek(skinFile.size());
            return true;
        }
        else
        {
            qDebug() << skinFile.errorString();
            return false;
        }
    }
    else
    {
        if(skinFile.size() > 10000)
        {
            QDir t;
            t.setPath(logLocation);
            t.remove(skinFile.fileName());
            initializeSkinFile();
            return false;
        }
        else
        {
            return true;
        }


    }
}

bool QOrbiterLogger::writeCommandMessage(QString m)
{
    if(skinFile.isOpen()){
        QTextStream t(&commandFile);

        t<< m;
        return true;
    }
    else if(!commandFile.open(QFile::ReadWrite))
    {
        qDebug() << "Couldnt write!";
        return false;
    }
    else{
        QTextStream t(&commandFile);
        t<< m;
        return true;
    }
}

bool QOrbiterLogger::writeGuiMessage(QString m)
{
    if(guiFile.isOpen()){
        QTextStream t(&guiFile);
        t<< m;
        return true;
    }
    else if(!guiFile.open(QFile::ReadWrite))
    {
        qDebug() << "Couldnt write!";
        return false;
    }
    else{
        QTextStream t(&guiFile);
        t<< m;
        return true;
    }
}

bool QOrbiterLogger::writeSkinMessage(QString m)
{
    if(skinFile.isOpen()){
        QTextStream t(&skinFile);
        t<< m;
        return true;
    }
    else if(!skinFile.open(QFile::ReadWrite))
    {
        qDebug() << "Couldnt write!";
        return false;
    }
    else{
        QTextStream t(&skinFile);
        t<< m;
        return true;
    }
}

void QOrbiterLogger::initLogs()
{


    if (initializeCommandFile()){

        logCommandMessage(" Command file initialized - New log section");
        logCommandMessage("-----------------------"+QTime::currentTime().toString()+"-----------------------------");
        setCommandLogStatus(true);
    }
    else
    {

        setCommandLogStatus(false);
    }

    if(initializeGuiFile()){
        logGuiMessage("GUI logfile ready - new Log Section");
        logGuiMessage("-----------------------"+QTime::currentTime().toString()+"-----------------------------");
        setGuiLogStatus(true);
    }
    else
    {
        setGuiLogStatus(false);
    }

    if (initializeSkinFile()){
        logSkinMessage("Skin logfile ready New Log Section");
        logSkinMessage("-----------------------"+QTime::currentTime().toString()+"-----------------------------");
    }
    else
    {
        setQtLogStatus(false);
    }

}







