#ifndef FLOORPLANDEVICEPROVIDER_H
#define FLOORPLANDEVICEPROVIDER_H


#include <QtGlobal>
#ifdef QT5
#include <QtQml/QQmlImageProviderBase>

#else
#include <QtDeclarative/QDeclarativeImageProvider>
#endif
#include <datamodels/floorplanmodel.h>
#include <qorbitermanager.h>

#ifdef QT5
class FloorplanDeviceProvider : public QObject, public QQuickImageProvider
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
