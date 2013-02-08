/*
 *Color fix provided by Emmanuel Granatello
 *https://bugreports.qt-project.org/browse/QTBUG-8737
 */

#ifndef VIDEOWIDGETPLAYER_H
#define VIDEOWIDGETPLAYER_H
#ifndef ANDROID
#ifdef QT5
#include <QtWidgets/QWidget>
#else
#include <QWidget>
#include <Phonon>
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
    bool sourceError;
    QString sourceErrorMsg;

signals:
    void sourceChanged(QUrl f);
    void play();
    void stop();
    void mediaSourceError();
      void mediaSourceErrorMsg();

public slots:
    void setSource(QString f) {
        QString g = f;
        currentSource = g;
    }
    void startPlayback();
    void stopPlayback() { emit stop(); }

    void setCurrentSize(int h, int w) {videoSurface->setFixedSize(h, w);}
    QSize getCurrentSize() {return videoSurface->size();}

    void setSourceErrorMsg(QString e) {sourceErrorMsg = e; emit mediaSourceErrorMsg();}
    QString getSourceErrorMsg(){return sourceErrorMsg;}




};
#endif
#endif // VIDEOWIDGETPLAYER_H
