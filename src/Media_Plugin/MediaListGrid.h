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
#ifndef MediaListGrid_H
#define MediaListGrid_H

#include "DataGrid.h"
namespace DCE
{
	class Media_Plugin;
	class Datagrid_Plugin;
}

using namespace DCE;

class FileBrowserInfo
{
public:
	typedef enum {
		fbi_File,
		fbi_DiscJukebox,
		fbi_Bookmark,
		fbi_Attribute,
		fbi_Downloadable
	} enumTypeOfMedia;

	bool m_bIsDirectory,m_bIsBack;  // Keep track of whether this is a 'go back' or not
	string m_sDisplayGroup,m_sDisplayName;
	string m_sMRL,m_sPictureThumb;
	int m_PK_File,m_PK_Disc,m_PK_Picture,m_PK_Attribute,m_PK_Bookmark,m_PK_FileFormat;
	char m_cMediaSource; // F=File, D=Disc, L=Download, A=Attribute
	enumTypeOfMedia m_enumTypeOfMedia;
	time_t m_tLastViewed;

	// Constructor for file info
	FileBrowserInfo(string sDisplayName, string sPath,int PK_File_Or_Disc,int PK_FileFormat, char *pLastViewed, char cMediaSource, bool bIsDirectory,bool bIsBack) 
	{
		if( sDisplayName.empty() )
			m_sDisplayName=FileUtils::FilenameWithoutPath(sPath);
		else
			m_sDisplayName=sDisplayName;
		m_enumTypeOfMedia=fbi_File;

		if( cMediaSource=='D' )
		{
			m_PK_Disc=PK_File_Or_Disc;
			m_PK_File=0;
		}
		else
		{
			m_PK_Disc=0;
			m_PK_File=PK_File_Or_Disc;
		}
		m_PK_Picture=0;
		m_PK_Attribute=0;
		m_bIsDirectory=bIsDirectory;
		m_bIsBack=bIsBack;
		m_PK_Bookmark=0;
		m_sMRL=sPath;
		m_PK_FileFormat=PK_FileFormat;
		m_cMediaSource=cMediaSource;
		m_tLastViewed=pLastViewed ? StringUtils::SQLDateTime(pLastViewed) : 0;
	}
	// Constructor for attribute info
	FileBrowserInfo(string sDisplayName, string sMRL,int PK_Attribute,time_t tLastViewed=0) 
	{
		m_sDisplayName=sDisplayName;
		m_enumTypeOfMedia=fbi_Attribute;
		m_PK_Attribute=PK_Attribute;
		m_PK_Disc=0;
		m_PK_Picture=0;
		m_PK_File=0;
		m_PK_Bookmark=0;
		m_sMRL=sMRL;
		m_bIsDirectory=false;
		m_bIsBack=false;
		m_PK_FileFormat=0;
		m_cMediaSource='A';
		m_tLastViewed=tLastViewed;
	}
	// Constructor for bookmark
	FileBrowserInfo(string sDisplayName, int PK_Bookmark, int PK_Picture, int PK_File, int PK_Disc,int PK_FileFormat, char cMediaSource)
	{
		m_sDisplayName=sDisplayName;
		m_enumTypeOfMedia=fbi_Bookmark;
		m_PK_Disc=PK_Disc;
		m_PK_Picture=PK_Picture;
		m_PK_Attribute=0;
		m_PK_File=PK_File;
		m_PK_Bookmark=PK_Bookmark;
		m_sMRL="!B" + StringUtils::itos(PK_Bookmark);
		m_bIsDirectory=false;
		m_bIsBack=false;
		m_PK_FileFormat=PK_FileFormat;
		m_cMediaSource=cMediaSource;
		m_tLastViewed=0;
	}
};

typedef FileBrowserInfo * FileBrowserInfoPtr;

static bool FileBrowserInfoComparer(FileBrowserInfo *x, FileBrowserInfo *y)
{
	if( x->m_bIsBack && !y->m_bIsBack )
		return true;

	if( !x->m_bIsBack && y->m_bIsBack )
		return false;

	if( x->m_bIsDirectory && !y->m_bIsDirectory )
		return true;

	if( !x->m_bIsDirectory && y->m_bIsDirectory )
		return false;

	return stricmp( (x->m_sDisplayGroup+x->m_sDisplayName).c_str(), (y->m_sDisplayGroup+y->m_sDisplayName).c_str() )<0;
}

static bool FileBrowserInfoComparerLastViewed(FileBrowserInfo *x, FileBrowserInfo *y)
{
	return x->m_tLastViewed > y->m_tLastViewed;
}

class DatabaseInfoOnPath
{
public:
	int m_PK_File,m_PK_Picture;
	bool m_bDirectory;

	DatabaseInfoOnPath(int PK_File,bool bDirectory,int PK_Picture)
	{
		m_PK_File=PK_File;
		m_PK_Picture=PK_Picture;
		m_bDirectory=bDirectory;
	}
};

class MediaListGrid : public DataGridTable
{
public:
	FileBrowserInfoPtr *m_pFileBrowserInfoPtr;  // This will be the same as m_listFileBrowserInfo, but in an array for faster retrieval by row number

	MediaListGrid(class Datagrid_Plugin *pDatagrid_Plugin,class Media_Plugin *pMedia_Plugin,int PK_MediaType) : m_pDatagrid_Plugin(pDatagrid_Plugin), m_pMedia_Plugin(pMedia_Plugin) { m_pFileBrowserInfoPtr=NULL; m_pMediaListGrid_Master=NULL; m_iPK_MediaType=PK_MediaType; }
	MediaListGrid(class Datagrid_Plugin *pDatagrid_Plugin,class Media_Plugin *pMedia_Plugin,MediaListGrid *pMediaListGrid) : m_pDatagrid_Plugin(pDatagrid_Plugin), m_pMedia_Plugin(pMedia_Plugin), m_pMediaListGrid_Master(pMediaListGrid) { m_pFileBrowserInfoPtr=NULL; m_iPK_MediaType=m_pMediaListGrid_Master->m_iPK_MediaType; }
	~MediaListGrid()
	{
		delete[] m_pFileBrowserInfoPtr;
		m_pFileBrowserInfoPtr=NULL;

		for(list<FileBrowserInfo *>::iterator it=m_listFileBrowserInfo.begin();it!=m_listFileBrowserInfo.end();++it)
			delete *it;
	}
	class Datagrid_Plugin *m_pDatagrid_Plugin;
	class Media_Plugin *m_pMedia_Plugin;
	MediaListGrid *m_pMediaListGrid_Master;
	string m_sIconFile;
	int m_iPK_MediaType;
	void ToData(string GridID,int &Size, char* &Data, int *ColStart, int *RowStart, int ColCount, int RowCount);
	list<FileBrowserInfo *> m_listFileBrowserInfo;
};

#endif

