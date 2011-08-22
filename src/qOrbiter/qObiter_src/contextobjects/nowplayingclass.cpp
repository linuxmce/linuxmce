#include "nowplayingclass.h"

NowPlayingClass::NowPlayingClass(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{

    b_mediaPlaying = false;
    qs_playbackSpeed = "1x";
    i_playbackSpeed = 1;
}

void NowPlayingClass::setMediaSpeed(int speed)
{
    if (i_playbackSpeed  = 1)
    {
        i_playbackSpeed = speed * 2;
       setStringSpeed(QString::number(i_playbackSpeed)+"x");
    }
    else if (i_playbackSpeed = 2||-2)
    {
        i_playbackSpeed = speed * i_playbackSpeed;
         setStringSpeed(QString::number(i_playbackSpeed)+"x");
    }
    else if (i_playbackSpeed = 4||-4)
    {
        i_playbackSpeed = speed * i_playbackSpeed;
         setStringSpeed(QString::number(i_playbackSpeed)+"x");
    }
    else if (i_playbackSpeed = 8||-8)
    {
        i_playbackSpeed = speed * i_playbackSpeed;
         setStringSpeed(QString::number(i_playbackSpeed)+"x");
    }
    else if (i_playbackSpeed = 16||-16)
    {
        i_playbackSpeed = speed * i_playbackSpeed;
         setStringSpeed(QString::number(i_playbackSpeed)+"x");
    }
    else if (i_playbackSpeed = 64||-64)
    {
        i_playbackSpeed = 1 * speed;
         setStringSpeed(QString::number(i_playbackSpeed)+"x");
    }


}
