/*
 MediaListGrid

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

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
#include "MediaListGrid.h"
#include "Media_Plugin/MediaAttributes.h"
#include "Media_Plugin/Media_Plugin.h"
#include "pluto_media/Table_File.h"

#define USE_PRECACHED_DATABASE_MAP

using namespace std;

void MediaListGrid::ToData(string GridID,int &Size, char* &Data, int *ColStart, int *RowStart, int ColCount, int RowCount)
{
	if( !m_pMediaListGrid_Master || *ColStart>0 )  // It's just the list -- nothing special to do here.  And there's no horizontal paging
	{
		DataGridTable::ToData(GridID,Size,Data,ColStart,RowStart,ColCount,RowCount);
		return;
	}

#ifdef DEBUG
	clock_t cStart=clock(); // move this to within #debug
#endif
	
	if( *ColStart<0 || *RowStart<0 )
	{
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "MediaListGrid::ToData colstart: %d rowstart: %d",
			*ColStart, *RowStart );
		return;
	}

	for(int row=*RowStart;row<*RowStart+RowCount;++row)
	{
		for(int col=*ColStart;col<*ColStart+ColCount;++col)
		{
			int OriginalRow = row*ColCount+col;
			if( OriginalRow>=m_pMediaListGrid_Master->m_TotalRows )
				break;

			DataGridCell *pDefaultCell = GetData(col,row);
			if( pDefaultCell )
				continue; // We already did this cell once.  user must have scrolled away and is coming back

			pDefaultCell = m_pMediaListGrid_Master->GetData(0,OriginalRow);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaListGrid::row %d graphic data: %p rowstart: %d rowCount: %d totalrows: %d",
	row,(pDefaultCell ? pDefaultCell->m_pGraphicData : NULL),*RowStart,RowCount,m_TotalRows);
#endif
			if( m_pMediaListGrid_Master->m_pFileBrowserInfoPtr == NULL )
			{
				LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "MediaListGrid::ToData m_pFileBrowserInfoPtr is NULL");
				return;
			}
			FileBrowserInfo *pFileBrowserInfo = m_pMediaListGrid_Master->m_pFileBrowserInfoPtr[OriginalRow];
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Pic for %s is: %d",pFileBrowserInfo->m_sMRL.c_str( ), pFileBrowserInfo->m_PK_Picture);
#endif

			DataGridCell *pCell = new DataGridCell(pDefaultCell->GetText(),pDefaultCell->GetValue());

			char *pIconBuffer = NULL;
			string PictureFile_Full;

			if( m_iPK_MediaType==MEDIATYPE_pluto_Pictures_CONST && !pFileBrowserInfo->m_bIsDirectory && !pFileBrowserInfo->m_bIsBack )
				pCell->SetImagePath(pFileBrowserInfo->m_sPictureThumb.c_str());
			else if(pFileBrowserInfo->m_PK_Picture > 0)
			{
				PictureFile_Full = "/home/mediapics/" + StringUtils::itos(pFileBrowserInfo->m_PK_Picture) + "_tn.jpg";
				/*
				pIconBuffer = FileUtils::ReadFileIntoBuffer( "/home/mediapics/" + StringUtils::itos(pFileBrowserInfo->m_PK_Picture) + "_tn.jpg",stIconSize);
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Pic file: %d has size: %d", pFileBrowserInfo->m_PK_Picture,stIconSize);
#endif
			}

			if( pIconBuffer )
			{*/
				pCell->SetImagePath(PictureFile_Full.c_str());
				
				/*
				pCell->SetImage(pIconBuffer,(int) stIconSize,GR_JPG); */
			}
			SetData(col,row,pCell); 
		}
	}

	m_TotalRows = m_pMediaListGrid_Master->m_TotalRows / ColCount;
	if( m_pMediaListGrid_Master->m_TotalRows % ColCount )
		m_TotalRows++;

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

