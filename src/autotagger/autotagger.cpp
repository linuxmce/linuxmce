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

    QRegExp i_movieFilter( ".[1-9]\\d\\d\\d.(?!.S)| \\d\\d\\d\\d(?!.\\d)|[1-9][0-9][0-9][0-9](?!.S)" );                   //setup movie reg expression filter
    i_movieFilter.setCaseSensitivity(Qt::CaseInsensitive);
    QRegExp i_tvFilter("pt|s[0-9]|s[0-9][0-9]|e[0-9]|e[0-9][0-9]|[1-9][0-9][0-9](?!\\d)|[0-9]x[0-9]|ep[0-9]|ep[0-9]|BBC|PBS|[0-9]dof[0-9]|_[0-9]|\\s[0-9]|(?=.\\d\\d\\d\\d.)\\d\\d\\d(?!\\d)");    //setup tv reg expression filter
    i_tvFilter.setCaseSensitivity(Qt::CaseInsensitive);
    incfileName.chop(4);

    cout << "------------------Identifying Data" << "-----------------" << endl;
    i_movieFilter.indexIn(incfileName.simplified ());                     // find the index of the match
    mCap = i_movieFilter.cap(0);
    cout << "-------Movie Filter Hits::" <<  qPrintable(i_movieFilter.capturedTexts().join(" | ")) << endl;

    i_tvFilter.indexIn(incfileName.simplified ());                        // find index of match
    tvCap = i_tvFilter.cap(0);
    cout << "-------Tv Filter Hits::" << qPrintable(i_tvFilter.capturedTexts().join(" | ")) << endl;

    if ((mCap.length() > tvCap.length()))
	{
	return 1;
	}
    else if ((tvCap.length() > mCap.length() ))
	{
	return 2;
	}
    else
	{
	return 1;
	};
};


