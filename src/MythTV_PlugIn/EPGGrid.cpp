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

#include "MythTV_PlugIn.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/CommonIncludes.h"


#ifndef WIN32
#include <dirent.h>
#endif

#include <vector>
#include <sstream>
#include <time.h>
#include <string.h>
#include "EPGGrid.h"

EPGGrid::EPGGrid(class DBHelper *pDBHelper_Myth) : m_pDBHelper_Myth(pDBHelper_Myth)
{
	string sSQL = "SELECT channum, callsign, chanid, icon FROM channel ORDER BY CAST(channum AS SIGNED)";
	PlutoSqlResult result_set;
	DB_ROW row;
	DataGridCell *pCell = new DataGridCell("None","0");
	SetData(0,0,pCell);
	int iRow=0;
	if( (result_set.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL)) )
	{
		while ((row = db_wrapper_fetch_row(result_set.r)))
		{
			size_t fSize;
			char *fLogo;
			fLogo=FileUtils::ReadFileIntoBuffer(row[3], fSize);

			pCell = new DataGridCell(row[0]+string("\n")+row[1], row[2]);
			SetData(0,iRow,pCell);
			pCell = new DataGridCell("");
			if (fLogo)		
				pCell->SetImage(fLogo, fSize, GR_JPG);
	
			SetData(1,iRow++,pCell);
		}
	}
}

EPGGrid::~EPGGrid()
{
}

void EPGGrid::ToData(string GridID,int &Size, char* &Data, int *ColStart, int *RowStart, int ColCount, int RowCount)
{
	PlutoSqlResult result_set;
	DB_ROW row;

	for(int irow=*RowStart;irow<=*RowStart+RowCount && irow<m_TotalRows;++irow)
	{
		DataGridCell *pCell = GetData(0,irow);
		string chanid = pCell->GetValue();
		// Todo "description" in this table will contain the longer program description
		string sSQL = "SELECT title FROM program WHERE program.chanid = "+chanid+" AND program.starttime < UTC_TIMESTAMP() AND program.endtime > UTC_TIMESTAMP()";
		if ((result_set.r=m_pDBHelper_Myth->db_wrapper_query_result(sSQL)) && (row = db_wrapper_fetch_row(result_set.r)))
		{
			SetData(2, irow, new DataGridCell(row[0]));
		}	
	}
	DataGridTable::ToData(GridID,Size,Data,ColStart,RowStart,ColCount,RowCount);
}
