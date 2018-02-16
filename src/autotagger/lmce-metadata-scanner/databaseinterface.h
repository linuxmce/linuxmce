#pragma once
#include <QObject>
#include <linuxmcefile.h>

class DatabaseInterface : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseInterface(QObject *parent = 0);

signals:


public slots:
    int findExistingFileId(QString filename);
    QString findIdFromFilename(int fileid);

    bool addMetadataToDatabase(LinuxmceFile * filemetadata);


private:
    bool connectedToDatabase;

private:
    void initDatabaseConnection();


};


