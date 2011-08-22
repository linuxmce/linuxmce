#include "nowplayingclass.h"

NowPlayingClass::NowPlayingClass(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{

    b_mediaPlaying = false;
}

void NowPlayingClass::setMediaSpeed(int speed)
{
    if (i_playbackSpeed = 1)
    {
        i_playbackSpeed = speed * 2;
    }
}
