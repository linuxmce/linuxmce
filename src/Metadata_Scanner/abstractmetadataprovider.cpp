#include "abstractmetadataprovider.h"

AbstractMetadataProvider::AbstractMetadataProvider(QString providerName, QObject *parent) : QObject(parent),
    m_displayName(providerName)
{

}

QVector<int> AbstractMetadataProvider::getSupportedMediaTypes()
{
    return m_supportedMediaTypes;
}

