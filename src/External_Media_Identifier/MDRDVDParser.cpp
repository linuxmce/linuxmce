/**
 * MDRDVDParser - A class that parses a data reply from dvdxml.com.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 *
 * needs --std=c++11 as it uses for ranges.
 *
 */

#include "MDRDVDParser.h"
#include "pugixml.hpp"
#include <iostream>

using namespace std;
using namespace pugi;

MDRDVDParser::MDRDVDParser(string sReplyXML)
{
  m_sReplyXML=sReplyXML;
}

MDRDVDParser::~MDRDVDParser()
{
  if (m_pReply)
    delete m_pReply;
}

bool MDRDVDParser::parse()
{
  xml_document doc;
  xml_parse_result result = doc.load_buffer(m_sReplyXML.data(),m_sReplyXML.length());

  if (result.status != status_ok)
    return false;

  string sDVDTitle=doc.child("METADATA").child("MDR-DVD").child_value("dvdTitle");
  cout << "Added Title: " << sDVDTitle << endl;
  string sStudio=doc.child("METADATA").child("MDR-DVD").child_value("studio");
  cout << "Added Studio: " << sStudio << endl;
  string sActors=doc.child("METADATA").child("MDR-DVD").child_value("actors");
  cout << "Added Actors: " << sActors << endl;
  string sDirector=doc.child("METADATA").child("MDR-DVD").child_value("director");
  cout << "Added Director: " << sDirector << endl;
  string sRating=doc.child("METADATA").child("MDR-DVD").child_value("MPAARating");
  cout << "Added Rating: " << sRating << endl; 
  string sReleaseDate=doc.child("METADATA").child("MDR-DVD").child_value("releaseDate");
  cout << "Added Release Date: " << sReleaseDate << endl;
  string sGenre=doc.child("METADATA").child("MDR-DVD").child_value("genre");
  cout << "Added Genre: " << sGenre << endl;
  string sDVDCoverURL=doc.child("METADATA").child("MDR-DVD").child_value("largeCoverParams");
  sDVDCoverURL=string(DVD_COVER_URL_PREFIX) + "/" + sDVDCoverURL;
  cout << "Added DVD Cover URL: " << sDVDCoverURL << endl;
  
  m_pReply=new MDRDVDReply(sDVDTitle,
			   sStudio,
			   sActors,
			   sDirector,
			   sRating,
			   sReleaseDate,
			   sGenre,
			   sDVDCoverURL);

  cout << "Created Reply object" << endl;
  
  for (pugi::xml_node xnTITLE: doc.child("METADATA").child("MDR-DVD").children())
    {
      string sNodeName=xnTITLE.name();

      if (sNodeName != "title")
	continue;

      cout << "Constructing MDRTitle" << endl;

      xml_node xnTitleNum=xnTITLE;
      xml_node xnTitle=xnTITLE;
      xml_node xnStudio=xnTITLE;
      xml_node xnDirector=xnTITLE;
      xml_node xnActors=xnTITLE;
      xml_node xnRating=xnTITLE;
      xml_node xnGenre=xnTITLE;
      string sTitleNum=xnTitleNum.child_value("titleNum");
      cout << "Added Title #" << sTitleNum << endl;
      string sTitle=xnTitle.child_value("titleTitle");
      cout << "Added Title: " << sTitle << endl;
      string sStudio=xnStudio.child_value("studio");
      cout << "Added Title: " << sStudio << endl;
      string sDirector=xnDirector.child_value("director");
      cout << "Added Director: " << sDirector << endl;
      string sActors=xnActors.child_value("actors");
      cout << "Added Actors: " << sActors << endl;
      string sRating=xnRating.child_value("MPAARating");
      cout << "Added Rating: " << sRating << endl;
      string sGenre=xnGenre.child_value("genre");
      cout << "Added Genre: " << sGenre << endl;
      MDRTitle* t = new MDRTitle(sTitleNum,
				 sTitle,
				 sStudio,
				 sDirector,
				 sActors,
				 sRating,
				 sGenre);
      
      cout << "MDRTitle Constructed" << endl;

      for (pugi::xml_node xnCHAPTER: xnTITLE.children("chapter"))
	{
	  cout << "Constructing MDRChapter" << endl;
	  xml_node xnChapterNum=xnCHAPTER;
	  xml_node xnChapterTitle=xnCHAPTER;
	  string sChapterNum=xnChapterNum.child_value("chapterNum");
	  cout << "Added Chapter #" << sChapterNum << endl;
	  string sChapterTitle=xnChapterTitle.child_value("chapterTitle");
	  cout << "Added Chapter Title: " << sChapterTitle << endl;
	  t->Chapter_add(new MDRChapter(sChapterNum, sChapterTitle));
	  cout << "Added Chapter to title. " << endl;
	}
      m_pReply->Title_add(t);
      cout << "Added Title to Reply." << endl;
    }

  return true;
}

