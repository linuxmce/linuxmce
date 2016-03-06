/**
 * DVDXMLParser - A class that parses a data reply from dvdxml.com.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 *
 */

#include "DVDXMLParser.h"
#include "pugixml.hpp"
#include <iostream>

using namespace std;
using namespace pugi;

DVDXMLParser::DVDXMLParser(string sReplyXML)
{
  m_sReplyXML=sReplyXML;
}

DVDXMLParser::~DVDXMLParser()
{
  if (m_pReply)
    delete m_pReply;
}

bool DVDXMLParser::parse()
{
  xml_document doc;
  xml_parse_result result = doc.load_buffer(m_sReplyXML.data(),m_sReplyXML.length());

  if (result.status != status_ok)
    return false;

  xml_node dvdxml = doc.child("dvdXml");
  string sTitle = doc.child("dvdXml").child("dvd").child_value("title");
  string sCover = doc.child("dvdXml").child("dvd").child_value("dvdCover");
  string sYear = doc.child("dvdXml").child("dvd").child_value("year");
  string sLength = doc.child("dvdXml").child("dvd").child_value("length");
  string sRating = doc.child("dvdXml").child("dvd").child_value("rating");
  string sGenre = doc.child("dvdXml").child("dvd").child_value("genre");
  string sStudio = doc.child("dvdXml").child("dvd").child_value("studio");
  string sCast = doc.child("dvdXml").child("dvd").child_value("cast");
  string sPlot = doc.child("dvdXml").child("dvd").child_value("plot");
  
  m_pReply=new DVDXMLReply(sTitle,
			   sCover,
			   sYear,
			   sLength,
			   sRating,
			   sGenre,
			   sStudio,
			   sCast,
			   sPlot);
  
  return true;
}

// int main(int argc, char** argv)
// {
//   string sXML="";

//   sXML+="<dvdXml><authenticated>True</authenticated><dvd><title>The Departed [WS] [2 Discs]</title><dvdCover>http://www.dvdxml.com/dvdcover/3B509BE3243449DA.jpg</dvdCover><year>2006</year><length>151</length><rating>R</rating><dvdId>3B509BE3243449DA</dvdId><genre>Crime</genre><studio>Warner Home Video</studio><cast>Leonardo DiCaprio, Matt Damon, Jack Nicholson, Mark Wahlberg, Martin Sheen</cast><plot>Legendary director Martin Scorsese takes the helm for this tale of questionable loyalties and blurring identities set in the South Boston organized crime scene and inspired by the wildly popular 2002 Hong Kong crime film Infernal Affairs. As the police force attempts to reign in the increasingly powerful Irish mafia, authorities are faced with the prospect of sending in an undercover agent or seeing their already frail grip on the criminal underworld slip even further. Billy Costigan (Leonardo DiCaprio) is a young cop looking to make a name for himself in the world of law enforcement. Collin Sullivan (Matt Damon) is a street-smart criminal who has successfully infiltrated the police department with the sole intention of reporting their every move to ruthless syndicate head Frank Costello (Jack Nicholson). When Costigan is assigned the task of working his way into Costello's tightly guarded inner circle, Sullivan is faced with the responsibility of rooting out the informer before things get out of hand. With the stakes constantly rising and time quickly running out for the undercover cop and his criminal counterpart, each man must work feverishly to reveal his counterpart before his identity is exposed by the other. Martin Sheen, Alec Baldwin, and Ray Winstone co-star, and writer William Monahan adapts a screenplay originally penned by Alan Mak and Felix Chong. </plot></dvd></dvdXml>";

//   DVDXMLParser* p=new DVDXMLParser(sXML);
  
//   if (p->parse())
//     {
//       DVDXMLReply* r = p->Reply_get();
//       cout << r->Title_get() << endl;
//       cout << r->DVDCoverURL_get() << endl;
//       cout << r->Year_get() << endl;
//       cout << r->Length_get() << endl;
//       cout << r->Rating_get() << endl;
//       cout << r->Genre_get() << endl;
//       cout << r->Studio_get() << endl;
//       cout << r->Cast_get() << endl;
//       cout << r->Plot_get() << endl;
//     }

//   return 0;
// }
