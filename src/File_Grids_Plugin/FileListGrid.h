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
#ifndef FILELISTGRID_H
#define FILELISTGRID_H

#include "DataGrid.h"
namespace DCE
{
	class Media_Plugin;
}

using namespace DCE;

class FileListInfo
{
public:
	FileListInfo(bool bIsDirectory,string sPath,bool bIsBack) : m_PKID_MED_Attribute(0), m_bIsDirectory(bIsDirectory), m_bIsBack(bIsBack), m_sPath(sPath) {}
	FileListInfo(int PKID_MED_Attribute) : m_PKID_MED_Attribute(PKID_MED_Attribute) {}
	int m_PKID_MED_Attribute;
	bool m_bIsDirectory,m_bIsBack;  // Keep track of whether this is a 'go back' or not
	string m_sPath;
};

class FileListGrid : public DataGridTable
{
public:
	FileListGrid(class Datagrid_Plugin *pDatagrid_Plugin,class Media_Plugin *pMedia_Plugin) : m_pDatagrid_Plugin(pDatagrid_Plugin), m_pMedia_Plugin(pMedia_Plugin) {}
	~FileListGrid();
	class Datagrid_Plugin *m_pDatagrid_Plugin;
	class Media_Plugin *m_pMedia_Plugin;
	string m_sIconFile;
	int m_iType;
	void ToData(string GridID,int &Size, char* &Data, int *ColStart, int *RowStart, int ColCount, int RowCount);
	vector<FileListInfo *> m_vectFileInfo;
};

#endif