// int main(int argc, char** argv)
// {
//   MDRDVDParser* p = new MDRDVDParser("<?xml version='1.0' encoding='UTF-8' ?><METADATA xmlns:sql='urn:schemas-microsoft-com:xml-sql'><MDR-DVD><version>4.0</version><dvdTitle>The Departed [WS] [2 Discs]</dvdTitle><studio>Warner Home Video</studio><leadPerformer>Leonardo DiCaprio; Matt Damon; Jack Nicholson; Mark Wahlberg; Martin Sheen</leadPerformer><actors>Leonardo DiCaprio; Matt Damon; Jack Nicholson; Mark Wahlberg; Martin Sheen</actors><director>Martin Scorsese</director><MPAARating>R</MPAARating><releaseDate>2007 02 13</releaseDate><genre>Crime</genre><largeCoverParams>cov150/drt900/t993/t99353lzkro.jpg</largeCoverParams><smallCoverParams>cov075/drt900/t993/t99353lzkro.jpg</smallCoverParams><dataProvider>AMG</dataProvider><wmid_dvd>89E423D4-DB4F-4131-9B0A-BB16AEF70DE9</wmid_dvd><dv_id>E   153922          </dv_id><dataProviderParams>Provider=AMG</dataProviderParams><dataProviderLogo>Provider=AMG</dataProviderLogo><moreInfoParams></moreInfoParams><title><titleNum>1</titleNum><titleTitle>The Departed [WS] [2 Discs]</titleTitle><studio>Warner Home Video</studio><director>Martin Scorsese</director><leadPerformer>Leonardo DiCaprio; Matt Damon; Jack Nicholson; Mark Wahlberg; Martin Sheen</leadPerformer><actors>Leonardo DiCaprio; Matt Damon; Jack Nicholson; Mark Wahlberg; Martin Sheen</actors><MPAARating>R</MPAARating><genre>Crime</genre><providerRating>8</providerRating><communityRating></communityRating><chapter><chapterNum>1</chapterNum><chapterTitle>Costello Credo [5:01]</chapterTitle></chapter><chapter><chapterNum>2</chapterNum><chapterTitle>School&apos;s Out [3:48]</chapterTitle></chapter><chapter><chapterNum>3</chapterNum><chapterTitle>You&apos;re No Cop [6:03]</chapterTitle></chapter><chapter><chapterNum>4</chapterNum><chapterTitle>Serve the Commonwealth [4:15]</chapterTitle></chapter><chapter><chapterNum>5</chapterNum><chapterTitle>He&apos;s Jackie&apos;s Nephew [4:50]</chapterTitle></chapter><chapter><chapterNum>6</chapterNum><chapterTitle>Dignam Briefs the Girls [1:59]</chapterTitle></chapter><chapter><chapterNum>7</chapterNum><chapterTitle>Tipped Off, Ticked Off [4:05]</chapterTitle></chapter><chapter><chapterNum>8</chapterNum><chapterTitle>Impervious to Analysis [2:14]</chapterTitle></chapter><chapter><chapterNum>9</chapterNum><chapterTitle>Necessary Torture [4:50]</chapterTitle></chapter><chapter><chapterNum>10</chapterNum><chapterTitle>Two Guys From Providence [5:33]</chapterTitle></chapter><chapter><chapterNum>11</chapterNum><chapterTitle>May I Remind You [4:41]</chapterTitle></chapter><chapter><chapterNum>12</chapterNum><chapterTitle>Call Your Mother [3:55]</chapterTitle></chapter><chapter><chapterNum>13</chapterNum><chapterTitle>How Billy Feels [3:26]</chapterTitle></chapter><chapter><chapterNum>14</chapterNum><chapterTitle>Hang Tight for Me [4:52]</chapterTitle></chapter><chapter><chapterNum>15</chapterNum><chapterTitle>Call the Game [3:25]</chapterTitle></chapter><chapter><chapterNum>16</chapterNum><chapterTitle>No Tickee, No Laundry [4:36]</chapterTitle></chapter><chapter><chapterNum>17</chapterNum><chapterTitle>Relationship ups and Downs [5:17]</chapterTitle></chapter><chapter><chapterNum>18</chapterNum><chapterTitle>If You Coulda [1:15]</chapterTitle></chapter><chapter><chapterNum>19</chapterNum><chapterTitle>Protected and Benumbed [:38]</chapterTitle></chapter><chapter><chapterNum>20</chapterNum><chapterTitle>Real Vulnerability [4:30]</chapterTitle></chapter><chapter><chapterNum>21</chapterNum><chapterTitle>Take Care of Business [3:16]</chapterTitle></chapter><chapter><chapterNum>22</chapterNum><chapterTitle>Each Other&apos;s Trail [4:38]</chapterTitle></chapter><chapter><chapterNum>23</chapterNum><chapterTitle>Heavy Lies the Crown [2:55]</chapterTitle></chapter><chapter><chapterNum>24</chapterNum><chapterTitle>The One That Gets Out [6:13]</chapterTitle></chapter><chapter><chapterNum>25</chapterNum><chapterTitle>Tail on Queenan [2:58]</chapterTitle></chapter><chapter><chapterNum>26</chapterNum><chapterTitle>Taking the Fall [4:16]</chapterTitle></chapter><chapter><chapterNum>27</chapterNum><chapterTitle>Wrong Number [3:57]</chapterTitle></chapter><chapter><chapterNum>28</chapterNum><chapterTitle>Dead Guy&apos;s Phone [4:06]</chapterTitle></chapter><chapter><chapterNum>29</chapterNum><chapterTitle>Nation of Rats [5:00]</chapterTitle></chapter><chapter><chapterNum>30</chapterNum><chapterTitle>Sheffield Shootout [3:58]</chapterTitle></chapter><chapter><chapterNum>31</chapterNum><chapterTitle>Getting Frank [2:13]</chapterTitle></chapter><chapter><chapterNum>32</chapterNum><chapterTitle>Deleted [2:56]</chapterTitle></chapter><chapter><chapterNum>33</chapterNum><chapterTitle>Play Me Now [4:49]</chapterTitle></chapter><chapter><chapterNum>34</chapterNum><chapterTitle>Rats on a Rooftop [4:26]</chapterTitle></chapter><chapter><chapterNum>35</chapterNum><chapterTitle>The Departed [3:36]</chapterTitle></chapter><chapter><chapterNum>36</chapterNum><chapterTitle>Okay [4:22]</chapterTitle></chapter><chapter><chapterNum>37</chapterNum><chapterTitle>End Credits [1:26]</chapterTitle></chapter></title></MDR-DVD></METADATA>");

