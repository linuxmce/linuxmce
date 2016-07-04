/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "id3info.h"

#include <id3/tag.h>
#include <id3/utils.h>
#include <id3/misc_support.h>
#include <id3/readers.h>
#include <id3/globals.h>

#include "../../pluto_media/Define_AttributeType.h"

#ifdef ENABLE_TEST_UNIT
	#include "PlutoUtils/FileUtils.h"
#endif

using namespace dami;
using std::cout;
using std::endl;

#include <map>
using namespace std;

#define MAX_TAG_SIZE_ALLOWED 10 * 1024 * 1024 //10MB

void GetUserDefinedInformation(string sFilename, char *&pData, size_t& Size)
{
 	pData = NULL;
	Size = 0;

	ID3_Tag tag; 
	tag.Link(sFilename.c_str());

	ID3_Frame* frame = tag.Find(ID3FID_GENERALOBJECT);
	if(NULL != frame)
	{
		ID3_Field* fld = frame->GetField(ID3FN_DATA);
		if(NULL != fld)
		{
			Size = fld->Size();
			if(Size > MAX_TAG_SIZE_ALLOWED)
			{
				cout << "ERROR! Field size too big: " << int(Size) << endl;
				tag.RemoveFrame(frame);
				tag.Update(ID3TT_ID3); 
				return;
			}


			pData = new char[Size];
			memcpy(pData, fld->GetRawBinary(), Size);
		}
	}
}

void SetUserDefinedInformation(string sFilename, char *pData, size_t& Size)
{
	ID3_Tag tag; 
	tag.Link(sFilename.c_str());

	ID3_Frame* frame = tag.Find(ID3FID_GENERALOBJECT);
	if(NULL == frame)
	{
		frame = new ID3_Frame(ID3FID_GENERALOBJECT);
		tag.AttachFrame(frame);
	}

	if(NULL != frame)
	{
		ID3_Field* fld = frame->GetField(ID3FN_DATA);
		uchar *pFieldData = new uchar[Size];
		memcpy(pFieldData, pData, Size);
		fld->Set(pFieldData, Size);
		delete []pFieldData;
	}

    tag.Update(ID3TT_ID3); 
}

