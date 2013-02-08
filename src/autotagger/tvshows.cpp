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
#include <pilot.h>

int tvshow::isDisc (QString fname)
{
    QRegExp diskCheck(".iso|.dvd");                   //regex match for dvd disc images
    fname= "null";
    QRegExp discSetter("s\\dd\\d");

    /* -if its an iso, dvd
       or a directory with video_ts - db check to see if it has a subdir video_ts

    */
    return 0;
}

QString tvshow::setEpisode(QString filename)
{
    //this block deals with extracting the episode
    //the regex method employed to extract identifiers is based on the assertion we can locate at least 1
    //identifying marker. We then use the location of the match, and its length to then delete all data
    //which is characters in this case around our match, leaving what we want. This method is employed for
    //season, episode and show title in their respective functions
    //also, QT has nice pre-built regex app that tests different regex's against strings in real time, very helpfull.

    episodeGrab.setPattern("e[1-9]|e([1-9][0-9])|e0([1-9])|ep([1-9]|\\d\\d)|ep([0-9][1-9])|pt([1-9])|pt([0-9][0-9])|(?:[1-9]of)([1-9]|[1-9])|\\dof([0-9][0-9])|\\dx[1-9]\\d|pt.(I)|pt.(II)|pt.(III)|pt.(IV)|pt.(V)|pt.(VI)|pt.(VII)|pt.(VIII)|pt.(IX)|pt.(X)|(?:[1-9])[1-9]([1-9])(?!\\d)|\\b[0-9][0-9]\\b|\\b\\d([0-9][0-9])\\b|\\_([0-9][0-9])(?!e|[0-9])|(?:0[1-9])\\d\\d|(\\dx)([1-9])|\\dx0[1-9]");
    episodeGrab.setCaseSensitivity(Qt::CaseInsensitive);
    QStringList RomanNum;
    RomanNum << "bump" <<"I" << "II" << "III" << "IV" << "V" << "VI" << "VII" << "VIII" << "IX" << "X" ;
    QRegExp romanRegExp("I(?!=X)|(II)|(III)|(IV)|(V)|(VI)|p(VII)|(VIII)|(IX)|(X)");
    romanRegExp.setCaseSensitivity(Qt::CaseInsensitive);

    QString epNum;
    QString tmp_filename = filename;
    tmp_filename.chop(4);
    tmp_filename.replace (" ", ".");
    episodeGrab.setCaseSensitivity(Qt::CaseInsensitive);
    QString thisFile = tmp_filename;                                //i am bad and didnt set filename as protected. therefore i move it into a different string to prevent mangling it.

    int epIndex = episodeGrab.indexIn(tmp_filename);                //index of the match
    //cout << epIndex;
    int emPos = episodeGrab.matchedLength();                        //length of the match
    // qDebug () <<"Episode Matches" <<episodeGrab.capturedTexts().join("|");
    tmp_filename.replace(0,epIndex,"");                             //replacing all characters from the start position (0) to the position of our match (epIndex)
    int eBack =tmp_filename.size()-emPos;                           //calculating the amount of characters to remove AFTER our matched string
    tmp_filename.remove(emPos, eBack);                              //removing characters from our matched index+the lengtch of the match to the end of the string.
    QRegExp eClean;
    eClean.setPattern("[1-9]0(?=[1-9])|[1-9](?=[1-9]\\d)|(e)|(e0)|(ep)|(_)(?=\\d)|\\dx|\\dx0|pt.|\\s|\\dof|0(?=[1-9])");                         //removing any bogies from our airspace
    eClean.setCaseSensitivity(Qt::CaseInsensitive);
    tmp_filename.replace(eClean, "");

    int roman = romanRegExp.indexIn(tmp_filename);
    if (roman != -1)
	{
	cout << "Roman Numerals...attempting to decode...";

	epNum = QString::number(RomanNum.indexOf(tmp_filename));
	cout << qPrintable(epNum) << endl;
	}
    else
	{
	epNum = tmp_filename;                                              
	}
    if (epNum == "-1")
	{
	epNum = "null";
	}
    return epNum;                                                  //return the string. this example is employed as mentioned for series name, season, and episode.
}

