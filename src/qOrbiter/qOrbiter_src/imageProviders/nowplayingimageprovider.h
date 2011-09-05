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
        qDebug() << "NOWPLAYINGPROVIDER STARTING";
        QImage result;
        QImage key = managerreference->updatedObjectImage;


        if (requestedSize.isValid()) {
            result = key.scaled(requestedSize);
        } else {
            result = key;
        }
        return result;

    }


signals:

public slots:

private:
    qorbiterManager * managerreference;
};

#endif // NOWPLAYINGIMAGEPROVIDER_H