void GetInformation(const ID3_Tag &myTag, map<int,string>& mapAttributes,
					list<pair<char *, size_t> >& listPictures)
{
  ID3_Tag::ConstIterator* iter = myTag.CreateIterator();
  const ID3_Frame* frame = NULL;

  while (NULL != (frame = iter->GetNext()))
  {
    const char* desc = frame->GetDescription();
	string id = frame->GetTextID();
	
    if (!desc) desc = "";

	cout << "=== " << id << " (" << desc << "): ";

	int PK_Attr = 0;
	if(id == "TALB")
		PK_Attr = ATTRIBUTETYPE_Album_CONST;
	else if(id == "TIT2")
		PK_Attr = ATTRIBUTETYPE_Title_CONST;
	else if(id == "TIT3")
		PK_Attr = ATTRIBUTETYPE_Episode_CONST;
	else if(id == "TPE1")
		PK_Attr = ATTRIBUTETYPE_Performer_CONST;
	else if(id == "TPE2")
		PK_Attr = ATTRIBUTETYPE_Album_Artist_CONST;
	else if(id == "TCON")
		PK_Attr = ATTRIBUTETYPE_Genre_CONST;
	else if(id == "TRCK")
		PK_Attr = ATTRIBUTETYPE_Track_CONST;
	else if(id == "TYER")
		PK_Attr = ATTRIBUTETYPE_Release_Date_CONST;
	else if(id == "COMM") //comments
		PK_Attr = -2; // todo ask
	else if(id == "TMED") //media
		PK_Attr = -3; // todo ask
	else if(id == "TLEN") //length
		PK_Attr = -4; // todo ask;
    else if(id == "WXXX")
	    PK_Attr = ATTRIBUTETYPE_Website_CONST; 
	else if(id == "TCOM")
	    PK_Attr = ATTRIBUTETYPE_ComposerWriter_CONST;
	else if(id == "TENC") //encoded by
        PK_Attr = -5; // todo ask
    else if(id == "TCOP") //copyright
        PK_Attr = -6; // todo ask
    else if(id == "TOPE") //Original artist(s)/performer(s)
        PK_Attr = -7; // todo ask;
	else
		PK_Attr = -1000;

	string value;
	
    ID3_FrameID eFrameID = frame->GetID();
    switch (eFrameID)
    {
      case ID3FID_ALBUM:
      case ID3FID_BPM:
      case ID3FID_COMPOSER:
      case ID3FID_CONTENTTYPE:
      case ID3FID_COPYRIGHT:
      case ID3FID_DATE:
      case ID3FID_PLAYLISTDELAY:
      case ID3FID_ENCODEDBY:
      case ID3FID_LYRICIST:
      case ID3FID_FILETYPE:
      case ID3FID_TIME:
      case ID3FID_CONTENTGROUP:
      case ID3FID_TITLE:
      case ID3FID_SUBTITLE:
      case ID3FID_INITIALKEY:
      case ID3FID_LANGUAGE:
      case ID3FID_SONGLEN:
      case ID3FID_MEDIATYPE:
      case ID3FID_ORIGALBUM:
      case ID3FID_ORIGFILENAME:
      case ID3FID_ORIGLYRICIST:
      case ID3FID_ORIGARTIST:
      case ID3FID_ORIGYEAR:
      case ID3FID_FILEOWNER:
      case ID3FID_LEADARTIST:
      case ID3FID_BAND:
      case ID3FID_CONDUCTOR:
      case ID3FID_MIXARTIST:
      case ID3FID_PARTINSET:
      case ID3FID_PUBLISHER:
      case ID3FID_TRACKNUM:
      case ID3FID_RECORDINGDATES:
      case ID3FID_NETRADIOSTATION:
      case ID3FID_NETRADIOOWNER:
      case ID3FID_SIZE:
      case ID3FID_ISRC:
      case ID3FID_ENCODERSETTINGS:
      case ID3FID_YEAR:
      {
        char *sText = ID3_GetString(frame, ID3FN_TEXT);
        cout << sText << endl;
		value = sText;
        delete [] sText;
        break;
      }
      case ID3FID_USERTEXT:
      {
        char 
        *sText = ID3_GetString(frame, ID3FN_TEXT), 
        *sDesc = ID3_GetString(frame, ID3FN_DESCRIPTION);
        cout << "(" << sDesc << "): " << sText << endl;
		value = sText; //ignoring Description
        delete [] sText;
        delete [] sDesc;
        break;
      }
      case ID3FID_COMMENT:
      case ID3FID_UNSYNCEDLYRICS:
      {
        char 
        *sText = ID3_GetString(frame, ID3FN_TEXT), 
        *sDesc = ID3_GetString(frame, ID3FN_DESCRIPTION), 
        *sLang = ID3_GetString(frame, ID3FN_LANGUAGE);
        cout << "(" << sDesc << ")[" << sLang << "]: "
             << sText << endl;

		value = string("(") + sDesc + ")[" + sLang + "]: " + sText;
		
        delete [] sText;
        delete [] sDesc;
        delete [] sLang;
        break;
      }
      case ID3FID_WWWAUDIOFILE:
      case ID3FID_WWWARTIST:
      case ID3FID_WWWAUDIOSOURCE:
      case ID3FID_WWWCOMMERCIALINFO:
      case ID3FID_WWWCOPYRIGHT:
      case ID3FID_WWWPUBLISHER:
      case ID3FID_WWWPAYMENT:
      case ID3FID_WWWRADIOPAGE:
      {
        char *sURL = ID3_GetString(frame, ID3FN_URL);
        cout << sURL << endl;
		value = sURL;
        delete [] sURL;
        break;
      }
      case ID3FID_WWWUSER:
      {
        char 
        *sURL = ID3_GetString(frame, ID3FN_URL),
        *sDesc = ID3_GetString(frame, ID3FN_DESCRIPTION);
        cout << "(" << sDesc << "): " << sURL << endl;
		value = sURL;
        delete [] sURL;
        delete [] sDesc;
        break;
      }
      case ID3FID_INVOLVEDPEOPLE:
      {
        size_t nItems = frame->GetField(ID3FN_TEXT)->GetNumTextItems();
        for (size_t nIndex = 0; nIndex < nItems; nIndex++)
        {
          char *sPeople = ID3_GetString(frame, ID3FN_TEXT, nIndex);
          cout << sPeople;
		  value += sPeople;
          delete [] sPeople;
          if (nIndex + 1 < nItems)
          {
            cout << ", ";
			value += ", ";
          }
        }
        cout << endl;
        break;
      }
      case ID3FID_PICTURE:
      {
        char
        *sMimeType = ID3_GetString(frame, ID3FN_MIMETYPE),
        *sDesc     = ID3_GetString(frame, ID3FN_DESCRIPTION),
        *sFormat   = ID3_GetString(frame, ID3FN_IMAGEFORMAT);
        size_t
        nPicType   = frame->GetField(ID3FN_PICTURETYPE)->Get(),
        nDataSize  = frame->GetField(ID3FN_DATA)->Size();

		char *pPictureData = NULL;

        ID3_Field* fld = frame->GetField(ID3FN_DATA);
        if (fld)
        {
			size_t nBinSize = fld->BinSize();
			pPictureData = new char[nBinSize];
			memcpy(pPictureData, fld->GetRawBinary(), nBinSize);
			listPictures.push_back(make_pair(pPictureData, nBinSize));
		}

        cout << "(" << sDesc << ")[" << sFormat << ", "
             << int(nPicType) << "]: " << sMimeType << ", " << int(nDataSize)
             << " bytes" << endl;

		value = string("(") + sDesc + ")[" + sFormat + "]: " + sMimeType;
        delete [] sMimeType;
        delete [] sDesc;
        delete [] sFormat;
        break;
      }
      case ID3FID_GENERALOBJECT:
      {
        char 
        *sMimeType = ID3_GetString(frame, ID3FN_MIMETYPE), 
        *sDesc = ID3_GetString(frame, ID3FN_DESCRIPTION), 
        *sFileName = ID3_GetString(frame, ID3FN_FILENAME);
        size_t 
        nDataSize = frame->GetField(ID3FN_DATA)->Size();
        cout << "(" << sDesc << ")[" 
             << sFileName << "]: " << sMimeType << ", " << int(nDataSize)
             << " bytes" << endl;
		value = string("(") + sDesc + ")[" + sFileName + "]: " + sMimeType;
        delete [] sMimeType;
        delete [] sDesc;
        delete [] sFileName;
        break;
      }
      case ID3FID_UNIQUEFILEID:
      {
        char *sOwner = ID3_GetString(frame, ID3FN_OWNER);
        size_t nDataSize = frame->GetField(ID3FN_DATA)->Size();
        cout << sOwner << ", " << int(nDataSize)
             << " bytes" << endl;
		value = sOwner;
        delete [] sOwner;
        break;
      }
      case ID3FID_PLAYCOUNTER:
      {
        size_t nCounter = frame->GetField(ID3FN_COUNTER)->Get();
        cout << int(nCounter) << endl;
        break;
      }
      case ID3FID_POPULARIMETER:
      {
        char *sEmail = ID3_GetString(frame, ID3FN_EMAIL);
        size_t
        nCounter = frame->GetField(ID3FN_COUNTER)->Get(),
        nRating = frame->GetField(ID3FN_RATING)->Get();
        cout << sEmail << ", counter=" 
             << int(nCounter) << " rating=" << int(nRating) << endl;
		value = sEmail;
        delete [] sEmail;
        break;
      }
      case ID3FID_CRYPTOREG:
      case ID3FID_GROUPINGREG:
      {
        char *sOwner = ID3_GetString(frame, ID3FN_OWNER);
        size_t 
        nSymbol = frame->GetField(ID3FN_ID)->Get(),
        nDataSize = frame->GetField(ID3FN_DATA)->Size();
        cout << "(" << int(nSymbol) << "): " << sOwner
             << ", " << int(nDataSize) << " bytes" << endl;
        break;
      }
      case ID3FID_AUDIOCRYPTO:
      case ID3FID_EQUALIZATION:
      case ID3FID_EVENTTIMING:
      case ID3FID_CDID:
      case ID3FID_MPEGLOOKUP:
      case ID3FID_OWNERSHIP:
      case ID3FID_PRIVATE:
      case ID3FID_POSITIONSYNC:
      case ID3FID_BUFFERSIZE:
      case ID3FID_VOLUMEADJ:
      case ID3FID_REVERB:
      case ID3FID_SYNCEDTEMPO:
      case ID3FID_METACRYPTO:
      {
        cout << " (unimplemented)" << endl;
        break;
      }
      default:
      {
        cout << " frame" << endl;
        break;
      }
    }

	if(PK_Attr == ATTRIBUTETYPE_Genre_CONST)
	{
		string genre;
		for(string::iterator it = value.begin(); it != value.end(); ++it)
		{
			char c = *it;
			if(c >= '0' && c <= '9')
				genre += c;
		}

		if(value == genre || value == "(" + genre + ")")
		{
			int nGenre = atoi(genre.c_str());
			if(nGenre < ID3_NR_OF_V1_GENRES)
				value = ID3_v1_genre_description[nGenre];
		}
	}
	// Don't pass attribute on to LMCE if it does not know how to deal with it
	if (PK_Attr != -1000)
		mapAttributes[PK_Attr] = value;
  }
  delete iter;
}

