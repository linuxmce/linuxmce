#include "mediaitem.h"
#include "QMap"
MediaItem::MediaItem(int fileNumber, QObject *parent) : QObject(parent)
{
    i_dceFileNumber = fileNumber;
}

void MediaItem::setAttribute(int attributeType, QString attributeData)
{
    if(m_mapAttributes.contains(attributeType))
        return;

    m_mapAttributes.insert(attributeType, attributeData);
}