QString tvshow::setSeason(QString filename)
{
    seasonGrab.setPattern("s([0-9][0-9])|s([1-9])|s0([1-9])|pt([1-9])|pt([0-9][0-9])|([1-9])(?=\\d[1-9])|([1-9])(?=of[0-9][0-9])|([1-9])(?=x[0-9])(?!\\d\\d)|(s\\d)*(?=D\\d)|season\\s([1-9])|season\\s[1-9][0-9]");
    seasonGrab.setCaseSensitivity(Qt::CaseInsensitive);
                                                                             //this block deals with extracting the season
    QString tmp_filename = filename;
    int sIndex = seasonGrab.indexIn(tmp_filename);
    int sPos = seasonGrab.matchedLength();
    QStringList sNumRes = seasonGrab.capturedTexts();
    tmp_filename.replace(0, sIndex, "");
    int sBack = tmp_filename.size()-sPos;
    tmp_filename.remove(sPos, sBack);
    QRegExp sClean;
    sClean.setPattern("season|(s)|(s0)|(pt)|(x\\d)|(x\\d)|(\\s)");
    sClean.setCaseSensitivity(Qt::CaseInsensitive);
    tmp_filename.replace(sClean, "");
    QString sNum = tmp_filename;

    if (sIndex == -1)
        {
        sNum = "1";
        }
	else if (sIndex == 0)
	{
	 sNum = "1";
	}
    return sNum ;
}

QString tvshow::setShow(QString filename)
{
    QString workingFile = filename;

    QRegExp fileCleaner(" \\- |\\_|\\s|\\b\\d\\b");
    workingFile.replace(" ", ".");
    QRegExp newSeries(".([1-9]\\d\\d\\d).");

    rez.setPattern("(720|1080)p|(720|1080)");
    int tv_rez = rez.indexIn(workingFile);
    QString temp_rez = rez.cap (0);
    temp_rez.remove ("p");

    if (temp_rez.isEmpty ())
	{
	this->t_rez ="3";
	}
    else if (temp_rez == "720")
	{
	t_rez="4";
	}
    else if (temp_rez == "1080")
	{
	t_rez = "5";
	};

    titleGrab.setPattern("s[1-9]\\d|s([1-9])|s(0[1-9])|ep\\d|pt([1-9])|pt([1-9]\\d)|.([1-9])(?=\\d[1-9])|.([0-9][1-9])|([1-9])(?=of\\d\\d)|(_)(?=\\d)|(\\dof\\d)|(\\[\\dx\\d)|pt(?=.[a-z])|(?=\\d\\d)(?!\\d\\d)|([1-9]x)|\\s0\\d\\d\\d|ep\\d|episode*\\d|episode|season");
    titleGrab.setCaseSensitivity(Qt::CaseInsensitive);
    int thisTitle = titleGrab.indexIn(workingFile);
   //qDebug () << "Title Matches" << titleGrab.capturedTexts ().join ("|");
    int fileLen = workingFile.count();
    int matchLen = titleGrab.matchedLength();
    int ChopPoint = fileLen - thisTitle;
    //qDebug () << titleGrab.capturedTexts ().join ("|");
    QString cleanTitle;
    if (matchLen > 0)
    {
     workingFile.remove(thisTitle, ChopPoint);
     workingFile.replace(fileCleaner,".");
     workingFile.replace("."," ");
     cleanTitle= workingFile.simplified ();
     int n_series = newSeries.indexIn (filename);

     if (n_series == -1)
	 {
	 cout << "Name=" << qPrintable(cleanTitle) << endl;
	 //qDebug () << cleanTitle;
	 return cleanTitle;
	 }
     else
	 {
	 cleanTitle.append (" "+newSeries.capturedTexts ().at (1));
	 cleanTitle.replace ("(", "");

	 cout << "Name=" << qPrintable(cleanTitle) << endl;
	 return cleanTitle.simplified ();
	 }

    }
    else
    {
    cleanTitle = workingFile;
    cout <<"Failed to Parse: " << qPrintable(workingFile) << "Trying Anyways" << endl;
    return cleanTitle;
    }
}


