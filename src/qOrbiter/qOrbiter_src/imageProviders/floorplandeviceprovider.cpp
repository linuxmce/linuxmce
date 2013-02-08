#include <QtGlobal>
#ifdef QT5
//NOTE: Doesn't seem to need anything - is hdr needed?
#include <QtWidgets/QGraphicsPixmapItem>
#else
#include <QtDeclarative/qdeclarative.h>
#endif

#include "floorplandeviceprovider.h"



#ifdef QT5
FloorplanDeviceProvider::FloorplanDeviceProvider(qorbiterManager *ui): QQuickImageProvider(QQuickImageProvider::Pixmap), ui_reference(ui)
#else
FloorplanDeviceProvider::FloorplanDeviceProvider(qorbiterManager *ui): QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap), ui_reference(ui)
#endif
{

}

QPixmap FloorplanDeviceProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QPixmap key;
    QPixmap result;
     if (requestedSize.isValid()) {
            result = key.scaled(requestedSize, Qt::KeepAspectRatio);
        } else {
            result = key;
        }
        *size = result.size();
        return result;
}
