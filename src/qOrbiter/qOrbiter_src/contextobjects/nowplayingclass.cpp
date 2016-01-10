#include "nowplayingclass.h"
#include "qOrbiter/qOrbiter.h"
#if (QT5)
NowPlayingClass::NowPlayingClass(QObject *qOrbiter_ptr, QObject *parent) :
    QObject(parent)
  #else
NowPlayingClass::NowPlayingClass(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
  #endif
{

    qOrbiter * ptr = qobject_cast<qOrbiter*>(qOrbiter_ptr);
    QObject::connect(ptr, &qOrbiter::np_storageDeviceChanged, this, &NowPlayingClass::setStorageDevice, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::np_pathChanged, this, &NowPlayingClass::setPath, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::np_filename, this, &NowPlayingClass::setFilePath,Qt::QueuedConnection);

    QObject::connect(ptr, &qOrbiter::np_mediaTitleChanged, this, &NowPlayingClass::setMediaTitle,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::np_album, this, &NowPlayingClass::setAlbum,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::np_track, this, &NowPlayingClass::setTrack,Qt::QueuedConnection);
    QObject::connect(ptr,&qOrbiter::np_channel, this, &NowPlayingClass::setChannel,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::np_network, this, &NowPlayingClass::setNetwork ,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::np_channelID, this, &NowPlayingClass::setChannelID,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::np_program, this, &NowPlayingClass::setProgram,Qt::QueuedConnection);

    QObject::connect(ptr, &qOrbiter::np_director, this, &NowPlayingClass::setDirector,Qt::QueuedConnection);
    QObject::connect(ptr,&qOrbiter::np_episode, this, &NowPlayingClass::setEpisode,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::np_filename, this, &NowPlayingClass::setFilePath,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::np_genre, this, &NowPlayingClass::setGenre,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::np_localpathChanged, this, &NowPlayingClass::setLocalPath,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::np_performer, this, &NowPlayingClass::setPerformers,Qt::QueuedConnection);

    QObject::connect(ptr, &qOrbiter::np_program, this, &NowPlayingClass::setProgram,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::np_releaseDate, this, &NowPlayingClass::setRelease,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::np_title1Changed, this, &NowPlayingClass::setTitle,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::np_title2Changed, this, &NowPlayingClass::setTitle2,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::subtitleChanged, this, &NowPlayingClass::setSubTitle,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::np_synopsisChanged, this, &NowPlayingClass::setSynop,Qt::QueuedConnection);
    QObject::connect(ptr,&qOrbiter::playlistPositionChanged, this, &NowPlayingClass::setPlaylistPostion,Qt::QueuedConnection);

    QObject::connect(ptr,SIGNAL(resetNowPlaying()), this, SLOT(resetData()));
    QObject::connect(ptr, SIGNAL(setPlaylistPosition(int)), this, SLOT(setPlaylistPostion(int)),Qt::QueuedConnection);

    QObject::connect(ptr, &qOrbiter::nowPlayingDeviceListChanged, this, &NowPlayingClass::setNowPlayingDeviceList, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::setNowPlaying, this,&NowPlayingClass::setStatus,Qt::QueuedConnection);
    QObject::connect(ptr,&qOrbiter::streamIdChanged, this, &NowPlayingClass::setStreamID,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::currentScreenChanged, this, &NowPlayingClass::setScreen,Qt::QueuedConnection);
    QObject::connect(ptr,&qOrbiter::objectUpdate, this, &NowPlayingClass::setImageData, Qt::QueuedConnection);

   // QObject::connect(this, SIGNAL(statusMessage(QString)), &w, SLOT(setDceResponse(QString)));

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

