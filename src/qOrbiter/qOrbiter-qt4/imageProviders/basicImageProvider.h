/*
  The basic image provider is here to provide images either from the qrc or locally included style related images
  complex loading of media and network images is done in another imageprovider.
  */

#ifndef BASICIMAGEPROVIDER_H
#define BASICIMAGEPROVIDER_H

#include <QtGlobal>

#include <QObject>
#include <QDeclarativeImageProvider>
#include <QDebug>
#include <QVariant>

#ifdef debug
#include <QDebug>
#endif
#include <qorbitermanager.h>

class basicImageProvider : public QDeclarativeImageProvider, public QObject

{

public:

    basicImageProvider():QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
    {
    }

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize)
    {

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


signals:

public slots:

};

#endif // BASICIMAGEPROVIDER_H
