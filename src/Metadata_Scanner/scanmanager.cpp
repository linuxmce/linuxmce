#include "scanmanager.h"
#include "qdebug.h"
#include "qdatetime.h"
#include "services/metadataservicebase.h"
ScanManager::ScanManager(QObject *parent) : QObject(parent)
{

    log("Started Manager");

    if(!registerMetadataProviders() )
        exit(1);
    else
        exit(0);
}

void ScanManager::log(QString msg)
{
    qDebug() << QDateTime::currentDateTime() << " ::" << msg;
}

void ScanManager::addItemForUpdate(MediaItem *item)
{
    m_outgoingQueue.push_back(item);
}

bool ScanManager::registerMetadataProviders()
{
    log(QString(Q_FUNC_INFO));

    return true;
}