void tvshow::replyFinished(QNetworkReply *)
{
    //reply = ((QNetworkReply *)sender());
    cout << "got response, reading....";
    QByteArray bytes = rep->readAll();

    QDomDocument tvxml("tV");
    tvxml.setContent(bytes);

    QDomElement root= tvxml.documentElement();

    QDomElement s = root.firstChildElement("Series");
    if (root.tagName() != "Data")
        {
	cout << "wrong element, xml error!" << endl;
	return;
        }
    int nodeLen;
    nodeLen = s.childNodes().count();

    QDomNodeList rList = root.elementsByTagName("Series");
    showTitle = s.firstChildElement("SeriesName").text();               //sets object show title
    seriesID = s.firstChildElement("id").text();                          //sets object series id - unique per show
    if (seriesID.count() > 0)
	{
	cout << "Processing Identifiers..." << endl;
	//cout << "Extended Data for: " << qPrintable(showTitle) << "--Season:" << qPrintable(season) << "|| Episode: " << qPrintable(episodeNum) << endl;
        this->getData();
        }
    else
	{
	cout << "Insufficient identifying data, moving to next file." << endl; return;
	}
}

void tvshow::createRequest()
{
     QNetworkAccessManager *n =  new QNetworkAccessManager(this);
     QUrl url("http://thetvdb.com/api/GetSeries.php?seriesname="+showTitle);
     QNetworkRequest req(url);
    cout << "Finding Series ID:" << qPrintable(url.toString ()) << endl;

    this->connect(n, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)));
     //this->connect(rep, SIGNAL(finished()), this, SLOT(replyFinished()));    
    rep = n->get(req);

     QEventLoop eventLoop;                                                          // execute an event loop to process the request (nearly-synchronous)
     connect(n, SIGNAL(finished(QNetworkReply *)), &eventLoop, SLOT(quit()));       // this loop MUST be utilized as you will have no way of making your current file loop wait for data to
     eventLoop.exec();                                                              //continue. also dispose the event loop after the reply has arrived
}

void tvshow::error()
{
    cout << qPrintable(reply->errorString()) << endl;
}

void tvshow::downloadFresh ()
{
    cout << "reaching out to TheTVDB.com----------";
    QEventLoop XeventLoop;
    QNetworkAccessManager *xmlGet =  new QNetworkAccessManager(this);
    QUrl url("http://www.thetvdb.com/api/"+apiKey+"/series/"+seriesID+"/all/en.zip");
    QNetworkRequest xmlreq(url);
    // qDebug () << url;
    this->connect(xmlGet, SIGNAL(finished(QNetworkReply *)), &XeventLoop, SLOT(quit()));

    cout << "initiating xml request " << endl;
    QNetworkReply *xmlrep = xmlGet->get(xmlreq);                                                              // execute an event loop to process the request (nearly-synchronous)
    connect(xmlGet, SIGNAL(finished(QNetworkReply *)), &XeventLoop, SLOT(quit()));       // this loop MUST be utilized as you will have no way of making your current file loop wait for data to
    XeventLoop.exec();                                                              //continue. also dispose the event loop after the reply has arrived

    QByteArray data_zipFile = xmlrep->readAll ();
    QFile zipFile("/var/tmp/"+seriesID+"/en.zip");
    zipFile.setPermissions (QFile::ExeOther);

    if (!zipFile.open (QFile::WriteOnly))
	{
	cout << "Cant write Open file!" << endl;
	//qDebug () << zipFile.errorString ();
	return;
	}
    else
	{
	cout << "Wrote Zip Data for:" << qPrintable(showTitle) << endl;
	zipFile.write (data_zipFile);
	zipFile.close ();

	QDir working;
	working.setPath ("/var/tmp/"+seriesID+"/");


	QProcess *unzip = new QProcess(this);
	unzip->setWorkingDirectory ("/var/tmp/"+seriesID);
	cout << "Expanding" << endl;
	//QTime wth;
	unzip->start("unzip -o en.zip");
	if (unzip->Running)
	    {
	   // QByteArray prog = unzip->readAllStandardOutput();                                //my attempt to monitor unzip progress outside of a 'finished' signal. not working
	    //qDebug () << prog;
	    }

	QEventLoop waitForZip;
	connect(unzip, SIGNAL(finished(int,QProcess::ExitStatus)), &waitForZip, SLOT(quit()));
	waitForZip.exec();
	QFile::remove (zipFile.fileName ());
	dataFinished();
	}
}

