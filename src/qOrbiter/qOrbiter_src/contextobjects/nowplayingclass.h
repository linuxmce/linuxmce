/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
   Langston Ball  golgoj4@gmail.com
    QOrbiter is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QOrbiter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QOrbiter.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef NOWPLAYINGCLASS_H
#define NOWPLAYINGCLASS_H

/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
   Langston Ball  golgoj4@gmail.com
    QOrbiter is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QOrbiter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QOrbiter.  If not, see <http://www.gnu.org/licenses/>.

  This class is a c++ representation of the 'now playing' button for LinuxMCE It purpose is to provide simple, api like
  access to the now playing button properties as well as automatically indicate the state of media that playing and any
  other relevant data related to the now playing button. For example, it holds the screen associated with the currently
  playing media type so that when clicked, the proper remote is shown.

  Currently, this clas will hold vars for ALL mediatypes and its up to the designer to add the proper ones
  I will try to note what is what, but i cant guarantee 100% accuracy at this time
  */

#include <QtGlobal>
#if (QT_VERSION >= 0x050000)
#include <QtQuick/QQuickPaintedItem>
#include <QtQuick/QQuickItem>
#else
#include <QtDeclarative/QtDeclarative>
#endif
#include <QtGui/QImage>
#include <QtCore/QTime>
#ifdef debug
#include <QtCore/QDebug>
#endif
#ifdef __ANDROID__
#include <QFile>
#include <QImageReader>
#endif

#if (QT_VERSION >= 0x050000)
class NowPlayingClass : public QQuickItem
#else
class NowPlayingClass : public QDeclarativeItem
#endif

