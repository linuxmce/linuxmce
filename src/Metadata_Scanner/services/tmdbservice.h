#ifndef TMDBSERVICE_H
#define TMDBSERVICE_H
#include "metadataservicebase.h"

class TMDBService : public MetadataServiceBase
{
public:
    TMDBService(QString service , QObject *parent=0);

private:
    void handleMediaInfoResponse();
    void getMediaInfo();
};

#endif // TMDBSERVICE_H
