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

#include "imdb.h"
#include "autotagger.h"

imdb::imdb(QObject *parent) :
    QObject(parent)
{
    movieApiKey = "6ba51a94858c1748757dc06f7d4afbaa";
    apiUrl = "http://api.themoviedb.org/2.1/Movie.search/en/xml/6ba51a94858c1748757dc06f7d4afbaa/";
    dataUrl = "http://api.themoviedb.org/2.1/Movie.getInfo/en/xml/6ba51a94858c1748757dc06f7d4afbaa/";
}

void imdb::imdb_getMovie(QString tmdb_ID)
{
tmdb_ID="null";
}

void imdb::setMovieName(QString up_MovieName)
{
   QString tmp_Filename = up_MovieName;
   QRegExp movieYear;
   movieYear.setPattern("(\\d\\d\\d\\d)");
   int yearMatch = movieYear.indexIn(tmp_Filename);
   if (yearMatch !=0)
       {
       imdb_Year= movieYear.capturedTexts().at(0);}
   else
       {
       imdb_Year ="";
       }
   cout << "Guessed Year :" << qPrintable(imdb_Year) << endl;
   QRegExp TitleGuess;
   TitleGuess.setPattern("(\\d\\d\\d\\d)|\\[|\\]\\-|\\_|\\*|\\(|\\)");
   tmp_Filename.chop(4);
   tmp_Filename.replace(TitleGuess, "");
   cout << "Guessed Title:" << qPrintable(tmp_Filename) << endl;
   imdb_Name = tmp_Filename;
}

void imdb::tmdb_getID(QString p_MovieName)
{p_MovieName = "null";
    QNetworkRequest iRequest(searchUrl);
    QNetworkAccessManager *iManager = new QNetworkAccessManager(this);
    if(imdb_Year.length() == 0)
	{
	searchUrl = apiUrl+imdb_Name;
	}
    else
	{
	searchUrl = apiUrl+imdb_Name+" +"+imdb_Year;
	}
    iRequest.setUrl(searchUrl);
    cout << "Request Url:" << qPrintable(searchUrl.toString()) << endl;
    this->connect(iManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(searchReply(QNetworkReply *)));
    iReply =iManager->get(iRequest);

    QEventLoop movieWait;
    connect(iManager, SIGNAL(finished(QNetworkReply *)), &movieWait, SLOT(quit()));
    cout << "Waiting for ID" << endl;
    movieWait.exec();
}

void imdb::searchReply(QNetworkReply *)
{
    cout << "got reply" << endl;
    QByteArray iBytes = iReply->readAll();
    QDomDocument searchResult("films");

    searchResult.setContent(iBytes);

    QDomElement root = searchResult.documentElement();
    //extracts movie id, release year, imdb and rating from 1st match.
    imdb_ID = root.firstChildElement("movies").firstChild().firstChildElement("imdb_id").toElement().text();
    cout  << "finished" << endl;
}

void imdb::dataReply(QNetworkReply *)
{
    cout << "Got it!" << endl;
    QByteArray imdb_page_resp = imdb_Reply->readAll();
    QDomDocument imdb_Data;
    imdb_Data.setContent(imdb_page_resp);
    QDomElement imdb_Top = imdb_Data.documentElement();
    cout << qPrintable(imdb_Top.toElement().text()) << endl;
    cout << "Done parsing" << endl;
}


void imdb::get_imdb_Page(QString imdb_ID)
{
    cout << "Getting IMDB Page: " << qPrintable(imdb_ID) << endl;
    imdb_Url = "http://www.imdb.com/title/"+imdb_ID;
    cout << qPrintable(imdb_Url) << endl;
    QNetworkRequest imdb_Request(imdb_Url);
    QNetworkAccessManager *imdb_Manager = new QNetworkAccessManager(this);
    this->connect(imdb_Manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(dataReply(QNetworkReply *)));
    imdb_Reply = imdb_Manager->get(imdb_Request);

    QEventLoop imdb_timer;
    connect(imdb_Manager, SIGNAL(finished(QNetworkReply *)), &imdb_timer, SLOT(quit()));
    cout << "Fetching" << endl;
    imdb_timer.exec();
}
