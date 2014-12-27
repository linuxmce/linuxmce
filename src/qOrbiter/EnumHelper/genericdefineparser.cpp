#include "genericdefineparser.h"
#include "QFile"
#include "QDir"
#include "QDebug"

GenericDefineParser::GenericDefineParser( QString startingFile, int index,QObject *parent) :
    QObject(parent), m_dgOutFile(startingFile), m_isReady(false), idx(index)
{
    m_logFile = new QFile(QDir::tempPath()+"/genericDefineParser-"+startingFile);
    qDebug() << "Logfile name " << m_logFile->fileName();
    if(setupLogFile()){
        m_isReady =true;
    } else {
        qDebug() << "Unable to setup logging.";
    }
}

void GenericDefineParser::startParsing(){
qDebug() << "\n\n";
    qDebug() << name() << " Started parsing.";
    qDebug() << "Output file" << m_dgOutFile;
    qDebug() << "Using markers? " << m_usingTags;
    qDebug() << name() << " is done parsing.";
    emit finishedParsing(idx, true);
}

bool GenericDefineParser::setupLogFile()
{


    if(m_logFile->exists()){

        if(!m_logFile->remove()){
            qDebug() << m_logFile->errorString();
            return false;
        }

    }

    if(m_logFile->open(QFile::ReadWrite)){
        qDebug() << " Logging enabled.";
        return true;
    } else {
        qDebug() << m_logFile->errorString();
        return false;
    }


}
