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
#ifndef MediaTitle_h
#define MediaTitle_h

#include "MediaSection.h"

class MediaTitle
{
public:
	deque<MediaSection *> m_dequeMediaSection;
	MediaTitle() {
	}

	~MediaTitle() {
		for(size_t s=0;s<m_dequeMediaSection.size();++s)
			if( m_dequeMediaSection[s] )
				delete m_dequeMediaSection[s];
	}

	map< int,list_int > m_mapPK_Attribute;  /** An external media identification script may set attributes here, PK_AttributeType=PK_Attribute */
    list_int *m_mapPK_Attribute_Find(int PK_AttributeType) { map<int,list_int >::iterator it = m_mapPK_Attribute.find(PK_AttributeType); return it==m_mapPK_Attribute.end() ? NULL : &((*it).second); }
};

#endif
