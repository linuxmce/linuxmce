#include "qorbiterlogger.h"
#include <QDebug>
#include <QTime>
#include <QDir>
#ifdef QT5
#include <QtWidgets/QApplication>
#else
#include <QApplication>
#endif

QOrbiterLogger::QOrbiterLogger(QObject *parent) :
    QObject(parent)
{
    loggingEnabled = false;
#if defined(ANDROID)
    logLocation="";
#elif defined (Q_OS_IOS)
    QDir dir;
    dir.setPath(QDir::homePath());
    qDebug()<<"iOS current Path::" << dir.absolutePath();
    qDebug() << "Directories::"<< dir.entryList().join("\n");
    setLogLocation(dir.absolutePath()+"/Library/Application_Support/LinuxMCE");
#else
    logLocation = QDir::homePath()+"/linuxmce/";
    qDebug() << logLocation;
    setLogLocation(logLocation);

#endif
}


void QOrbiterLogger::setLogLocation(QString l){

    if(l != ""){
        logLocation = l;
        if(!logLocation.endsWith("/")){
            logLocation.append("/");
        }
#ifdef __ANDROID__
        logLocation.append("LinuxMCE/");
#endif

        QDir fileLocation;
        fileLocation.setPath(logLocation);
        qDebug() << "New Log Location! " << fileLocation;
  QFile configFile(fileLocation.path()+"/config.xml");
        if(!fileLocation.exists()){
            qDebug() <<fileLocation.path()<< "-----------------log directory doesnt exist, setting up.";

            if(!fileLocation.mkpath(logLocation)){
                qDebug() << "---------------------------Cant create logfile directory!";

            } else{

                qDebug() << "Log Location sucessfully created? " << fileLocation.exists();
                loggingEnabled = true;


                qDebug() << configFile.fileName();
                if(!configFile.exists()){
                    QFile defaultConfig;
                    defaultConfig.setFileName(":/main/config.xml");

                    if(!defaultConfig.open(QFile::ReadOnly)){
                        qDebug() << "Cannot find config in QRC";
                        qDebug() << defaultConfig.errorString();
                    } else {
                        if(defaultConfig.copy(fileLocation.path()+"/config.xml")){
                           qDebug() <<"Intalled Config.";

                        }
                    }

                }
            }
        } else {
            qDebug() << "Log Location sucessfully set. ";

            configFile.setPermissions(fileLocation.path()+"/config.xml", QFile::WriteGroup | QFile::ReadGroup | QFile::ReadOwner | QFile::WriteOwner);

             qDebug() << "Config Permissions:"<< configFile.permissions();
        }
    }
    else{
        qDebug() << "Bad Location " << logLocation;
    }
    emit logLocationChanged(logLocation);
    if(loggingEnabled)
        initLogs();
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
    if(commandFile.exists()){
        qDebug() << commandFile.fileName() << " exists and its open state is "<<commandFile.isOpen();
        if(commandFile.size() > 10000)
        {
            qDebug() << "Rotating logfile";
            QDir t;
            t.setPath(logLocation);
            t.remove(commandFile.fileName());
        } else {
            qDebug() << "File size is " << commandFile.size() << " and wont be rotated";
        }


        if(commandFile.isOpen() || commandFile.open(QFile::Append)){
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
        qWarning("File Doesnt Exist!");
        commandFile.open(QFile::ReadWrite);
        if(commandFile.exists()){
            return true;
        } else{
            return false;
        }

    }
}

bool QOrbiterLogger::initializeGuiFile()
{
    guiFile.setFileName(logLocation+"qorbiter-gui.log");
    qDebug()<< "-----------------------Attempting to open logfile " << guiFile.fileName()  ;
    if(guiFile.exists()){

        if(guiFile.size() > 10000)
        {
            QDir t;
            t.setPath(logLocation);
            t.remove(guiFile.fileName());
            qDebug() << "Rotating logfile";
        } else {
            qDebug() << "File size is " << guiFile.size() << " and wont be rotated";
        }

        if(guiFile.isOpen() || guiFile.open(QFile::Append)){
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
        qWarning("File doesnt exist!");
        guiFile.open(QFile::ReadWrite);
        if(guiFile.exists()){
            return true;
        } else{
            return false;
        }
    }
}

bool QOrbiterLogger::initializeSkinFile()
{
    skinFile.setFileName(logLocation+"qorbiter-qml.log");
    qDebug()<< "-----------------------Attempting to open logfile " << skinFile.fileName()  ;

    if(skinFile.exists()){

        qDebug() << skinFile.fileName() << " exists and its open state is "<<skinFile.isOpen();
        if(skinFile.size() > 10000)
        {
            QDir t;
            t.setPath(logLocation);
            t.remove(skinFile.fileName());
            qDebug() << "Rotating logfile";
        } else {
            qDebug() << "File size is " << skinFile.size() << " and wont be rotated";
        }

        if(skinFile.isOpen() || skinFile.open(QFile::Append)){
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
        qWarning("File Doesnt Exist!");
        skinFile.open(QFile::ReadWrite);
        if(skinFile.exists()){
            return true;
        } else{
            return false;
        }
    }
}

bool QOrbiterLogger::writeCommandMessage(QString m)
{
    if(commandFile.isOpen()){
        QTextStream t(&commandFile);

        t<< m;
        return true;
    }
    else if(!commandFile.open(QFile::ReadWrite))
    {
        qDebug() << "Couldnt write->"<<commandFile.errorString() ;
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

        qDebug() << "Command log was intialized";
        logCommandMessage(" Command file initialized - New log section");
        logCommandMessage("-----------------------"+QTime::currentTime().toString()+"-----------------------------");
        setCommandLogStatus(true);
    }
    else
    {

        setCommandLogStatus(false);
    }

    if(initializeGuiFile()){
        qDebug() << "GUI log was intialized";
        logGuiMessage("GUI logfile ready - new Log Section");
        logGuiMessage("-----------------------"+QTime::currentTime().toString()+"-----------------------------");
        setGuiLogStatus(true);
    }
    else
    {
        setGuiLogStatus(false);
    }

    if (initializeSkinFile()){
        qDebug() << "Skin log was intialized";
        logSkinMessage("Skin logfile ready New Log Section");
        logSkinMessage("-----------------------"+QTime::currentTime().toString()+"-----------------------------");
    }
    else
    {
        setQtLogStatus(false);
    }

}







