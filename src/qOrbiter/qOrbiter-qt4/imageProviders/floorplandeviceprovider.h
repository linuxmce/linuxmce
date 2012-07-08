#ifndef FLOORPLANDEVICEPROVIDER_H
#define FLOORPLANDEVICEPROVIDER_H


#include <QtGlobal>
#include <QtDeclarative/QDeclarativeImageProvider>
#include <datamodels/floorplanmodel.h>
#include <qorbitermanager.h>

class FloorplanDeviceProvider : public QObject, public QDeclarativeImageProvider
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
