#include "nowplayingclass.h"

#if (QT5)
NowPlayingClass::NowPlayingClass(QObject *parent) :
    QObject(parent)
#else
NowPlayingClass::NowPlayingClass(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
#endif
{
    b_mediaPlaying = false;   
    fileImage.load("qrc:/icons/icon.png");
    resetData();
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
    setMediaType(0);
    setSynop(""); emit synopChanged();
    setStreamID(0);
    setProgram("");
    setRelease("");
  //  setImageAspectRatio(0.0);
    setImageAspect("");
//    setImage(QImage());
//    setStreamImage(QImage());


}