//   if (p->parse())
//     {
//       MDRDVDReply* r = p->Reply_get();
//       cout << " -- Reply Global Data ---------------------- " << endl;
//       cout << r->DVDTitle_get() << endl;
//       cout << r->Studio_get() << endl;
//       cout << r->Actors_get() << endl;
//       cout << r->Director_get() << endl;
//       cout << r->Rating_get() << endl;
//       cout << r->Genre_get() << endl;
//       vector<MDRTitle*> vectTitles = r->Titles_get();
//       for (vector<MDRTitle*>::iterator it=vectTitles.begin(); it!=vectTitles.end(); ++it)
// 	{
// 	  MDRTitle* t = *it;
// 	  cout << " ------------------------------------------- " << endl;
//       	  cout << t->TitleNum_get() << endl;
// 	  cout << t->Title_get() << endl;
// 	  cout << t->Studio_get() << endl;
// 	  cout << t->Director_get() << endl;
// 	  cout << t->Actors_get() << endl;
// 	  cout << t->Rating_get() << endl;
// 	  cout << t->Genre_get() << endl;
// 	  vector<MDRChapter*> vectChapters = t->Chapters_get();
// 	  for (vector<MDRChapter*>::iterator cit=vectChapters.begin(); cit!=vectChapters.end(); ++cit)
// 	    {
// 	      MDRChapter* c = *cit;
// 	      cout << " ======================================= " << endl;
// 	      cout << c->ChapterNum_get() << endl;
// 	      cout << c->ChapterTitle_get() << endl;
// 	    }
// 	}
//     }

//   delete p;
//   return 0;
// }
