#include <QtGlobal>
#if (QT_VERSION >= 0x050000)
//NOTE: Doesn't seem to need anything - is hdr needed?
#else
#include <QtDeclarative/qdeclarative.h>
#endif

#include "floorplandeviceprovider.h"



#if (QT_VERSION >= 0x050000)
FloorplanDeviceProvider::FloorplanDeviceProvider(qorbiterManager *ui): QQmlImageProvider(QQmlImageProvider::Pixmap), ui_reference(ui)
#else
FloorplanDeviceProvider::FloorplanDeviceProvider(qorbiterManager *ui): QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap), ui_reference(ui)
#endif
{

}

QPixmap FloorplanDeviceProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QPixmap key;
    QPixmap result;

    QObject * view = ui_reference->qorbiterUIwin->rootObject();
//NOTE: What is this doing? doesn't work under Qt5.0
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    QDeclarativeItem * iconObj = view->findChild<QDeclarativeItem*>(id);
    QGraphicsPixmapItem * t = (QGraphicsPixmapItem*)iconObj->toGraphicsObject();
#endif

     if (requestedSize.isValid()) {
            result = key.scaled(requestedSize, Qt::KeepAspectRatio);
        } else {
            result = key;
        }
        *size = result.size();
        return result;
}