void tvshow::getData()
{
    /*
    this function serves to download the .zip of the series
    it constructs the url, downloads the .zip to its working
    files are <lang>.xml, actors.xml, banners.xml
    QProcess, and external process monitoring class  is utilized to monitor wget, and then    unzip
    in conjunction with QEventLoop to monitor their progress befor xml parsing continues.
    I didnt utilize qt's stream functions as i could not get them working due to file permissions it seems
    */
    apiKey = "4C6CEBDFB4558279";
    QDir dataDir("/var/tmp/"+seriesID);
    if (dataDir.exists())
	{
	cout << "Directory - Check" << endl;
	QFile langXml("/var/tmp/"+seriesID+"/en.xml");

	if (langXml.open (QFile::ReadOnly))
	    {
	    cout << "Data Exists, checking freshness.....";
	    langXml.close ();
	    QRegExp timeGrab("(?=\\d\\d\\s)(\\d\\d)");
	    QDateTime dlClock;
	    QDateTime fileClock;
	    //qDebug () << dlClock.currentDateTime ();
	    QFileInfo xmlDate;
	    QString timeText;
	    timeGrab.indexIn (dlClock.currentDateTime ().toString ());
	    QString eff = timeGrab.capturedTexts ().at (0);

	    xmlDate.setFile (langXml);
	    xmlDate.setCaching (false);
	    timeGrab.indexIn (xmlDate.created ().toString ("ddd MMM dd hh:mm:ss:z yyyy"));

	    QString tText = timeGrab.capturedTexts ().at (0);
	   // qDebug () << tText;
	    timeGrab.indexIn (xmlDate.created ().toString ());

	    timeText = timeGrab.capturedTexts ().at (0);
	    int dayOffset = 1;
	    int offset = tText.toInt () + dayOffset;
	    cout << "update Interval set to ("<< dayOffset << ") day(s).....";
	    //qDebug () << eff ;qDebug () << offset;
	    if (offset > timeText.toInt ())
		{
		cout << "Data fresh!" << endl;
		dataFinished();
		}
	    else
		{
		cout << "Data old and moldy, freshening up" << endl;
		downloadFresh();
		}
	    }
	else
	    {
	    cout << "No Files in directory" << endl;
	   downloadFresh();
	    }
	}
    else
	{
	cout << "No Existing Directory, Creating....";
	QProcess *perm = new QProcess(this);
	perm->start("mkdir /var/tmp/"+seriesID+"/ -m 0777");
	cout << "..done! Getting Data" << endl;
	downloadFresh();
	}
}

void tvshow::dataFinished()                                                            //this is the function that utilizes unzip after the wget request
{
    cout << "Cleanup" << endl;

    cout << "Setting Attributes" << endl;
    setAttributes();
}