void GetId3Info(string sFilename, map<int,string>& mapAttributes,
				list<pair<char *, size_t> >& listPictures)
{
	ID3_Tag myTag; 
	myTag.Link(sFilename.c_str(), ID3TT_ALL);
	myTag.GetMp3HeaderInfo();
    GetInformation(myTag, mapAttributes, listPictures);
}

size_t ID3_RemoveComposers(ID3_Tag *tag)
{
    size_t num_removed = 0;
    ID3_Frame *frame = NULL;

    if (NULL == tag)
    {
        return num_removed;
    }

    while ((frame = tag->Find(ID3FID_COMPOSER)))
    {
        frame = tag->RemoveFrame(frame);
        delete frame;
        num_removed++;
    }

    return num_removed;
} 

ID3_Frame* ID3_AddComposer(ID3_Tag *tag, const char *text, bool replace = true)
{
    ID3_Frame* frame = NULL;
    if (NULL != tag && NULL != text && strlen(text) > 0)
    {
		cout << "replace: " << replace << endl;
        if (replace)
        {
            ID3_RemoveComposers(tag);
        }
        if (replace || tag->Find(ID3FID_COMPOSER) == NULL)
        {
            frame = new ID3_Frame(ID3FID_COMPOSER);
            if (frame)
            {
                frame->GetField(ID3FN_TEXT)->Set(text);
                tag->AttachFrame(frame);
            }
        }
    }

    return frame;
} 

