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
#include <QVariant>
#include <qjson/parser.h>


film::film(QObject *parent){
    /* Init all of the basic info, empty strings; */

    movieApiKey = "6ba51a94858c1748757dc06f7d4afbaa";
    apiUrl = "http://api.themoviedb.org/";
    dataUrl = "http://api.themoviedb.org/";

    configurationUrl = apiUrl+"3/configuration?api_key="+movieApiKey;
    movieUrl = apiUrl+"3/movie/MOVIE_ID?api_key="+movieApiKey;
    searchUrlPath = apiUrl+"3/search/movie?api_key="+movieApiKey+"&";
    configurationUrlPath = "3/configuration?api_key="+movieApiKey+"&";
    imagesUrlPath = "3/movie/MOVIE_ID/images?api_key="+movieApiKey+"&";     /* note, must do string replace on MOVIE_ID with actual numerical ID; */
    castUrlPath =apiUrl+"/3/movie/MOVIE_ID/casts?api_key="+movieApiKey+"&";        /* note, must do string replace on MOVIE_ID with actual numerical ID; */
    trailersUrlPath = "/3/movie/MOVIE_ID/trailers?api_key="+movieApiKey+"&"; /* note, must do string replace on MOVIE_ID with actual numerical ID; */
    imageBaseUrl="";
    searchUrl = apiUrl+searchUrlPath;
    movieId = -1;

    posterPath ="";
    backdropPath="";

    getConfiguration();

}

void film::getConfiguration(){


    QNetworkRequest sRequest(configurationUrl);
    QNetworkAccessManager *sManager = new QNetworkAccessManager(this);
    this->connect(sManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(setConfiguration(QNetworkReply *)));
    QNetworkReply *cReply=sManager->get(sRequest);

    QEventLoop movieWait;
    connect(sManager, SIGNAL(finished(QNetworkReply *)), &movieWait, SLOT(quit()));
    cout << "Waiting for Extra Data from::" << configurationUrl.toStdString() << endl;
    movieWait.exec();
}

void film::setConfiguration(QNetworkReply *r){
    cout << "Response Recieved" << endl;
    QByteArray mBytes = r->readAll();
    QJson::Parser ps;
    configData = ps.parse(mBytes).toMap();

    imageBaseUrl = configData["images"].toMap()["base_url"].toString();

}

void film::setIdent(QString &n)                                         //this function sets the movie year(if possible) and title.
{
    movieFileName = n;
    movieFileName.chop(4);
    QRegExp movieYear;
    movieYear.setPattern("\\d\\d\\d\\d(?!p)");
    int yearMatch = movieYear.indexIn(movieFileName);

     if(movieYear.capturedTexts().length() == 1){
            releaseYear = movieYear.capturedTexts().at(0);
        }
        else{
             qDebug() << movieYear.capturedTexts().join("\n");
             releaseYear="";
        }

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
    mTitle.remove(temp_rez);

}

void film::searchMovie()                                                                                  //execute search for movie id
{

    QString final(searchUrlPath);
    QNetworkAccessManager *iManager = new QNetworkAccessManager(this);
    mTitle.replace("_", "");
    if(releaseYear.count() == 0)
    {
        final.append("&query="+mTitle);
        searchUrl = apiUrl+mTitle;
    }
    else
    {
        final.append("&query="+mTitle);
        final.append("&year="+releaseYear);
        searchUrl = apiUrl+mTitle+"+"+releaseYear;
    }
    searchUrl.setUrl(final);
    QNetworkRequest iRequest(searchUrl);
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
    qWarning() << "Search Reply \n";

    QByteArray iBytes = iReply->readAll();    
    if(iBytes.isNull()){
        qWarning() << "Empty Search reply!!";
        qWarning() << "No title found.";
        return;
    }
    QJson::Parser pr;
    QVariantMap p = pr.parse(iBytes).toMap();


    QVariantList top = p["results"].toList(); 

    if(top.isEmpty()){
        qWarning() << "No title found.";
        return;
    }
    QVariantMap topChoice = top.at(0).toMap();

    QDomDocument searchResult("films");
    searchResult.setContent(iBytes);
    QDomElement root = searchResult.documentElement();

    //extracts movie id, release year, imdb and rating from 1st match.

    //qDebug () << root.text ();
    //QDomElement title = topChoice["original_title"]; // root.firstChildElement("movies").firstChildElement("movie").firstChildElement("name");

    mTitle = topChoice["original_title"].toString();
    movieID = topChoice["id"].toString();// root.firstChildElement("movies").firstChild().firstChildElement("id").toElement().text();
    cout << qPrintable(mTitle) << "--" << "id: " << qPrintable(movieID) << endl;

    releaseYear=topChoice["release_date"].toString();// root.firstChildElement("movies").firstChild().firstChildElement("released").toElement().text();
    //synopsis =root.firstChildElement("movies").firstChild().firstChildElement("overview").toElement().text();
    //mIMDB=root.firstChildElement("movies").firstChild().firstChildElement("imdb_id").toElement().text();
    //rating=root.firstChildElement("movies").firstChild().firstChildElement("certification").toElement().text();

}


