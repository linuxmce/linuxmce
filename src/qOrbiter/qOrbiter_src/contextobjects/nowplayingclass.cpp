#include "nowplayingclass.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
NowPlayingClass::NowPlayingClass(QQuickItem *parent) :
    QQuickItem(parent)
#else
NowPlayingClass::NowPlayingClass(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
#endif
{
    b_mediaPlaying = false;   
    fileImage.load("qrc:/icons/icon.png");
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
 fileImage.load("qrc:/icons/icon.png");
 emit imageChanged();
}

