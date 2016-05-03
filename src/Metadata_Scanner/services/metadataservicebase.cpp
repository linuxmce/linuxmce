#include "metadataservicebase.h"
#include "qmap.h"

MetadataServiceBase::MetadataServiceBase(QString service, QObject *parent) : QObject(parent)
{
    setServiceName(service);
}

QString MetadataServiceBase::serviceName() const
{
    return m_serviceName;
}

void MetadataServiceBase::setServiceName(const QString &serviceName)
{
    m_serviceName = serviceName;
}

void MetadataServiceBase::processMediaItem(MediaItem *item)
{
    m_itemList.push_back(item);
}

void MetadataServiceBase::setSupportedMediaTypes(QVector<int> mediaTypes)
{
    if(mediaTypes.isEmpty())
        return;
    m_SupportedMediaTypes = mediaTypes;
}

bool MetadataServiceBase::supportsFormat(int f)
{
    if(m_SupportedMediaTypes.isEmpty())
        return false;

    if(m_SupportedMediaTypes.contains(f))
        return true;
}


