#include "tvdbservice.h"

TVDBService::TVDBService(QString service, QObject *parent):MetadataServiceBase(service, parent)
{
    QVector<int> supportedVect;
    supportedVect << MEDIASUBTYPE_TV_Shows_CONST;
    setSupportedMediaTypes( supportedVect);
}

void TVDBService::getMediaInfo()
{

}

void TVDBService::handleMediaInfoResponse()
{

}

