#ifndef METADATASERVICEBASE_H
#define METADATASERVICEBASE_H

#include <QObject>
#include "items/mediaitem.h"
#include "qvector.h"


class MetadataServiceBase : public QObject
{
    Q_OBJECT
public:
    explicit MetadataServiceBase(QString service, QObject *parent = 0);

    QString serviceName() const;
    void setServiceName(const QString &serviceName);

signals:
    void updateMediaItem(MediaItem*item);


public slots:
    void processMediaItem(MediaItem *item);
    void setSupportedMediaTypes(QVector<int> mediaTypes);
    bool supportsFormat(int f);

private:
    QString m_serviceName;
    QList<MediaItem*> m_itemList;
    QVector<int> m_SupportedMediaTypes;

private:
    virtual void getMediaInfo() = 0;
    virtual void handleMediaInfoResponse() =0;
};

#endif // METADATASERVICEBASE_H
