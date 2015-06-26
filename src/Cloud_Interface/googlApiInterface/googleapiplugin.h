#ifndef GOOGLEAPIPLUGIN_H
#define GOOGLEAPIPLUGIN_H

#include "googlapiinterface_global.h"
#include <QObject>
#include "../Cloud_Interface/abstractcloudprovider.h"
class GoogleApiPlugin : public QObject, CloudServiceInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID CloudServiceInterface_iid FILE "googleapi.json")
    Q_INTERFACES(CloudServiceInterface)
public:
    GoogleApiPlugin();
    void setUserList(QList<QObject*>);
    QString name(){return tr("Google Api Plugin");}
    bool suppliesPhotos() {return true;}
    bool suppliesVideos() {return true;}
    bool suppliesContacts() {return true;}
    bool canSendEmail() {return true;}
    bool canSendFiles() {return true;}
    bool canSendPhotos() {return true;}
#endif // GOOGLEAPIPLUGIN_H
