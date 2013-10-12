#ifndef QTMEDIACALLBACKS_H
#define QTMEDIACALLBACKS_H

#include "QtMediaCallbacks_global.h"
#include <QObject>
#include <QDebug>

class QTMEDIACALLBACKSSHARED_EXPORT QtMediaCallbacks: public QObject
{
    Q_OBJECT

    bool playing;

signals:
    void mediaStarted();
    void mediaEnded();
    void trackChanged();
    void progressChanged(int t);
    void totalTrackTimeChanged(int t);
    void currentStatusChanged(QString s);

public:
    QtMediaCallbacks(QObject *parent = 0){}


    void updatePlayingStatus(){emit mediaStarted();}

    void mediaStopped() {emit mediaEnded();}
    void mediaBegan() {emit mediaStarted();}
    void setTotalTime(int t){qDebug () << "sending total time changed from shared lib c++"; emit totalTrackTimeChanged(t);}
};

#endif // QTMEDIACALLBACKS_H
