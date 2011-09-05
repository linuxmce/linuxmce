#ifndef NOWPLAYINGIMAGEPROVIDER_H
#define NOWPLAYINGIMAGEPROVIDER_H

#include <QDeclarativeImageProvider>
#include <qorbitermanager.h>
#include <QVariant>



class UpdateObjectImageProvider: public QObject , public QDeclarativeImageProvider
{
public:
    UpdateObjectImageProvider(qorbiterManager * manager);



    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize)
    {

        QImage key = managerreference->updatedObjectImage;
        if(key.isNull())
        {
             key.load(":/img/lmcesplash.jpg");

        }
        else
        {
        QImage result;

         if (requestedSize.isValid()) {
                result = key.scaled(requestedSize, Qt::KeepAspectRatio);
            } else {
                result = key;
            }
            *size = result.size();

            return result;
        }
    }
signals:

public slots:

private:
    qorbiterManager * managerreference;
};

#endif // NOWPLAYINGIMAGEPROVIDER_H
