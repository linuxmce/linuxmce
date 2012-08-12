#ifndef FILEDETAILSCLASS_H
#define FILEDETAILSCLASS_H

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

  The purpose of this class is to make sure the current file details are always availble to the file details object in qml
  we are using a qobject because its construction lends itself to multiple qml types, such as list models, images, etc.
  By implementing the file details in this manner, we can provide a more complex object to the file details QML implementation,
  making it more flexible for the user interface designers

  Currently, the architecture of attributes in the pluto_media table / the linuxmce ecosystem lends itself to a many to one
  relationship. This present a unique challenge in that for any given attribute, there can be multiple connected attributes
  the file has. So, our Qt object must be allowed to grow and shrink as needed.
  */


#include <QObject>
#include <QStringList>
#include <QImage>
#include <QUrl>

#ifdef debug
#include <QDebug>
#endif


class FileDetailsClass : public QObject
{
    Q_OBJECT

    Q_PROPERTY (bool showDetails READ isVisible WRITE setVisible NOTIFY VisibleChanged)
    Q_PROPERTY(QString file READ getFile WRITE setFile NOTIFY FileChanged)
    Q_PROPERTY(QString objecttitle READ getTitle WRITE setTitle NOTIFY objectChanged)
    Q_PROPERTY(QString synop READ getSynop WRITE setSynop NOTIFY synopChanged)
    Q_PROPERTY(QUrl screenshot READ getScreenShot WRITE setScreenshot NOTIFY imageChanged)
    Q_PROPERTY(QString filename READ getFilename WRITE setFilename NOTIFY fileNameChanged)
    Q_PROPERTY(QString path READ getPath WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QString qs_storageDevice READ getStorageDevice READ getStorageDevice NOTIFY storageDeviceChanged)

    Q_PROPERTY(QImage titleImage READ getTitleImage WRITE setTitleImage NOTIFY titleImageChanged)
   // Q_PROPERTY(QImage program READ getProgramImage WRITE setProgramImage NOTIFY objectChanged)

    //media title variable that can be independant of what is passed initially by now playing
    Q_PROPERTY(QString qs_mainTitle READ getTitle WRITE setTitle NOTIFY titleChanged )
    Q_PROPERTY (QString mediatitle READ getMediaTitle WRITE setMediaTitle NOTIFY mediaTitleChanged)
    Q_PROPERTY (QString genre READ getGenre WRITE setGenre NOTIFY genreChanged)
    Q_PROPERTY (QString studio READ getStudio WRITE setStudio NOTIFY studioChanged)
    //television related variables
    Q_PROPERTY (QString program READ getProgram WRITE setProgram NOTIFY programChanged)
    Q_PROPERTY (QString channel READ getChannel WRITE setChannel NOTIFY channelChanged)
    Q_PROPERTY (QString channelID READ getChannelID WRITE setChannelID NOTIFY channelChanged)
    Q_PROPERTY (QString episode READ getEpisode WRITE setEpisode NOTIFY episodeChanged)
    Q_PROPERTY (QString director READ getDirector WRITE setDirector NOTIFY directorChanged)
    //audio related
    Q_PROPERTY (QString album READ getAlbum WRITE setAlbum NOTIFY albumChanged)
    Q_PROPERTY (QString track READ getTrack WRITE setTrack NOTIFY trackChanged)
    Q_PROPERTY (QString performerlist READ getPerformers WRITE setPerformers NOTIFY performersChanged)
    Q_PROPERTY (QStringList performers READ getPerformerList)
    Q_PROPERTY (QString composerlist READ getComposers WRITE setComposers NOTIFY composersChanged)
    Q_PROPERTY (QStringList composers READ getComposerList)
    Q_PROPERTY (QString aspect READ getImageAspect WRITE setImageAspect NOTIFY imageAspectChanged )
    Q_PROPERTY (int i_aspectH READ getAspectH WRITE setAspectH NOTIFY aspectHChanged)
    Q_PROPERTY (int i_aspectW READ getAspectW WRITE setAspectW NOTIFY aspectWChanged)

public:
    explicit FileDetailsClass(QObject *parent = 0);

    QString objecttitle;
    int i_aspectH; //height then width
    int i_aspectW; //height then width
    QString aspect;
    QImage titleImage;
    QString file;
    QImage programImage;
    QUrl Season;
    QUrl imdb;
    QString qs_mainTitle;
    QString qs_mainTitle2;
    QString qs_subTitle;
    QString mediatitle;
    QString studio;

    //television related
    QString program;
    QString channel;
    QString channelID;
    QString episode;
    QString director;
    QStringList directors;

    //audio related
    QStringList performers;
    QString performerlist;
    QString album;
    QString track;
    QString genre;
    QStringList composers;
    QString composerlist;
    QString releasedate;
    QString rating;

    QImage qi_screenshot;
    QImage qi_bgimage;
    bool showDetails;
    QString synop;
    QUrl screenshot;
    QUrl bgImage;
    QString filename;
    QString path;

