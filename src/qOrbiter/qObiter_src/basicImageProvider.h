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
    QImage cellimage;
    cellimage.load(":/icons/video-blank.png");
    cellimage.scaledToWidth(200);
   return cellimage;
    }

signals:

public slots:

};

#endif // BASICIMAGEPROVIDER_H
