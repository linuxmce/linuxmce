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
#ifndef __MEDIA_IDENTIFIER_H__
#define __MEDIA_IDENTIFIER_H_

#include <string>
#include <map>
using namespace std;
//-----------------------------------------------------------------------------------------------------
//  Media Identifier class (no instance can/must be made)
//  usage: MediaIdentifier::GetFileMediaType(string)
//-----------------------------------------------------------------------------------------------------
class MediaIdentifier
{
private:
	MediaIdentifier()  {} 
	~MediaIdentifier() {}
	
	static void Init();
	static map<string, int> m_mapMediaIdentifier;
	
public:
	static int GetFileMediaType(string sFileName);
};
//-----------------------------------------------------------------------------------------------------
#endif
