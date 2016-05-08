#ifndef MEDIAITEM_H
#define MEDIAITEM_H

#include <QObject>
#include "qmap.h"
#include "../pluto_main/Define_MediaType.h"
#include "../pluto_media/Define_MediaSubType.h"
#include "../pluto_media/Define_AttributeType.h"



class MediaItem : public QObject
{
    Q_OBJECT
public:
    explicit MediaItem(int fileNumber,QObject *parent = 0);

signals:

public slots:
    void setAttribute(int attributeType, QString attributeData);

private:
   QMap <int, QString> m_mapAttributes;
   int i_dceFileNumber;
};

#endif // MEDIAITEM_H
