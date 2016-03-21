/**
 * DVDXMLParser - A class that parses a data reply from dvdxml.com.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 *
 */

/* <?xml version="1.0"?> */
/*     <dvdXml> */
/*     <authenticated>True</authenticated>             <dvd> */
/*     <title></title> */
/*     <dvdCover>http://www.dvdxml.com/dvdcover/.jpg</dvdCover> */
/*     <year>0</year> */
/*     <length>0</length> */
/*     <rating></rating> */
/*     <dvdId>3B509BE3|243449DA</dvdId> */
/*     <genre></genre> */
/*     <studio></studio> */
/*     <cast></cast> */
/*     <plot></plot> */
/*     </dvd */
/*     </dvdXml> */
    
#ifndef DVDXMLPARSER_H
#define DVDXMLPARSER_H

#include <string>

using namespace std;

class DVDXMLReply
{
  string m_sTitle;
  string m_sDVDCoverURL;
  string m_sYear;
  string m_sLength;
  string m_sRating;
  string m_sGenre;
  string m_sStudio;
  string m_sCast;
  string m_sPlot;

 public: 
  DVDXMLReply(string sTitle,
	      string sDVDCoverURL,
	      string sYear,
	      string sLength,
	      string sRating,
	      string sGenre,
	      string sStudio,
	      string sCast,
	      string sPlot)
    {
      m_sTitle=sTitle;
      m_sDVDCoverURL=sDVDCoverURL;
      m_sYear=sYear;
      m_sLength=sLength;
      m_sRating=sRating;
      m_sGenre=sGenre;
      m_sStudio=sStudio;
      m_sCast=sCast;
      m_sPlot=sPlot;
    }

  string Title_get() {return m_sTitle;}
  string DVDCoverURL_get() {return m_sDVDCoverURL;}
  string Year_get() {return m_sYear;}
  string Length_get() {return m_sLength;}
  string Rating_get() {return m_sRating;}
  string Genre_get() {return m_sGenre;}
  string Studio_get() {return m_sStudio;}
  string Cast_get() {return m_sCast;}
  string Plot_get() {return m_sPlot;}

};

class DVDXMLParser
{
  string m_sReplyXML;
  DVDXMLReply* m_pReply;

 public:

  DVDXMLParser(string sReplyXML);
  virtual ~DVDXMLParser();

  bool parse();
  DVDXMLReply* Reply_get() {return m_pReply;}
  
};

#endif /* DVDXMLPARSER_H */
