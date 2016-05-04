#include "tmdbservice.h"

TMDBService::TMDBService(QString service, QObject *parent) : MetadataServiceBase(service, parent)
{
    QVector<int> supportedVect;
    supportedVect << MEDIASUBTYPE_Movies_CONST;
    setSupportedMediaTypes( supportedVect);
}

void TMDBService::handleMediaInfoResponse()
{

}

void TMDBService::getMediaInfo()
{

}

