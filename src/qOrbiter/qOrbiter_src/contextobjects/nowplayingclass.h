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
#if (QT5)
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

/*!
 * \brief The NowPlayingClass class contains information about the currently playing media.
 *\ingroup context_objects
 *This class contains all of the metadata associated with media playing in an entertain area. It contains information about the media and
 *it keeps track of any relevant device information that LinuxMCE provides as well.
 * \warning Please note that this class is subject to pruning as there may be duplicate attributes.
 * \ingroup context_objects
 */
#if (QT5)
class NowPlayingClass : public QObject
        #else
class NowPlayingClass : public QDeclarativeItem
        #endif

{
    Q_OBJECT

    Q_PROPERTY (bool b_mediaPlaying READ getStatus  NOTIFY mediaStatusChanged)/*!< \brief The current playing status of media \ingroup now_playing*/
    Q_PROPERTY (int stream READ getStreamID NOTIFY streamIdChanged)

    Q_PROPERTY (QImage fileImage READ getImage WRITE setImage NOTIFY imageChanged)/*!< \brief The image associated with this file \ingroup now_playing*/
    Q_PROPERTY (QImage streamImage READ getStreamImage WRITE setStreamImage NOTIFY streamImageChanged)/*!< \brief Streamed image. \ingroup now_playing*/
    Q_PROPERTY(QString path READ getPath WRITE setPath NOTIFY pathChanged)/*!< \brief Path of the current media without filename \ingroup now_playing*/
    Q_PROPERTY(QString localpath READ getLocalPath WRITE setLocalPath NOTIFY localPathChanged)/*!< \brief Local path...it means something... \ingroup now_playing*/
    Q_PROPERTY(QString qs_storageDevice READ getStorageDevice READ getStorageDevice NOTIFY storageDeviceChanged)/*!< \brief Storage device of the now playing media \ingroup now_playing*/

    //set now playing - text to assign
    Q_PROPERTY (QString qs_mainTitle READ getTitle WRITE setTitle NOTIFY titleChanged) /*!< \brief Title of the media. \ingroup now_playing*/
    Q_PROPERTY (QString qs_mainTitle2 READ getTitle2 WRITE setTitle2 NOTIFY titleChanged2) /*!< \brief Second line title, like tv program, album, etc \ingroup now_playing*/

    //set now playing - value
    Q_PROPERTY (QString qs_subTitle READ getSubTitle WRITE setSubTitle NOTIFY titleChanged )/*!< \brief Too many damned titles being sent... \ingroup now_playing*/

    //internal reference to know what the current media screen is
    Q_PROPERTY (QString qs_screen READ getScreen WRITE setScreen NOTIFY screenTypeChanged)

    //file path of the file in question
    Q_PROPERTY (QString filepath READ getFilePath WRITE setFilePath NOTIFY filePathChanged)

    //metadata related image url
    Q_PROPERTY (QUrl nowPlayingImageUrl READ getImageUrl WRITE setImageUrl NOTIFY imageUrlChanged)

    //internal playlist position tracker
    Q_PROPERTY (int m_iplaylistPosition READ getPlaylistPosition WRITE setPlaylistPostion NOTIFY playListPositionChanged)/*!< \brief The current playlist position. \ingroup now_playing*/

    //media title variable that can be independant of what is passed initially by now playing
    Q_PROPERTY (QString mediatitle READ getMediaTitle WRITE setMediaTitle NOTIFY mediaTitleChanged)/*!< \brief Another damn  title. \ingroup now_playing*/
    Q_PROPERTY (QString genre READ getGenre WRITE setGenre NOTIFY genreChanged)/*!< \brief Genre \ingroup now_playing*/
    //television related variables
    Q_PROPERTY (QString tvProgram READ getProgram WRITE setProgram NOTIFY programChanged)/*!< \brief The current playing tv program. \ingroup now_playing*/
    Q_PROPERTY (QString channel READ getChannel WRITE setChannel NOTIFY channelChanged)/*!< \brief The current channel. \ingroup now_playing*/
    Q_PROPERTY (QString channelID READ getChannelID WRITE setChannelID NOTIFY chanIDchanged)/*!< \brief Schedules direct id. \ingroup now_playing*/
    Q_PROPERTY (QString network WRITE setNetwork READ getNetwork NOTIFY networkChanged)/*!< \brief The current tv network. \ingroup now_playing*/
    Q_PROPERTY (QString episode READ getEpisode WRITE setEpisode NOTIFY episodeChanged)/*!< \brief The current episode. \ingroup now_playing*/
    //movies

    //audio related
    Q_PROPERTY (QString album READ getAlbum WRITE setAlbum NOTIFY albumChanged)/*!< \brief The current playing album. \ingroup now_playing*/
    Q_PROPERTY (QString track READ getTrack WRITE setTrack NOTIFY trackChanged)/*!< \brief The current track number \ingroup now_playing*/
    Q_PROPERTY (QString performerlist READ getPerformers WRITE setPerformers NOTIFY performersChanged)/*!< \brief '|' seperated list of performers. \ingroup now_playing*/
    //shared
    Q_PROPERTY(QString synop READ getSynop WRITE setSynop NOTIFY synopChanged)/*!< \brief The current media synopsis. \ingroup now_playing*/
    Q_PROPERTY (QString director READ getDirector WRITE setDirector NOTIFY directorChanged)/*!< \brief The director. \ingroup now_playing*/
    Q_PROPERTY(QString releasedate READ getRelease WRITE setRelease NOTIFY rlsChanged)/*!< \brief Release date. \ingroup now_playing*/
    Q_PROPERTY(double imageAspectRatio READ getImageAspectRatio WRITE setImageAspectRatio NOTIFY imageAspectRatioChanged)
    Q_PROPERTY (QString aspect READ getImageAspect WRITE setImageAspect NOTIFY imageAspectChanged ) /*!< \brief Aspect ratio \todo Change now playing aspect to enum. \ingroup now_playing*/

    //device related
    Q_PROPERTY(bool nowPlayingDiscreetAudio READ nowPlayingDiscreetAudio  NOTIFY discreetAudioChanged )
public:
#if (QT5)
    explicit NowPlayingClass(QObject *qOrbiter_ptr, QObject *parent = 0);
#else
    explicit NowPlayingClass(QDeclarativeItem *parent = 0);
#endif

    //general variables - set by now playing slot from dce router when media is started or paused

    QString filepath;
    QString localpath;
    QString qs_screen;
    QString qs_imagePath;
    QString qs_mainTitle;
    QString qs_mainTitle2;
    QString qs_subTitle;
    int i_mediaType;
    int i_streamID;
    int storageDeviceNo;
    QString qs_storageDevice;

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
    QString filename;
    QString path;
    double imageAspectRatio;





signals:
    void discreetAudioChanged();
    void nowPlayingDevicesChanged();
    void streamIdChanged();
    //general signals
    void storageDeviceChanged();
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
    void imageAspectRatioChanged();

    void pathChanged();
    void filePathChanged();
    void fileNameChanged();
    void localPathChanged();
    void FileChanged(QString f);


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

    bool nowPlayingDiscreetAudio() const;
    void resetData();

    void setLocalPath(QString p) {localpath = p; emit localPathChanged();}
    QString getLocalPath(){return localpath;}

    void setStorageDevice(QString device) {
        qs_storageDevice = device;
        if(device=="-1")
        {
            storageDeviceNo=-1;
        }
        else
        {
            storageDeviceNo = qs_storageDevice.toInt();
        }
        emit storageDeviceChanged();
    }


    QString getFilename() {return filename;}
    inline QString getPath() {return path;}
    QString getStorageDevice() {return qs_storageDevice;}
    QString getNetwork() {return network;}
    QString getImageAspect() { return aspect;}
    QString getProgram () {return tvProgram;}


    QUrl getImageUrl () { return nowPlayingImageUrl;}



    QImage getImage() {return fileImage;}
    /*!
     * \brief setStreamImage this function sets the stream image only.
     * This function sets the stream image as opposed to the screenshot, or media image. A stream image is unique in that
     * it is a screen grab from a current playing media device and instead of triggering the screenshotAttributes, it just
     * updates the image.
     * \warning do not request images too quickly
     * \todo add frame govonor for Now playing stream image grab?
     * \param t
     *
     */
    void setStreamImage(QImage t){
        qDebug("Setting stream image");
        streamImage = t;
        emit streamImageChanged();
    }
    QImage getStreamImage() { return streamImage;}
    QString getScreen () {return qs_screen;}
    void setUrl (QString inc_url) {qs_imagePath = inc_url; emit imageUrlChanged();}
    QString getUrl () {return qs_imagePath;}
    QString getTitle () {return qs_mainTitle;}
    QString getTitle2 () {return qs_mainTitle2;}
    QString getSubTitle () {return qs_subTitle;}
    bool getStatus () {return b_mediaPlaying;}
    int  getMediaType () {return i_mediaType;}
    QString getFilePath () {return filepath;}
    //general media getters and setters ----//
    QString getMediaTitle () {return mediatitle;}
    int getPlaylistPosition() {return m_iplaylistPosition;}
    //--tv getters and setters-------------//
    QString getChannel () {return channel;}
    QString getChannelID () {return channelID;}
    QString getEpisode () {return episode;}
    QString getAlbum () {return album;}

    QString getTrack() {return track;}
    QString getPerformers() { return performerlist;}
    QString getDirector() {director = directors.join(" | "); return director;}
    QString getGenre() { return genre;}
    QString getRelease() {return releasedate;}
    inline QString getSynop() {return synop;}
    int getStreamID() {return i_streamID;}

    void clearGraphics(){
        setImage(QImage());
    }

    Q_INVOKABLE int nowPlayingDevice() {return m_NowPlayingDevice;}
    Q_INVOKABLE int nowPlayingAudioDevice(){return m_nowPlayingAudioDevice;}
    Q_INVOKABLE int nowPlayingVideoDevice() {return m_nowPlayingVideoDevice;}
    Q_INVOKABLE int nowPlayingCaptureCard() {return m_nowPlayingCaptureCard;}

     void setImageAspect(const QString i_aspect) { aspect = i_aspect; emit imageAspectChanged();}

private slots:
     void setNowPlayingDiscreetAudio(bool nowPlayingDiscreetAudio);
    void setStreamID(int stream) {i_streamID = stream; emit streamIdChanged();}
    void setSynop(QString s) { synop = s;  emit synopChanged(); }
    void setRelease (QString inc_rls) {releasedate = inc_rls;  emit rlsChanged();}
    void setGenre (QString inc_genre) { if(!genre.contains(inc_genre)) {genre.append(inc_genre+" | "); emit genreChanged();} }
    void setDirector (QString inc_director) {directors << inc_director;  emit directorChanged();}
    void setPerformers (QString inc_performer) { if(!performerlist.contains(inc_performer)) {performerlist.append( inc_performer + " | "); emit performersChanged();}}

    void setScreen(QString inc_screen) {qs_screen = inc_screen; emit screenTypeChanged();}
    void setTitle (QString inc_title) {qs_mainTitle = inc_title; emit titleChanged();}
    void setTitle2 (QString inc_title) {qs_mainTitle2 = inc_title; emit titleChanged2();}
    void setSubTitle (QString inc_subTitle) {qs_subTitle = inc_subTitle; emit titleChanged();}
    void setStatus (bool status) {resetData(); b_mediaPlaying = status; if(b_mediaPlaying==false){clearGraphics(); }  emit mediaStatusChanged(); }
    void setMediaType (int inc_mediaType) {i_mediaType = inc_mediaType; emit mediaTypeChanged();}
    void setFilePath (QString inc_fp) {filepath = inc_fp;  emit filePathChanged();}
    void setMediaTitle (QString inc_mediaTitle) {mediatitle = inc_mediaTitle;  emit mediaTitleChanged();}
    void setPlaylistPostion(int i_pls) { m_iplaylistPosition = i_pls;  emit playListPositionChanged( m_iplaylistPosition);}
    void setChannel (QString inc_channel) {channel = inc_channel;  emit channelChanged();}
    void setChannelID (QString inc_channelID) {channelID = inc_channelID;  emit chanIDchanged();}
    void setEpisode (QString inc_episode) {episode = inc_episode;  emit episodeChanged();}
    void setAlbum (QString inc_album) {  album = inc_album;  emit albumChanged();}
    void setTrack (QString inc_track) {track = inc_track;  emit trackChanged();}
    void setNowPlayingDeviceList(QVariantList d){
        if(d.isEmpty())
            return;

        QVariantMap npDevices = d.at(0).toMap();
        m_NowPlayingDevice = npDevices.value("Now Playing").toInt();
        m_nowPlayingAudioDevice= npDevices.value("Now Playing Audio").toInt();
        m_nowPlayingCaptureCard = npDevices.value("Capture Card").toInt();
        m_nowPlayingVideoDevice = npDevices.value("Video Device").toInt();
        emit nowPlayingDevicesChanged();

        qDebug() << d.at(0).toMap();
        qDebug() << d.at(1).toMap();
    }

    void setImageAspectRatio(float r){ imageAspectRatio = r; emit imageAspectRatioChanged(); qDebug() << "Aspect Ratio" << imageAspectRatio;}
    void setFilename (QString f) {filename = f; emit fileNameChanged();}
    void setPath (QString f) {path = f; emit pathChanged();}
    void setNetwork(const QString bcast_network) {network = bcast_network; emit networkChanged();}

    double getImageAspectRatio(){ return imageAspectRatio;}
    void setProgram(const QString newProgram) {tvProgram = newProgram; emit programChanged();}
    void setImageUrl(const QUrl incImage) {nowPlayingImageUrl = incImage;}
    void setImageData(  QByteArray b) {
        QImage t;
        if( t.loadFromData(b))
        {
            setImage(t);
            emit statusMessage("Now Playing Class::Set Cover Art!");
        }
        else
        {
            emit statusMessage("Now Playing Class::Update Object Image Conversion Failed:");
        }
    }
    void setImage(QImage img) {
        fileImage = img;

        if(!fileImage.isNull()){

            double mi_aspect=((double)fileImage.width() / (double)fileImage.height());
            qDebug() << fileImage.width() << "w x "<< fileImage.height()<<"h.";
            qDebug() <<((double)fileImage.width() / (double)fileImage.height());
            setImageAspectRatio((double)mi_aspect);
            emit statusMessage("Set aspect ratio! ");
            if(mi_aspect >= 1 && mi_aspect < 1.5){
                setImageAspect("cover");
            }else if (mi_aspect >= 1.5 && mi_aspect < 2){
                setImageAspect("wide");
            }else if (mi_aspect > 0 && mi_aspect < 1){
                setImageAspect("poster");
            }
            emit imageChanged();
        }

    }

private:
    long m_NowPlayingDevice;
    long m_nowPlayingAudioDevice;
    long m_nowPlayingVideoDevice;
    long m_nowPlayingCaptureCard;
    bool m_nowPlayingDiscreetAudio;
};

#endif // NOWPLAYINGCLASS_H
