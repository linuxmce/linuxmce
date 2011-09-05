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
        QImage key;
        qDebug() << "CREATED KEY";
        if (managerreference->updatedObjectImage.isEmpty())
        {
            qDebug() << "EMPTY UPDATE IMAGE BUFFER, SCHEDULE RELOAD";
            key.load("qrc:/icons/videos.png");
            return key;
        }
        else
        {
            QByteArray localbytes = managerreference->updatedObjectImage;
            qDebug() << "LOADING DATA FROM QBYTE ARRAY:" << managerreference->updatedObjectImage.size();
            key.loadFromData(localbytes);
        }

        qDebug() << "KEY IMAGE STATE::" << key.isNull();

        if (requestedSize.isValid())
        {
            result = key.scaled(requestedSize);
            qDebug() << "SCALED to SIZE";
        }
        else
        {
            qDebug() << "COULD NOT SCALE!!";
            result = key;
        }

        qDebug() << "RETURNING IMAGE FROM PROVIDER AND FINISHING";
        return key;
    }


signals:

public slots:

private:
    qorbiterManager * managerreference;
};

#endif // NOWPLAYINGIMAGEPROVIDER_H