size_t ID3_RemoveUserDefinedText(ID3_Tag *tag)
{
	size_t num_removed = 0;
	ID3_Frame *frame = NULL;

	if (NULL == tag)
	{
		return num_removed;
	}

	while ((frame = tag->Find(ID3FID_USERTEXT)))
	{
		frame = tag->RemoveFrame(frame);
		delete frame;
		num_removed++;
	}

	return num_removed;
} 

ID3_Frame* ID3_AddUserDefinedText(ID3_Tag *tag, const char *text, bool replace = true)
{
	ID3_Frame* frame = NULL;
	if (NULL != tag && NULL != text && strlen(text) > 0)
	{
		cout << "replace: " << replace << endl;
		if (replace)
		{
			ID3_RemoveUserDefinedText(tag);
		}
		if (replace || tag->Find(ID3FID_USERTEXT) == NULL)
		{
			frame = new ID3_Frame(ID3FID_USERTEXT);
			if (frame)
			{
				frame->GetField(ID3FN_TEXT)->Set(text);
				tag->AttachFrame(frame);
			}
		}
	}

	return frame;
} 

void RemoveId3PictureTag(ID3_Tag *tag)
{
    size_t num_removed = 0;
    ID3_Frame *frame = NULL;

    while ((frame = tag->Find(ID3FID_PICTURE)))
    {
        frame = tag->RemoveFrame(frame);
        delete frame;
        num_removed++;
    }
}

ID3_Frame* AddID3PictureTag(ID3_Tag *tag, const char *pData, size_t nSize)
{
	ID3_Frame* frame = NULL;
	if (NULL != tag && NULL != pData && nSize > 0)
	{
		frame = new ID3_Frame(ID3FID_PICTURE);
		if (frame)
		{
			frame->GetField(ID3FN_DATA)->Set(reinterpret_cast<const uchar *>(pData), nSize);
			tag->AttachFrame(frame);
		}
	}

	return frame;
} 

