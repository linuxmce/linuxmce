/**
 * MDRCDParser - A class that parses a data reply from MDR
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 *
 */

#ifndef DVDXMLPARSER_H
#define DVDXMLPARSER_H

#include <string>
#include <vector>

using namespace std;

class MDRTrack
{
 private:
  string m_sTrackNumber;
  string m_sTrackTitle;
  
 public:
  MDRTrack(string sTrackNumber, string sTrackTitle)
    {
      m_sTrackNumber=sTrackNumber;
      m_sTrackTitle=sTrackTitle;
    }
  virtual ~MDRTrack()
    {
    
    }

  string TrackNumber_get() {return m_sTrackNumber;}
  string TrackTitle_get() {return m_sTrackTitle;}

};

class MDRCDReply
{
 private:
  string m_sAlbumTitle;
  string m_sAlbumArtist;
  string m_sReleaseDate;
  string m_sLabel; // map to Studio
  string m_sGenre;
  string m_sCoverURL;
  vector<MDRTrack*> m_vectTracks;

 public:
  MDRCDReply(string sAlbumTitle,
	     string sAlbumArtist,
	     string sReleaseDate,
	     string sLabel,
	     string sGenre,
	     string sCoverURL)
    {
      m_sAlbumTitle=sAlbumTitle;
      m_sAlbumArtist=sAlbumArtist;
      m_sReleaseDate=sReleaseDate;
      m_sLabel=sLabel;
      m_sGenre=sGenre;
      m_sCoverURL=sCoverURL;
    }

  virtual ~MDRCDReply()
    {
      for (vector<MDRTrack*>::iterator it=m_vectTracks.begin();it!=m_vectTracks.end();++it)
	{
	  delete *it;
	}
    }

  string AlbumTitle_get() {return m_sAlbumTitle;}
  string AlbumArtist_get() {return m_sAlbumArtist;}
  string ReleaseDate_get() {return m_sReleaseDate;}
  string Label_get() {return m_sLabel;}
  string Genre_get() {return m_sGenre;}
  string CoverURL_get() {return m_sCoverURL;}
  vector<MDRTrack*> Tracks_get() {return m_vectTracks;}
  void Track_add(MDRTrack* track) {m_vectTracks.push_back(track);}
};

class MDRCDParser
{
 private:

  string m_sReplyXML;
  MDRCDReply* m_pReply;

 public:

  MDRCDParser(string sReplyXML);
  virtual ~MDRCDParser();

  bool parse();
  MDRCDReply* Reply_get() {return m_pReply;}
  
};

#endif /* MDRCDPARSER_H */

