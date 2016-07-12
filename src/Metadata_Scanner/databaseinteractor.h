#ifndef DATABASEINTERACTOR_H
#define DATABASEINTERACTOR_H

#include <QObject>
#include "items/mediaitem.h"
#include "qvector.h"
#include "QtSql/QSqlDriver"

class DatabaseInteractor : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseInteractor(QObject *parent = 0);

    bool myqlConnected() const;
    void setMyqlConnected(bool myqlConnected);

signals:
    void mediaItemUpdated(MediaItem*item);
    void nextFile();

public slots:
    void updateMediaItem(MediaItem *item);


private slots:
    void updateItem();
    QString findFileNumber(QString fullFilePath, QString fullFileName);
    QString checkAttributeForFile(QString attribute, QString fileNo, int attributeType);
    QString checkDbForAttribute(QString attribute, int attributeType);
    int findFileMediaType(QString fileNo);

    QString insertNewAttribute(QString attribute, QString attributeType);
    int associatFileWithAttribute(QString fileNo, QString fkAttributeNo);
    int savePic(QString picUrl, QString attribute);

    void setFileMediaTypes(QString file_format, QString sub_type, QString fileNo);


private:
     QVector<MediaItem*> m_updateQueue;
     bool busyWithFile;

     bool m_myqlConnected;
};

#endif // DATABASEINTERACTOR_H