void tvshow::setAttributes()                                                        //this function begins to set the attributes for the current object before we begin sql queries
{
    seriesXml.setFileName("/var/tmp/"+seriesID+"/en.xml");
    actXml.setFileName("/var/tmp/"+seriesID+"/actors.xml");
    bannerXml.setFileName("/var/tmp/"+seriesID+"/banners.xml");

     if(!bannerXml.open(QFile::ReadOnly))                                           //open the banners file and get the url for the season banner. this is later used for the show or season
	{
	cout << "Frakking couldnt open Banners.xml" << endl;
	bannerXml.close();
	return;
	}
   else
       {
       cout << "Opened Banners File, setting content...." ;
       QByteArray feeder = bannerXml.readAll ();
      Banners.setContent(feeder);
       if(feeder.isNull ())
	   {
	   cout << "Couldnt open Banners File. Frak!" << endl;
	   bannerXml.close ();
	   return ;
	   }       
       else
	   {
	   bannerXml.close ();
	   cout << "Now Searching for images..." <<endl;

	   QDomElement bannerRoot = Banners.documentElement ();
	   //qDebug () << bannerRoot.childNodes ().length ();

	   QDomNodeList b_list = bannerRoot.childNodes();

	   for (int listCnt = 0; listCnt < b_list.length (); listCnt++)
	       {
	       QDomNode currentBanner = b_list.at (listCnt).toElement ();

	       if (currentBanner.toElement ().elementsByTagName ("Season").at (0).toElement ().text ()== this->season ) //identifying season banner
		   {
                   seasonImg.append ("http://thetvdb.com/banners/"+currentBanner.toElement ().elementsByTagName ("BannerPath").at (0).toElement ().text ());
		   }
	       else if (currentBanner.toElement ().elementsByTagName ("BannerType").at (0).toElement ().text ()== "series" && currentBanner.toElement ().elementsByTagName ("BannerType2").item (0).toElement ().text ()== "graphical")
		   {
                    seriesBanner.append ("http://thetvdb.com/banners/"+currentBanner.toElement ().elementsByTagName ("BannerPath").at (0).toElement ().text ());

		   }

	       else if
		       ( currentBanner.toElement ().elementsByTagName ("BannerType").at (0).toElement ().text ()== "fanart")
		   {
		       //cout << "Chosen Vigniette has rating of:" << picRating << endl;
		       if (currentBanner.toElement ().elementsByTagName ("SeriesName").at (0).toElement ().text ()== "true")
		       {
                           seriesImg.append ("http://thetvdb.com/banners/"+currentBanner.toElement ().elementsByTagName ("BannerPath").at (0).toElement ().text ());
			   }
		       else
			   {
                    seriesGraphic.append ("http://thetvdb.com/banners/"+currentBanner.toElement ().elementsByTagName ("BannerPath").at (0).toElement ().text ());
                    viginettes.append("http://thetvdb.com/banners/"+currentBanner.toElement ().elementsByTagName ("VignettePath").at (0).toElement ().text ());
			   }
		   }
	       }
	   }
       cout << endl << "Found ("<< seriesImg.count ()<< ") Series Poster(s) with text" <<endl;
       cout << "Found (" << seasonImg.count () << ") Season "<< qPrintable(this->season) << " banner(s) with text" << endl;

       cout <<  "found ("<< viginettes.count () << ") Show Viginette(s)" << endl;
       cout <<  "found ("<< seriesGraphic.count () << ") blank series graphic(s)" << endl;
     // qDebug () << txtVig.join (",");
       }



   if (!seriesXml.open(QFile::ReadOnly))
       {
       cout << "Error opening Base Series Record :(" << endl;
       return ;
       }
   else
       {
       if(!Series.setContent(&seriesXml))                                        //'series' is of type QDomDocument declared in header
	   {
	   cout << "Cant set series Data!" << endl;
	   seriesXml.close();
	   return ;
	   }
       else
	   {
	   seriesXml.close();
	   QDomElement route = Series.documentElement();                         //sets document root at series
	   //a more elegant manner could be employed as this method of selecting search results is quite simple. it grabs the 1st one, assuming that its the closest match
	   //this is not always the case.
	   genre = route.firstChildElement("Series").elementsByTagName("Genre").at(0).toElement().text();
	   IMDB = route.firstChildElement("Series").elementsByTagName("IMDB_ID").at(0).toElement().text();
	   tvNetwork = route.firstChildElement("Series").elementsByTagName("Network").at(0).toElement().text();
	   tvRating = route.firstChildElement("Series").elementsByTagName("ContentRating").at(0).toElement().text();
	   actor = route.firstChildElement("Series").elementsByTagName("Actors").at(0).toElement().text();
	   seriesID = route.firstChildElement("Series").elementsByTagName("SeriesID").at(0).toElement().text();
	   QDomNodeList eps = route.elementsByTagName("Episode");
	   int epNodeLen = eps.length();
	   int epCounter = 0;
	   while (epCounter < epNodeLen)                                                        // this is a brute force method that looks for the matching params (season + episode no)
	       {
	       QDomNode currentEp = eps.item(epCounter);

	       if (currentEp.namedItem("SeasonNumber").toElement().text() == season && currentEp.namedItem("EpisodeNumber").toElement().text() == episodeNum)
		   {
		   cout << "Found the Episode!" << endl;                                              //once episode is located, get information before exiting loop
		   QDomElement episode = currentEp.toElement();
		   episodeTitle = episode.firstChildElement("EpisodeName").toElement().text();
		   synopsis = episode.firstChildElement("Overview").toElement().text();
		   tvDirector = episode.firstChildElement("Director").toElement().text().replace("|" , episode.firstChildElement("Director").toElement().text());
		   guestStars = episode.firstChildElement("GuestStars").toElement().text();
		   airdate = episode.firstChildElement("FirstAired").toElement().text();
                   epImage ="http://thetvdb.com/banners/"+episode.firstChildElement("filename").toElement().text();
		   dvdSeason= episode.firstChildElement("DVD_season").toElement().text();
		   dvdChapter= episode.firstChildElement("DVD_chapter").toElement().text();
		   dvdEpisode= episode.firstChildElement("DVD_episode").toElement().text();
		   dvdDiscID= episode.firstChildElement("DVD_discID").toElement().text();
		   progID = episode.firstChildElement("id").toElement().text();                      //prog ID is mapped to the id for specific episode
		   seasonID = episode.firstChildElement("seasonid").toElement().text();              //mapped as specific season identifier
		   seriesID = episode.firstChildElement("seriesid").toElement().text();

		   //debugging block to verify data is being read and processed.
		   cout << "|====Attributes===|" << endl;
		   cout << "Show: " << qPrintable(showTitle) << ", Season: " << qPrintable(season) << ", Episode Number: " << qPrintable(episodeNum) << endl;
		   cout << "Episode: " << qPrintable(episodeTitle) << " |-----|  Director: " << qPrintable(tvDirector) <<endl;
		   cout << "Regular Performers: " << qPrintable(actor) << endl;
		   cout << "Guest Stars: " << qPrintable(guestStars) << endl;
		   cout << "Synopsis: " << endl;
		   cout << qPrintable(synopsis) << endl;
		   cout << "IMDB: " << qPrintable(IMDB) << " == Rating: " << qPrintable(tvRating) << endl;
		   cout <<  "Program ID:" << qPrintable(progID)<< "||  Show ID: " << qPrintable(progID) << endl;
		   cout << "Genre(s): " << qPrintable(genre) << endl;
		   cout << "Tv Network: " << qPrintable(tvNetwork) << endl;
		   cout << "Dvd Info********************" << endl;
		   cout << "Dvd season:" << qPrintable(dvdSeason) << ", Dvd Episode: " << qPrintable(dvdEpisode) << ", Dvd Chapter:" << qPrintable(dvdChapter) <<  endl;
		   cout << "Dvd Disc ID:" << qPrintable(dvdDiscID) << endl;
		   cout << "End DVD Info****************" << endl;
		   cout << "|======End xml extraction========================|" << endl;
		   return;
		   }
	       epCounter++;
	       }
	   }

       cout << "|====Attributes===|" << endl;
       cout << "|====Episode Not Matched!!===|" << endl;
       cout << "|======End========|" << endl;

       }
   }