{
    Q_OBJECT

    Q_PROPERTY (bool b_mediaPlaying READ getStatus WRITE setStatus NOTIFY mediaStatusChanged) //property to know if media is playing

    Q_PROPERTY (QImage fileImage READ getImage WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY (QImage streamImage READ getStreamImage WRITE setStreamImage NOTIFY streamImageChanged)


    //set now playing - text to assign
    Q_PROPERTY (QString qs_mainTitle READ getTitle WRITE setTitle NOTIFY titleChanged) //the text set sent by now playing command
    Q_PROPERTY (QString qs_mainTitle2 READ getTitle2 WRITE setTitle2 NOTIFY titleChanged2) // in the case of audio, multiple metada value are sent  thus title 2

    //set now playing - value
    Q_PROPERTY (QString qs_subTitle READ getSubTitle WRITE setSubTitle NOTIFY titleChanged )//the secondary text sent in a seperate variable from now playing

    //internal reference to know what the current media screen is
    Q_PROPERTY (QString qs_screen READ getScreen WRITE setScreen NOTIFY screenTypeChanged)

    //file path of the file in question
    Q_PROPERTY (QString filepath READ getFilePath WRITE setFilePath NOTIFY filePathChanged)

    //metadata related image url
    Q_PROPERTY (QUrl nowPlayingImageUrl READ getImageUrl WRITE setImageUrl NOTIFY imageUrlChanged)

    //internal playlist position tracker
    Q_PROPERTY (int m_iplaylistPosition READ getPlaylistPosition WRITE setPlaylistPostion NOTIFY playListPositionChanged)

    //media title variable that can be independant of what is passed initially by now playing
    Q_PROPERTY (QString mediatitle READ getMediaTitle WRITE setMediaTitle NOTIFY mediaTitleChanged)
    Q_PROPERTY (QString genre READ getGenre WRITE setGenre NOTIFY genreChanged)
    //television related variables
    Q_PROPERTY (QString tvProgram READ getProgram WRITE setProgram NOTIFY programChanged)
    Q_PROPERTY (QString channel READ getChannel WRITE setChannel NOTIFY channelChanged)
    Q_PROPERTY (QString channelID READ getChannelID WRITE setChannelID NOTIFY chanIDchanged)
    Q_PROPERTY (QString network WRITE setNetwork READ getNetwork NOTIFY networkChanged)
    Q_PROPERTY (QString episode READ getEpisode WRITE setEpisode NOTIFY episodeChanged)
    //movies

    //audio related
    Q_PROPERTY (QString album READ getAlbum WRITE setAlbum NOTIFY albumChanged)
    Q_PROPERTY (QString track READ getTrack WRITE setTrack NOTIFY trackChanged)
    Q_PROPERTY (QString performerlist READ getPerformers WRITE setPerformers NOTIFY performersChanged)
    //shared
    Q_PROPERTY(QString synop READ getSynop WRITE setSynop NOTIFY synopChanged)
    Q_PROPERTY (QString director READ getDirector WRITE setDirector NOTIFY directorChanged)
    Q_PROPERTY(QString releasedate READ getRelease WRITE setRelease NOTIFY rlsChanged)

    Q_PROPERTY (QString aspect READ getImageAspect WRITE setImageAspect NOTIFY imageAspectChanged )
public:
#if (QT_VERSION >= 0x050000)
    explicit NowPlayingClass(QQuickItem *parent = 0);
#else
    explicit NowPlayingClass(QDeclarativeItem *parent = 0);
#endif



    //general variables - set by now playing slot from dce router when media is started or paused
    QString filepath;
    QString qs_screen;
    QString qs_imagePath;
    QString qs_mainTitle;
    QString qs_mainTitle2;
    QString qs_subTitle;
    int i_mediaType;
    int i_streamID;

    //dont laugh, not sure where these come from but are relevant for other functions
    bool b_mediaPlaying;

    QUrl nowPlayingImageUrl;
    int m_iplaylistPosition;
    QImage fileImage;
    QImage streamImage;

    //media  related--------------------------
    QString mediatitle; //special if the media itself for some reason has a title different than the now playing
    QString synop;
    //television related
    QString tvProgram;
    QString channel;
    QString channelID;
    QString network;
    QString episode;
    QString director;
    QStringList directors;
    //audio related
    QStringList performers;
    QString performerlist;
    QString album;
    QString track;
    QString genre;
    QStringList composer;
    QString releasedate;
    QString aspect;


signals:
    //general signals
    void mediaTitleChanged();
    void playlistChanged();
    void mediaChanged();
    void mediaEnded();
    void mediaStarted();
    void imageUrlChanged();
    void imageChanged();
    void streamImageChanged();
    void screenTypeChanged();
    void titleChanged();
    void titleChanged2();
    void mediaTypeChanged();
    void mediaStatusChanged();
    void filePathChanged();
    void mediaSpeedChanged();
    void playListPositionChanged(int p);
    void genreChanged();
    void rlsChanged();
    void synopChanged();
    void statusMessage(QString msg);
    //audio signals
    void albumChanged();
    void trackChanged();
    void performersChanged();
    void composerChanged();
    //video signals
    void directorChanged();

    //games signals

    //dvd signals

    //mythtv, vdr and live tv signals
    void programChanged();
    void channelChanged();
    void episodeChanged();
    void networkChanged();
    void chanIDchanged();
    void imageAspectChanged();

public slots:
    void resetData();

    void setNetwork(QString bcast_network) {network = bcast_network; emit networkChanged();}
    QString getNetwork() {return network;}

    void setImageAspect(QString i_aspect) { aspect = i_aspect; emit imageAspectChanged();}
    QString getImageAspect() { return aspect;}

    void setProgram(QString newProgram) {tvProgram = newProgram; emit programChanged();}
    QString getProgram () {return tvProgram;}

    void setImageUrl(QUrl incImage) {nowPlayingImageUrl = incImage;}

    QUrl getImageUrl () { return nowPlayingImageUrl;}

    void setImageData(QImage t) {
        emit statusMessage("Now Playing Class::Recieved Image data");

        setImage(t);
    }
    void setImageData( char data, int iData_size) {
       emit statusMessage("Now Playing Class::Android ARM Now Playing Process Called");
        QImage t;
        const uchar *localDat =(uchar*)&data;
        int dataSize = iData_size;
        if( t.loadFromData(localDat, dataSize, "4"))
        {

            setImage(t);
           emit statusMessage("Now Playing Class::Set Cover Art");
        }
        else
        {
           emit statusMessage("Now Playing Class::Update Object Image Conversion Failed:");
        }
    }


    void setImage(QImage img) {fileImage = img; emit imageChanged();}
    QImage getImage() {return fileImage;}

    void setStreamImage(QImage t){
        qDebug("Setting stream image");
        streamImage = t;
        emit streamImageChanged();
    }
    QImage getStreamImage() { return streamImage;}

    void setScreen(QString inc_screen) {qs_screen = inc_screen; emit screenTypeChanged();}
    QString getScreen () {return qs_screen;}

    void setUrl (QString inc_url) {qs_imagePath = inc_url; emit imageUrlChanged();}
    QString getUrl () {return qs_imagePath;}

    void setTitle (QString inc_title) {qs_mainTitle = inc_title; emit titleChanged();}
    QString getTitle () {return qs_mainTitle;}

    void setTitle2 (QString inc_title) {qs_mainTitle2 = inc_title; emit titleChanged2();}
    QString getTitle2 () {return qs_mainTitle2;}

    void setSubTitle (QString inc_subTitle) {qs_subTitle = inc_subTitle; emit titleChanged();}
    QString getSubTitle () {return qs_subTitle;}

    void setStatus (bool status) {resetData(); b_mediaPlaying = status;  emit mediaStatusChanged(); }
    bool getStatus () {return b_mediaPlaying;}

    void setMediaType (int inc_mediaType) {i_mediaType = inc_mediaType; emit mediaTypeChanged();}
    int  getMediaType () {return i_mediaType;}

    void setFilePath (QString inc_fp) {filepath = inc_fp;  emit filePathChanged();}
    QString getFilePath () {return filepath;}

    //general media getters and setters ----//

    void setMediaTitle (QString inc_mediaTitle) {mediatitle = inc_mediaTitle;  emit mediaTitleChanged();}
    QString getMediaTitle () {return mediatitle;}

    void setPlaylistPostion(int i_pls) { m_iplaylistPosition = i_pls;  emit playListPositionChanged( m_iplaylistPosition);}
    int getPlaylistPosition() {return m_iplaylistPosition;}

    //--tv getters and setters-------------//

    void setChannel (QString inc_channel) {channel = inc_channel;  emit channelChanged();}
    QString getChannel () {return channel;}

    void setChannelID (QString inc_channelID) {channelID = inc_channelID;  emit chanIDchanged();}
    QString getChannelID () {return channelID;}

    void setEpisode (QString inc_episode) {episode = inc_episode;  emit episodeChanged();}
    QString getEpisode () {return episode;}

    //-----audio getters and setter--------//
    void setAlbum (QString inc_album) {album = inc_album;  emit albumChanged();}
    QString getAlbum () {return album;}

    void setTrack (QString inc_track) {track = inc_track;  emit trackChanged();}
    QString getTrack() {return track;}

    void setPerformers (QString inc_performer) { if(!performerlist.contains(inc_performer)) {performerlist.append( inc_performer + " | "); emit performersChanged();}}
    QString getPerformers() { return performerlist;}

    void setDirector (QString inc_director) {directors << inc_director;  emit directorChanged();}
    QString getDirector() {director = directors.join(" | "); return director;}

    void setGenre (QString inc_genre) { if(!genre.contains(inc_genre)) {genre.append(inc_genre+" | "); emit genreChanged();} }
    QString getGenre() { return genre;}

    void setRelease (QString inc_rls) {releasedate = inc_rls;  emit rlsChanged();}
    QString getRelease() {return releasedate;}

    inline QString getSynop() {return synop;}
    inline void setSynop(QString s) { synop = s;  emit synopChanged(); }

    void setStreamID(int stream) {i_streamID = stream;}
    int getStreamID() {return i_streamID;}
};

#endif // NOWPLAYINGCLASS_H
