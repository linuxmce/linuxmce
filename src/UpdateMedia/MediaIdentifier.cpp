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
#include "MediaIdentifier.h"
#include "pluto_main/Define_MediaType.h"
#include "PlutoUtils/PlutoDefs.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"

#include <iostream>
using namespace std;
//-----------------------------------------------------------------------------------------------------
map<string, int> MediaIdentifier::m_mapMediaIdentifier;
//-----------------------------------------------------------------------------------------------------
/*static*/ void MediaIdentifier::Init()
{
	m_mapMediaIdentifier["MP3 file with ID3 version 2.3.0 tag"] = MEDIATYPE_pluto_StoredAudio_CONST;	
	m_mapMediaIdentifier["MPEG ADTS"] = MEDIATYPE_pluto_StoredAudio_CONST;	

	m_mapMediaIdentifier["FLAC audio bitstream data"] = MEDIATYPE_pluto_StoredAudio_CONST;
    m_mapMediaIdentifier["Apple QuickTime movie"] = MEDIATYPE_pluto_StoredVideo_CONST;
	m_mapMediaIdentifier["MPEG sequence"] = MEDIATYPE_pluto_StoredVideo_CONST;

	m_mapMediaIdentifier["ISO 9660 CD-ROM filesystem data"] = MEDIATYPE_pluto_DVD_CONST;
}
//-----------------------------------------------------------------------------------------------------			
/*static*/ int MediaIdentifier::GetFileMediaType(string sFilename)
{
#ifdef WIN32
    cout << "Not implemented!" << endl;
    return 0;
#endif
    
    if(!m_mapMediaIdentifier.size())
		Init();
	
	string sMediaTypeFile = "/tmp/pluto-media-type.txt";
	string sCmdLine = "file -b \"" + sFilename + "\" > " + sMediaTypeFile;
	system(sCmdLine.c_str());

	size_t nSize = 0;
	char *pData = FileUtils::ReadFileIntoBuffer(sMediaTypeFile, nSize);
	string sMediaType(pData);
	PLUTO_SAFE_DELETE(pData);

	cout << "Raw type: " << sMediaType << endl;

	sMediaType = sMediaType.substr(0, sMediaType.find("\n")); //only first line
	sMediaType = sMediaType.substr(0, sMediaType.find(",")); //skip details
	sMediaType = sMediaType.substr(0, sMediaType.find("(")); //skip details
	sMediaType = sMediaType.substr(0, sMediaType.find("'")); //skip details
	sMediaType = StringUtils::TrimSpaces(sMediaType);

	cout << "Media type to find in the map: " << sMediaType << endl;
	
	map<string, int>::iterator it = m_mapMediaIdentifier.find(sMediaType);
	return it == m_mapMediaIdentifier.end() ? 0 : (*it).second;
}
//-----------------------------------------------------------------------------------------------------

