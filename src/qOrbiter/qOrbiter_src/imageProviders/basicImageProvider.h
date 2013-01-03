/*
  The basic image provider is here to provide images either from the qrc or locally included style related images
  complex loading of media and network images is done in another imageprovider.
  */

/*!
 *\class basicImageProvider
 *\brief This is a simple implementation of an QmlEngines image provider.
 *
 *\ingroup qt_controllers
 *
 *
 *
 *\sa QAbstractImageProvider
 *
 */
#ifndef BASICIMAGEPROVIDER_H
#define BASICIMAGEPROVIDER_H

#include <QtGlobal>
#if (QT5)

#include <QtQuick/QQuickImageProvider>
#include <QtCore/QObject>
#include <QtCore/QDebug>
#include <QtCore/QVariant>
#else
#include <QObject>
#include <QDeclarativeImageProvider>
#include <QDebug>
#include <QVariant>
#endif
#ifdef debug
#include <QDebug>
#endif

#include <qorbitermanager.h>

#if (QT5)
class basicImageProvider : public QQuickImageProvider, public QObject
#else
class basicImageProvider : public QDeclarativeImageProvider, public QObject
#endif
{

public:
#if (QT5)
    basicImageProvider():QQuickImageProvider(QQuickImageProvider::Image)
  #else
    basicImageProvider():QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
#endif
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
