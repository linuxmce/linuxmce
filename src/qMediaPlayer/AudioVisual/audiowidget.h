#ifndef AUDIOWIDGET_H
#define AUDIOWIDGET_H
#ifndef ANDROID

#ifdef QT5
#include <QtWidgets/QWidget>
#else
#include <QWidget>
#include <phonon/Phonon>

#endif


class AudioWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AudioWidget(QWidget *parent = 0);
    Phonon::AudioOutput *audioPath;
    Phonon::MediaObject *audioObject;
    
signals:
    
public slots:
    void start(QString fileToPlay);
    void restart();
    void setNextTrack(QString f);
    void stop();
    void seek(int msec);
    void seekForward(int speed);
    void seekBackward(int speed);
    
};
#endif
#endif // AUDIOWIDGET_H
