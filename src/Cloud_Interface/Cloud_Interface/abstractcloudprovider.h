#ifndef ABSTRACTCLOUDPROVIDER_H
#define ABSTRACTCLOUDPROVIDER_H

#include <QtPlugin>

class QString;
class CloudServiceInterface
{

public:
    virtual ~CloudServiceInterface(){}
    virtual void setUserList(QList<QObject*>)=0;
   virtual QString name() =0;

};

QT_BEGIN_NAMESPACE
#define CloudServiceInterface_iid "CloudServiceInterface"
Q_DECLARE_INTERFACE(CloudServiceInterface, CloudServiceInterface_iid)
QT_END_NAMESPACE

#endif // ABSTRACTCLOUDPROVIDER_H
