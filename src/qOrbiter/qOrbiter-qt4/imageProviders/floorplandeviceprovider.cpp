#include <QtGlobal>

#include <QtDeclarative/qdeclarative.h>
#include "floorplandeviceprovider.h"
FloorplanDeviceProvider::FloorplanDeviceProvider(qorbiterManager *ui): QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap), ui_reference(ui)
{

}

QPixmap FloorplanDeviceProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QPixmap key;
    QPixmap result;

    QObject * view = ui_reference->qorbiterUIwin->rootObject();
//NOTE: What is this doing? doesn't work under Qt5.0
/* (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    QDeclarativeItem * iconObj = view->findChild<QDeclarativeItem*>(id);
    QGraphicsPixmapItem * t = (QGraphicsPixmapItem*)iconObj->toGraphicsObject();
*/

     if (requestedSize.isValid()) {
            result = key.scaled(requestedSize, Qt::KeepAspectRatio);
        } else {
            result = key;
        }
        *size = result.size();
        return result;
}
