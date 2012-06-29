#ifndef FLOORPLANDEVICEPROVIDER_H
#define FLOORPLANDEVICEPROVIDER_H


#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QQmlImageProvider>
#else
#include <QtDeclarative/QDeclarativeImageProvider>
#endif
#include <datamodels/floorplanmodel.h>
#include <qorbitermanager.h>

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
class FloorplanDeviceProvider : public QObject, public QQmlImageProvider
#else
class FloorplanDeviceProvider : public QObject, public QDeclarativeImageProvider
#endif
{
    Q_OBJECT
public:
    explicit FloorplanDeviceProvider(qorbiterManager *ui );
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize& requestedSize );
    
signals:
    
public slots:

private:
    qorbiterManager *ui_reference;
    
};

#endif // FLOORPLANDEVICEPROVIDER_H
