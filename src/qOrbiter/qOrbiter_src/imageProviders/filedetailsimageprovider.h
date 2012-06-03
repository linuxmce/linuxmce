#ifndef FILEDETAILSIMAGEPROVIDER_H
#define FILEDETAILSIMAGEPROVIDER_H

#include <QtDeclarative/QDeclarativeImageProvider>
#include <QDebug>
#include <QVariant>
#include <contextobjects/filedetailsclass.h>

class FileDetailsImageProvider: public QDeclarativeImageProvider
{
public:
    FileDetailsImageProvider(FileDetailsClass *manager);

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize)
    {
        QImage result;
        QImage key= f->getScreenshotimage();

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
    FileDetailsClass * f;


};

#endif // FILEDETAILSIMAGEPROVIDER_H
