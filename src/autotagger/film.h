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

#ifndef FILM_H
#define FILM_H
#include <autotagger.h>

class film : public QObject
{
    Q_OBJECT
public:
    QFile resultFile;
    QNetworkReply *iReply;
    QNetworkReply *sReply;
    QDomDocument Result;
    QDomDocument sResult;
    QEventLoop iMovieWait;
    QEventLoop sMovieWait;
    QByteArray iBytes;
    QByteArray sBytes;
    QUrl searchUrl;    

    QString dlUrl;
    QString pUrl;

    QStringList picUrl;
    QStringList picID;

    QStringList bgUrl;
    QStringList bgID;

    void searchMovie();
    void dlData();
    QString m_rez;
    QString mfileID;                        //linuxmce file number in pluto_media
    QString movieID;                        //The MovieDB.com id
    QString movieFileName;
    QString releaseYear;
    QString mTitle;
    QString synopsis;
    QString director;                       //note, many of these fields contain multiple values seperated by a "|" and then split at insert time.
    QString actor;
    QString genre;
    QString studio;
    QString mIMDB;                          //imdb. this is used as the unique attribute for movie files.
    QString rating;
    QByteArray dataLookup(QString *title, int releaseDate);
    void setIdent(QString &n);

protected:
    QString movieApiKey ;
    QString apiUrl;
    QString dataUrl;
    QString mFilename;
public slots:
  void searchReply(QNetworkReply *);
   void dataReply(QNetworkReply *);
};
#endif // FILM_H