void SetId3Info(string sFilename, const map<int,string>& mapAttributes, 
				const list<pair<char *, size_t> >& listPictures)
{
    ID3_Tag myTag; 
    myTag.Link(sFilename.c_str());

    for(map<int,string>::const_iterator it = mapAttributes.begin(); it != mapAttributes.end(); it++)
    {
        cout << "***** Writing: PK_Attr = " << (*it).first << "\t\t" << (*it).second << endl;

        int PK_Attr = (*it).first;
        string sValue = (*it).second;

        switch(PK_Attr)
        {
	    // TODO: deal with ATTRIBUTETYPE_Album_Artist_CONST, no generalized function??
	    case ATTRIBUTETYPE_Album_Artist_CONST:
		//ID3x_AddAlbumArtist(&myTag, sValue.c_str(), true); // ??
	        break;

            case ATTRIBUTETYPE_Performer_CONST:
                ID3_AddArtist(&myTag, sValue.c_str(), true); 
                break;

            case ATTRIBUTETYPE_Album_CONST:
                ID3_AddAlbum(&myTag, sValue.c_str(), true);
                break;

            case ATTRIBUTETYPE_Title_CONST:
                ID3_AddTitle(&myTag, sValue.c_str(), true); 
                break;

            case ATTRIBUTETYPE_Release_Date_CONST:
                ID3_AddYear(&myTag, sValue.c_str(), true);
                break;

            case ATTRIBUTETYPE_Genre_CONST:
                ID3_AddGenre(&myTag, sValue.c_str(), true); 
                break;

            case ATTRIBUTETYPE_Track_CONST:
                ID3_AddTrack(&myTag, atoi(sValue.c_str()), 0, true); 
                break;

            case ATTRIBUTETYPE_ComposerWriter_CONST:
                ID3_AddComposer(&myTag, sValue.c_str(), true);
				break;
            default:
                cout << "Don't know how to save to id3tag with PK_Attr = " << PK_Attr << " and value " << sValue << ", but it will be saved in the id3 GEOB block for LinuxMCE" << endl;
        }
    }

	RemoveId3PictureTag(&myTag);
	for(list<pair<char *, size_t> >::const_iterator itp = listPictures.begin(); itp != listPictures.end(); itp++)
		AddID3PictureTag(&myTag, itp->first, itp->second);
    
    myTag.Update(ID3TT_ID3); 
}

void RemoveId3Tag(string sFilename, int nTagType, string sValue)
{
	ID3_Tag myTag; 
	myTag.Link(sFilename.c_str());

        cout << "***** Removing: nTagType = " << nTagType << ", value = " << sValue << endl;
	switch(nTagType)
	{
	    // TODO: deal with ATTRIBUTETYPE_Album_Artist_CONST, no generalized function??
		case ATTRIBUTETYPE_Album_Artist_CONST:
			//ID3x_RemoveAlbumArtist(&myTag); // ??
			break;

		case ATTRIBUTETYPE_Performer_CONST:
			ID3_RemoveArtists(&myTag); 
			break;

		case ATTRIBUTETYPE_Album_CONST:
			ID3_RemoveAlbums(&myTag);
			break;

		case ATTRIBUTETYPE_Title_CONST:
			ID3_RemoveTitles(&myTag); 
			break;

		case ATTRIBUTETYPE_Release_Date_CONST:
			ID3_RemoveYears(&myTag);
			break;

		case ATTRIBUTETYPE_Genre_CONST:
			ID3_RemoveGenres(&myTag); 
			break;

		case ATTRIBUTETYPE_Track_CONST:
			ID3_RemoveTracks(&myTag); 
			break;

		case ATTRIBUTETYPE_ComposerWriter_CONST:
			ID3_RemoveComposers(&myTag);
			break;

		default:
			cout << "Don't know yet how to remove tags with PK_Attr = " << nTagType << " and value " << sValue << endl;
	}

    myTag.Update(ID3TT_ID3); 
}

#ifdef ENABLE_TEST_UNIT

void DisplayInfo(const map<int,string>& mapAttributes, const list<pair<char *, size_t> >& listPictures)
{
    cout << endl << "Size: " << int(mapAttributes.size()) << endl;
    for(map<int,string>::const_iterator it = mapAttributes.begin(); it != mapAttributes.end(); it++)
	    cout << "PK_Attr = " << (*it).first << "\t\t" << (*it).second << endl;

	for(list<pair<char *, size_t> >::const_iterator itp = listPictures.begin(); itp != listPictures.end(); itp++)
	{
		size_t nSize = itp->second;
		cout << "Picture size " << static_cast<unsigned long>(nSize) << endl;
	}
}

int main( unsigned int argc, char * const argv[])
{
    if(argc != 2)
    {
	    cout << "Usage: id3info file" << endl;
	    return 1;
    }

    map<int,string> mapAttributes;
	list<pair<char *, size_t> > listPictures;

    //reading attr
    GetId3Info(argv[1], mapAttributes, listPictures);

	//display info
	DisplayInfo(mapAttributes, listPictures);
    return 0;
}
#endif

