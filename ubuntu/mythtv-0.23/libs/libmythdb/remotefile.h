#ifndef REMOTEFILE_H_
#define REMOTEFILE_H_

#include <QStringList>
#include <QMutex>

#include "mythexp.h"

class MythContext;
class MythSocket;

class MPUBLIC RemoteFile
{
  public:
    RemoteFile(const QString &url = "",
               bool write = false,
               bool usereadahead = true,
               int retries = -1,
               const QStringList *possibleAuxiliaryFiles = NULL);
   ~RemoteFile();

    bool Open();
    void Close(void);

    long long Seek(long long pos, int whence, long long curpos = -1);

    static bool DeleteFile(const QString &url);
    bool DeleteFile(void);
    static bool Exists(const QString &url);
    bool Exists(void);
    static QString GetFileHash(const QString &url);
    QString GetFileHash(void);
    int Write(const void *data, int size);
    int Read(void *data, int size);
    void Reset(void);

    bool SaveAs(QByteArray &data);

    void SetURL(const QString &url) { path = url; }
    void SetTimeout(bool fast);

    bool isOpen(void) const
        { return sock && controlSock; }
    long long GetFileSize(void) const
        { return filesize; }

    const MythSocket *getSocket(void) const
        { return sock; }
    MythSocket *getSocket(void)
        { return sock; }

    QStringList GetAuxiliaryFiles(void) const
        { return auxfiles; }

  private:
    MythSocket     *openSocket(bool control);

    QString         path;
    bool            usereadahead;
    int             retries;
    long long       filesize;
    bool            timeoutisfast;
    long long       readposition;
    int             recordernum;

    mutable QMutex  lock;
    MythSocket     *controlSock;
    MythSocket     *sock;
    QString         query;

    bool            writemode;

    QStringList     possibleauxfiles;
    QStringList     auxfiles;
};

#endif
