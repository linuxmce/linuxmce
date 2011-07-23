#ifndef BASICIMAGEPROVIDER_H
#define BASICIMAGEPROVIDER_H

#include <QObject>
#include <QDeclarativeImageProvider>
#include <QDebug>

class basicImageProvider : public QDeclarativeImageProvider
{

public:
    basicImageProvider():QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
    {
    }


    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize)
    {
       // QImage key= QImage(QByteArray::fromRawData(id.toStdString().c_str(), id.size()));

        QString rsrcid = ":/" + id;
            QImage image(rsrcid);
            qDebug() << image.isNull();
            QImage result;

            if (requestedSize.isValid()) {
                result = image.scaled(requestedSize, Qt::KeepAspectRatio);
            } else {
                result = image;
            }
            *size = result.size();
            return result;
    }


signals:

public slots:

};

#endif // BASICIMAGEPROVIDER_H
