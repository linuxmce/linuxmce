/**
 * MDRCDParser - A class that parses a data reply from MDR
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 *
 */

#include "MDRCDParser.h"
#include "pugixml.hpp"
#include <iostream>

#define CD_COVER_URL_PREFIX "http://images.metaservices.microsoft.com/cover/"

using namespace std;
using namespace pugi;

MDRCDParser::MDRCDParser(string sReplyXML)
{
  m_sReplyXML=sReplyXML;
}

MDRCDParser::~MDRCDParser()
{
  if (m_pReply)
    delete m_pReply;
}

bool MDRCDParser::parse()
{
  xml_document doc;
  xml_parse_result result = doc.load_buffer(m_sReplyXML.data(),m_sReplyXML.length());
  
  if (result.status != status_ok)
    return false;
  
  string sAlbumTitle=doc.child("METADATA").child("MDR-CD").child_value("albumTitle");
  string sAlbumArtist=doc.child("METADATA").child("MDR-CD").child_value("albumArtist");
  string sReleaseDate=doc.child("METADATA").child("MDR-CD").child_value("releaseDate");
  string sLabel=doc.child("METADATA").child("MDR-CD").child_value("label");
  string sGenre=doc.child("METADATA").child("MDR-CD").child_value("genre");
  string sCoverURL=doc.child("METADATA").child("MDR-CD").child_value("largeCoverParams");
  sCoverURL=string(CD_COVER_URL_PREFIX) + "/" + sCoverURL;

  if (sAlbumTitle.empty() && sAlbumArtist.empty())
    return false;

  m_pReply=new MDRCDReply(sAlbumTitle,
			  sAlbumArtist,
			  sReleaseDate,
			  sLabel,
			  sGenre,
			  sCoverURL);

  for (pugi::xml_node xnTRACK: doc.child("METADATA").child("MDR-CD").children())
    {
      string sNodeName=xnTRACK.name();
     
      if (sNodeName != "track")
	continue;

      xml_node xnTrackNum=xnTRACK;
      xml_node xnTrackTitle=xnTRACK;
      string sTrackNum=xnTrackNum.child_value("trackNumber");
      string sTrackPerformer=xnTrackNum.child_value("trackPerformer");
      string sTrackTitle=xnTrackNum.child_value("trackTitle");
      MDRTrack *t = new MDRTrack(sTrackNum, sTrackPerformer, sTrackTitle);
      m_pReply->Track_add(t);
    }
  return true;
}

