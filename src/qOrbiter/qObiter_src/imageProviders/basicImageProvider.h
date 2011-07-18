#ifndef BASICIMAGEPROVIDER_H
#define BASICIMAGEPROVIDER_H

#include <QObject>
#include <QDeclarativeImageProvider>

class basicImageProvider : public QDeclarativeImageProvider
{

public:
    basicImageProvider():QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
    {
    }


    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize)
    {
   QString key = QString("image://datagridimageprovider/%l").arg(id);
    QString verify(id);
    QImage cellimage;

    if ( verify == "4")
    {
        cellimage.load(":/icons/song-item.png");
        cellimage.scaledToWidth(200);
    }
    else if (verify=="5")
    {
    cellimage.load(":/icons/video-blank.png");
    cellimage.scaledToWidth(200);
    }
    else if (verify == "29")
    {
        cellimage.load(":/icons/media-games.png");
        cellimage.scaledToWidth(200);
    }
   return cellimage;
    }


signals:

public slots:

};

#endif // BASICIMAGEPROVIDER_H
