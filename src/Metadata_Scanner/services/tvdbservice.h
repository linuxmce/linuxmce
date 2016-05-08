#ifndef TVDBSERVICE_H
#define TVDBSERVICE_H
#include "metadataservicebase.h"

class TVDBService : public MetadataServiceBase
{
public:
    TVDBService(QString service, QObject*parent=0);

private:
    void getMediaInfo();
    void handleMediaInfoResponse();
};

#endif // TVDBSERVICE_H
