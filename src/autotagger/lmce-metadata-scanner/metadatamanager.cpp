#include "metadatamanager.h"

MetadataManager::MetadataManager(QObject *parent) : QObject(parent)
  ,m_filenameParser(new FilenameParser())
  ,m_database(new DatabaseInterface())
  ,m_networkMetadata(new NetworkMetadataManager() )
{

}

void MetadataManager::addFileToQueue(QString incomingFile){

    if(!m_filenameQueue.contains(incomingFile)){
        m_filenameQueue.append(incomingFile);
        emit fileQueueListChanged();
    }
}
