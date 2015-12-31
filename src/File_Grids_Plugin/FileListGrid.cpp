/*
 FileListGrid

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "File_Grids_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/CommonIncludes.h"

#include <vector>
#include <sstream>
#include <time.h>
#include <string.h>
#include <map>

#include "DataGrid.h"
#include "FileListGrid.h"
#include "Media_Plugin/MediaAttributes.h"
#include "Media_Plugin/Media_Plugin.h"
#include "pluto_media/Table_File.h"

#define USE_PRECACHED_DATABASE_MAP

using namespace std;

FileListGrid::~FileListGrid()
{
	for(vector<FileListInfo *>::iterator it = m_vectFileInfo.begin(); it != m_vectFileInfo.end(); ++it)
		delete *it;
	m_vectFileInfo.clear();
}

void FileListGrid::ToData(string GridID,int &Size, char* &Data, int *ColStart, int *RowStart, int ColCount, int RowCount)
{
	// Don't render icons in the first column if we're not displaying the first column
	if( *ColStart )
	{
		DataGridTable::ToData(GridID,Size,Data,ColStart,RowStart,ColCount,RowCount);
		return;
	}

#ifdef USE_PRECACHED_DATABASE_MAP
	//get the map with FileName <-> pair(PK_Picture, Picture's extension)
	map<string, pair<int, string> > mapDatabaseFiles;
	if(m_vectFileInfo.size() > 0)
	{
		FileListInfo *flInfo = m_vectFileInfo[m_vectFileInfo.size() - 1];

		string sSql = 
			"( "
				"SELECT Filename, Picture_Attribute.FK_Picture, Extension "
				"FROM File "
					"JOIN File_Attribute ON PK_File = FK_File "
					"JOIN Picture_Attribute ON Picture_Attribute.FK_Attribute = File_Attribute.FK_Attribute "
					"JOIN Picture ON Picture_Attribute.FK_Picture = PK_Picture "
					"JOIN Attribute ON Picture_Attribute.FK_Attribute = PK_Attribute "
					"JOIN AttributeType ON FK_AttributeType = PK_AttributeType "
				"WHERE Path = \"" + FileUtils::BasePath(flInfo->m_sPath) + "\" "
				"GROUP BY Filename "
				"ORDER BY PicPriority DESC "
			") "
			"UNION "
			"( "
			"SELECT Filename, FK_Picture, Extension "
			"FROM File "
			"JOIN Picture_File ON PK_File = FK_File "
			"JOIN Picture ON FK_Picture = PK_Picture "
			"WHERE Path = \"" + FileUtils::BasePath(flInfo->m_sPath) + "\""
			")";

		PlutoSqlResult result;
		DB_ROW row;
		if( 
			(result.r = m_pMedia_Plugin->m_pDatabase_pluto_media->db_wrapper_query_result(sSql.c_str())) 
		)
		{
			while(NULL != result.r && (row = db_wrapper_fetch_row(result.r)))
			{
				//got any picture?
				if(NULL != row[1] && NULL != row[2])
				{
					mapDatabaseFiles[row[0]] = make_pair(atoi(row[1]), row[2]);
				}
			}
		}
	}
#endif

#ifdef DEBUG
	clock_t cStart=clock(); // move this to within #debug
#endif

	for(int row=*RowStart;row<=*RowStart+RowCount && row<m_TotalRows;++row)
	{
		DataGridCell *pCell = GetData(0,row);
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_STATUS,"filelistgrid::row %d graphic data: %p rowstart: %d rowCount: %d totalrows: %d",
	row,(pCell ? pCell->m_pGraphicData : NULL),*RowStart,RowCount,m_TotalRows);
#endif
		if( (!pCell || !pCell->m_pGraphicData) && (!pCell || pCell->m_Text==NULL || pCell->m_Text[0]==0) ) // We haven't already set a picture for this cell.  Skip it if it's a cell with text
		{
			string Extension,PictureFile;
			int PKID_MED_Picture = 0;
			enum eGraphicFormat format;

			FileListInfo *flInfo = m_vectFileInfo[row];
			if( flInfo->m_bIsBack )
				continue;

			string sFileName = FileUtils::FilenameWithoutPath(flInfo->m_sPath, true);

#ifdef USE_PRECACHED_DATABASE_MAP

			map<string, pair<int, string> >::iterator it = mapDatabaseFiles.find(sFileName);
			if(it != mapDatabaseFiles.end())
			{
				PKID_MED_Picture = it->second.first;
				Extension = it->second.second;
			}

#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Pic for %s is: %d",flInfo->m_sPath.c_str( ), PKID_MED_Picture);
#endif

#else //USE_PRECACHED_DATABASE_MAP

			string sFilePath = FileUtils::BasePath(flInfo->m_sPath);
			PlutoMediaFile PlutoMediaFile_(m_pMedia_Plugin->m_pDatabase_pluto_media, m_pMedia_Plugin->m_pDatabase_pluto_main, sFilePath, sFileName);
			int PK_File = PlutoMediaFile_.GetFileAttribute(false);
			if(!PK_File ||  !(PKID_MED_Picture = PlutoMediaFile_.GetPicAttribute(PK_File)))
			{
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "No id3 tag for picture id for %s", flInfo->m_sPath.c_str());
#endif
				continue;
			}

#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Pic for %s is: %d",flInfo->m_sPath.c_str( ), PKID_MED_Picture);
#endif

			Extension = "jpg";

#endif //USE_PRECACHED_DATABASE_MAP

			char *pIconBuffer = NULL;
			size_t stIconSize;

			if(PKID_MED_Picture > 0)
			{
				PictureFile = "/home/mediapics/" + StringUtils::itos(PKID_MED_Picture) + "_tn." + Extension;
				pIconBuffer = FileUtils::ReadFileIntoBuffer(PictureFile,stIconSize);
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Pic file: %s has size: %d", PictureFile.c_str(),stIconSize);
#endif
				format = GR_JPG;
			}

			if( pIconBuffer )
			{
				if( !pCell )
					pCell = new DataGridCell("","");
				pCell->SetImage(pIconBuffer,(int) stIconSize,format);
				SetData(0,row,pCell);
			}
		}
	}

#ifdef DEBUG
	//profiling
	clock_t cStop = clock();
	if( cStop-cStart>CLOCKS_PER_SEC/2 )  // Nothing should take 500 ms
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"file grid pics took %d ms",(int) (cStop-cStart));
	} 
	else if( cStop-cStart>CLOCKS_PER_SEC/10 )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING,"file grid pics took %d ms",(int) (cStop-cStart));
	}

	LoggerWrapper::GetInstance()->Write(LV_WARNING,"file grid pics took %d ms",(int) (cStop-cStart));
#endif

	DataGridTable::ToData(GridID,Size,Data,ColStart,RowStart,ColCount,RowCount);
}

