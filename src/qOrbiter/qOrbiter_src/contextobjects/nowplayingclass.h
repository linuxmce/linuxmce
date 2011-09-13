#ifndef NOWPLAYINGCLASS_H
#define NOWPLAYINGCLASS_H

/*
  This class is a c++ representation of the 'now playing' button for LinuxMCE It purpose is to provide simple, api like
  access to the now playing button properties as well as automatically indicate the state of media that playing and any
  other relevant data related to the now playing button. For example, it holds the screen associated with the currently
  playing media type so that when clicked, the proper remote is shown.

  Currently, this clas will hold vars for ALL mediatypes and its up to the designer to add the proper ones
  I will try to note what is what, but i cant guarantee 100% accuracy at this time
  */

#include <QDeclarativeItem>
#include <QTime>
#include <QDebug>


class NowPlayingClass : public QDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY (bool b_mediaPlaying READ getStatus WRITE setStatus NOTIFY mediaStatusChanged) //property to know if media is playing
   //set now playing - text to assign
    Q_PROPERTY (QString qs_mainTitle READ getTitle WRITE setTitle NOTIFY titleChanged) //the text set sent by now playing command
     Q_PROPERTY (QString qs_mainTitle2 READ getTitle2 WRITE setTitle2 NOTIFY titleChanged2) // in the case of audio, multiple metada value are sent  thus title 2
    //set now playing - value
    Q_PROPERTY (QString qs_subTitle READ getSubTitle WRITE setSubTitle NOTIFY titleChanged )//the secondary text sent in a seperate variable from now playing
    //internal reference to know what the current media screen is
    Q_PROPERTY (QString qs_screen READ getScreen WRITE setScreen NOTIFY screenTypeChanged)
    //file path of the file in question
    Q_PROPERTY (QString filepath READ getFilePath WRITE setFilePath NOTIFY filePathChanged)
    //current playback speed
    Q_PROPERTY (QString qs_playbackSpeed READ getMediaSpeed WRITE setStringSpeed NOTIFY mediaSpeedChanged)
    //metadata related image url
    Q_PROPERTY (QUrl nowPlayingImage READ getImage WRITE setImage NOTIFY imageChanged)
    //internal playlist position tracker
    Q_PROPERTY (int m_iplaylistPosition READ getPlaylistPosition WRITE setPlaylistPostion NOTIFY playListPositionChanged)
    //television related variables
    Q_PROPERTY (QString program READ getProgram WRITE setProgram NOTIFY programChanged)
    Q_PROPERTY (QString channel READ getChannel WRITE setChannel NOTIFY channelChanged)
    Q_PROPERTY (QString channelID READ getChannelID WRITE setChannelID NOTIFY channelChanged)
    Q_PROPERTY (QString episode READ getEpisode WRITE setEpisode NOTIFY episodeChanged)

public:
    explicit NowPlayingClass(QDeclarativeItem *parent = 0);
    QString filepath;
    QString qs_screen;
    QString qs_imagePath;
    QString qs_mainTitle;
    QString qs_mainTitle2;
    QString qs_subTitle;
    bool b_mediaPlaying;
    int i_mediaType;
    int i_streamID;
    int i_playbackSpeed;
    QTime *timecode;
    QString qs_playbackSpeed;
    QUrl nowPlayingImage;
    int m_iplaylistPosition;
    QImage fileImage;
    QString synopsis;
    //television related
    QString program;
    QString channel;
    QString channelID;
    QString episode;


signals:
    void playlistChanged();
    void mediaChanged();
    void mediaEnded();
    void mediaStarted();
    void imageChanged();
    void screenTypeChanged();
    void titleChanged();
    void titleChanged2();
    void mediaTypeChanged();
    void mediaStatusChanged();
    void filePathChanged();
    void mediaSpeedChanged();
    void playListPositionChanged();
    void programChanged();
    void channelChanged();
    void episodeChanged();



public slots:

    void setPlaylistPostion(int i_pls) {m_iplaylistPosition = i_pls ; qDebug() << "Playlist Position now" << m_iplaylistPosition ; emit playListPositionChanged();}
    int getPlaylistPosition() {return m_iplaylistPosition;}

    void setProgram(QString newProgram) {program = newProgram; emit programChanged();}
    QString getProgram () {return program;}

    void setImage(QUrl incImage) {nowPlayingImage = incImage; emit imageChanged();}
    QUrl getImage () { return nowPlayingImage;}

    void setScreen(QString inc_screen) {qs_screen = inc_screen; emit screenTypeChanged();}
    QString getScreen () {return qs_screen;}

    void setUrl (QString inc_url) {qs_imagePath = inc_url; emit imageChanged();}
    QString getUrl () {return qs_imagePath;}

    void setTitle (QString inc_title) {qs_mainTitle = inc_title; emit titleChanged();}
    QString getTitle () {return qs_mainTitle;}

    void setTitle2 (QString inc_title) {qs_mainTitle2 = inc_title; emit titleChanged2();}
    QString getTitle2 () {return qs_mainTitle2;}

    void setSubTitle (QString inc_subTitle) {qs_subTitle = inc_subTitle; emit titleChanged();}
    QString getSubTitle () {return qs_subTitle;}

    void setStatus (bool status) {b_mediaPlaying = status; emit mediaStatusChanged(); }
    bool getStatus () {return b_mediaPlaying;}

    void setMediaType (int inc_mediaType) {i_mediaType = inc_mediaType; emit mediaTypeChanged();}
    int  getMediaType () {return i_mediaType;}

    void setFilePath (QString inc_fp) {filepath = inc_fp; qDebug() <<"FilePath:" << filepath; emit filePathChanged();}
    QString getFilePath () {return filepath;}

    void setMediaSpeed(int speed);
    void setStringSpeed(QString s) {qs_playbackSpeed = s; qDebug() << qs_playbackSpeed; emit mediaSpeedChanged();}
    QString getMediaSpeed() {return qs_playbackSpeed;}

    void setChannel (QString inc_channel) {channel = inc_channel;  emit channelChanged();}
    QString getChannel () {return channel;}

    void setChannelID (QString inc_channelID) {channelID = inc_channelID;  emit channelChanged();}
    QString getChannelID () {return channelID;}

    void setEpisode (QString inc_episode) {episode = inc_episode;  emit episodeChanged();}
    QString getEpisode () {return episode;}

    };

    #endif // NOWPLAYINGCLASS_H
