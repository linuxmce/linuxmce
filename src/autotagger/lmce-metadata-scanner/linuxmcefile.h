#ifndef LINUXMCEFILE_H
#define LINUXMCEFILE_H

#include "QString"
#include "QObject"

class LinuxmceFile : public QObject
{
    Q_OBJECT
public:
    explicit LinuxmceFile(QObject *parent = 0);

signals:

public slots:

private:
    int m_iFileId;
    QString m_fileName;
};

#endif // LINUXMCEFILE_H
