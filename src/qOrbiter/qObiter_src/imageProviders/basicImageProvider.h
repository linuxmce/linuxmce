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
   QString key = QString("image://datagridimageprovider/%1").arg(id);
    QString verify(id);
    QImage cellimage;

    if ( verify == "music")
    {
        cellimage.load(":/icons/song-item.png");
        cellimage.scaledToWidth(180);
    }
    else
    {
    cellimage.load(":/icons/video-blank.png");
    cellimage.scaledToWidth(200);
    }
   return cellimage;
    }

signals:

public slots:

};

#endif // BASICIMAGEPROVIDER_H
