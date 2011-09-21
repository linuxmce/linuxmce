#ifndef ABSTRACTIMAGEPROVIDER_H
#define ABSTRACTIMAGEPROVIDER_H

#include <QObject>
#include <QDeclarativeImageProvider>
#include <QDebug>
#include <QVariant>
#include <qorbitermanager.h>

class AbstractImageProvider: public QObject , public QDeclarativeImageProvider
{
public:
    AbstractImageProvider(qorbiterManager * manager);



    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize)
    {
        qDebug() << id;
        qDebug() << "Abstract Image Provider STARTING";
        QImage result;
        QImage key;

        if (id.contains("floorplan"))
        {
            key= managerreference->floorplans->getPageImage(managerreference->floorplans->currentPage);
        }

        if (key.isNull())
        {
             key.load(":/icons/playlist.png");

        }

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

#endif // ABSTRACTIMAGEPROVIDER_H
