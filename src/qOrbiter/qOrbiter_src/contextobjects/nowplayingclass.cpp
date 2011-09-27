#include "nowplayingclass.h"

NowPlayingClass::NowPlayingClass(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{

    b_mediaPlaying = false;
    qs_playbackSpeed = "1x";
    i_playbackSpeed = 1;
   // m_iplaylistPosition = 0;


}

void NowPlayingClass::setMediaSpeed(int speed)
{
    if (i_playbackSpeed  = 1)
    {
        i_playbackSpeed = speed * 1;
      // setStringSpeed(QString::number(i_playbackSpeed)+"x");
    }
    else if (i_playbackSpeed = 2||-2)
    {
        i_playbackSpeed = speed * i_playbackSpeed;
       //  setStringSpeed(QString::number(i_playbackSpeed)+"x");
    }
    else if (i_playbackSpeed = 4||-4)
    {
        i_playbackSpeed = speed * i_playbackSpeed;
        // setStringSpeed(QString::number(i_playbackSpeed)+"x");
    }
    else if (i_playbackSpeed = 8 ||-8)
    {
        i_playbackSpeed = speed * i_playbackSpeed;
      //   setStringSpeed(QString::number(i_playbackSpeed)+"x");
    }
    else if (i_playbackSpeed = 16||-16)
    {
        i_playbackSpeed = speed * i_playbackSpeed;
       //  setStringSpeed(QString::number(i_playbackSpeed)+"x");
    }
    else if (i_playbackSpeed = 32||-32)
    {
        i_playbackSpeed = speed * i_playbackSpeed;
       //  setStringSpeed(QString::number(i_playbackSpeed)+"x");
    }
    else if (i_playbackSpeed < 64 )
    {
        i_playbackSpeed = 1 ;
       //  setStringSpeed(QString::number(i_playbackSpeed)+"x");
    }

}

void NowPlayingClass::resetData()
{
    setTitle("");
    setProgram("");
    setSubTitle("");
    setTitle2("");
    genre.clear(); emit genreChanged();
    performers.clear();
    performerlist.clear();emit performersChanged();
    setChannel("");
    setChannelID("");
    setEpisode("");
    setAlbum("");
    directors.clear(); emit directorChanged();
    setDirector("");
    setTrack(""); emit trackChanged();
    setMediaTitle("");
    setDuration("");
    setTimeCode("");
    setMediaType(NULL);
    setSynop(""); emit synopChanged();

}

