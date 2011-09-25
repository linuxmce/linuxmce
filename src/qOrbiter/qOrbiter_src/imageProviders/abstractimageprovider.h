#ifndef ABSTRACTIMAGEPROVIDER_H
#define ABSTRACTIMAGEPROVIDER_H

/*
  This class is intended to emcompass most of the image requests for qOrbiter. It has a reference to the
  qOrbitermanager, allowing for access to all of its class objects, include in the dce file request for
  remote loading of images and external. It has been decided that with a few exceptions, all listmodels will
  be responsible for managing their own image associations. This means that when any model activity occurs in the
  ui, the c++ model will be responsible for making the underlying changes to the model such as sorting,
  image association or other functions not defined at this time.
  */

#include <QObject>
#include <QDeclarativeImageProvider>
#include <QDebug>
#include <QVariant>
#include <qorbitermanager.h>

class AbstractImageProvider: public QObject , public QDeclarativeImageProvider
{
public:
    AbstractImageProvider(qorbiterManager * manager);

QImage result;
QImage key;

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize)
    {
        QImage temp;
        if (id.contains("floorplan"))
        {
             temp = floorplanProvider();
       }
        else if(id.contains("securityimage"))
        {
            temp = securityProvider();
        }
        else if(id.contains("updateobject"))
        {
            temp = updateObjectProvider();
        }
        else if (id.contains("filedetailsprovider"))
        {
            temp = fileDetails();
            if(temp.height() > temp.width())
            {

                managerreference->filedetailsclass->setImageAspect("poster");
            }
            else
            {

                managerreference->filedetailsclass->setImageAspect("wide");
            }
        }

        //aspect checking


        if (requestedSize.isValid()) {
            result = temp.scaled(requestedSize);
        } else {
            result = temp;
        }
        return result;
    }


signals:


public slots:


private:
    qorbiterManager * managerreference;
    QImage floorplanProvider();
    QImage securityProvider();
    QImage updateObjectProvider();
    QImage fileDetails();

};

#endif // ABSTRACTIMAGEPROVIDER_H
