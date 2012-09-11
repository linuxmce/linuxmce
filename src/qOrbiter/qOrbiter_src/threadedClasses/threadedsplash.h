#ifndef THREADEDSPLASH_H
#define THREADEDSPLASH_H

#include <QObject>
#if (QT5)
#include <QtQuick/QQuickView>
#else
#include <QtDeclarative/QDeclarativeView>
#endif


/*
  This class is an experiment in subclassing threading based on this page that corrects the docs on threads
  http://labs.qt.nokia.com/2010/06/17/youre-doing-it-wrong/
  */

class ThreadedSplash : public QObject
{
    Q_OBJECT

    Q_PROPERTY (QString qs_progress READ getMessage WRITE setMessage NOTIFY qsProgress)
    Q_PROPERTY (int mi_progress READ getProgress WRITE setProgess NOTIFY iProgress)

public:
    explicit ThreadedSplash(QObject *parent = 0);

    QString qs_progress;
#if (QT5)
    QQuickView *splashView;
#else
    QDeclarativeView *splashView;
#endif
    int mi_progress;
    QUrl qrcPath;

signals:
    void splashReady();
    void iProgress();
    void qsProgress();

public slots:
    void closeSplash(bool);
    void raiseSplash();

    void setMessage(QString s);
    QString getMessage();

    void setProgess(int p);
    int getProgress();



};

#endif // THREADEDSPLASH_H
