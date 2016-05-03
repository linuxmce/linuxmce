#ifndef ABSTRACTMETADATAPROVIDER_H
#define ABSTRACTMETADATAPROVIDER_H

#include <QObject>
#include <QVector>

class AbstractMetadataProvider : public QObject
{
    Q_OBJECT
public:
    explicit AbstractMetadataProvider(QString providerName, QObject *parent = 0 );

signals:
    void mediaIdentified();

public slots:
    //housekeeping
    virtual void setSupportedMediaTypes(QVector<int> supportedMedia);
    QVector<int> getSupportedMediaTypes();

private:
    QString m_displayName;
    QVector<int> m_supportedMediaTypes;

};

#endif // ABSTRACTMETADATAPROVIDER_H
