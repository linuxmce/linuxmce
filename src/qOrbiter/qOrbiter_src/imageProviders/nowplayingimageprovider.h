#ifndef NOWPLAYINGIMAGEPROVIDER_H
#define NOWPLAYINGIMAGEPROVIDER_H

#include <QDeclarativeImageProvider>
#include <contextobjects/nowplayingclass.h>



class NowPlayingImageProvider: public QDeclarativeImageProvider
{
public:
    NowPlayingImageProvider(NowPlayingClass* nowplaying, QObject* parent):QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
    {
    }

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize)
    {


/*
        ref->pqOrbiter->getfileForDG("mediapics");
        const char* pData;
        int pSize;
        pData= id.toStdString().data();
        pSize = id.toStdString().size();
        qDebug() << pData << "- " << pSize;
        QByteArray t;
        t.setRawData(pData, pSize);


        key.loadFromData(t, "GIF");
        //key.scaled(&requestedSize);

        qDebug() << key.isNull();
        */
        QImage key;
        QImage result;

         if (requestedSize.isValid()) {
                result = key.scaled(requestedSize, Qt::KeepAspectRatio);
            } else {
                result = key;
            }
            *size = result.size();

            return result;
    }
};

#endif // NOWPLAYINGIMAGEPROVIDER_H
