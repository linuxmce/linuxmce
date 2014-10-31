#ifndef ANDROIDJNIPLAYER_H
#define ANDROIDJNIPLAYER_H

#include <QObject>
#include <jni.h>
class MediaManager;
class AndroidJniPlayer : public QObject
{
    Q_OBJECT
public:
    explicit AndroidJniPlayer(MediaManager *controller,QObject *parent = 0);
    


signals:
    

public slots:
//    void playMedia(QString url);
//    void stopMedia();
//    void startAudioService();
//    void setVolume(double vol);
//    void pauseMedia();
//    void resumeMedia();

    bool initializeConnections();
    
private:
    jobject m_qtActivity;
    jobject m_audioService;
    MediaManager *mp_mediaManager;
};

#endif // ANDROIDJNIPLAYER_H
