#ifndef ABSTRACTCLOUDPROVIDER_H
#define ABSTRACTCLOUDPROVIDER_H

#include <QtPlugin>

class QString;
class CloudServiceInterface
{

public:
    virtual ~CloudServiceInterface(){}
    virtual void setUserList(QList<QObject*>)=0;
    virtual bool suppliesPhotos()=0;
    virtual bool suppliesVideos()=0;
    virtual bool suppliesContacts()=0;
    virtual bool canSendEmail()=0;
    virtual bool canSendFiles()=0;
    virtual bool canSendPhotos()=0;

   virtual QString name() =0;

};

QT_BEGIN_NAMESPACE
#define CloudServiceInterface_iid "CloudServiceInterface"
Q_DECLARE_INTERFACE(CloudServiceInterface, CloudServiceInterface_iid)
QT_END_NAMESPACE

#endif // ABSTRACTCLOUDPROVIDER_H
