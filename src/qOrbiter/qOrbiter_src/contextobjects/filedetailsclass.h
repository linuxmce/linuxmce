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
#include "QQmlListProperty"

#ifdef debug
#include <QDebug>
#endif
#include "qmap.h"
#include "../pluto_media/Define_AttributeType.h"

/*!
 * \brief The FileDetailsObject class. Used to represent a single Attribute object
 */
class FileDetailsObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString attribute READ attribute WRITE setAttribute NOTIFY attributeChanged)
    Q_PROPERTY(int attributeValue READ attributeValue WRITE setAttributeValue NOTIFY attributeValueChanged)
    Q_PROPERTY(int attributeType READ attributeType WRITE setAttributeType NOTIFY attributeTypeChanged)

public:
    FileDetailsObject(int a, QString b, int c):m_attributeValue(a), m_attribute(b), m_attributeType(c) { }
    FileDetailsObject(){}

    QString attribute() const{ return m_attribute; }
    void setAttribute(const QString &attribute){ m_attribute= attribute; emit attributeChanged();}

    int attributeValue() const{ return m_attributeValue; }
    void setAttributeValue(int attributeValue) { m_attributeValue = attributeValue; emit attributeValueChanged(); }

    int attributeType() {return m_attributeType;}
    void setAttributeType(int attributeType){m_attributeType = attributeType; emit attributeTypeChanged(); }

signals:
    void attributeValueChanged();
    void attributeChanged();
    void attributeTypeChanged();
private:
    QString m_attribute;
    int m_attributeValue;
    int m_attributeType;
};

/*!
 * \brief The FileDetailsClass class is responsible for presenting a files properties in one object.
 * \ingroup context_objects
 * \note Access properties of this object using the prefix 'filedetailsclass'.
 */
class FileDetailsClass : public QObject
{
    Q_OBJECT

    Q_PROPERTY (bool showDetails READ isVisible WRITE setVisible NOTIFY VisibleChanged)/*!< \brief If the item is showing or not. Used for popups. \ingroup file_details */
    Q_PROPERTY(QString file READ getFile WRITE setFile NOTIFY FileChanged)/*!< \brief The current file with full filepath \ingroup file_details */
    Q_PROPERTY(QString objecttitle READ getTitle WRITE setTitle NOTIFY objectChanged)/*!< \brief The current media objects title, unparsed.  \ingroup file_details*/
    Q_PROPERTY(QString synop READ getSynop WRITE setSynop NOTIFY synopChanged)/*!< \brief The Current media synopsis \ingroup file_details*/
    Q_PROPERTY(QString screenshot READ getScreenShot WRITE setScreenshot NOTIFY screenShotChanged)/*!< \brief Contains url to the current screenshot \ingroup file_details*/
    Q_PROPERTY(QString filename READ getFilename WRITE setFilename NOTIFY fileNameChanged)/*!< \brief Filename of the current file, with no path. \ingroup file_details*/
    Q_PROPERTY(QString path READ getPath WRITE setPath NOTIFY pathChanged)/*!< \brief Path of file, minus the filename. \ingroup file_details*/
    Q_PROPERTY(QString qs_storageDevice READ getStorageDevice READ getStorageDevice NOTIFY storageDeviceChanged)/*!< \brief The string value of the storage device. \ingroup file_details*/


    // Q_PROPERTY(QImage program READ getProgramImage WRITE setProgramImage NOTIFY objectChanged)
    Q_PROPERTY(QString rating READ getRating WRITE setRating NOTIFY ratingChanged)/*!< \brief The rating if availible of the media.  \ingroup file_details*/
    //media title variable that can be independant of what is passed initially by now playing
    Q_PROPERTY(QString qs_mainTitle READ getTitle WRITE setTitle NOTIFY titleChanged )/*!< \brief The main title of the media \ingroup file_details*/
    Q_PROPERTY (QString mediatitle READ getMediaTitle WRITE setMediaTitle NOTIFY mediaTitleChanged)/*!< \brief Duplicate of qs_maintitle? \ingroup file_details*/

    Q_PROPERTY (QString genre READ getGenre WRITE setGenre NOTIFY genreChanged) /*!< \brief Genre of media \ingroup file_details*/
    Q_PROPERTY(QQmlListProperty<FileDetailsObject> genreList READ genreList NOTIFY genreChanged)

    Q_PROPERTY (QString studio READ getStudio WRITE setStudio NOTIFY studioChanged) /*!< \brief Publishing studio if availible  \ingroup file_details*/
    Q_PROPERTY(QQmlListProperty<FileDetailsObject> studioList READ studioList NOTIFY studioChanged)
    //television related variables
    Q_PROPERTY (QString program READ getProgram WRITE setProgram NOTIFY programChanged) /*!< \brief The current program. Applies to tv and some broadcast media  \ingroup file_details*/
    Q_PROPERTY (QString channel READ getChannel WRITE setChannel NOTIFY channelChanged)/*!<  \brief The current channel for broadcast media file_details*/
    Q_PROPERTY (QString channelID READ getChannelID WRITE setChannelID NOTIFY channelChanged) /*!< \brief Schedules direct channel id  \ingroup file_details*/
    Q_PROPERTY (QString episode READ getEpisode WRITE setEpisode NOTIFY episodeChanged) /*!< \brief Episode number   \ingroup file_details*/

