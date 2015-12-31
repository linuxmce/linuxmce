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
#ifndef __ID3INFO_H_
#define __ID3INFO_H_

#include <map>
#include <list>
#include <string>
using namespace std;

void GetId3Info(string sFilename, map<int,string>& mapAttributes, list<pair<char *, size_t> >& listPictures);
void SetId3Info(string sFilename, const map<int,string>& mapAttributes, const list<pair<char *, size_t> >& listPictures);
void RemoveId3Tag(string sFilename, int nTagType, string sValue);
void RemoveId3PictureTag(class ID3_Tag *tag);

void GetUserDefinedInformation(string sFilename, char *&pData, size_t& Size);
void SetUserDefinedInformation(string sFilename, char *pData, size_t& Size);

#endif
