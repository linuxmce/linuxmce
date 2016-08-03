
#include "filedetailsclass.h"
#include "../pluto_media/Define_AttributeType.h"
#include <qOrbiter/qOrbiter.h>
#include "qmap.h"


FileDetailsClass::FileDetailsClass(QObject* qorbiter_ptr, QObject *parent) :
    QObject(parent)
{
    qOrbiter*ptr = qobject_cast<qOrbiter*>(qorbiter_ptr);
    qmlRegisterType<FileDetailsObject>("FileAttribute", 1, 0, "FileAttribute");

    QObject::connect(ptr, &qOrbiter::newFileDetailAttribute, this, &FileDetailsClass::handleNewFileAttribute, Qt::BlockingQueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_programChanged,this, &FileDetailsClass::setProgram,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_mediaTitleChanged,this, &FileDetailsClass::setMediaTitle,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_chanIdChanged, this, &FileDetailsClass::setChannelID, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_episodeChanged,this, &FileDetailsClass::setEpisode,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_performersChanged,this, &FileDetailsClass::setPerformers,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_composersChanged, this, &FileDetailsClass::setComposers, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_directorChanged,this, &FileDetailsClass::setDirector,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_genreChanged,this, &FileDetailsClass::setGenre,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_albumChanged,this, &FileDetailsClass::setAlbum,Qt::QueuedConnection);
    QObject::connect(ptr,&qOrbiter::fd_studioChanged,this, &FileDetailsClass::setStudio, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_trackChanged,this, &FileDetailsClass::setTrack,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_ratingChanged,this, &FileDetailsClass::setRating, Qt::QueuedConnection);

    QObject::connect(ptr, &qOrbiter::fd_imageUrlChanged,this, &FileDetailsClass::setScreenshot, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_titleChanged,this, &FileDetailsClass::setTitle, Qt::QueuedConnection);
    QObject::connect(ptr,&qOrbiter::fd_storageDeviceChanged,this, &FileDetailsClass::setStorageDevice, Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_synopChanged,this, &FileDetailsClass::setSynop,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_channelChanged,this, &FileDetailsClass::setChannel,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_pathChanged,this, &FileDetailsClass::setPath,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_fileChanged,this, &FileDetailsClass::setFile,Qt::QueuedConnection);
    QObject::connect(ptr, &qOrbiter::fd_fileNameChanged,this, &FileDetailsClass::setFilename,Qt::QueuedConnection);
    clear();
}

void FileDetailsClass::handleNewFileAttribute(int attribType, int attribute, QString val)
{
    qDebug() << " Handling  attribute " << attribute << " value:: " << val;

    switch (attribType) {
    case ATTRIBUTETYPE_Director_CONST:m_directorList.append(new FileDetailsObject(attribute, val, attribType));             emit directorChanged();    break;
    case ATTRIBUTETYPE_Performer_CONST:m_performerList.push_back(new FileDetailsObject(attribute, val, attribType));        emit performersChanged();  break;
    case ATTRIBUTETYPE_Album_CONST:m_singleItemMap.insert(attribType, new FileDetailsObject(attribute, val, attribType)) ;  emit albumChanged();       break;
    case ATTRIBUTETYPE_Track_CONST:m_singleItemMap.insert(attribType, new FileDetailsObject(attribute, val, attribType) );  emit trackChanged();       break;
    case ATTRIBUTETYPE_Program_CONST:m_singleItemMap.insert(attribType, new FileDetailsObject(attribute, val, attribType)); emit programChanged();     break;
    case ATTRIBUTETYPE_Rated_CONST: m_singleItemMap.insert(attribType, new FileDetailsObject(attribute, val, attribType) ); emit ratingChanged();      break;
    case ATTRIBUTETYPE_Genre_CONST: m_genreList.append(new FileDetailsObject(attribute, val, attribType));                  emit genreChanged();       break;
    case ATTRIBUTETYPE_Episode_CONST:m_singleItemMap.insert(attribType, new FileDetailsObject(attribute, val, attribType)); emit episodeChanged();     break;
    case ATTRIBUTETYPE_Studio_CONST:m_studioList.append(new FileDetailsObject(attribute, val, attribType));                 emit studioChanged();      break;
    case ATTRIBUTETYPE_ComposerWriter_CONST:m_composerWriterList.append(new FileDetailsObject(attribute, val, attribType)); emit composersChanged();   break;
    case ATTRIBUTETYPE_Album_Artist_CONST:m_albumArtistList.append(new FileDetailsObject(attribute, val, attribType));      emit albumArtistChanged(); break;
        break;
    default:
        qDebug() << " No handler for attribute " << attribute << " value:: " << val;
        break;
    }
}



void FileDetailsClass::setFileMediaType()
{
}


void FileDetailsClass::clear(){
    m_attributeMap.clear();
    m_performerMap.clear();
    m_performerList.clear();
    m_composerWriterList.clear();
    m_studioList.clear();
    m_directorList.clear();
    directors.clear();
    composers.clear();
    composerlist.clear();
    performers.clear();
    performerlist.clear();
    qs_mainTitle.clear();
    qs_mainTitle2.clear();
    qs_subTitle.clear();
    mediatitle.clear();
    program.clear();
    channel.clear();
    channelID.clear();
    episode.clear();
    album.clear();
    track.clear();
    genre.clear();
    releasedate.clear();
    studio.clear();
    rating.clear();
    synop.clear();

    setScreenshot("");
    setFile("");
    setFilename("");
    setSynop("");
    setTitle("");
    setTitle2("");
    setMediaTitle("");
    setAlbum("");
    setTrack("");
    setRunTime("");


    emit objectChanged();
    emit screenTypeChanged();
    emit genreChanged();
    emit albumChanged();
    emit trackChanged();
    emit performersChanged();
    emit composersChanged();
    emit directorChanged();
    emit programChanged();
    emit channelChanged();
    emit episodeChanged();
    emit studioChanged();
    emit ratingChanged();
    emit screenShotChanged();

}

