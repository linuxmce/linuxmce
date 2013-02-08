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

#include <autotagger.h>
#include <film.h>

void film::setIdent(QString &n)                                         //this function sets the movie year(if possible) and title.
{
    movieApiKey = "6ba51a94858c1748757dc06f7d4afbaa";
    apiUrl = "http://api.themoviedb.org/2.1/Movie.search/en/xml/6ba51a94858c1748757dc06f7d4afbaa/";
    dataUrl = "http://api.themoviedb.org/2.1/Movie.getInfo/en/xml/6ba51a94858c1748757dc06f7d4afbaa/";

   movieFileName = n;
   movieFileName.chop(4);
   QRegExp movieYear;
   movieYear.setPattern("(\\d\\d\\d\\d)");
   int yearMatch = movieYear.indexIn(movieFileName);
   if (yearMatch !=0)
   {releaseYear = movieYear.capturedTexts().at(0);}
   else
   {releaseYear ="";}
   cout << "Guessed Year :" << qPrintable(releaseYear) << endl;
   QRegExp TitleGuess;
   TitleGuess.setPattern("(\\d\\d\\d\\d)|\\[|\\]|\\*|\\(|\\)|dvd-rip|dvdrip\\S+|720|1080|\\s\\s|\\s\\s\\s\\s|\\[\\S+|\\(\\S+|DVD|rip|HDTV|x264|ac3|(720|1080)p|CD\\d|DISC\\d|extended.edition|directors.cut|special.edition|xvid|BluRay|H264|BrRip");
   QRegExp TitleGuess2;
   TitleGuess2.setPattern ("\\_|\\.|\\-");
   TitleGuess.setCaseSensitivity(Qt::CaseInsensitive);
   movieFileName.replace(TitleGuess, "");
   movieFileName.replace(TitleGuess2, " ");
   mTitle = movieFileName.simplified ();
   cout << "Guessed Title:" << qPrintable(mTitle) << endl;

   QRegExp rez;
   rez.setPattern("(720|1080)p|(720|1080)");
   int tv_rez = rez.indexIn(n);
   QString temp_rez = rez.cap (0);
   temp_rez.remove ("p");

   if (temp_rez.isEmpty ())
       {
       this->m_rez ="3";
       }
   else if (temp_rez == "720")
       {
       m_rez="4";
       }
   else if (temp_rez == "1080")
       {
       m_rez = "5";
       };

   }