    Q_PROPERTY (QString director READ getDirector WRITE setDirector NOTIFY directorChanged) /*!< \brief The director of the current media.  \ingroup file_details*/
    Q_PROPERTY(QQmlListProperty <FileDetailsObject> directorList READ directorList NOTIFY directorChanged )


    //audio related
    Q_PROPERTY (QString album READ getAlbum WRITE setAlbum NOTIFY albumChanged) /*!< \brief The album  \ingroup file_details*/
    Q_PROPERTY (QString track READ getTrack WRITE setTrack NOTIFY trackChanged) /*!< \brief The current track \ingroup file_details*/

    Q_PROPERTY (QString performerlist READ getPerformers WRITE setPerformers NOTIFY performersChanged) /*!< \brief String of '|' seperated performers.  \ingroup file_details*/
    Q_PROPERTY (QStringList performers READ getPerformerList) /*!< \brief List of performers \warning experimental  \ingroup file_details*/
    Q_PROPERTY(QQmlListProperty <FileDetailsObject>performersList READ performersList NOTIFY performersChanged )
    Q_PROPERTY(QQmlListProperty<FileDetailsObject> albumArtistList READ albumArtistList NOTIFY albumArtistChanged)
    Q_PROPERTY (QString composerlist READ getComposers WRITE setComposers NOTIFY composersChanged) /*!< Composer  \ingroup file_details*/
    Q_PROPERTY (QStringList composers READ getComposerList) /*!< \brief List of composers \warning Experimental  \ingroup file_details*/

    Q_PROPERTY (QString aspect READ getImageAspect WRITE setImageAspect NOTIFY imageAspectChanged ) /*!< \brief Aspect ratio \todo change to enum.  \ingroup file_details*/
    Q_PROPERTY (int i_aspectH READ getAspectH WRITE setAspectH NOTIFY aspectHChanged)
    Q_PROPERTY (int i_aspectW READ getAspectW WRITE setAspectW NOTIFY aspectWChanged)

public:
    explicit FileDetailsClass(QObject *qorbiter_ptr, QObject *parent = 0);

    QString objecttitle;
    int i_aspectH; //height then width
    int i_aspectW; //height then width
    QString aspect;

    QString file;

    QString season;
    QString imdb;
    QString qs_mainTitle;
    QString qs_mainTitle2;
    QString qs_subTitle;
    QString mediatitle;
    QString studio;

    QString duration;

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

    bool showDetails;
    QString synop;
    QString screenshot;
    QUrl bgImage;
    QString filename;
    QString path;

    //storage device related
    QString qs_storageDevice;
    int storageDeviceNo;


signals:
    void albumArtistChanged();
    void storageDeviceChanged();
    void objectChanged();
    void pathChanged();
    void synopChanged();
    void fileNameChanged();
    void FileChanged(QString f);
    void VisibleChanged(bool vis);
    //void ImageChanged(QUrl);
    void screenShotChanged();
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

    void handleNewFileAttribute(int attribType, int attribute, QString val);

    void clear();

    void setRating(const QString r) {rating = r; emit ratingChanged();}
    QString getRating(){return rating;}


    void setStorageDevice(const QString device) {
        qs_storageDevice = device;
        if(device=="-1") {  storageDeviceNo=-1; }
        else
        { storageDeviceNo = qs_storageDevice.toInt(); }
        emit storageDeviceChanged();
    }

    QString getStorageDevice() {return qs_storageDevice;}

    void setProgram(const QString newProgram) {program = newProgram;  emit programChanged();}
    QString getProgram () {
        if(!m_attributeMap.contains(ATTRIBUTETYPE_Program_CONST)) return "";
        return m_attributeMap.value(ATTRIBUTETYPE_Program_CONST).at(0)->attribute();
    }

    void setTitle (const QString inc_title) {qs_mainTitle = inc_title; emit titleChanged();}
    QString getTitle () {return qs_mainTitle;}

    void setTitle2 (const QString inc_title) {qs_mainTitle2 = inc_title; emit titleChanged2();}
    QString getTitle2 () {return qs_mainTitle2;}

    void setSubTitle (const QString inc_subTitle) {qs_subTitle = inc_subTitle; emit titleChanged();}
    QString getSubTitle () {return qs_subTitle;}


    //general media getters and setters ----//

    void setMediaTitle (const QString inc_mediaTitle) {mediatitle = inc_mediaTitle;  emit mediaTitleChanged();}
    QString getMediaTitle () {return mediatitle;}

