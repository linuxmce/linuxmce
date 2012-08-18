#ifndef VIDEOWIDGETPLAYER_H
#define VIDEOWIDGETPLAYER_H
#ifndef ANDROID
#ifdef QT5
#include <QtWidgets/QWidget>
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



class videoWidgetPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit videoWidgetPlayer(QWidget *parent = 0);
    Phonon::MediaObject *videoObject;
    Phonon::AudioOutput *videoAudio;
   Phonon::VideoWidget *videoSurface;
    Phonon::VideoPlayer *qPlayer;

   // QWidget * w;
   // QVBoxLayout *l;

    QString currentSource;
    qint64 currentTime;

signals:
    void sourceChanged(QUrl f);
    void play();
    void stop();

public slots:
    void setSource(QString f) {
        QString g = f;
        currentSource = g;
    }
    void startPlayback();
    void stopPlayback() { emit stop(); }



};
#endif
#endif // VIDEOWIDGETPLAYER_H
