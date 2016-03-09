/**
 * MDRDVDParser - A class that parses a data reply from dvdxml.com.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 *
 */

#ifndef DVDXMLPARSER_H
#define DVDXMLPARSER_H

#include <string>
#include <vector>

#define DVD_COVER_URL_PREFIX "http://images.windowsmedia.com/dvdcover/"

using namespace std;

class MDRChapter
{
  string m_sChapterNum;
  string m_sChapterTitle;

 public:

  MDRChapter(string sChapterNum, string sChapterTitle)
    {
      m_sChapterNum=sChapterNum;
      m_sChapterTitle=sChapterTitle;
    }

  string ChapterNum_get() {return m_sChapterNum;}
  string ChapterTitle_get() {return m_sChapterTitle;}
  
};

class MDRTitle
{
  string m_sTitleNum;
  string m_sTitle;
  string m_sStudio;
  string m_sDirector;
  string m_sActors;
  string m_sRating;
  string m_sGenre;
  vector <MDRChapter*> m_vectChapters;
  
 public:
  MDRTitle(string sTitleNum,
	   string sTitle,
	   string sStudio,
	   string sDirector,
	   string sActors,
	   string sRating,
	   string sGenre)
    { 
      m_sTitleNum=sTitleNum;
      m_sTitle=sTitle;
      m_sStudio=sStudio;
      m_sDirector=sDirector;
      m_sActors=sActors;
      m_sRating=sRating;
      m_sGenre=sGenre;
    }

  virtual ~MDRTitle()
    {
      for (vector<MDRChapter*>::iterator it=m_vectChapters.begin();it!=m_vectChapters.end();++it)
	{
	  delete *it;
	}
    }

  string TitleNum_get() {return m_sTitleNum;}
  string Title_get() {return m_sTitle;}
  string Studio_get() {return m_sStudio;}
  string Director_get() {return m_sDirector;}
  string Actors_get() {return m_sActors;}
  string Rating_get() {return m_sRating;}
  string Genre_get() {return m_sGenre;}

  vector<MDRChapter*> Chapters_get() {return m_vectChapters;}

  void Chapter_add(MDRChapter* chapter) {m_vectChapters.push_back(chapter);}
};

class MDRDVDReply
{
  string m_sDVDTitle;
  string m_sStudio;
  string m_sActors;
  string m_sDirector;
  string m_sRating;
  string m_sReleaseDate;
  string m_sGenre;
  string m_sDVDCoverURL;
  vector<MDRTitle*> m_vectTitles;
  
 public:
  MDRDVDReply(string sDVDTitle,
	      string sStudio,
	      string sActors,
	      string sDirector,
	      string sRating,
	      string sReleaseDate,
	      string sGenre,
	      string sDVDCoverURL)
    {
      m_sDVDTitle=sDVDTitle;
      m_sStudio=sStudio;
      m_sActors=sActors;
      m_sDirector=sDirector;
      m_sRating=sRating;
      m_sReleaseDate=sReleaseDate;
      m_sGenre=sGenre;
    }

  virtual ~MDRDVDReply()
    {
      for (vector<MDRTitle*>::iterator it=m_vectTitles.begin();it!=m_vectTitles.end();++it)
	{
	  delete *it;
	}
    }

  string DVDTitle_get() {return m_sDVDTitle;}
  string Studio_get() {return m_sStudio;}
  string Actors_get() {return m_sActors;}
  string Director_get() {return m_sDirector;}
  string Rating_get() {return m_sRating;}
  string Genre_get() {return m_sGenre;}

  vector<MDRTitle*> Titles_get() {return m_vectTitles;}

  void Title_add(MDRTitle* title) {m_vectTitles.push_back(title);}

};

class MDRDVDParser
{
  string m_sReplyXML;
  MDRDVDReply* m_pReply;

 public:

  MDRDVDParser(string sReplyXML);
  virtual ~MDRDVDParser();

  bool parse();
  MDRDVDReply* Reply_get() {return m_pReply;}
  
};

#endif /* MDRDVDPARSER_H */
