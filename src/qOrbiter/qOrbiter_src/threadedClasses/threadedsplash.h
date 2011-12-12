#ifndef THREADEDSPLASH_H
#define THREADEDSPLASH_H

#include <QThread>

class ThreadedSplash : public QThread
{
    Q_OBJECT
public:
    explicit ThreadedSplash(QObject *parent = 0);

signals:

public slots:

};

#endif // THREADEDSPLASH_H
