#ifndef GOOGLEAPIPLUGIN_H
#define GOOGLEAPIPLUGIN_H

#include "googlapiinterface_global.h"
#include <QObject>
#include "../Cloud_Interface/abstractcloudprovider.h"
#include  "../../pluto_main/Define_DeviceTemplate.h"

class GoogleApiPlugin : public QObject, CloudServiceInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID CloudServiceInterface_iid FILE "googleapi.json")
    Q_INTERFACES(CloudServiceInterface)

public:
    GoogleApiPlugin();

    QString name(){return tr("Google Api Plugin");}
    void setUserList(QList<QObject *>l);
    bool suppliesPhotos() {return true;}
    bool suppliesVideos() {return true;}
    bool suppliesContacts() {return true;}
    bool canSendEmail() {return true;}
    bool canSendFiles() {return true;}
    bool canSendPhotos() {return true;}
    int deviceTemplate() {return DEVICETEMPLATE_Google_API_Interface_CONST; }
};
#endif // GOOGLEAPIPLUGIN_H
