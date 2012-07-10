#ifndef AUDIOVISUAL_H
#define AUDIOVISUAL_H


#include <QDeclarativeItem>
#include <phonon/Phonon>

#include <QString>
#include <QUrl>
#include "videowidgetplayer.h"

class AudioVisual : public QDeclarativeItem
{
    Q_OBJECT
public:
    explicit AudioVisual(QDeclarativeItem *parent = 0);
    bool playMedia(const QString& media);
    bool playVideo(const QString& video);
    bool playAudio(const QString& audio);

    
signals:
    
public slots:
    
};



#endif // AUDIOVISUAL_H