void film::searchMovie()                                                                                  //execute search for movie id
{
    QNetworkRequest iRequest(searchUrl);
    QNetworkAccessManager *iManager = new QNetworkAccessManager(this);
    mTitle.replace("_", "");
    if(releaseYear.count() == 0)
	{
	searchUrl = apiUrl+mTitle;
	}
    else
	{
	searchUrl = apiUrl+mTitle+"+"+releaseYear;
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
                                                                                                                //this function still needs decide the match better
void film::searchReply(QNetworkReply *)                                                                          //recieve reply, and then retrieve extended data.
{                                                                                                             //needs a clause to die if no result


   QByteArray iBytes = iReply->readAll();

   QDomDocument searchResult("films");
   searchResult.setContent(iBytes);
   QDomElement root = searchResult.documentElement();

   //extracts movie id, release year, imdb and rating from 1st match.

   //qDebug () << root.text ();
   QDomElement title = root.firstChildElement("movies").firstChildElement("movie").firstChildElement("name");

   mTitle = title.text();
   movieID = root.firstChildElement("movies").firstChild().firstChildElement("id").toElement().text();

   cout << qPrintable(mTitle) << "--" << "id: " << qPrintable(movieID) << endl;
   releaseYear= root.firstChildElement("movies").firstChild().firstChildElement("released").toElement().text();
   synopsis =root.firstChildElement("movies").firstChild().firstChildElement("overview").toElement().text();
   mIMDB=root.firstChildElement("movies").firstChild().firstChildElement("imdb_id").toElement().text();
   rating=root.firstChildElement("movies").firstChild().firstChildElement("certification").toElement().text();
   cout << "Getting Extended Data" << endl;
   dlData();
}


void film::dlData()                                                                                               //download extended data based on movieID
{
   dlUrl = dataUrl+movieID;
   QNetworkRequest sRequest(dlUrl);
   QNetworkAccessManager *sManager = new QNetworkAccessManager(this);
   this->connect(sManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(dataReply(QNetworkReply *)));
   sReply =sManager->get(sRequest);

   QEventLoop movieWait;
   connect(sManager, SIGNAL(finished(QNetworkReply *)), &movieWait, SLOT(quit()));
   cout << "Waiting for Extra Data" << endl;
   movieWait.exec();
}

void film::dataReply(QNetworkReply *)                                                                               //additional data downloaded and parsed for linuxmceDB
{
    cout << "Response Recieved" << endl;

    QByteArray mBytes = sReply->readAll();

    QDomDocument searchResult("movieInfo");
    searchResult.setContent(mBytes);

    QDomElement root = searchResult.documentElement();  
    QDomElement curElement = root.firstChildElement("movies").firstChildElement("movie").firstChildElement("cast").toElement();
    QDomElement gElement = root.firstChildElement("movies").firstChildElement("movie").firstChildElement("categories").toElement();
    QDomElement sElement = root.firstChildElement("movies").firstChildElement("movie").firstChildElement("studios").toElement();
    QDomElement pRoot = root.firstChildElement("movies").firstChildElement("movie").firstChildElement("images").toElement();
    QDomNodeList posters = root.firstChildElement("movies").firstChildElement("movie").firstChildElement("images").toElement ().childNodes ();
    pUrl.append(root.firstChildElement("movies").firstChildElement("movie").firstChildElement("images").firstChild().toElement().attribute("url"));
    //bgUrl.append();
    QDomNodeList persons = curElement.childNodes();
    QDomNodeList categories = gElement.childNodes();
    QDomNodeList studios = sElement.childNodes();

	int z = 0;
	while
		(!posters.at(z).isNull())                                                   //this loop iterates through the 'persons' extracting director(s) | actor(s)
	    {

	    if
		    (posters.at(z).toElement().attribute("type") == "poster")
		{
		picUrl.append (posters.at (z).toElement ().attribute ("url"));
		picID.append(posters.at(z).toElement().attribute("id"));
		}
	    else if
		    (posters.at(z).toElement().attribute("type")=="backdrop")
		{
		bgUrl.append(posters.at(z).toElement().attribute("url"));
		bgID.append(posters.at(z).toElement().attribute("id"));
		}
	    z++;
	    }

	z =0;
	while
		(!persons.at(z).isNull())                                                   //this loop iterates through the 'persons' extracting director(s) | actor(s)
	    {

	    if
		    (persons.at(z).toElement().attribute("job") == "Director")
		{
		director.append(persons.at(z).toElement().attribute("name")+"|");
		}
	    else if
		    (persons.at(z).toElement().attribute("job")=="Actor")
		{
		actor.append(persons.at(z).toElement().attribute("name")+"|");
		}
	    z++;
	    }

	 z= 0;
	while
		(!categories.at(z).isNull())                                                   //this loop iterates through the 'categories' extracting genre(s)
	    {                                                                                   //there is no need to selectively get the attributes as we want all the genres
	    genre.append(categories.at(z).toElement().attribute("name")+"|");
	    z++;
	    }

	z=0;
	while
		(!studios.at(z).isNull())                                                   //this loop iterates through the 'studios' extracting studios
	    {                                                                                   //there is no need to selectively get the attributes as we want all the studios
	    studio.append(studios.at(z).toElement().attribute("name")+"|");
	    z++;
	    }

	cout << "|====Attributes===|" << endl;                                                                         //console output for debugging.
	cout << "Movie: " << qPrintable(mTitle) << " - " << qPrintable(releaseYear) << endl;
	cout << "Genre: " << qPrintable(genre) <<endl;
	cout << "Director: " << qPrintable(director) <<endl;
	cout << "Cast:" << qPrintable(actor) << endl;
	cout << qPrintable(synopsis) << endl;
	cout << "IMDB: " << qPrintable(mIMDB) << " == Rating: " << qPrintable(rating) << endl;
	cout <<  "Open Movie ID: " << qPrintable(movieID) << endl;
	cout << "Studio: " << qPrintable(studio) << endl;
	cout << "|======End========|" << endl;
    }
