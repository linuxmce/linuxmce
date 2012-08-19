#include "audioplayerbase.h"

AudioPlayerBase::AudioPlayerBase(QWidget *parent) :
    QWidget(parent)
{
    audioSink = new Phonon::AudioOutput(this);
    visualizationSink = new Phonon::VideoWidget(this);
    audioObject = new Phonon::MediaObject();

    currentAudioSource = "";
}

void AudioPlayerBase::setAudioSource(QString s)
{

}


void AudioPlayerBase::setMediaPosition(int msec)
{
    qint64 mediaPosition = msec;
    audioObject->seek(mediaPosition);
}
