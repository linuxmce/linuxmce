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
int videoMedia:: videoMediaType (QString incfileName) //here to determine if the file is a movie or tv
{
    using namespace std;

    QStringList matches;
    QStringList filmMatches;

    QRegExp i_movieFilter( ".[1-9]\\d\\d\\d.(?!.S)| \\d\\d\\d\\d(?!.\\d)|[1-9][0-9][0-9][0-9](?!.S)" );                   //setup movie reg expression filter
    i_movieFilter.setCaseSensitivity(Qt::CaseInsensitive);

    QRegExp i_tvFilter("pt|s[0-9]|s[0-9][0-9]|season\\s\\d");                                                                     //catch season markers

    QRegExp i_tvFilter_2("e[0-9]|e[0-9][0-9]|ep[0-9]|ep[0-9]|episode_\\D");                                                                      //catch episode markers

    QRegExp i_tvFilter_3("[0-9]x[0-9]| \\b\\d\\d\\d\\b");                                     // digit based tv shows

    QRegExp i_tvFilter_4("\\dof\\d|BBC|PBS");    //special 1 off series

    i_tvFilter.setCaseSensitivity(Qt::CaseInsensitive);
    i_tvFilter_2.setCaseSensitivity(Qt::CaseInsensitive);
    i_tvFilter_3.setCaseSensitivity(Qt::CaseInsensitive);
    i_tvFilter_4.setCaseSensitivity(Qt::CaseInsensitive);
    incfileName.chop(4);
    incfileName.remove (QRegExp ("720|1080|x264"));


 i_movieFilter.indexIn(incfileName.simplified ());
   if (!i_movieFilter.capturedTexts().isEmpty())
    {
       filmMatches.append(i_movieFilter.capturedTexts().join("|"));                                                                    // search movie string match
       mCap = i_movieFilter.cap(0);
   }

    i_tvFilter.indexIn(incfileName.simplified ());                        // find index of match seasons
   if (!i_tvFilter.cap(0).isNull())
    {
       matches.append("season" + i_tvFilter.capturedTexts().join("*"));
        tvCap = i_tvFilter.cap(0);
    }

   i_tvFilter_2.indexIn(incfileName.simplified ());                        // find index of match episodes
  if (!i_tvFilter_2.cap(0).isNull())
   {
      matches.append("episode-" + i_tvFilter_2.capturedTexts().join("*"));

   }

  i_tvFilter_3.indexIn(incfileName.simplified ());                        // find index of match seasons
 if (!i_tvFilter_3.cap(0).isNull())
  {
     matches.append("digit pattern-" + i_tvFilter_3.capturedTexts().join("*"));

  }

 i_tvFilter_4.indexIn(incfileName.simplified ());                        // find index of match specials and oneoffs
if (!i_tvFilter_4.cap(0).isNull())
 {
    matches.append("special tv" + i_tvFilter_4.capturedTexts().join("*"));

 }


    cout << "------------------Identifying Data" << "-----------------" << endl;
    if (i_movieFilter.capturedTexts().isEmpty())
    {
        cout << "No Movie Identifier" << endl;
    }
    else
    {
        cout << "-------Movie Filter Hits::" <<  qPrintable(i_movieFilter.capturedTexts().join(" | ")) << endl;
    }

    if (matches.isEmpty())
    {
        cout << "No Tv Identifiers Found!" << endl;
    }
    else
    {
    cout << "-------Tv Filter Hits::" << qPrintable(i_tvFilter.capturedTexts().join(" | "));
    cout << qPrintable(matches.join("|-|")) << endl;
    }

    if (matches.count() >= filmMatches.count())
    {
        cout << "tux determines television show - The TVDB.com"  << endl;
        return 2;

    }
    else if (filmMatches.isEmpty() && matches.isEmpty())
    {
        cout << "tux has no idea what this is. try a more descrptive name" << endl;
        return 0;
    }
    else
    {
        cout << "tux determines this is a film. How good it is, we cant know.";
        return 1;
      }




    if ((i_tvFilter.numCaptures () == i_movieFilter.numCaptures ()))
       {

	QString dblchk =  i_tvFilter.capturedTexts ().join ("");
	QRegExp regchk(dblchk);
	//qDebug () << dblchk << endl;
	int f = regchk.indexIn (i_movieFilter.capturedTexts ().join (""));
	//qDebug() << f << endl;
	if (f == -1)
	    {
	    cout << "Tv" << endl;
	    return 2;
	    }
	else
	    {
	    cout << "Movie!!!!!!!!!!!!!!!!!!!!" << endl;
       return 1;
	    }
       }
   else if ((mCap.count() > tvCap.count()))
	{
	return 1;
	}
    else if ((tvCap.count() > mCap.count() ))
	{
	return 2;
	}

    else
	{
	return 1;
	};
};


