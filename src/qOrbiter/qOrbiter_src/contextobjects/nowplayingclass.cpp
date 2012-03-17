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
   emit newMediaSpeed(speed);

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
    setStreamID(0);
    setProgram("");
   // setImage(QImage());
}

