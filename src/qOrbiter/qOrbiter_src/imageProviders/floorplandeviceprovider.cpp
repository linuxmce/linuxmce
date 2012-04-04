#include "floorplandeviceprovider.h"
#include <qdeclarative.h>

FloorplanDeviceProvider::FloorplanDeviceProvider(qorbiterManager *ui): QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap), ui_reference(ui)
{

}

QPixmap FloorplanDeviceProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QPixmap key;
    QPixmap result;

    QObject * view = ui_reference->qorbiterUIwin->rootObject();
    QDeclarativeItem * iconObj = view->findChild<QDeclarativeItem*>(id);
    QGraphicsPixmapItem * t = (QGraphicsPixmapItem*)iconObj->toGraphicsObject();

     if (requestedSize.isValid()) {
            result = key.scaled(requestedSize, Qt::KeepAspectRatio);
        } else {
            result = key;
        }
        *size = result.size();
        return result;
}
