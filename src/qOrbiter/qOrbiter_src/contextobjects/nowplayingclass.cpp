#include "nowplayingclass.h"

NowPlayingClass::NowPlayingClass(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
    b_mediaPlaying = false;   
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
    setMediaType(NULL);
    setSynop(""); emit synopChanged();
    setStreamID(0);
    setProgram("");
   //setImage(QImage());
}

