#ifndef FILEDETAILSIMAGEPROVIDER_H
#define FILEDETAILSIMAGEPROVIDER_H

#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QQmlImageProvider>
#else
#include <QDeclarativeImageProvider>
#include <QtDeclarative/QDeclarativeImageProvider>
#endif
#include <QDebug>
#include <QVariant>
#include <contextobjects/filedetailsclass.h>

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
class FileDetailsImageProvider: public QQmlImageProvider
#else
class FileDetailsImageProvider: public QDeclarativeImageProvider
#endif
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
