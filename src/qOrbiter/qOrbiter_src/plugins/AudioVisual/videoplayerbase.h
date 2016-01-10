#ifndef VIDEOPLAYERBASE_H
#define VIDEOPLAYERBASE_H
#ifndef ANDROID
#ifdef QT5
#include <QtWidgets/QWidget>
#include <QtMultimedia>
#else
#include <QWidget>
#include <phonon/Phonon>
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <phonon/VideoWidget>
#include <QUrl>
#include <QVBoxLayout>
#endif


class VideoPlayerBase : public QWidget
{
    Q_OBJECT
public:
    explicit VideoPlayerBase(QWidget *parent = 0);

#ifdef QT4
    Phonon::MediaObject *videoObject;
    Phonon::AudioOutput *videoAudio;
    Phonon::VideoWidget *videoSurface;
#else
    QAbstractAudioOutput *videoAudio;
    QAbstractVideoBuffer *videoObject;

#endif


    QString currentSource;
    qint64 currentTime;
    bool sourceError;
    QString sourceErrorMsg;

signals:
    void sourceChanged(QUrl f);
    void playingMedia();
    void stop();
    void mediaSourceError();
    void mediaSourceErrorMsg(QString);

public slots:
    void setSource(QString f) {
        QString g = f;
        currentSource = g;
    }
    void startPlayback();
    void stopPlayback() { emit stop(); }
    void pause();
    void setPlayBackSpeed();
    void setMediaPosition(int msec);
    void setCurrentSize(int h, int w) {videoSurface->setFixedSize(h, w);}
    QSize getCurrentSize() {return videoSurface->size();}

    void setSourceErrorMsg(QString e) {sourceErrorMsg = e; emit mediaSourceErrorMsg(sourceErrorMsg);}
    QString getSourceErrorMsg(){return sourceErrorMsg;}




};
#endif
#endif // VIDEOPLAYERBASE_H
