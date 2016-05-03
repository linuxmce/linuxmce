#ifndef DATABASEINTERACTOR_H
#define DATABASEINTERACTOR_H

#include <QObject>
#include "items/mediaitem.h"
#include "qvector.h"
class DatabaseInteractor : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseInteractor(QObject *parent = 0);

signals:
    void mediaItemUpdated(MediaItem*item);
    void nextFile();

public slots:
    void updateMediaItem(MediaItem *item);


private slots:
    void updateItem();

private:
     QVector<MediaItem*> m_updateQueue;
     bool busyWithFile;
};

#endif // DATABASEINTERACTOR_H
