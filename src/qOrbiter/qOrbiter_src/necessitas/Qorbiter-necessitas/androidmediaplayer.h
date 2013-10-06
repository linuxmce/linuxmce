#ifndef ANDROIDMEDIAPLAYER_H
#define ANDROIDMEDIAPLAYER_H
#include <jni.h>
#include <QObject>

class androidMediaPlayer : public QObject
{
    Q_OBJECT
public:
    explicit androidMediaPlayer(QObject *parent = 0);
    ~androidMediaPlayer();
    QString currentMedia;
    
signals:
    void mediaStarted();
    void mediaStopped();

    void mediaUrlChanged();
    void mediaUrlSet();


    
public slots:
    bool playMedia(QString url);



private:
    jobject m_qtActivity;
    
};

#endif // ANDROIDMEDIAPLAYER_H
