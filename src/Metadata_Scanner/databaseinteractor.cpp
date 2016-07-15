#include "databaseinteractor.h"
#include "qvector.h"
DatabaseInteractor::DatabaseInteractor(QObject *parent) : QObject(parent)
{
    busyWithFile=false;
}

void DatabaseInteractor::updateMediaItem(MediaItem *item)
{
    if(item)
        m_updateQueue.push_back(item);


}

void DatabaseInteractor::updateItem()
{
    if(busyWithFile)
        return;

    busyWithFile = true;

    MediaItem * item = m_updateQueue.first();

    //do some stuff to and update in db

    emit mediaItemUpdated(item);

    busyWithFile = false;

    emit nextFile();

}

QString DatabaseInteractor::findFileNumber(QString fullFilePath, QString fullFileName)
{

}

QString DatabaseInteractor::checkAttributeForFile(QString attribute, QString fileNo, int attributeType)
{

}

QString DatabaseInteractor::checkDbForAttribute(QString attribute, int attributeType)
{

}

int DatabaseInteractor::findFileMediaType(QString fileNo)
{

}

QString DatabaseInteractor::insertNewAttribute(QString attribute, QString attributeType)
{

}

int DatabaseInteractor::associatFileWithAttribute(QString fileNo, QString fkAttributeNo)
{

}

int DatabaseInteractor::savePic(QString picUrl, QString attribute)
{

}

void DatabaseInteractor::setFileMediaTypes(QString file_format, QString sub_type, QString fileNo)
{

}
bool DatabaseInteractor::myqlConnected() const
{
    return m_myqlConnected;
}

void DatabaseInteractor::setMyqlConnected(bool myqlConnected)
{
    m_myqlConnected = myqlConnected;
}