// int main(int argc, char** argv)
// {
//   MDRCDParser* p = new MDRCDParser("<METADATA><MDR-CD><version>5.0</version><mdqRequestID>00000000-0000-0000-0000-000000000000</mdqRequestID><WMCollectionID>4abb1f33-ec1a-4ba4-81ed-650d12931625</WMCollectionID><WMCollectionGroupID>4abb1f33-ec1a-4ba4-81ed-650d12931625</WMCollectionGroupID><uniqueFileID>AMGa_id=R   949826</uniqueFileID><albumTitle>8-Bit Operators</albumTitle><albumArtist>Various Artists</albumArtist><releaseDate>2007-02-06</releaseDate><label>Astralwerks</label><genre>Electronica</genre><providerStyle>Electronic</providerStyle><publisherRating> </publisherRating><buyParams>providerName=AMG&amp;albumID=4ABB1F33-EC1A-4BA4-81ED-650D12931625&amp;a_id=R%20%20%20949826&amp;album=8-Bit%20Operators&amp;artistID=80064449-C40C-486A-8D0E-D3ACB5DA99B9&amp;p_id=%20&amp;artist=Various%20Artists</buyParams><largeCoverParams>200/dri300/i373/i37310m6var.jpg</largeCoverParams><smallCoverParams>075/dri300/i373/i37310m6var.jpg</smallCoverParams><moreInfoParams>a_id=R%20%20%20949826</moreInfoParams><dataProvider>AMG</dataProvider><dataProviderParams>Provider=AMG</dataProviderParams><dataProviderLogo>Provider=AMG</dataProviderLogo><needIDs>0</needIDs><track><WMContentID>36168192-2d03-490e-aaec-c87da4e70c7c</WMContentID><trackRequestID></trackRequestID><trackTitle>The Robots (Die Roboter)</trackTitle><uniqueFileID>AMGp_id=VA;AMGt_id=T 10021395</uniqueFileID><trackNumber>1</trackNumber><trackPerformer>Bacalao</trackPerformer><trackComposer>Florian Schneider; Karl Bartos; Kraftwerk; Ralf Hütter</trackComposer><trackConductor></trackConductor><period></period><explicitLyrics>0</explicitLyrics></track><track><WMContentID>3a3e938e-9ab9-45c6-ba79-883f0435981c</WMContentID><trackRequestID></trackRequestID><trackTitle>Pocket Calculator</trackTitle><uniqueFileID>AMGp_id=VA;AMGt_id=T 10021396</uniqueFileID><trackNumber>2</trackNumber><trackPerformer>Glomag</trackPerformer><trackComposer>Karl Bartos; Ralf KraftwerkHütter; Schult</trackComposer><trackConductor></trackConductor><period></period><explicitLyrics>0</explicitLyrics></track><track><WMContentID>f2c76806-1a09-49a5-888e-4e08837d5bce</WMContentID><trackRequestID></trackRequestID><trackTitle>Computer Love</trackTitle><uniqueFileID>AMGp_id=VA;AMGt_id=T 10021397</uniqueFileID><trackNumber>3</trackNumber><trackPerformer>Covox</trackPerformer><trackComposer>Emil Schult; Karl Bartos; Ralf KraftwerkHütter</trackComposer><trackConductor></trackConductor><period></period><explicitLyrics>0</explicitLyrics></track><track><WMContentID>72939e05-cf5d-4a32-a4e3-8ed2ce4d098a</WMContentID><trackRequestID></trackRequestID><trackTitle>Showroom Dummies</trackTitle><uniqueFileID>AMGp_id=VA;AMGt_id=T 10021398</uniqueFileID><trackNumber>4</trackNumber><trackPerformer>Role Model</trackPerformer><trackComposer>Kraftwerk; Ralf Hütter</trackComposer><trackConductor></trackConductor><period></period><explicitLyrics>0</explicitLyrics></track><track><WMContentID>6f0bfcbf-0fd4-4637-833d-ef612100cc35</WMContentID><trackRequestID></trackRequestID><trackTitle>The Model</trackTitle><uniqueFileID>AMGp_id=VA;AMGt_id=T 10021399</uniqueFileID><trackNumber>5</trackNumber><trackPerformer>Nullsleep</trackPerformer><trackComposer>Florian Schneider; Karl Bartos; Kraftwerk; Ralf Hütter</trackComposer><trackConductor></trackConductor><period></period><explicitLyrics>0</explicitLyrics></track><track><WMContentID>f1b1e59e-1d23-42d7-84dd-d9ce1685efb2</WMContentID><trackRequestID></trackRequestID><trackTitle>Radioactivity</trackTitle><uniqueFileID>AMGp_id=VA;AMGt_id=T 10021400</uniqueFileID><trackNumber>6</trackNumber><trackPerformer>David Sugar</trackPerformer><trackComposer>Emil Schult; Florian Schneider; Kraftwerk; Ralf Hütter</trackComposer><trackConductor></trackConductor><period></period><explicitLyrics>0</explicitLyrics></track><track><WMContentID>5012e93b-8822-4f1a-9c36-1928a0e02d8d</WMContentID><trackRequestID></trackRequestID><trackTitle>Kristallo</trackTitle><uniqueFileID>AMGp_id=VA;AMGt_id=T 10021401</uniqueFileID><trackNumber>7</trackNumber><trackPerformer>Oliver Wittchow</trackPerformer><trackComposer>Florian Schneider; Kraftwerk; Ralf Hütter</trackComposer><trackConductor></trackConductor><period></period><explicitLyrics>0</explicitLyrics></track><track><WMContentID>b46d3490-d258-4a1f-a2b2-a718a0e499ea</WMContentID><trackRequestID></trackRequestID><trackTitle>Spacelab</trackTitle><uniqueFileID>AMGp_id=VA;AMGt_id=T 10021402</uniqueFileID><trackNumber>8</trackNumber><trackPerformer>8-Bit Weapon</trackPerformer><trackComposer>Karl Bartos; Kraftwerk; Ralf Hütter</trackComposer><trackConductor></trackConductor><period></period><explicitLyrics>0</explicitLyrics></track><track><WMContentID>4225b406-1960-48d4-877f-dde2a0f5b40b</WMContentID><trackRequestID></trackRequestID><trackTitle>Computer World [Computerwelt]</trackTitle><uniqueFileID>AMGp_id=VA;AMGt_id=T 10021403</uniqueFileID><trackNumber>9</trackNumber><trackPerformer>Firestarter</trackPerformer><trackComposer>Emil Schult; Florian Schneider; Karl Bartos; Kraftwerk; Ralf Hütter</trackComposer><trackConductor></trackConductor><period></period><explicitLyrics>0</explicitLyrics></track><track><WMContentID>9bb2bf36-dc2e-4487-8ec1-48e5f802310a</WMContentID><trackRequestID></trackRequestID><trackTitle>Electric Cafe</trackTitle><uniqueFileID>AMGp_id=VA;AMGt_id=T 10021404</uniqueFileID><trackNumber>10</trackNumber><trackPerformer>Neotericz</trackPerformer><trackComposer>Florian Schneider; Karl Bartos; Kraftwerk; Maxime Schmitt; Ralf Hütter</trackComposer><trackConductor></trackConductor><period></period><explicitLyrics>0</explicitLyrics></track><track><WMContentID>88647804-4212-4597-b008-b9f7681c44dc</WMContentID><trackRequestID></trackRequestID><trackTitle>Trans-Europe Express</trackTitle><uniqueFileID>AMGp_id=VA;AMGt_id=T 10021405</uniqueFileID><trackNumber>11</trackNumber><trackPerformer>Receptors</trackPerformer><trackComposer>Emil Schult; Kraftwerk; Ralf Hütter</trackComposer><trackConductor></trackConductor><period></period><explicitLyrics>0</explicitLyrics></track><track><WMContentID>97f1dc89-58cf-49b2-aeeb-2384581ee406</WMContentID><trackRequestID></trackRequestID><trackTitle>Tanzmusik</trackTitle><uniqueFileID>AMGp_id=VA;AMGt_id=T 10021406</uniqueFileID><trackNumber>12</trackNumber><trackPerformer>Herbert Weixelbaum</trackPerformer><trackComposer>Florian Schneider; Kraftwerk; Ralf Hütter</trackComposer><trackConductor></trackConductor><period></period><explicitLyrics>0</explicitLyrics></track><track><WMContentID>c03b886f-acda-4273-b9ec-368b81fa0d2c</WMContentID><trackRequestID></trackRequestID><trackTitle>It's More Fun to Compute</trackTitle><uniqueFileID>AMGp_id=VA;AMGt_id=T 10021407</uniqueFileID><trackNumber>13</trackNumber><trackPerformer>Bubblyfish</trackPerformer><trackComposer>Florian Schneider; Karl Bartos; Kraftwerk; Ralf Hütter</trackComposer><trackConductor></trackConductor><period></period><explicitLyrics>0</explicitLyrics></track><track><WMContentID>a3aff1b3-6c94-44f8-a2ac-deabc8d9871a</WMContentID><trackRequestID></trackRequestID><trackTitle>Antenna</trackTitle><uniqueFileID>AMGp_id=VA;AMGt_id=T 10021408</uniqueFileID><trackNumber>14</trackNumber><trackPerformer>Bit Shifter</trackPerformer><trackComposer>Emil Schult; Florian Schneider; Kraftwerk; Ralf Hütter</trackComposer><trackConductor></trackConductor><period></period><explicitLyrics>0</explicitLyrics></track><track><WMContentID>4eb0515a-c129-41d3-9c14-7997abbdb42b</WMContentID><trackRequestID></trackRequestID><trackTitle>The Man-Machine [Die Mensch-Maschine]</trackTitle><uniqueFileID>AMGp_id=VA;AMGt_id=T 10021409</uniqueFileID><trackNumber>15</trackNumber><trackPerformer>GwEm and Counter Reset</trackPerformer><trackComposer>Florian Schneider; Karl Bartos; Kraftwerk; Ralf Hütter</trackComposer><trackConductor></trackConductor><period></period><explicitLyrics>0</explicitLyrics></track></MDR-CD><Backoff><Time>5</Time></Backoff></METADATA>");
//   if (p->parse())
//     {
//       MDRCDReply* r = p->Reply_get();
//       cout << " -- Reply Global Data ------------------------------------- " << endl;
//       cout << r->AlbumTitle_get() << endl;
//       cout << r->AlbumArtist_get() << endl;
//       cout << r->ReleaseDate_get() << endl;
//       cout << r->Label_get() << endl;
//       cout << r->Genre_get() << endl;
//       cout << r->CoverURL_get() << endl;
//       cout << " == Tracks ================================================ " << endl;
//       vector<MDRTrack*> vectTracks = r->Tracks_get();
//       for (vector<MDRTrack*>::iterator it=vectTracks.begin(); it!=vectTracks.end(); ++it)
// 	{
// 	  MDRTrack* t = *it;
// 	  cout << "Track: " << t->TrackNumber_get() << endl;
// 	  cout << "Performer: " << t->TrackPerformer_get() << endl;
// 	  cout << "Title: " << t->TrackTitle_get() << endl;
// 	  cout << "-------------------------------------------------------" << endl;
// 	}
//     }

//   delete p;
//   return 0;
// }