    void setStudio (const QString inc_studio) {studio.append(inc_studio) = inc_studio;  emit studioChanged();}
    QString getStudio () {
        if(!m_attributeMap.contains(ATTRIBUTETYPE_Studio_CONST) || m_attributeMap.value(ATTRIBUTETYPE_Studio_CONST).isEmpty())
            return "";
        return m_attributeMap.value(ATTRIBUTETYPE_Studio_CONST).first()->attribute();
    }
    QQmlListProperty<FileDetailsObject> studioList(){
        return QQmlListProperty<FileDetailsObject>(this, m_studioList);
    }

    //--tv getters and setters-------------//



    void setChannel (const QString inc_channel) {channel = inc_channel;  emit channelChanged();}
    QString getChannel () {return channel;}

    void setChannelID (const QString inc_channelID) {channelID = inc_channelID;  emit channelChanged();}
    QString getChannelID () {return channelID;}

    void setEpisode (const QString inc_episode) {episode = inc_episode;  emit episodeChanged();}
    QString getEpisode () {return episode;}

    //-----audio getters and setter--------//
    void setAlbum (const QString inc_album) {album = inc_album;  emit albumChanged();}
    QString getAlbum () {return album;}

    void setTrack (const QString inc_track) {track = inc_track;  emit trackChanged();}
    QString getTrack() {return track;}

    void setPerformers (const QString inc_performer) {performers << inc_performer; emit performersChanged();}
    QString getPerformers() {performerlist = performers.join(" | "); return performerlist;}
    QStringList getPerformerList() {return performers;}

    QQmlListProperty <FileDetailsObject> performersList(){
        return QQmlListProperty<FileDetailsObject>(this, m_performerList);
    }

    QQmlListProperty<FileDetailsObject> albumArtistList(){
        return QQmlListProperty<FileDetailsObject>(this, m_albumArtistList);
    }

    void setComposers (const QString inc_composer) {composers << inc_composer;  emit composersChanged();}
    QString getComposers() {composerlist = composers.join(" | "); return composerlist;}
    QStringList getComposerList() {return composers;}

    void setDirector (const QString inc_director) {directors << inc_director;  emit directorChanged();}
    QString getDirector() {director = directors.join(" | "); return director;}

    QQmlListProperty <FileDetailsObject> directorList() {
       QList<FileDetailsObject*> t =  m_attributeMap.value(ATTRIBUTETYPE_Director_CONST);
        return QQmlListProperty<FileDetailsObject>(this,t);
    }

    void setGenre (const QString inc_genre) {genre.append(inc_genre+" | ");  emit genreChanged();}
    QString getGenre() { return genre;}

    QQmlListProperty<FileDetailsObject> genreList(){
        return QQmlListProperty<FileDetailsObject>(this, m_genreList);
    }

    void setFileMediaType();

    void setAspectH(int h) {i_aspectH = h ; emit aspectHChanged();}
    int getAspectH() { return i_aspectH;}

    void setAspectW(int w) {i_aspectW = w ; emit aspectWChanged();}
    int getAspectW() { return i_aspectW;}

    void setImageAspect(QString i_aspect) { aspect = i_aspect; emit imageAspectChanged();}
    QString getImageAspect() {return aspect;}

    inline QString getFile () {return file;}
    inline void setFile(const QString incFile) { file = incFile; emit FileChanged(file); }



    // Q_INVOKABLE void setSelectionStatus(QString format);
    // Q_INVOKABLE bool getSelectionStatus();


    inline QString getSynop() {return synop;}
    inline void setSynop(const QString s) { synop = s; emit synopChanged(); }

    inline void setScreenshot(QString u) {
        screenshot = u.remove("home/mediapics/") ; emit screenShotChanged();
    }
    inline QString getScreenShot() {return screenshot;}

    inline QString getFilename() {return filename;}
    inline void setFilename (const QString f) {filename = f; emit fileNameChanged();}

    inline QString getPath() {return path;}
    inline void setPath (const QString f) {path = f; emit pathChanged();}

    inline bool isVisible () { return showDetails; }
    inline void setVisible (bool state) { clear(); showDetails = state; emit VisibleChanged( showDetails);}

    // Q_INVOKABLE void setSelectionStatus(QString format);
    // Q_INVOKABLE bool getSelectionStatus();
private:
    QHash<int, QList<FileDetailsObject*> > m_attributeMap;
    QMap<int, QString> m_performerMap;

    QList<FileDetailsObject*>  m_performerList;
    QList<FileDetailsObject*>  m_albumArtistList;
    QList<FileDetailsObject*>  m_studioList;
    QList<FileDetailsObject*>  m_genreList;
    QList<FileDetailsObject*>  m_composerWriterList;
    QList<FileDetailsObject*>  m_directorList;
    QMap<int, FileDetailsObject*>  m_singleItemMap;

};

#endif // FILEDETAILSCLASS_H
