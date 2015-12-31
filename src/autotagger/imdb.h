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

#ifndef IMDB_H
#define IMDB_H

#include <QObject>
#include <autotagger.h>

class imdb : public QObject
{
    Q_OBJECT
public:
    explicit imdb(QObject *parent = 0);
    //object variables

    QString movieApiKey ;
    QString apiUrl;
    QString dataUrl;
    QFile resultFile;
    QUrl searchUrl;
    QString imdb_Url;
    QNetworkRequest imdb_Request;
    QNetworkReply *iReply;
    QNetworkReply *imdb_Reply;


    QString imdb_Name;
    QString imdb_Director;
    QString imdb_studio;
    QString imdb_Rating;
    QString imdb_Synopsis;
    QString imdb_Writer;
    QString imdb_Year;
    QString imdb_ID;
    QString imdb_Runtime;
    QString tmdb_ID;

    void setMovieName(QString up_MovieName);
    void  imdb_getMovie(QString tmdb_ID);
    void  tmdb_getID(QString p_MovieName);
void get_imdb_Page(QString imdb_ID);

signals:

public slots:
void searchReply(QNetworkReply *);
void dataReply(QNetworkReply *);

};

#endif // IMDB_H
