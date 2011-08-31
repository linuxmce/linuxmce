#ifndef NOWPLAYINGCLASS_H
#define NOWPLAYINGCLASS_H

/*
  This class is a c++ representation of the 'now playing' button for LinuxMCE It purpose is to provide simple, api like
  access to the now playing button properties as well as automatically indicate the state of media that playing and any
  other relevant data related to the now playing button. For example, it holds the screen associated with the currently
  playing media type so that when clicked, the proper remote is shown
  */

#include <QDeclarativeItem>
#include <QTime>
#include <QDebug>


class NowPlayingClass : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY (bool b_mediaPlaying READ getStatus WRITE setStatus NOTIFY mediaStatusChanged)
    Q_PROPERTY (QString qs_mainTitle READ getTitle WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY (QString qs_subTitle READ getSubTitle WRITE setSubTitle NOTIFY titleChanged )
    Q_PROPERTY (QString qs_screen READ getScreen WRITE setScreen NOTIFY screenTypeChanged)

    Q_PROPERTY (QString filepath READ getFilePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY (QString qs_playbackSpeed READ getMediaSpeed WRITE setStringSpeed NOTIFY mediaSpeedChanged)
    Q_PROPERTY (QUrl nowPlayingImage READ getImage WRITE setImage NOTIFY imageChanged)


public:
    explicit NowPlayingClass(QDeclarativeItem *parent = 0);
    QString filepath;
    QString qs_screen;
    QString qs_imagePath;
    QString qs_mainTitle;
    QString qs_subTitle;
    bool b_mediaPlaying;
    int i_mediaType;
    int i_streamID;
    int i_playbackSpeed;
    QTime *timecode;
    QString qs_playbackSpeed;
    QUrl nowPlayingImage;

signals:
    void playlistChanged();
    void mediaChanged();
    void mediaEnded();
    void mediaStarted();
    void imageChanged();
    void screenTypeChanged();
    void titleChanged();
    void mediaTypeChanged();
    void mediaStatusChanged();
    void filePathChanged();
    void mediaSpeedChanged();



public slots:

    void setImage(QUrl incImage) {nowPlayingImage = incImage; emit imageChanged();}
    QUrl getImage () { return nowPlayingImage;}

    void setScreen(QString inc_screen) {qs_screen = inc_screen; emit screenTypeChanged();}
    QString getScreen () {return qs_screen;}

    void setUrl (QString inc_url) {qs_imagePath = inc_url; emit imageChanged();}
    QString getUrl () {return qs_imagePath;}

    void setTitle (QString inc_title) {qs_mainTitle = inc_title; emit titleChanged();}
    QString getTitle () {return qs_mainTitle;}

    void setSubTitle (QString inc_subTitle) {qs_subTitle = inc_subTitle; emit titleChanged();}
    QString getSubTitle () {return qs_subTitle;}

    void setStatus (bool status) {b_mediaPlaying = status; emit mediaStatusChanged(); }
    bool getStatus () {return b_mediaPlaying;}

    void setMediaType (int inc_mediaType) {i_mediaType = inc_mediaType; emit mediaTypeChanged();}
    int  getMediaType () {return i_mediaType;}

    void setFilePath (QString inc_fp) {filepath = inc_fp; emit filePathChanged();}
    QString getFilePath () {return filepath;}

    void setMediaSpeed(int speed);
    void setStringSpeed(QString s) {qs_playbackSpeed = s; qDebug() << qs_playbackSpeed; emit mediaSpeedChanged();}
    QString getMediaSpeed() {return qs_playbackSpeed;}

    };

    #endif // NOWPLAYINGCLASS_H
