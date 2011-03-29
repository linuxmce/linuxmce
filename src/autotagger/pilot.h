/*
        LinuxMCE autotagger
        Langston Ball golgoj4@gmail.com
        GPLv2 Licensed
        LinuxMCE auto-metadata lookup
        Utilizes www.thetvdb.com and
        www.themoviedb.com to find
        relevant information and populate the
        linuxmce Database
*/

#ifndef PILOT_H
#define PILOT_H
#include <autotagger.h>

class tvshow : public QObject
{
    Q_OBJECT
public:
    //file operation to determine .iso , video_ts or .dvd
    int isDisc(QString fname);
    //reg ex variables
    QRegExp seasonGrab;
    QRegExp titleGrab;
    QRegExp episodeGrab;
    QStringList epNumRes;
    QRegExp rez;

    //network variables
    void createRequest();
    QNetworkReply *rep;
    QByteArray bytes;
    QNetworkRequest *req;
    QNetworkAccessManager *n;
    QNetworkReply *reply;
    QNetworkReply *Dreply;
    QString resp;                       //text dump output of xml
    QFile *cZip;
    int isDvd;
    int video_ts;

    //xml parsing
    QDomDocument seachResult;
    QDomDocument Episode;
    QDomDocument Series;
    QDomDocument Banners;
    QDomDocument Actors;
    QFile actXml;
    QFile mirXml;
    QFile seriesXml;
    QFile bannerXml;
    //QDomDocument xmlTvShow;

    // internal class functions
    void downloadFresh();
    QString setSeason(QString filename);
    QString setEpisode(QString filename);
    QString setShow(QString filename);
    QString apiKey;

    void getData();
    // object attributes
    QString t_rez;
    QString fileID;                        //linuxMCE file number in pluto_media
    QString showTitle;                    // Name of the Program
    QString episodeTitle;                 // Name of the Episode
    QString season;                       // actual season number such as 5 or 12
    QString episodeNum;                   // episode number within the season
    QString synopsis;
    QString actor;                       // actors -----see below
    QString guestStars;                   //guest star(s) see below
    QString tvDirector;                   //director(s) compacted into string and parsed later. this was done because of inconsistent data from the tvdb.com
    QString epImage;                      //url to episode image

					  //show poster itself
    QString seasonPoster;                 // url to poster for season linke to season id

    QStringList seriesBanner;             //banner
    QStringList seriesImg;                //series image, wide format with series name on it
    QStringList viginettes;               //vignettes for future use
    QStringList seasonImg;                // season image
    QStringList seriesGraphic;             //for series ID

    QString IMDB;                         //imdb identifier
    QString tvNetwork;
    QString genre;                        //similar string approach taken as cast list
    QString airdate;
    QString dvdSeason;
    QString dvdChapter;
    QString dvdEpisode;
    QString dvdDiscID;
    QString seriesID;                     //unique identifier for the entire series
    QString progID;                       //unique identifier for this episode
    QString seasonID;                     //unique identifier for this season
    QString tvRating;

    bool isSpecial;

public slots:
    void dataFinished();
    void setAttributes();
    void replyFinished(QNetworkReply *);
    void error();


};

#endif // PILOT_H