bool film::dlData()                                                                                               //download extended data based on movieID
{

    QEventLoop movieWait;
    QByteArray dlData;
    QJson::Parser dp;

    dlUrl = castUrlPath.replace("MOVIE_ID", movieID);
    QNetworkRequest sRequest(dlUrl);
    QNetworkAccessManager *sManager = new QNetworkAccessManager(this);
    QNetworkReply *lReply =sManager->get(sRequest);

    connect(sManager, SIGNAL(finished(QNetworkReply *)), &movieWait, SLOT(quit()));
    cout << "Waiting for Extra Data from::" << dlUrl.toStdString() << endl;
    movieWait.exec();

    QVariantMap Cast;
    dlData = lReply->readAll();
    Cast = dp.parse(dlData).toMap();
    //parse cast here;

    QVariantList castList = Cast["cast"].toList();

    foreach(QVariant v, castList){

        QVariantMap inner =v.toMap();
        actor.append(inner["name"].toString()+"|");
    }

    QVariantList crewList = Cast["crew"].toList();
    foreach(QVariant l, crewList){
        QVariantMap cInner = l.toMap();
        QString dept = cInner["department"].toString();
        if(dept=="Producer"){

        }else if(dept=="Sound"){

        }else if(dept=="Camera"){

        }else if(dept=="Editing"){

        }else if(dept=="Casting"){

        }else if(dept=="Directing"){
            if(cInner["job"]=="Director"){
                director.append(cInner["name"].toString()+"|");
            }

        }else if(dept == "Writing"){

        }
    }

    dlUrl = movieUrl.replace("MOVIE_ID", movieID);
     sRequest.setUrl(dlUrl);
    //QNetworkAccessManager *sManager = new QNetworkAccessManager(this);
    this->connect(sManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(dataReply(QNetworkReply *)));
    sReply =sManager->get(sRequest);

//    QEventLoop movieWait;
//    connect(sManager, SIGNAL(finished(QNetworkReply *)), &movieWait, SLOT(quit()));
    cout << "Waiting for Extra Data from::" << dlUrl.toStdString() << endl;
    movieWait.exec();

    return true;
}

void film::dataReply(QNetworkReply *)                                                                               //additional data downloaded and parsed for linuxmceDB
{
    cout << "Response Recieved" << endl;
    QByteArray mBytes = sReply->readAll();

    QJson::Parser ps;
    QVariantMap movieInfo = ps.parse(mBytes).toMap();

    backdropPath =imageBaseUrl+"original"+movieInfo["backdrop_path"].toString();
    posterPath =imageBaseUrl+"original"+movieInfo["poster_path"].toString();
    picUrl.append(posterPath);
    bgUrl.append(backdropPath);
    mIMDB=movieInfo["imdb_id"].toString();
    synopsis = movieInfo["overview"].toString();
    releaseYear = movieInfo["release_date"].toString();
    QVariantList genres = movieInfo["genres"].toList();
    QVariantList studiosList = movieInfo["production_companies"].toList();

    genre="";
   foreach(QVariant gr, genres){
       QVariantMap innerGr = gr.toMap();
       genre.append(innerGr["name"].toString()+"|");
    }

   foreach(QVariant prd, studiosList){
       QVariantMap innerPrd = prd.toMap();
       studio.append(innerPrd["name"].toString()+"|");
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
