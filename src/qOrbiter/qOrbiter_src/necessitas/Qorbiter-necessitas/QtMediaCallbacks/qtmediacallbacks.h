#ifndef QTMEDIACALLBACKS_H
#define QTMEDIACALLBACKS_H

#include "QtMediaCallbacks_global.h"
#include <QObject>

class QTMEDIACALLBACKSSHARED_EXPORT QtMediaCallbacks: public QObject
{
    Q_OBJECT

 signals:
    void mediaStarted();
    void mediaEnded();
    void trackChanged();
    void progressChanged(int t);
    void totalTrackTimeChanged(int t);
    void currentStatusChanged(QString s);

public:
    QtMediaCallbacks(QObject *parent = 0);
};

#endif // QTMEDIACALLBACKS_H
