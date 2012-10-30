#ifndef AUDIOPLAYERBASE_H
#define AUDIOPLAYERBASE_H

#ifdef QT5
#include <QtWidgets/QWidget>

#else
#include <QWidget>
#include <Phonon>
#endif



class AudioPlayerBase : public QWidget
{
    Q_OBJECT
public:
    explicit AudioPlayerBase(QWidget *parent = 0);
    Phonon::AudioOutput * audioSink;
    Phonon::VideoWidget * visualizationSink;
    Phonon::MediaObject * audioObject;

    QString currentAudioSource;
    QString sourceError;

signals:
    void audioSourceError();
    void errorClear();
    void audioSourceChanged();
    void visualizationStarted();

    
public slots:
    void setAudioSource(QString s);
    void setErrorMessage();
    void startMediaPlayback();
    void stopMediaPlayback();
    void setMediaPosition(int msec);
    
};

#endif // AUDIOPLAYERBASE_H
