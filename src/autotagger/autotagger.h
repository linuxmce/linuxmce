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
#ifndef AUTOTAGGER_H
#define AUTOTAGGER_H

#include <iostream>
#include <QtCore/QCoreApplication>
#include <QRegExp>
#include <QtNetwork>
#include <QObject>
#include <QtXml>
#include <qtextstream.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdir.h>
#include <QtSql>
#include <pilot.h>
#include <databaseFunctions.h>
#include <film.h>


using namespace std;

enum attribTypes{                   //this enum makes sanitizing the validation sql calls easier as we can just pass standard parameters and recieve a reply and conditionally inserts
DIRECTOR = 1,
PERFORMER = 2,
GENRE=8,
CHANNEL=10,
EPISODE = 11,
PROGRAM = 12,
TITLE = 13,
DISCID = 14,
WRITER = 16,
STUDIO = 17,
RATING = 18,
RELEASEDATE = 19,
WEBSITE = 20,
CHAPTER = 21,
ENCODING = 22,
FORMAT = 23,
RATED = 26,
DVDRELEASE = 27,
RUNTIME = 28,
SHOWFORMAT = 31,
REGION = 32,
LANGUAGE = 33,
IMDB = 36,
SYNOPSIS = 37,
PRODUCER = 38,
EXECPRODUCER = 39,
TVCHANID = 41,
TVPROGRAMID = 42,
TVSERIESID = 43,
SEASON_NUMBER = 50,
EPISODE_NUMBER = 51,
TVSEASONID = 52,
        };

enum mediaTypes{
    HD10810 = 5,
    HD720 = 4,
    STD_DEF = 3,
};



class videoMedia : public QObject
{
    Q_OBJECT
public:

        int fileNumber;
        QRegExp i_movieFilter;
        QRegExp i_tvFilter;
        QString mCap;
        QString tvCap;
        QString epNum;
        QRegExp yearGrab;
        QString incfileName;
        QString mediaName;
        QString mediaType;
        int videoMediaType(QString incfileName);
	QString capTvres;
};


#endif // AUTOTAGGER_H
