#ifndef SCANMANAGER_H
#define SCANMANAGER_H

#include <QObject>
#include <items/mediaitem.h>
#include <services/metadataservicebase.h>
/*!
 * \brief The ScanManager class. Main object that will do the heavy lifting.
 */
class ScanManager : public QObject
{
    Q_OBJECT
public:
    explicit ScanManager(QObject *parent = 0);

signals:

public slots:
    void log(QString msg);
    void addItemForUpdate(MediaItem *item);

private:
    bool registerMetadataProviders();

private:
    QList<MediaItem*> m_itemQueue;
    QList<MediaItem*> m_outgoingQueue;
    QList<MetadataServiceBase*> m_services;
};

#endif // SCANMANAGER_H