    //storage device related
    QString qs_storageDevice;
    int storageDeviceNo;


signals:
    void storageDeviceChanged();
    void objectChanged();
    void pathChanged();
    void synopChanged();
    void fileNameChanged();
    void FileChanged(QString f);
    void VisibleChanged(bool vis);
    //void ImageChanged(QUrl);
    void titleImageChanged();
    //general signals
    void mediaTitleChanged();
    void playlistChanged();
    void mediaChanged();
    void mediaEnded();
    void mediaStarted();
    void studioChanged();
    void imageChanged();
    void screenTypeChanged();
    void titleChanged();
    void titleChanged2();
    void mediaTypeChanged();
    void mediaStatusChanged();
    void filePathChanged();
    void mediaSpeedChanged();
    void playListPositionChanged();
    void genreChanged();
    void imageAspectChanged();
    void aspectHChanged();
    void aspectWChanged();
    void ratingChanged();

    //audio signals
    void albumChanged();
    void trackChanged();
    void performersChanged();
    void composersChanged();

    //video signals
    void directorChanged();

    //games signals

    //dvd signals

    //mythtv, vdr and live tv signals
    void programChanged();
    void channelChanged();
    void episodeChanged();

public slots:
    void clear();

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

    QString getStorageDevice() {return qs_storageDevice;}



    void setProgram(QString newProgram) {program = newProgram;  emit programChanged();}
    QString getProgram () {return program;}

    void setTitle (QString inc_title) {qs_mainTitle = inc_title; emit titleChanged();}
    QString getTitle () {return qs_mainTitle;}

    void setTitle2 (QString inc_title) {qs_mainTitle2 = inc_title; emit titleChanged2();}
    QString getTitle2 () {return qs_mainTitle2;}

    void setSubTitle (QString inc_subTitle) {qs_subTitle = inc_subTitle; emit titleChanged();}
    QString getSubTitle () {return qs_subTitle;}


    //general media getters and setters ----//

    void setMediaTitle (QString inc_mediaTitle) {mediatitle = inc_mediaTitle;  emit mediaTitleChanged();}
    QString getMediaTitle () {return mediatitle;}

    void setStudio (QString inc_studio) {channel = inc_studio;  emit studioChanged();}
    QString getStudio () {return studio;}

    //--tv getters and setters-------------//

    void setChannel (QString inc_channel) {channel = inc_channel;  emit channelChanged();}
    QString getChannel () {return channel;}

    void setChannelID (QString inc_channelID) {channelID = inc_channelID;  emit channelChanged();}
    QString getChannelID () {return channelID;}

    void setEpisode (QString inc_episode) {episode = inc_episode;  emit episodeChanged();}
    QString getEpisode () {return episode;}

    //-----audio getters and setter--------//
    void setAlbum (QString inc_album) {album = inc_album;  emit albumChanged();}
    QString getAlbum () {return album;}

    void setTrack (QString inc_track) {track = inc_track;  emit trackChanged();}
    QString getTrack() {return track;}

    void setPerformers (QString inc_performer) {performers << inc_performer; emit performersChanged();}
    QString getPerformers() {performerlist = performers.join(" | "); return performerlist;}
    QStringList getPerformerList() {return performers;}

    void setComposers (QString inc_composer) {composers << inc_composer;  emit composersChanged();}
    QString getComposers() {composerlist = composers.join(" | "); return composerlist;}
    QStringList getComposerList() {return composers;}

    void setDirector (QString inc_director) {directors << inc_director;  emit directorChanged();}
    QString getDirector() {director = directors.join(" | "); return director;}

    void setGenre (QString inc_genre) {genre.append(inc_genre+" | ");  emit genreChanged();}
    QString getGenre() { return genre.left(genre.indexOf(" | "));}

    void setFileMediaType();

    void setAspectH(int h) {i_aspectH = h ; emit aspectHChanged();}
    int getAspectH() { return i_aspectH;}

    void setAspectW(int w) {i_aspectW = w ; emit aspectWChanged();}
    int getAspectW() { return i_aspectW;}

    void setImageAspect(QString i_aspect) { aspect = i_aspect; emit imageAspectChanged();}
    QString getImageAspect() {return aspect;}

    inline QString getFile () {return file;}
    inline void setFile(QString incFile) { file = incFile; emit FileChanged(file); }


    inline void setTitleImage (QImage inc_t) {titleImage = inc_t; emit imageChanged();}
    inline QImage getTitleImage () {return titleImage;}


    inline void setProgramImage (QImage inc_p) {programImage = inc_p; emit objectChanged();}
    inline QImage getProgramImage () {return programImage;}


    // Q_INVOKABLE void setSelectionStatus(QString format);
    // Q_INVOKABLE bool getSelectionStatus();


    inline QString getSynop() {return synop;}
    inline void setSynop(QString s) { synop = s; emit synopChanged(); }

    inline void setScreenshot(QUrl u) {screenshot = u;}
    inline QUrl getScreenShot() {return screenshot;}

    inline QString getFilename() {return filename;}
    inline void setFilename (QString f) {filename = f; emit fileNameChanged();}

    inline QString getPath() {return path;}
    inline void setPath (QString f) {path = f; emit pathChanged();}


    inline void setScreenshotimage(QImage img) {qi_screenshot= img; emit imageChanged();}
    inline QImage getScreenshotimage() {return qi_screenshot;}


    void setBGimage(QImage img) {qi_bgimage= img;}
    QImage getBGimage() {return qi_bgimage;}


    inline bool isVisible () { return showDetails; }
    inline void setVisible (bool state) { clear(); showDetails = state; emit VisibleChanged( showDetails);}

    // Q_INVOKABLE void setSelectionStatus(QString format);
    // Q_INVOKABLE bool getSelectionStatus();

};

#endif // FILEDETAILSCLASS_H
