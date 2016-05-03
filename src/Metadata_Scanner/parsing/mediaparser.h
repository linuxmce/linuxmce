#ifndef MEDIAPARSER_H
#define MEDIAPARSER_H

#include <QObject>
#include "items/mediaitem.h"

class MediaParser : public QObject
{
    Q_OBJECT
public:
    explicit MediaParser(QObject *parent = 0); 

signals:
    void mediaItemFinished(MediaItem *item);

public slots:

    //work relatedSlots
    QString identifyMedia(QString mediaName);

private:
    void setParsers();

};

#endif // MEDIAPARSER_H
