
#include "filedetailsclass.h"

FileDetailsClass::FileDetailsClass(QObject *parent) :
    QObject(parent)
{

}

void FileDetailsClass::setFileMediaType()
{
}


void FileDetailsClass::clear(){
     setScreenshot("");
     titleImage = QImage();

     emit screenShotChanged();
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



    emit synopChanged();
    emit objectChanged();
    emit synopChanged();
    emit titleImageChanged();
    emit mediaTitleChanged();

    emit screenTypeChanged();
    emit titleChanged();
    emit titleChanged2();
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
