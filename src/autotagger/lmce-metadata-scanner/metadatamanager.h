#pragma once
#include "filenameparser.h"
#include "databaseinterface.h"
#include "networkmetadatamanager.h"

#include <QStringList>
#include <QJsonObject>

class MetadataManager : public QObject
{
    Q_OBJECT
public:
    explicit MetadataManager(QObject *parent = 0);

signals:
    void fileQueueListChanged();
    void fileFinished();

public slots:
    void addFileToQueue(QString incomingFile);

private:
    FilenameParser *m_filenameParser;
    QStringList m_filenameQueue;
    QList<QJsonObject> m_parsedFileData;
    DatabaseInterface * m_database;
    NetworkMetadataManager * m_networkMetadata;

};

