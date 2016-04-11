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

    MediaItem * item = m_updateQueue.takeFirst();

    //do some stuff to and update in db

    emit mediaItemUpdated(item);

    busyWithFile = false;

    emit nextFile();

}

