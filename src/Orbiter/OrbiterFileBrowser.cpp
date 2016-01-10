/*
 OrbiterFileBrowser
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */
 
#include "PlutoUtils/CommonIncludes.h"	
#include "OrbiterFileBrowser.h"

bool OrbiterFileBrowser_Collection::UnknownSerialize(ItemToSerialize *pItem,bool bWriting,char *&pDataBlock,unsigned long &iAllocatedSize,char *&pCurrentPosition) 
{
	m_pcDataBlock=pDataBlock; m_dwAllocatedSize=iAllocatedSize; m_pcCurrentPosition=pCurrentPosition;
	if( bWriting )
	{
		switch(pItem->m_iSerializeDataType)
		{
		case SERIALIZE_DATA_TYPE_MAP_OBJFILEBROWSER:
			{
				MapOrbiterFileBrowser *pMap = (MapOrbiterFileBrowser *) pItem->m_pItem;
				Write_unsigned_long((unsigned long) pMap->size());
				for(MapOrbiterFileBrowser::iterator it=pMap->begin();it!=pMap->end();++it)
				{
					Write_unsigned_long((unsigned long) it->first);
					it->second->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
				}
				return true;  // We handled it
			}
			break;
		};
	}
	else
	{
		switch(pItem->m_iSerializeDataType)
		{
		case SERIALIZE_DATA_TYPE_MAP_OBJFILEBROWSER:
			{
				MapOrbiterFileBrowser *pMap = (MapOrbiterFileBrowser *) pItem->m_pItem;
				unsigned long count = Read_unsigned_long();
				for(unsigned long i=0;i<count;++i)
				{
					unsigned long ID = Read_unsigned_long();
					OrbiterFileBrowser_Entry *pOrbiterFileBrowser_Entry = new OrbiterFileBrowser_Entry();
					pOrbiterFileBrowser_Entry->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition);
					(*pMap)[ID]=pOrbiterFileBrowser_Entry;
				}
				return true;  // We handled it
			}
			break;
		};
	}

	pDataBlock=m_pcDataBlock; iAllocatedSize=m_dwAllocatedSize; pCurrentPosition=m_pcCurrentPosition;
	return true;
} 
