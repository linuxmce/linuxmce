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
/*
 Media_Plugin_Grids

 */

#include "Media_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/DatabaseUtils.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"

#include <algorithm>

#include "MediaHandlerInfo.h"
#include "MediaHandlerBase.h"
#include "Generic_NonPluto_Media.h"
#include "DCERouter.h"
#include "DeviceData_Router.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Define_Array.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_Template.h"
#include "pluto_main/Define_FloorplanObjectType.h"
#include "pluto_main/Table_Event.h"
#include "pluto_main/Table_EntertainArea.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_Command.h"
#include "pluto_main/Table_CommandGroup_EntertainArea.h"
#include "pluto_main/Table_CommandGroup_Command_CommandParameter.h"
#include "pluto_main/Table_RemoteControl.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_Device_EntertainArea.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Table_UI.h"
#include "pluto_main/Table_EventParameter.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Device_Device_Pipe.h"
#include "pluto_main/Table_MediaType.h"
#include "pluto_media/Database_pluto_media.h"
#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Table_Bookmark.h"
#include "pluto_media/Table_File_Attribute.h"
#include "pluto_media/Table_File.h"
#include "pluto_media/Table_Disc.h"
#include "pluto_media/Table_Picture_Disc.h"
#include "pluto_media/Table_Disc_Attribute.h"
#include "pluto_media/Table_Playlist.h"
#include "pluto_media/Table_Picture.h"
#include "pluto_media/Table_Picture_File.h"
#include "pluto_media/Table_Picture_Attribute.h"
#include "pluto_media/Table_ProviderSource.h"
#include "pluto_media/Table_AttributeType.h"
#include "pluto_media/Table_MediaProvider.h"
#include "pluto_media/Table_MediaSource.h"
#include "Gen_Devices/AllScreens.h"

#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "pluto_main/Define_DataGrid.h"
#include "DataGrid.h"
#include "MediaListGrid.h"
#include "File_Grids_Plugin/FileListOps.h"
#include "File_Grids_Plugin/FileListGrid.h"
#include "SerializeClass/ShapesColors.h"
#include "../VFD_LCD/VFD_LCD_Base.h"

extern int UniqueColors[MAX_MEDIA_COLORS];

// Parms = PK_MediaType | , sep PK_MediaSubType | , sep PK_FileFormat | Genres to include, or empty for all (, sep PK_Attribute) |
// , sep of sources -- hardcoded strings (below) or a path if sort=0 (sort by filename) | , sep PK_Users (0=public, empty=public only) | PK_AttributeType (sort by) | PK_Users (for ratings)


class DataGridTable *Media_Plugin::MediaBrowser( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	string::size_type pos=0;
	cout << "MediaBrowser Begin" << endl;
	int PK_MediaType = atoi(StringUtils::Tokenize( Parms,"|",pos ).c_str());
	string sPK_MediaSubType = StringUtils::Tokenize( Parms,"|",pos );
	string sPK_FileFormat = StringUtils::Tokenize( Parms,"|",pos );
	string sPK_Attribute_Genres = StringUtils::Tokenize( Parms,"|",pos );
	string sPK_Sources = StringUtils::Tokenize( Parms,"|",pos );
	string sPK_Users_Private = StringUtils::Tokenize( Parms,"|",pos );
	int PK_AttributeType_Sort = atoi(StringUtils::Tokenize( Parms,"|",pos ).c_str());
	int PK_Users = atoi(StringUtils::Tokenize( Parms,"|",pos ).c_str());
	int iLastViewed = atoi(StringUtils::Tokenize( Parms,"|",pos ).c_str());
	string sPK_Attribute = StringUtils::Tokenize( Parms,"|",pos );
	if( sPK_Sources.size()==0 )
		sPK_Sources = TOSTRING(MEDIASOURCE_File_CONST) "," TOSTRING(MEDIASOURCE_Jukebox_CONST) "," TOSTRING(MEDIASOURCE_Local_Disc_CONST);

	// FIXME: Find a better way to select the game grid, OTHER than hijacking the MediaBrowser grid!
	if (IsGameMediaType(PK_MediaType) && PK_AttributeType_Sort == 0 && sPK_Attribute.empty())   // We are being called from the file list.
	  {
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::MediaBrowser - Media Type is a Game System, setting Games Media Type and System Sort Value.");
	    PK_MediaType = MEDIATYPE_lmce_Game_CONST;
	    PK_AttributeType_Sort = ATTRIBUTETYPE_System_CONST;
	  }

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "MediaBrowser parms: mediatype %d, submediatype %s, "
		"fileformat %s, attribute_genres %s, sources %s, users_private %s, attributetype_sort %d, "
		"users %d, last_viewed %d, pk_attribute %s", 
		PK_MediaType, sPK_MediaSubType.c_str(), sPK_FileFormat.c_str(), sPK_Attribute_Genres.c_str(),
		sPK_Sources.c_str(), sPK_Users_Private.c_str(), PK_AttributeType_Sort, PK_Users,
		iLastViewed, sPK_Attribute.c_str());

#ifdef SIM_JUKEBOX
	// These are set in a sub-function
	g_bInclFiles=g_bInclDiscs=g_bInclDownload=true;
#endif

	cout << "XXX Before MediaListGrid initialize" << endl;
	MediaListGrid *pMediaListGrid = new MediaListGrid(m_pDatagrid_Plugin,this,PK_MediaType);
	if( sPK_Sources.size()==0 || !PK_MediaType )
		return pMediaListGrid;
	cout << "XXX After MediaListGrid Initialize" << endl;
	bool bIdentifiesFile = true;
	if( PK_AttributeType_Sort>0 )
		if( m_mapMediaType_AttributeType_Identifier.find( make_pair<int,int> ( PK_MediaType,PK_AttributeType_Sort ) )==m_mapMediaType_AttributeType_Identifier.end() )
			bIdentifiesFile = false;  // This combination doesn't identify an individual file to play (like a song), it identifies a group of attributes (like an album or performer)

	if( PK_MediaType==MEDIATYPE_misc_Playlist_CONST )
		PopulateFileBrowserInfoForPlayList(pMediaListGrid,sPK_Users_Private);
	else
		AttributesBrowser( pMediaListGrid, PK_MediaType, sPK_Attribute, PK_AttributeType_Sort, bIdentifiesFile, sPK_MediaSubType, sPK_FileFormat, sPK_Attribute_Genres, sPK_Sources, sPK_Users_Private, PK_Users, iLastViewed, iPK_Variable, sValue_To_Assign );

	if( PK_AttributeType_Sort==-1 )
		pMediaListGrid->m_listFileBrowserInfo.sort(FileBrowserInfoComparerLastViewed); 
	else
		pMediaListGrid->m_listFileBrowserInfo.sort(FileBrowserInfoComparer); 

//for(list<FileBrowserInfo *>::iterator ittemp=pMediaListGrid->m_listFileBrowserInfo.begin();ittemp!=pMediaListGrid->m_listFileBrowserInfo.end();++ittemp)
//{
//	FileBrowserInfo *pf = *ittemp;
//LoggerWrapper::GetInstance()->Write(LV_WARNING, "list %s : %s date: %d", pf->m_sDisplayGroup.c_str(), pf->m_sDisplayName.c_str(), (int) pf->m_tLastViewed);
//}

	pMediaListGrid->m_pFileBrowserInfoPtr = new FileBrowserInfoPtr[ pMediaListGrid->m_listFileBrowserInfo.size() ];

	DataGridCell *pCell;
	string sDisplayGroupPrior;
	int iRow=0;
	string sSource = " "; // Convert to a 1 character string
#ifdef SIM_JUKEBOX
	string sTerms;
#endif
	for(list<FileBrowserInfo *>::iterator it=pMediaListGrid->m_listFileBrowserInfo.begin();it!=pMediaListGrid->m_listFileBrowserInfo.end();++it)
	{
		FileBrowserInfo *pFileBrowserInfo = *it;
		pMediaListGrid->m_pFileBrowserInfoPtr[iRow] = pFileBrowserInfo;  // Store in an array for fast retrieval by row

		sSource[0] = pFileBrowserInfo->m_cMediaSource ? pFileBrowserInfo->m_cMediaSource : '?';
#ifdef SIM_JUKEBOX
		map< int, string >::iterator itPurch;
		if( pFileBrowserInfo->m_PK_File && (itPurch=m_mapPK_FilesForSimulatedPurchase.find( pFileBrowserInfo->m_PK_File ) ) != m_mapPK_FilesForSimulatedPurchase.end() )
		{
			sSource = "L";
			pFileBrowserInfo->m_cMediaSource = 'L';
			sTerms=itPurch->second;
		}

		if( !g_bInclFiles && pFileBrowserInfo->m_cMediaSource=='F' )
			continue;
		
		if( !g_bInclDiscs && pFileBrowserInfo->m_cMediaSource=='D' )
			continue;

		if( !g_bInclDownload && pFileBrowserInfo->m_cMediaSource=='L' )
			continue;
#endif

		if( pFileBrowserInfo->m_sDisplayGroup.size() && pFileBrowserInfo->m_sDisplayGroup!=sDisplayGroupPrior )
			pCell = new DataGridCell(pFileBrowserInfo->m_sDisplayGroup + " : " + pFileBrowserInfo->m_sDisplayName);
		else 
			pCell = new DataGridCell(pFileBrowserInfo->m_sDisplayName);
		pCell->m_mapAttributes["PK_FileFormat"] = StringUtils::itos(pFileBrowserInfo->m_PK_FileFormat);

		pCell->m_mapAttributes["Source"] = sSource;

#ifdef SIM_JUKEBOX
		if( pFileBrowserInfo->m_cMediaSource == 'L' )
			pCell->m_mapAttributes["Terms"] = sSource;
#endif

		pCell->SetValue(pFileBrowserInfo->m_sMRL);
		pMediaListGrid->SetData(0,iRow++,pCell);
	}

	MediaListGrid *pMediaListGrid_CoverArt = new MediaListGrid(m_pDatagrid_Plugin,this,pMediaListGrid);
	DataGridMap::iterator itdg=m_pDatagrid_Plugin->m_DataGrids.find("_" + GridID);
	if( itdg!=m_pDatagrid_Plugin->m_DataGrids.end() )
		delete itdg->second;
	m_pDatagrid_Plugin->m_DataGrids["_" + GridID]=pMediaListGrid_CoverArt;  // The same grid is used for both icons and lists.  If the name starts with an _, it is assumed to be an icon grid
	cout << "End MediaBrowser" << endl;
	return pMediaListGrid;
}

bool Media_Plugin::IsGameMediaType(int iPK_MediaType)
{
  bool bMediaType = 
    (iPK_MediaType == MEDIATYPE_lmce_Game_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_a2600_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_a5200_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_a7800_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_coleco_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_intv_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_famicom_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_nes_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_sms_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_sg1000_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_vectrex_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_tg16_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_genesis_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_snes_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_megadriv_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_snespal_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_sgx_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_pce_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_apple2_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_ps1_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_ps2_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_jaguar_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_vic20_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_c64_CONST) ||
    (iPK_MediaType == MEDIATYPE_lmce_Game_Atari800_CONST);
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"bMediaType is %d",bMediaType);
  return bMediaType;
}

string Media_Plugin::GameMediaTypes()
{
  string sMediaTypes = TOSTRING(MEDIATYPE_lmce_Game_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_a2600_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_a5200_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_a7800_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_coleco_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_intv_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_famicom_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_nes_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_sms_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_sg1000_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_vectrex_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_tg16_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_genesis_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_snes_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_megadriv_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_snespal_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_sgx_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_pce_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_apple2_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_ps1_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_ps2_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_jaguar_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_vic20_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_c64_CONST) ","
    TOSTRING(MEDIATYPE_lmce_Game_Atari800_CONST);
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Returning media types: %s",sMediaTypes.c_str());
  return sMediaTypes;
}

void Media_Plugin::AttributesBrowser( MediaListGrid *pMediaListGrid,int PK_MediaType, string sPK_Attribute, int PK_AttributeType_Sort, bool bShowFiles, string &sPK_MediaSubType, string &sPK_FileFormat, string &sPK_Attribute_Genres, string &sPK_Sources, string &sPK_Users_Private, int PK_Users, int iLastViewed, int *iPK_Variable, string *sValue_To_Assign )
{
	bool bFile=false,bDiscs=false,bSubDirectory=false,bBookmarks=false,bDownload=false;
	string sPath,sPath_Back;
	string::size_type pos=0;
	bool bUsingDirectory=false;
	if( sPK_Sources.find("\t!D")!=string::npos )  // The user is drilling down into a directory
	{
		string::size_type postab;
		postab = sPK_Sources.find('\t'); // This should always be here.  It signifies the topmost source (ie file, jukebox, etc.) when the user started drilling down into directories
		string::size_type posLastPath=postab+3,pos=postab+3;  // Skip past the \t!D

		while( (pos=sPK_Sources.find("','",pos))!=string::npos ) 
			posLastPath=pos++;

		sPath = sPK_Sources.substr(posLastPath==postab+3 ? posLastPath : posLastPath+2); // skip the last ', if we found any matching entries
		sPath_Back = sPK_Sources.substr(0,posLastPath==postab+3 ? posLastPath : posLastPath+1);  // Include the closing ' if we found matching entries
		if( sPath.empty() )
			sPK_Sources = sPK_Sources.substr(0,postab); // Get rid of the last \t!D.  This is the top level source
		else
		{
			bFile=true;
			bSubDirectory=true;
			pMediaListGrid->m_listFileBrowserInfo.push_back( new FileBrowserInfo("back (..)",sPath_Back,0,0,0,'F',true,true) );
			bUsingDirectory=true;
		}
	}

	if( sPK_Attribute.size()>2 && sPK_Attribute[0]=='!' && sPK_Attribute[1]=='e' )  
	{
		string::size_type pos=0;
		string sE = StringUtils::Tokenize(sPK_Attribute,",",pos);
		string sPK_MediaSource = StringUtils::Tokenize(sPK_Attribute,",",pos);
		string sURL = StringUtils::Tokenize(sPK_Attribute,",",pos);
		MediaHandlerBase *pMediaHandlerBase = m_mapMediaCatalog_Find(sPK_MediaSource);
		if( pMediaHandlerBase )
			pMediaHandlerBase->PopulateDataGrid(sPK_MediaSource, pMediaListGrid, PK_MediaType, sURL, PK_AttributeType_Sort, bShowFiles, sPK_MediaSubType, sPK_FileFormat, sPK_Attribute_Genres, sPK_Sources, sPK_Users_Private, PK_Users, iLastViewed, iPK_Variable, sValue_To_Assign );
		else
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::AttributesBrowser Bad media source directory %s", sPK_MediaSource.c_str());
		return;
	}
	if( !bUsingDirectory )
	{
		while(pos<sPK_Sources.size())
		{
			string sPK_MediaSource = StringUtils::Tokenize(sPK_Sources,",",pos);
			if( sPK_MediaSource.empty() )
				break; // Shouldn't happen
			
			if( sPK_MediaSource[0]>'9' ) // This is ascii, so the source is an external plugin
			{
				MediaHandlerBase *pMediaHandlerBase = m_mapMediaCatalog_Find(sPK_MediaSource);
				if( pMediaHandlerBase )
					pMediaHandlerBase->PopulateDataGrid(sPK_MediaSource, pMediaListGrid, PK_MediaType, sPK_Attribute, PK_AttributeType_Sort, bShowFiles, sPK_MediaSubType, sPK_FileFormat, sPK_Attribute_Genres, sPK_Sources, sPK_Users_Private, PK_Users, iLastViewed, iPK_Variable, sValue_To_Assign );
				else
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::AttributesBrowser Bad media source %s", sPK_MediaSource.c_str());
			}
			else
			{
				int PK_MediaSource = atoi(sPK_MediaSource.c_str());
				if( PK_MediaSource==MEDIASOURCE_Hard_Drives_CONST )
					bFile=true;
				else if( PK_MediaSource==MEDIASOURCE_Discs__Jukeboxes_CONST )
					bDiscs=true;
				else if( PK_MediaSource==MEDIASOURCE_Bookmarks_CONST )
					bBookmarks=true;
				else if( PK_MediaSource==MEDIASOURCE_Downloadable_CONST )
					bDownload=true;
			}
		}
#ifdef SIM_JUKEBOX
		// These are set in a sub-function
		g_bInclFiles=bFile;
		g_bInclDiscs=bDiscs;
		g_bInclDownload=bDownload;
#endif
		sPK_Sources = sPK_Sources + "\t";  // Put a tab before this which is the indicator up above that this is the topmost source when the user tabs back
		if( bFile && PK_AttributeType_Sort==0 || PK_AttributeType_Sort==-1 )
		{
			Row_MediaType *pRow_MediaType = m_pDatabase_pluto_main->MediaType_get()->GetRow(PK_MediaType);
			if( pRow_MediaType )
			{
				string::size_type pos=0;
				while( pos<sPK_Users_Private.size() )
				{
					string sUser = StringUtils::Tokenize(sPK_Users_Private,",",pos);
					if( sPath.empty()==false )
					{
						if( PK_AttributeType_Sort==-1 ) // Sorting by recently viewed, not directory, so find all directories
							sPath += " OR ";
						else
							sPath += ",";
					}
					if( sUser=="0" )
					{
						if( PK_AttributeType_Sort==-1 ) // Sorting by recently viewed, not directory, so find all directories
							sPath += "Path LIKE '/home/public/data/" + pRow_MediaType->Subdirectory_get() + "%'";
						else
							sPath += "'/home/public/data/" + pRow_MediaType->Subdirectory_get() + "'";
					}
					else
					{
						if( PK_AttributeType_Sort==-1 ) // Sorting by recently viewed, not directory, so find all directories
							sPath += "Path LIKE '/home/user_" + sUser + "/data/" + pRow_MediaType->Subdirectory_get() + "%'";
						else
							sPath += "'/home/user_" + sUser + "/data/" + pRow_MediaType->Subdirectory_get() + "'";
					}
				}
			}
		}
	}

	bool bBookmarksOnly = bBookmarks && !bFile && !bDiscs;

	// First get all matching PK_File's
	string sSQL_File,sSQL_Disc,sSQL_Where;
// temp hack to simulate the jukebox functionality
#ifdef SIM_JUKEBOX
	bool bFile_Original = bFile;
	if( bDiscs || bDownload )
		bFile=true;  // We're treating files as discs
#endif

	if( bFile || bBookmarksOnly )
		sSQL_File = "SELECT PK_File FROM File ";

	if( bDiscs || bBookmarksOnly  )
		sSQL_Disc = "SELECT PK_Disc FROM Disc JOIN DiscLocation ON DiscLocation.FK_Disc=PK_Disc ";
	
	if( sPK_Attribute_Genres.size() )
	{
		if( bFile || bBookmarksOnly   )
			sSQL_File += "LEFT JOIN File_Attribute AS FA_Genre ON FA_Genre.FK_File=PK_File "
				"LEFT JOIN Attribute AS Attribute_Genre ON FA_Genre.FK_Attribute=Attribute_Genre.PK_Attribute AND Attribute_Genre.FK_AttributeType=" TOSTRING(ATTRIBUTETYPE_Genre_CONST) " ";
		if( bDiscs || bBookmarksOnly  )
			sSQL_Disc += "LEFT JOIN Disc_Attribute AS DA_Genre ON DA_Genre.FK_Disc=PK_Disc "
				"LEFT JOIN Attribute AS Attribute_Genre ON DA_Genre.FK_Attribute=Attribute_Genre.PK_Attribute AND Attribute_Genre.FK_AttributeType=" TOSTRING(ATTRIBUTETYPE_Genre_CONST) " ";
	}

	string sSQL_Where_MediaType;

	if( PK_MediaType==MEDIATYPE_pluto_StoredAudio_CONST )
		sSQL_Where_MediaType = " EK_MediaType IN (" TOSTRING(MEDIATYPE_pluto_StoredAudio_CONST) "," TOSTRING(MEDIATYPE_pluto_CD_CONST) "," TOSTRING(MEDIATYPE_lmce_StreamedAudio_CONST)")";
	else if( PK_MediaType==MEDIATYPE_pluto_StoredVideo_CONST )
		sSQL_Where_MediaType = " EK_MediaType IN (" TOSTRING(MEDIATYPE_pluto_StoredVideo_CONST) "," TOSTRING(MEDIATYPE_pluto_DVD_CONST) "," TOSTRING(MEDIATYPE_pluto_HDDVD_CONST) "," TOSTRING(MEDIATYPE_pluto_BD_CONST) "," TOSTRING(MEDIATYPE_lmce_StreamedVideo_CONST)")";
        else if( IsGameMediaType(PK_MediaType) )
                sSQL_Where_MediaType = " EK_MediaType IN (" + GameMediaTypes() + ")";
	else 
		sSQL_Where_MediaType = " EK_MediaType=" + StringUtils::itos(PK_MediaType);

	sSQL_Where += "WHERE" + sSQL_Where_MediaType;

	if( sPK_Attribute_Genres.size() )
		sSQL_Where += " AND Attribute_Genre.PK_Attribute IN (" + sPK_Attribute_Genres + ")";

	if( sPK_FileFormat.size() )
		sSQL_Where += " AND FK_FileFormat IN (" + sPK_FileFormat + ")";

	if( sPK_MediaSubType.size() )
		sSQL_Where += " AND FK_MediaSubType IN (" + sPK_MediaSubType + ")";

	if( sPK_Users_Private.empty() || sPK_Users_Private=="0" )
		sSQL_Where += " AND EK_Users_Private IS NULL";
	else
	{
		bool bIncludePublicUser=false;
		if( sPK_Users_Private.size()>2 &&
			(sPK_Users_Private.substr(0,2)=="0," || sPK_Users_Private.substr( sPK_Users_Private.size()-2,2 )==",0" || sPK_Users_Private.find(",0,")!=string::npos ) )
				bIncludePublicUser=true;

		sSQL_Where += " AND (EK_Users_Private IN (" + sPK_Users_Private + ") " + (bIncludePublicUser ? "OR EK_Users_Private IS NULL)" : ")");
	}

	int PK_Attribute = atoi(sPK_Attribute.c_str());
	if( PK_Attribute > 0 )
	{
		sSQL_File += "LEFT JOIN File_Attribute AS FDA_Attr ON FDA_Attr.FK_File=PK_File ";
		sSQL_Disc += "LEFT JOIN Disc_Attribute AS FDA_Attr ON FDA_Attr.FK_Disc=PK_Disc ";
		sSQL_Where += " AND FDA_Attr.FK_Attribute=" + StringUtils::itos(PK_Attribute);
	}
	//else if( PK_Attribute < 0 )
	//{
	//	sSQL_Where += " AND PK_File NOT IN ( \n"
	//		"SELECT PK_File \n"
	//		"FROM File \n"
	//		"LEFT JOIN File_Attribute ON FK_File = PK_File \n"
	//		"LEFT JOIN Attribute ON FK_Attribute = PK_Attribute \n"
	//		"WHERE " + sSQL_Where_MediaType + " AND EK_Users_Private IS NULL \n"
	//		"AND `Ignore`=0 AND Missing=0  AND IsDirectory=0 AND FK_AttributeType = " + 
	//		StringUtils::ltos(PK_AttributeType_Sort) + " \n"
	//		") \n";
	//}

	if( time(NULL)-m_tLastScanOfOnlineDevices>300 )
		CMD_Refresh_List_of_Online_Devices();  // If it's been more than 5 minutes since we scanned for the list of devices

	string sOnline;
	if( m_sPK_Devices_Online.empty()==false )
		sOnline = " AND (EK_Device IS NULL OR EK_Device IN (" + m_sPK_Devices_Online + ")) ";

	if( iLastViewed!=2 )
		sSQL_Where += string(" AND DateLastViewed IS ") + (iLastViewed==1 ? "NOT" : "") + " NULL ";

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::AttributesBrowser - path: %s", sPath.c_str());
	
	//path condition correction
	string sPath_Clone(sPath);
	StringUtils::Replace(&sPath_Clone, "'", "");
	vector<string> vectDirectories;
	StringUtils::Tokenize(sPath_Clone, ",", vectDirectories);

	string sPathCondition;
	if( PK_AttributeType_Sort==-1 )
		sPathCondition = " AND (" + sPath;
	else
		sPathCondition = " AND (Path IN (" + sPath + ") ";

	if(PK_Attribute != 0 && PK_AttributeType_Sort!=-1 )
	{
		for(vector<string>::iterator it = vectDirectories.begin(); it != vectDirectories.end(); ++it)
		{
			string sDir = *it;
			sPathCondition += " OR Path LIKE '" + FileUtils::IncludeTrailingSlash(sDir) + "%' ";
		}
	}

	sPathCondition += ") ";

    if(sPath.empty())
		sPathCondition = "";

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::AttributesBrowser - path condition : %s", sPathCondition.c_str());

	string sPK_File,sPK_Disc;
    PlutoSqlResult resultf,resultd;
    DB_ROW row;
	if( (bFile || bBookmarksOnly) && ( resultf.r=m_pDatabase_pluto_media->db_wrapper_query_result( sSQL_File + sSQL_Where + sOnline + " AND `Ignore`=0 AND Missing=0 " + 
		 sPathCondition + (PK_AttributeType_Sort!=0 ? " AND IsDirectory=0" : "") ) ) )
        while( ( row=db_wrapper_fetch_row( resultf.r ) ) )
			sPK_File += row[0] + string(",");

    if( (bDiscs || bBookmarksOnly) && m_sPK_Devices_Online.empty()==false && ( resultd.r=m_pDatabase_pluto_media->db_wrapper_query_result( sSQL_Disc + sSQL_Where + " AND EK_Device IN (" + m_sPK_Devices_Online + ")") ) )
        while( ( row=db_wrapper_fetch_row( resultd.r ) ) )
			sPK_Disc += row[0] + string(",");

	// Find all the pictures for the files and discs
	map<int,int> mapFile_To_Pic,mapDisc_To_Pic;

	if( sPK_File.size() )
		sPK_File[ sPK_File.size()-1 ]=' '; // Get rid of the trailing comma
	if( sPK_Disc.size() )
		sPK_Disc[ sPK_Disc.size()-1 ]=' '; // Get rid of the trailing comma

	if( bFile && sPK_File.size() )
	{
		if( bShowFiles )
		{
			FetchPictures("File",sPK_File,mapFile_To_Pic,PK_AttributeType_Sort);
			PopulateFileBrowserInfoForFile(pMediaListGrid,PK_AttributeType_Sort,bSubDirectory,sPK_Sources /* this how we get back to where we are now */,sPK_File,mapFile_To_Pic);
		}
		else
			PopulateFileBrowserInfoForAttribute(pMediaListGrid,PK_AttributeType_Sort,PK_Attribute,sPK_File,"File");
	}

	if( bDiscs && sPK_Disc.size() )
	{
		if( bShowFiles )
		{
			FetchPictures("Disc",sPK_Disc,mapDisc_To_Pic,PK_AttributeType_Sort);
			PopulateFileBrowserInfoForDisc(pMediaListGrid,PK_AttributeType_Sort,sPK_Disc,mapDisc_To_Pic);
		}
		else
			PopulateFileBrowserInfoForAttribute(pMediaListGrid,PK_AttributeType_Sort,PK_Attribute,sPK_Disc,"Disc");
	}
	if( bBookmarks )
	{
		PopulateFileBrowserInfoForBookmark(pMediaListGrid,sPK_File,sPK_Disc);
	}

//	void PopulateFileBrowserInfoForFile(MediaListGrid *pMediaListGrid,int PK_AttributeType_Sort, string &sPath, string &sPK_File,map<int,int> &mapFile_To_Pic);
//	if( bDiscs || bLocalDisc )
//		PopulateFileBrowserInfoForDisc(pMediaListGrid,sPK_File,mapFile_To_Pic);
}

void Media_Plugin::FetchPictures(string sWhichTable,string &sPK_File_Or_Disc,map<int,int> &mapFile_To_Pic,int PK_AttributeType_Sort)
{
	string sSQL	= "SELECT FK_" + sWhichTable + ",FK_Picture FROM Picture_" + sWhichTable + " WHERE FK_" + sWhichTable + " IN (" + sPK_File_Or_Disc + ")";
    PlutoSqlResult result;
    DB_ROW row;
	string sPK_AlreadyGot;

	if( (result.r=m_pDatabase_pluto_media->db_wrapper_query_result( sSQL )) )
        while( ( row=db_wrapper_fetch_row( result.r ) ) )
		{
			sPK_AlreadyGot += row[0] + string(",");
			mapFile_To_Pic[ atoi(row[0]) ] = atoi(row[1]);
		}

	// If we're retrieving filenames or specific titles, don't do a cross-match to find attributes that have pictures.  Otherwise
	// you can have a file with no pictures, but if it's in the Genre 'Action', it will find a picture for some other Action file
	if( PK_AttributeType_Sort==0 ||  PK_AttributeType_Sort==-1 || PK_AttributeType_Sort==ATTRIBUTETYPE_Title_CONST )
		return;

	sSQL = "SELECT FK_" + sWhichTable + ",FK_Picture FROM " + sWhichTable + "_Attribute JOIN Picture_Attribute ON Picture_Attribute.FK_Attribute=" + sWhichTable + "_Attribute.FK_Attribute WHERE FK_" + sWhichTable + " IN (" + sPK_File_Or_Disc + ")";
	if( sPK_AlreadyGot.size() )
	{
		sPK_AlreadyGot[ sPK_AlreadyGot.size()-1 ]=' '; // Get rid of the trailing comma
		sSQL += " AND FK_" + sWhichTable + " NOT IN (" + sPK_AlreadyGot + ")";
	}
    PlutoSqlResult result2;
	if( (result2.r=m_pDatabase_pluto_media->db_wrapper_query_result( sSQL )) )
        while( ( row=db_wrapper_fetch_row( result2.r ) ) )
		{
			sPK_AlreadyGot += row[0] + string(",");
			mapFile_To_Pic[ atoi(row[0]) ] = atoi(row[1]);
		}
}

void Media_Plugin::PopulateFileBrowserInfoForPlayList(MediaListGrid *pMediaListGrid,string sPK_Users_Private)
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::PopulateFileBrowserInfoForPlayList Called to populate: %s", sPK_Users_Private.c_str());
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	if( sPK_Users_Private.size() )
		sPK_Users_Private = "," + sPK_Users_Private;  // So the syntax is right below

    string SQL = "SELECT PK_Playlist, Name, FK_Picture from Playlist where EK_User IS Null OR EK_User IN ( 0 " + sPK_Users_Private + " ) ORDER BY Name";

    PlutoSqlResult result;
    DB_ROW row;
    //int RowCount=0;

	FileBrowserInfo *pFileBrowserInfo;
    if( (result.r=m_pDatabase_pluto_media->db_wrapper_query_result(SQL)) )
        while( (row=db_wrapper_fetch_row(result.r)) )
		{
			pFileBrowserInfo = new FileBrowserInfo(row[1],string("!P") + row[0],0);
			if( row[2] )
				pFileBrowserInfo->m_PK_Picture = atoi(row[2]);
			pMediaListGrid->m_listFileBrowserInfo.push_back(pFileBrowserInfo);
		}
}

void Media_Plugin::PopulateFileBrowserInfoForDisc(MediaListGrid *pMediaListGrid,int PK_AttributeType_Sort,string &sPK_Disk,map<int,int> &mapDisk_To_Pic)
{
	string sSQL_Sort = "SELECT PK_Disc,Name,FK_FileFormat,Track,DateLastViewed FROM Disc JOIN Disc_Attribute ON FK_Disc=PK_Disc JOIN Attribute ON FK_Attribute=PK_Attribute AND FK_AttributeType=" + StringUtils::itos(PK_AttributeType_Sort) + " WHERE PK_Disc in (" + sPK_Disk + ")";

    PlutoSqlResult result;
    DB_ROW row;
	map<int,int>::iterator it;
	FileBrowserInfo *pFileBrowserInfo;
	// 0 =PK_Disc, 1=Name, 2=FileFormat
    if( (result.r=m_pDatabase_pluto_media->db_wrapper_query_result( sSQL_Sort )) )
        while( ( row=db_wrapper_fetch_row( result.r ) ) )
		{
			pFileBrowserInfo = new FileBrowserInfo(row[1],string("!r") + row[0] + (row[3] && atoi(row[3]) ? string(".") + row[3] : ""),atoi(row[0]),row[2] ? atoi(row[2]) : 0,row[4],'D',false,false);
			if( (it=mapDisk_To_Pic.find( atoi(row[0]) ))!=mapDisk_To_Pic.end() )
				pFileBrowserInfo->m_PK_Picture = it->second;
			pMediaListGrid->m_listFileBrowserInfo.push_back(pFileBrowserInfo);
		}
}

void Media_Plugin::PopulateFileBrowserInfoForFile(MediaListGrid *pMediaListGrid,int PK_AttributeType_Sort, 
	bool bSubDirectory, string &sPath, string &sPK_File,map<int,int> &mapFile_To_Pic)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "PopulateFileBrowserInfoForFile : attrtype_sort %d, "
		" subdir %d", PK_AttributeType_Sort, bSubDirectory);

	string sSQL_Sort;
	if( PK_AttributeType_Sort==0 || PK_AttributeType_Sort==-1 )
		sSQL_Sort = "SELECT PK_File,Path,Filename,IsDirectory,FK_FileFormat,Filename,DateLastViewed FROM File WHERE PK_File in (" + sPK_File + ")";
	else
		// TODO ___ FIND A BETTER WAY TO DO THIS QUERY.  It's returning a record for the file for each attribute, causing it to skip repeatedly, and it's very inefficient, and requires the double sorting order clauses!
		// TODO EVEN FURTHER, I'm using a subselect to get episode names, This just makes me feel all dirty inside..

		if ( PK_AttributeType_Sort==13 )
			sSQL_Sort = "SELECT PK_File,'',IFNULL(GROUP_CONCAT(Name ORDER BY FK_AttributeType DESC SEPARATOR '-'),Filename) As Name,0,FK_FileFormat,Filename,DateLastViewed FROM File F1 LEFT JOIN File_Attribute ON FK_File = PK_FILE LEFT JOIN Attribute ON FK_Attribute = PK_Attribute AND FK_AttributeType IN (11," + StringUtils::itos(PK_AttributeType_Sort) + ") Where  IsDirectory = 0 AND PK_File in ( " + sPK_File + " ) GROUP BY PK_File ORDER BY Name ASC";
		else
			sSQL_Sort = "SELECT PK_File,'',Name,0,FK_FileFormat,Filename,DateLastViewed FROM File LEFT JOIN File_Attribute ON FK_File=PK_File LEFT JOIN Attribute ON FK_Attribute=PK_Attribute AND FK_AttributeType=" + StringUtils::itos(PK_AttributeType_Sort) + " WHERE IsDirectory=0 AND PK_File in (" + sPK_File + ") AND (FK_AttributeType IS NULL OR FK_AttributeType=" + StringUtils::itos(PK_AttributeType_Sort) + ") ORDER BY PK_File,PK_Attribute DESC";

    PlutoSqlResult result;
    DB_ROW row;
	map<int,int>::iterator it;
	FileBrowserInfo *pFileBrowserInfo;
	int iLastPK_File=0;  // if the there are 2 attributes of the same twice the file may appear more than once
	// 0 =PK_File, 1=Path, 2=Name, 3=IsDirectory, 4=File Format
    if( (result.r=m_pDatabase_pluto_media->db_wrapper_query_result( sSQL_Sort )) )
        while( ( row=db_wrapper_fetch_row( result.r ) ) )
		{
			int PK_File = atoi(row[0]);
			if( PK_File==iLastPK_File )
				continue;
			iLastPK_File=PK_File;
			if( row[3][0]=='1' )
			{
				string sThisPath = string(row[1]) + "/" + row[2];
				StringUtils::Replace(&sThisPath,"'","\\'");  // Make it , separated, ' quoted and escaped so it works as a sql in (path) clause
				// sPath will be the sources (juke box, etc.) + \t + any prior directories
				if( sPath.find("\t!D")==string::npos )  // !D 1 directory
					pFileBrowserInfo = new FileBrowserInfo(row[2],sPath + "!D'" + sThisPath +"'",PK_File,row[4] ? atoi(row[4]) : 0,row[6],'F',true,false);
				else
					pFileBrowserInfo = new FileBrowserInfo(row[2],sPath + ",'" + sThisPath +"'",PK_File,row[4] ? atoi(row[4]) : 0,row[6],'F',true,false);
			}
			else
#ifdef SIM_JUKEBOX
				pFileBrowserInfo = new FileBrowserInfo(row[2] ? row[2] : row[5],string("!F") + row[0],PK_File,row[4] ? atoi(row[4]) : 0,
					row[5] && strstr(row[5],".dvd")!=NULL ? 'D' : 'F', false,false);
#else
			if( pMediaListGrid->m_iPK_MediaType==MEDIATYPE_pluto_Pictures_CONST )
			{
				pFileBrowserInfo = new FileBrowserInfo(row[2] ? row[2] : row[5],string("!F")+row[0],PK_File,row[4] ? atoi(row[4]) : 0,row[6],'F',false,false);
				pFileBrowserInfo->m_sPictureThumb = string(row[1]) + "/" + row[2]+".tnj";
			}
			else
			{
					// I have to fix the case where lack of title attribute no longer returns null.

					if (row[2] != NULL && string(row[2]).empty()) {
						row[2] = NULL;					
					}

					pFileBrowserInfo = new FileBrowserInfo(row[2] ? row[2] : row[5],string("!F") + row[0],PK_File,row[4] ? atoi(row[4]) : 0,row[6],'F',false,false);
					}
#endif
			if( (it=mapFile_To_Pic.find( atoi(row[0]) ))!=mapFile_To_Pic.end() )
				pFileBrowserInfo->m_PK_Picture = it->second;
			pMediaListGrid->m_listFileBrowserInfo.push_back(pFileBrowserInfo);
		}
}

void Media_Plugin::PopulateFileBrowserInfoForAttribute(MediaListGrid *pMediaListGrid,int PK_AttributeType_Sort, int PK_Attribute, string &sPK_File_Or_Disc,string sTable)
{
	string sSQL_Sort = "SELECT PK_Attribute,Name,min(FK_Picture) AS FK_Picture FROM " + sTable + " JOIN " + 
		sTable + "_Attribute ON FK_" + sTable + "=PK_" + sTable + " JOIN Attribute ON " + 
		sTable + "_Attribute.FK_Attribute=PK_Attribute AND FK_AttributeType=" + StringUtils::itos(PK_AttributeType_Sort) + 
		" LEFT JOIN Picture_Attribute ON Picture_Attribute.FK_Attribute=PK_Attribute WHERE " + 
		(sTable=="File" ? "IsDirectory=0 AND " : "") +
		"PK_" + sTable + " in (" + sPK_File_Or_Disc + ") GROUP BY PK_Attribute,IF(SUBSTRING(Name,1,4)='The ',SUBSTRING(Name,5),Name)";

    PlutoSqlResult result;
    DB_ROW row;
	map<int,int>::iterator it;
    if( (result.r=m_pDatabase_pluto_media->db_wrapper_query_result( sSQL_Sort )) )
	{
		// If we're looking for albums and there are none, just show the songs
		if( result.r->row_count==0 && PK_AttributeType_Sort==ATTRIBUTETYPE_Album_CONST )
		{
			result.ClearResults();
			sSQL_Sort = "SELECT PK_Attribute,Name,min(FK_Picture) AS FK_Picture FROM " + sTable + " JOIN " + sTable + "_Attribute ON FK_" + sTable + "=PK_" + sTable + " JOIN Attribute ON " + sTable + "_Attribute.FK_Attribute=PK_Attribute AND FK_AttributeType IN (" TOSTRING(ATTRIBUTETYPE_Track_CONST) "," TOSTRING(ATTRIBUTETYPE_Title_CONST) ") LEFT JOIN Picture_Attribute ON Picture_Attribute.FK_Attribute=PK_Attribute WHERE IsDirectory=0 AND PK_" + sTable + " in (" + sPK_File_Or_Disc + ") GROUP BY PK_Attribute,Name";
			if( (result.r=m_pDatabase_pluto_media->db_wrapper_query_result( sSQL_Sort ))==NULL )
				return;
		}

        while( ( row=db_wrapper_fetch_row( result.r ) ) )
		{
			FileBrowserInfo *pFileBrowserInfo = new FileBrowserInfo(row[1],string("!A") + row[0],atoi(row[0]));
			if( row[2] )
				pFileBrowserInfo->m_PK_Picture = atoi(row[2]);
			pMediaListGrid->m_listFileBrowserInfo.push_back(pFileBrowserInfo);
		}
	}

	//Sort by attribute type: also include the file which don't have attributes of this type
	//into Unknown attribute name.
	//if(0 == PK_Attribute)
	//{
	//	FileBrowserInfo *pFileBrowserInfo = new FileBrowserInfo("Unknown", string("!A0") + StringUtils::ltos(-PK_Attribute), -PK_Attribute);
	//	pMediaListGrid->m_listFileBrowserInfo.push_back(pFileBrowserInfo);
	//}
}

void Media_Plugin::PopulateFileBrowserInfoForBookmark(MediaListGrid *pMediaListGrid,string &sPK_File,string &sPK_Disc)
{
	if( sPK_File.empty()==false )
	{
		string sSQL = 
			"SELECT PK_Bookmark,Bookmark.FK_File,Description,Name,FK_Picture,FK_FileFormat "
			"FROM Bookmark "
			"LEFT JOIN File ON Bookmark.FK_File=PK_File "
			"LEFT JOIN File_Attribute ON PK_File=File_Attribute.FK_File "
			"LEFT JOIN Attribute ON File_Attribute.FK_Attribute=PK_Attribute "
			"WHERE (FK_AttributeType IS NULL OR FK_AttributeType =" TOSTRING(ATTRIBUTETYPE_Title_CONST) ") AND IsAutoResume=0 "
			"AND Bookmark.FK_File IN (" + sPK_File + ") "
			"ORDER BY PK_Bookmark";

		PlutoSqlResult result;
		DB_ROW row;
		//int PK_Bookmark_Last=0; // Don't add the same bookmark twice
		if( (result.r=m_pDatabase_pluto_media->db_wrapper_query_result( sSQL )) )
			while( ( row=db_wrapper_fetch_row( result.r ) ) )
			{
				string sDescription;
				if( row[2] && row[3] ) // Both a title and a bookmark description
					sDescription = string(row[3]) + "/" + row[2];  // Title + bookmark
				else if( row[3] )
					sDescription = row[3];
				else if( row[2] )
					sDescription = row[2];
				else
					sDescription = "Bookmark"; // Shouldn't happen
				FileBrowserInfo *pFileBrowserInfo = new FileBrowserInfo(sDescription,atoi(row[0]),
					row[4] ? atoi(row[4]) : 0, row[1] ? atoi(row[1]) : 0, 0, row[5] ? atoi(row[5]) : 0,0);
				pMediaListGrid->m_listFileBrowserInfo.push_back(pFileBrowserInfo);
			}
	}
}

void Media_Plugin::FileBrowser( MediaListGrid *pMediaListGrid,int PK_MediaType, string &sPK_MediaSubType, string &sPK_FileFormat, string &sPK_Attribute_Genres, string &sPK_Sources, string &sPK_Users_Private, int PK_Users, int *iPK_Variable, string *sValue_To_Assign )
{
	// Maybe we won't use the FileBrowser anymore, and just always get the data from the database
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write(LV_WARNING,"Starting File list");
#endif
	// A comma separated list of file extensions.  Blank means all files
	Row_MediaType *pRow_MediaType=m_pDatabase_pluto_main->MediaType_get()->GetRow(PK_MediaType);
	if( !pRow_MediaType )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find Media Type: %d",PK_MediaType);
		return;
	}
	string Extensions = pRow_MediaType->Extensions_get();

	// The first time, sSources will be empty.  For all sub directories the value will be !D + sub directory
	// For each subsequent times, it will be !D + first dir + \t + second dir and so on.
	// To go back, skip the last directory on the list
	if( sPK_Sources.length()<3 || sPK_Sources[0]!='!' || sPK_Sources[1]!='D' )
		sPK_Sources = "";

	string::size_type posLastPath=2,pos=2;
	if( sPK_Sources.length() )
		while( (pos=sPK_Sources.find("\t",pos))!=string::npos )
			posLastPath=++pos;

	string sPaths;
	if( sPK_Sources.size() )
		sPaths = sPK_Sources.substr(posLastPath);
	else
		sPaths = "/home/public/data/videos";  // Todo, fill this in for private as well

//	int iRow=0;
	if( sPK_Sources.length() )
	{
		string sPrevious = posLastPath<3 ? "!D" : sPK_Sources.substr(0,posLastPath-1);
		pMediaListGrid->m_listFileBrowserInfo.push_back( new FileBrowserInfo("~S21~<-- Back (..)",sPrevious,0,0,0,'F',true,true) );
	}

	pos=0;
    while( pos<sPaths.size() )
	{
		string sSearchPath=StringUtils::Tokenize(sPaths,"\t",pos);

		list<FileDetails *> listFileDetails;
		GetDirContents(listFileDetails,sSearchPath,Extensions);

		map<string,DatabaseInfoOnPath *> mapDatabaseInfoOnPath;
		PopulateWithDatabaseInfoOnPath(mapDatabaseInfoOnPath,sSearchPath);

		for (list<FileDetails *>::iterator i = listFileDetails.begin(); i != listFileDetails.end(); i++)
		{
			FileDetails *pFileDetails = *i;
			map<string,DatabaseInfoOnPath *>::iterator itDIOP = mapDatabaseInfoOnPath.find( pFileDetails->m_sFileName );
			DatabaseInfoOnPath *pDatabaseInfoOnPath = itDIOP==mapDatabaseInfoOnPath.end() ? NULL : itDIOP->second;

			FileBrowserInfo *pFileBrowserInfo;
			if( pFileDetails->m_bIsDir && (pDatabaseInfoOnPath==NULL || pDatabaseInfoOnPath->m_bDirectory==true) )
			{
				if( sPK_Sources.length() )
					pFileBrowserInfo = new FileBrowserInfo(pFileDetails->m_sFileName,sPK_Sources +"\t" + pFileDetails->m_sBaseName + pFileDetails->m_sFileName,0,0,0,'F',true,false);
				else
					pFileBrowserInfo = new FileBrowserInfo(pFileDetails->m_sFileName,"!D" + pFileDetails->m_sBaseName + pFileDetails->m_sFileName,0,0,0,'F',true,false);
			}
			else
				pFileBrowserInfo = new FileBrowserInfo(pFileDetails->m_sFileName,pFileDetails->m_sBaseName + pFileDetails->m_sFileName,0,0,0,'F',false,false);

			pFileBrowserInfo->m_tLastViewed = pFileDetails->m_tDate;

			if( pDatabaseInfoOnPath )
			{
				pFileBrowserInfo->m_PK_File=pDatabaseInfoOnPath->m_PK_File;
				pFileBrowserInfo->m_PK_Picture=pDatabaseInfoOnPath->m_PK_Picture;
			}
			pMediaListGrid->m_listFileBrowserInfo.push_back( pFileBrowserInfo );

			delete pFileDetails; // We won't need it anymore and it was allocated on the heap
		}
		for(map<string,DatabaseInfoOnPath *>::iterator itDIOP=mapDatabaseInfoOnPath.begin();itDIOP!=mapDatabaseInfoOnPath.end();++itDIOP)
			delete itDIOP->second;
	}
}

void Media_Plugin::PopulateWithDatabaseInfoOnPath(map<string,DatabaseInfoOnPath *> &mapDatabaseInfoOnPath,string &sSearchPath)
{
	string sSql = 
		"( "
			"SELECT Filename, PK_File, IsDirectory, Picture_Attribute.FK_Picture "
			"FROM File "
				"JOIN File_Attribute ON PK_File = FK_File "
				"JOIN Picture_Attribute ON Picture_Attribute.FK_Attribute = File_Attribute.FK_Attribute "
				"JOIN Picture ON Picture_Attribute.FK_Picture = PK_Picture "
				"JOIN Attribute ON Picture_Attribute.FK_Attribute = PK_Attribute "
				"JOIN AttributeType ON FK_AttributeType = PK_AttributeType "
			"WHERE Path = \"" + sSearchPath + "\" "
			"GROUP BY Filename "
			"ORDER BY PicPriority DESC "
		") "
		"UNION "
		"( "
		"SELECT Filename, PK_File, IsDirectory, FK_Picture "
		"FROM File "
		"JOIN Picture_File ON PK_File = FK_File "
		"JOIN Picture ON FK_Picture = PK_Picture "
		"WHERE Path = \"" + sSearchPath + "\""
		")";

	PlutoSqlResult result;
	DB_ROW row;
    if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( sSql ) ) )
        while( ( row=db_wrapper_fetch_row( result.r ) ) )
			mapDatabaseInfoOnPath[row[0]] = new DatabaseInfoOnPath(atoi(row[1]),row[2][0]==1,row[3] ? atoi(row[3]) : 0);
}

/*  jukeboxes
	//Jukeboxes special hack
	vector<Row_Device *> vectRow_Device;
	if(sSources.find("JUKE_BOXES")!=string::npos)
	{
		//this is the movies top level folder; we'll add at the beginning an entry for each jukeboxe from current installation
		m_pDatabase_pluto_main->Device_get()->GetRows(
			"FK_DeviceTemplate = " + StringUtils::ltos(DEVICETEMPLATE_Powerfile_C200_CONST) + " AND "  + 
			"FK_Installation = " + StringUtils::ltos(m_pRouter->iPK_Installation_get()), 
			&vectRow_Device);	
		
		if(!sSubDirectory.length()) 
		{
			for(vector<Row_Device *>::iterator iPowerFile = vectRow_Device.begin(); iPowerFile != vectRow_Device.end(); iPowerFile++)
			{
				Row_Device *pRow_Device = *iPowerFile;
				string sItemName = "Jukebox: " + pRow_Device->Description_get();
				FileListInfo *pFileListInfo = new FileListInfo(true, sItemName, false);
		
				pCell = new DataGridCell("~S2~" + sItemName, sItemName);

				string newParams = sPaths + "\n" + Extensions + "\n" 
					+ "\n" + StringUtils::itos(iRow)+ "\n" + sItemName;
				DCE::CMD_NOREP_Populate_Datagrid CMDPDG(PK_Controller, m_pDatagrid_Plugin->m_dwPK_Device,
					"", GridID, DATAGRID_Directory_Listing_CONST, newParams, 0);
				pCell->m_pMessage = CMDPDG.m_pMessage;
				
				pDataGrid->SetData(0, iRow++, pCell);
				pDataGrid->m_vectFileInfo.push_back(pFileListInfo);
			}
		}
		else if( StringUtils::StartsWith(sSubDirectory,"Jukebox: ") )
		{
			//not a top level folder
			for(vector<Row_Device *>::iterator iPowerFile = vectRow_Device.begin(); iPowerFile != vectRow_Device.end(); iPowerFile++)
			{
				Row_Device *pRow_Device = *iPowerFile;
				
				if("Jukebox: " + pRow_Device->Description_get() == sSubDirectory)
				{
					//we are in a "jukebox"; let's show its movies
					string sStatus;
					CMD_Get_Jukebox_Status CMD_Get_Jukebox_Status_(m_dwPK_Device, pRow_Device->PK_Device_get(), "", &sStatus); // ""= force=no
					
					if(!SendCommand(CMD_Get_Jukebox_Status_))
					{
						FileListInfo *pFileListInfo = new FileListInfo(true, "", false);

						pCell = new DataGridCell("Unable to communicate with Powerfile '" + pRow_Device->Description_get() + "'", "");
						pDataGrid->SetData(0, iRow++, pCell);

						pDataGrid->m_vectFileInfo.push_back(pFileListInfo);					
						return pDataGrid;
					}
					
					vector<string> vectElems;
					StringUtils::Tokenize(sStatus, ",", vectElems);
					
					for(vector<string>::iterator it = vectElems.begin(); it != vectElems.end(); it++)
					{
						string sElem = *it;
						if(sElem.length() > 0 && sElem[0] == 'S')
						{
							string sSlotIndex = sElem.substr(1, sElem.find('=', 0) - 1);
							FileListInfo *pFileListInfo = new FileListInfo(true, sSlotIndex + ". " + "Movie", false);
							pCell = new DataGridCell("~S2~not identified", "not identified");
							
							pDataGrid->SetData(0, iRow++, pCell);
				
							pDataGrid->m_vectFileInfo.push_back(pFileListInfo);							
						}
					}
				}
			}
			
			return pDataGrid;
		}	
	}
*/

static bool MediaSectionGridComparer(MediaSectionGrid *x, MediaSectionGrid *y)
{
	if( x->m_iSort1==y->m_iSort1 )
		return x->m_iSort2<y->m_iSort2;
	else
		return x->m_iSort1<y->m_iSort1;
}

class DataGridTable *Media_Plugin::CurrentMediaSections( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	string::size_type pos=0;
	string s1 = StringUtils::Tokenize( Parms, ",", pos );	// 32
	string s2 = StringUtils::Tokenize( Parms, ",", pos );	// PK_Orbiter
	string s3 = StringUtils::Tokenize( Parms, ",", pos );	// PK_DesignObj of current screen
	string s4 = StringUtils::Tokenize( Parms, ",", pos );	// PK_Device (from VARIABLE_PK_Device_CONST)
	/*int PK_User = */atoi(StringUtils::Tokenize( Parms, ",", pos ).c_str());

	*iPK_Variable=VARIABLE_Track_or_Playlist_Positio_CONST;
	*sValue_To_Assign="";

	class MediaStream *pMediaStream = NULL;
	string sPK_EntertainArea = pMessage->m_mapParameters[COMMANDPARAMETER_PK_EntertainArea_CONST];
	if (!sPK_EntertainArea.empty())
	{
		vector<class EntertainArea *> vectEntertainArea;
		DetermineEntArea(0, 0 , sPK_EntertainArea, vectEntertainArea);
		pMediaStream = vectEntertainArea.size() == 1 ? vectEntertainArea[0]->m_pMediaStream : NULL;
	}
	else
	{
		pMediaStream = DetermineStreamOnOrbiter(pMessage->m_dwPK_Device_From, true);
	}

    DataGridTable *pDataGrid = new DataGridTable();

    if ( pMediaStream == NULL )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "The message was not from an orbiter or it doesn't have a media stream on it!");

		//nothing is playing at this moment; return an empty datagrid
        return pDataGrid;
    }

	// Keep track of the ones we've already picked up from the database so we can show the user the extra ones that aren't there
	map< pair<int,int>,bool > mapSections;

    deque<MediaFile *>::iterator itFiles;
    string sCurrentFile;
 
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::CurrentMediaSections titles %d",(int) pMediaStream->m_dequeMediaTitle.size());

    int currentPos = 0;
	// First put the bookmarks at the top
	string sBookmarks;
	if( pMediaStream->m_dwPK_Disc )
		sBookmarks = "FK_Disc=" + StringUtils::itos(pMediaStream->m_dwPK_Disc) + " AND IsAutoResume=0 AND (EK_Users IS NULL OR EK_Users="+StringUtils::itos(pMediaStream->m_iPK_Users)+")";
	else if( pMediaStream->m_dequeMediaFile.size() &&
		pMediaStream->m_iDequeMediaFile_Pos>=0 &&
		pMediaStream->m_iDequeMediaFile_Pos<pMediaStream->m_dequeMediaFile.size() )
	{
		MediaFile *pMediaFile = pMediaStream->GetCurrentMediaFile();
		sBookmarks = "FK_File=" + StringUtils::itos(pMediaFile->m_dwPK_File) + " AND IsAutoResume=0 AND (EK_Users IS NULL OR EK_Users="+StringUtils::itos(pMediaStream->m_iPK_Users)+")";
	}
	if( sBookmarks.empty()==false )
	{
		vector<Row_Bookmark *> vectRow_Bookmark;
		m_pDatabase_pluto_media->Bookmark_get()->GetRows(sBookmarks,&vectRow_Bookmark);
		for(vector<Row_Bookmark *>::iterator it=vectRow_Bookmark.begin();it!=vectRow_Bookmark.end();++it)
			pDataGrid->SetData(0, currentPos++,new DataGridCell((*it)->Description_get(),(*it)->Position_get()));
	}

	// This branch executed for DVDs and other disc media with titles and sections.
	if( pMediaStream->m_bContainsTitlesOrSections )
	{
		for(size_t sTitle=0;sTitle<pMediaStream->m_dequeMediaTitle.size();++sTitle)
		{
			MediaTitle *pMediaTitle = pMediaStream->m_dequeMediaTitle[sTitle];
			list_int *listPK_Attribute = pMediaTitle->m_mapPK_Attribute_Find(ATTRIBUTETYPE_Chapter_CONST);
			int PK_Attribute_Title = listPK_Attribute && listPK_Attribute->size() ? *(listPK_Attribute->begin()) : 0;
			string sTitleName = m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetAttributeName(PK_Attribute_Title);
			for(size_t sSection=0;sSection<pMediaTitle->m_dequeMediaSection.size();++sSection)
			{
				MediaSection *pMediaSection = pMediaTitle->m_dequeMediaSection[sSection];
				list_int *listPK_Attribute = pMediaSection->m_mapPK_Attribute_Find(ATTRIBUTETYPE_Chapter_CONST);
				int PK_Attribute_Title = listPK_Attribute && listPK_Attribute->size() ? *(listPK_Attribute->begin()) : 0;
				string sCell = StringUtils::itos(sSection+1) + " " + m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetAttributeName(PK_Attribute_Title);
				string sValue = " TITLE:" + StringUtils::itos(sTitle+1) + " CHAPTER:" + StringUtils::itos(sSection+1);
				pDataGrid->SetData(0, currentPos++,new DataGridCell(sCell,sValue));
				if( (int)sTitle==pMediaStream->m_iDequeMediaTitle_Pos && (int)sSection==pMediaStream->m_iDequeMediaSection_Pos )
					*sValue_To_Assign=sValue;
				mapSections[ make_pair<int,int> (sSection,sTitle) ] = true;
			}
		}
	}
	else
	{
		// This branch for file based media.
		int iSection=0;
		for ( itFiles = pMediaStream->m_dequeMediaFile.begin(); itFiles != pMediaStream->m_dequeMediaFile.end(); itFiles++ )
		{
			MediaFile *pMediaFile = *itFiles;
			// int index = itFiles - pMediaStream->m_dequeMediaFile.begin();
			sCurrentFile = pMediaFile->m_sDescription.size() ? pMediaFile->m_sDescription : FileUtils::FilenameWithoutPath(pMediaFile->FullyQualifiedFile());

			string sCurrentTitle; 
			
			if (pMediaFile->m_sEpisode.size()) {
			  sCurrentTitle = pMediaFile->m_sTitle + "\n    \"" + pMediaFile->m_sEpisode + "\"";
			} else if (pMediaFile->m_sSystem.size()) 
			  {
			    sCurrentTitle = pMediaFile->m_sTitle + "\n    " + pMediaFile->m_sSystem;
			  }
			else 
			  {
			    sCurrentTitle = pMediaFile->m_sTitle;
			  }
			
			if (sCurrentTitle.size()) {
			  pDataGrid->SetData(0, currentPos++, new DataGridCell(sCurrentTitle, StringUtils::itos(itFiles - pMediaStream->m_dequeMediaFile.begin())));
			} else {
			  pDataGrid->SetData(0, currentPos++,new DataGridCell(sCurrentFile, StringUtils::itos(itFiles - pMediaStream->m_dequeMediaFile.begin())));
			}

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Returning data: (%d) -> %s section %d", itFiles - pMediaStream->m_dequeMediaFile.begin(), ((*itFiles)->m_sFilename).c_str(),iSection);
			mapSections[ make_pair<int,int> (iSection,0) ] = true;
		}
		*sValue_To_Assign=StringUtils::itos(pMediaStream->m_iDequeMediaFile_Pos);
	}

	list<MediaSectionGrid *> listMediaSectionGrid;  // Store them here first so we can sort them
	map< pair<int,int>,bool >::iterator itSections;

	// For CD's playback started may report sections, but it's really going to be files
	if( pMediaStream->m_iPK_MediaType!=MEDIATYPE_pluto_CD_CONST )
	{
		for(map< pair<int,int>,string >::iterator it = pMediaStream->m_mapSections.begin(); it!=pMediaStream->m_mapSections.end(); ++it)
		{
			itSections = mapSections.find( make_pair<int,int> ( it->first.first, it->first.second ) );
			if( itSections==mapSections.end() )
			{
				if( pMediaStream->m_bContainsTitlesOrSections ) // There's a title
				{
					string sCell;
					if( it->first.second>=0 )
						sCell += " TITLE:" + StringUtils::itos(it->first.second+1);
					if( it->first.first>=0 )
						sCell += " CHAPTER:" + StringUtils::itos(it->first.first+1);  // Internally we're zero based

					if( it->first.second==pMediaStream->m_iDequeMediaTitle_Pos && it->first.first==pMediaStream->m_iDequeMediaSection_Pos )
						*sValue_To_Assign=sCell;

					listMediaSectionGrid.push_back(new MediaSectionGrid(it->first.second,it->first.first,new DataGridCell(it->second,sCell)));
				}
				else
					listMediaSectionGrid.push_back(new MediaSectionGrid(it->first.second,it->first.first,new DataGridCell(it->second, StringUtils::itos(it->first.first))));

			}
		}
	}

	listMediaSectionGrid.sort(MediaSectionGridComparer);
	for(list<MediaSectionGrid *>::iterator it=listMediaSectionGrid.begin();it!=listMediaSectionGrid.end();++it)
	{
		MediaSectionGrid *pMediaSectionGrid = *it;
		pDataGrid->SetData(0, currentPos++,pMediaSectionGrid->m_pDataGridCell);
		delete pMediaSectionGrid;
	}
    return pDataGrid;
}

class DataGridTable *Media_Plugin::CDTracks( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

    DataGridTable *pDataGrid = new DataGridTable();
    DataGridCell *pCell;

	MediaStream *pMediaStream = NULL;

	string sSQL;
	if( atoi(Parms.c_str())==0 )
	{
		vector<EntertainArea *> vectEntertainArea;
		DetermineEntArea( pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea );
		EntertainArea *pEntertainArea = vectEntertainArea.size() ? vectEntertainArea[0] : NULL;
		MediaFile *pMediaFile=NULL;
		if( !pEntertainArea || !pEntertainArea->m_pMediaStream || (pMediaFile=pEntertainArea->m_pMediaStream->GetCurrentMediaFile())==NULL )
		{
		    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::CDTracks no disk for %p %p", pEntertainArea, pMediaFile);
			return pDataGrid;
		}
		pMediaStream = pEntertainArea->m_pMediaStream;

		if( pMediaFile->m_dwPK_Disk )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::CDTracks - media file has disc id %d", pMediaFile->m_dwPK_Disk);

			sSQL = "select Track,Name FROM Attribute JOIN Disc_Attribute ON FK_Attribute=PK_Attribute WHERE FK_AttributeType=" TOSTRING(ATTRIBUTETYPE_Title_CONST) 
				" AND FK_Disc=" + StringUtils::itos(pMediaFile->m_dwPK_Disk);
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::CDTracks - media stream has disc id %d", pEntertainArea->m_pMediaStream->m_dwPK_Disc);

			sSQL = "select Track,Name FROM Attribute JOIN Disc_Attribute ON FK_Attribute=PK_Attribute WHERE FK_AttributeType=" TOSTRING(ATTRIBUTETYPE_Title_CONST) 
				" AND FK_Disc=" + StringUtils::itos(pEntertainArea->m_pMediaStream->m_dwPK_Disc);
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::CDTracks - using disc id from parms %s", Parms.c_str());

		sSQL = "select Track,Name FROM Attribute JOIN Disc_Attribute ON FK_Attribute=PK_Attribute WHERE FK_AttributeType=" TOSTRING(ATTRIBUTETYPE_Title_CONST) " AND FK_Disc=" + Parms;
	}

	PlutoSqlResult result;
    DB_ROW row;

	int currentPos=0;

	map<int,bool> mapTracksAdded;

    if( (result.r=m_pDatabase_pluto_media->db_wrapper_query_result(sSQL)) )
        while( (row=db_wrapper_fetch_row(result.r)) )
		{
			if( row[0] && row[1] )
			{
				pCell = new DataGridCell(row[1],row[0]);
				pDataGrid->SetData(0, currentPos++,pCell);
				mapTracksAdded[ atoi(row[0]) ]=true;
			}
		}

	if( pMediaStream )
	{
		for(deque<MediaFile *>::iterator it=pMediaStream->m_dequeMediaFile.begin();it!=pMediaStream->m_dequeMediaFile.end();++it)
		{
			MediaFile *pMediaFile = *it;
			if( mapTracksAdded.find( pMediaFile->m_iTrack )==mapTracksAdded.end() )
			{
				// It's an unidentified track
				pCell = new DataGridCell("Track " + StringUtils::itos(pMediaFile->m_iTrack),StringUtils::itos(pMediaFile->m_iTrack));
				pDataGrid->SetData(0, currentPos++,pCell);
			}
		}
	}

    return pDataGrid;
}

class DataGridTable *Media_Plugin::MediaSearchAutoCompl( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    FileListGrid *pDataGrid = new FileListGrid( m_pDatagrid_Plugin, this );
    DataGridCell *pCell;

	string::size_type pos=0;
	int PK_MediaType = atoi(StringUtils::Tokenize(Parms,"|",pos).c_str());
	string AC = StringUtils::Tokenize(Parms,"|",pos);
	string sPK_Users = StringUtils::Tokenize(Parms,"|",pos);

	if( AC.length( )==0 )
        return pDataGrid; // Nothing passed in yet

    DB_ROW row;
    PlutoSqlResult result_file;
	string SQL = "select DISTINCT PK_File,Path,Filename,FK_Picture "
		"FROM File "
		"LEFT JOIN Picture_File ON FK_File=PK_File "
		"WHERE Filename Like '%" + StringUtils::SQLEscape(AC) + "%' "
		"AND (EK_Users_Private IS NULL " + (sPK_Users.empty()==false ? " OR EK_Users_Private IN (" + sPK_Users + ")" : "") + ") "
		"AND Missing=0 ";

	if( PK_MediaType==MEDIATYPE_pluto_StoredAudio_CONST )
		SQL += " AND EK_MediaType IN (" TOSTRING(MEDIATYPE_pluto_StoredAudio_CONST) "," TOSTRING(MEDIATYPE_pluto_CD_CONST) ")";
	else if( PK_MediaType==MEDIATYPE_pluto_StoredVideo_CONST )
		SQL += " AND EK_MediaType IN (" TOSTRING(MEDIATYPE_pluto_StoredVideo_CONST) "," TOSTRING(MEDIATYPE_pluto_DVD_CONST) "," TOSTRING(MEDIATYPE_pluto_HDDVD_CONST) "," TOSTRING(MEDIATYPE_pluto_BD_CONST) ")";
	else 
		SQL += " AND EK_MediaType=" + StringUtils::itos(PK_MediaType);

	SQL += " ORDER BY Filename limit 30";

    int RowCount=0;

	if( ( result_file.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) )
    {
        while( ( row=db_wrapper_fetch_row( result_file.r ) ) )
        {
            string label = row[2];
            label += string( "\n" ) + row[1];
            pCell = new DataGridCell( "", string("!F") + row[0] );
			if( row[3] && row[3][0]!='0' )
			{
				size_t st=0;
				pCell->m_pGraphicData = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + string(row[3]) + "_tn.jpg",st);
				pCell->m_GraphicLength=st;
			}
            pDataGrid->SetData( 0, RowCount, pCell );

            pCell = new DataGridCell( label, string("!F") + row[0] );
            pCell->m_Colspan = 5;
            pDataGrid->m_vectFileInfo.push_back( new FileListInfo( false, string(row[1]) + "/" + row[2], false ) );
            pDataGrid->SetData( 1, RowCount++, pCell );
        }
    }

    SQL = 
		"select DISTINCT PK_Attribute, Name, Description, FK_Picture FROM Attribute "
		"JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "
		"JOIN MediaType_AttributeType ON MediaType_AttributeType.FK_AttributeType=PK_AttributeType "
		"LEFT JOIN Picture_Attribute ON FK_Attribute=PK_Attribute "
		"WHERE Name Like '" + StringUtils::SQLEscape(AC) + "%' AND Identifier>0 "
		"AND MediaType_AttributeType.EK_MediaType=" + StringUtils::itos(PK_MediaType) + " "
		"ORDER BY Name limit 30";

    PlutoSqlResult result;

    string AttributesFirstSearch; // Because we're going to search twice and want to exclude any attributes we hit the first search

    if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) )
    {
        while( ( row=db_wrapper_fetch_row( result.r ) ) )
        {
            string label = /* string( "~`S24`" ) + */ row[1];
            label += string( "\n" ) + row[2];
            pCell = new DataGridCell( "", string("!A") + row[0] );
			if( row[3] && row[3][0]!='0' )
			{
				size_t st=0;
				pCell->m_pGraphicData = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + string(row[3]) + "_tn.jpg",st);
				pCell->m_GraphicLength=st;
			}
            pDataGrid->SetData( 0, RowCount, pCell );

            if( AttributesFirstSearch.length() )
                AttributesFirstSearch += ",";
            AttributesFirstSearch += row[0];

            pCell = new DataGridCell( label,  string("!A") + row[0] );
            pCell->m_Colspan = 5;
            pDataGrid->m_vectFileInfo.push_back( new FileListInfo( atoi( row[0] ) ) );
            pDataGrid->SetData( 1, RowCount++, pCell );
        }
    }

	// If we get less than 5 rows, we will do a 2nd search
	if( result.r->row_count> 4 )
		return pDataGrid;

    SQL = 
		"select DISTINCT PK_Attribute, Name, Description, FK_Picture FROM SearchToken "
		"JOIN SearchToken_Attribute ON PK_SearchToken=FK_SearchToken "
		"JOIN Attribute ON SearchToken_Attribute.FK_Attribute=PK_Attribute "
		"JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "
		"JOIN MediaType_AttributeType ON MediaType_AttributeType.FK_AttributeType=PK_AttributeType "
		"LEFT JOIN Picture_Attribute ON Picture_Attribute.FK_Attribute=PK_Attribute "
		+ (AttributesFirstSearch.length() ? "AND PK_Attribute NOT IN (" + StringUtils::SQLEscape(AttributesFirstSearch) + ") " : "") +
		"WHERE Token like '" + StringUtils::SQLEscape(AC) + "%' "
		"AND MediaType_AttributeType.EK_MediaType=" + StringUtils::itos(PK_MediaType) + " "
		"AND Identifier>0 "
		"ORDER BY Name "
		"limit 30";


	result.ClearResults();

    if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) )
    {
        while( ( row=db_wrapper_fetch_row( result.r ) ) )
        {
            string label = /*string( "~`S24`" ) + */ row[1];
            label += string( "\n" ) + row[2];
            pCell = new DataGridCell( "",  string("!A") + row[0] );
			if( row[3] && row[3][0]!='0' )
			{
				size_t st=0;
				pCell->m_pGraphicData = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + string(row[3]) + "_tn.jpg",st);
				pCell->m_GraphicLength=st;
			}
            pDataGrid->SetData( 0, RowCount, pCell );

            pCell = new DataGridCell( label,  string("!A") + row[0] );
            pCell->m_Colspan = 5;
            pDataGrid->m_vectFileInfo.push_back( new FileListInfo( atoi( row[0] ) ) );
            pDataGrid->SetData( 1, RowCount++, pCell );
        }
    }
    return pDataGrid;
}

class DataGridTable *Media_Plugin::MediaAttrFiles( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    DataGridTable *pDataGrid = new DataGridTable( );
    DataGridCell *pCell;

    string PK_Attribute = Parms;

    if( PK_Attribute.substr( 0, 2 )=="!A" )
        PK_Attribute = PK_Attribute.substr( 2 );
    else if( PK_Attribute.substr( 0, 2 )=="!F" )
        PK_Attribute = StringUtils::itos(m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetAttributeFromFileID( atoi( PK_Attribute.substr( 2 ).c_str( ) ) ) );

    if ( PK_Attribute == "" )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Got a null attributte from string: %s", Parms.c_str());
        return new DataGridTable();;
    }

    string SQL="select DISTINCT Dest.FK_File, Attribute.Name, AttributeType.Description "\
        "FROM File_Attribute As Source "\
        "JOIN File_Attribute As Dest "\
		"ON Source.FK_File=Dest.FK_File AND Source.FK_Attribute=" + StringUtils::itos(atoi(PK_Attribute.c_str())) +
        " JOIN File ON Dest.FK_File=PK_File "\
        "JOIN Attribute ON Dest.FK_Attribute=PK_Attribute "\
        "JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "\
        "JOIN MediaType_AttributeType ON File.EK_MediaType=MediaType_AttributeType.EK_MediaType "\
        "AND MediaType_AttributeType.FK_AttributeType=Attribute.FK_AttributeType "\
        "WHERE Identifier=1 ORDER BY Name limit 200;";

    PlutoSqlResult result;
    DB_ROW row;
    int RowCount=0;

    if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) )
    {
        while( ( row=db_wrapper_fetch_row( result.r ) ) )
        {
            string label = row[1];
            label += string( "\n" ) + row[2];
            pCell = new DataGridCell( label, row[0] );
            pDataGrid->SetData( 0, RowCount++, pCell );
        }
    }
    return pDataGrid;
}

class DataGridTable *Media_Plugin::MediaAttrCollections( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    DataGridTable *pDataGrid = new DataGridTable( );
    DataGridCell *pCell;

	if( Parms.length()==0 )
		return pDataGrid;

    string PK_Attribute = Parms;
    if( PK_Attribute.substr( 0, 2 )=="!A" )
        PK_Attribute = PK_Attribute.substr( 2 );
    else if( PK_Attribute.substr( 0, 2 )=="!F" )
        PK_Attribute = StringUtils::itos(
            m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetAttributeFromFileID( atoi( PK_Attribute.substr( 2 ).c_str( ) ) )
         );

    if ( PK_Attribute == "" )
    {
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Got a null attributte from string: %s", Parms.c_str());
        return new DataGridTable();
    }

    string SQL="select DISTINCT Dest.FK_Attribute, Attribute.Name, AttributeType.Description "\
        "FROM File_Attribute As Source "\
        "JOIN File_Attribute As Dest "\
        "ON Source.FK_File=Dest.FK_File AND Source.FK_Attribute='" + StringUtils::itos(atoi(PK_Attribute.c_str())) +
        "' JOIN File ON Dest.FK_File=PK_File "\
        "JOIN Attribute ON Dest.FK_Attribute=PK_Attribute "\
        "JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "\
        "JOIN MediaType_AttributeType ON File.EK_MediaType=MediaType_AttributeType.EK_MediaType "\
        "AND MediaType_AttributeType.FK_AttributeType=Attribute.FK_AttributeType "\
        "WHERE Identifier=2 ORDER BY Name limit 100;";

    PlutoSqlResult result;
    DB_ROW row;
    int RowCount=0;

    if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) )
    {
        while( ( row=db_wrapper_fetch_row( result.r ) ) )
        {
            string label = row[1];
            label += string( "\n" ) + row[2];
            pCell = new DataGridCell( label, row[0] );
            pDataGrid->SetData( 0, RowCount++, pCell );
        }
    }
    return pDataGrid;
}

class DataGridTable *Media_Plugin::MediaAttrXref( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    DataGridTable *pDataGrid = new DataGridTable( );
    DataGridCell *pCell;

	string::size_type pos=0;
	string PK_MediaType = StringUtils::Tokenize(Parms,"|",pos);
	string PK_Attribute = StringUtils::Tokenize(Parms,"|",pos);
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got this PK_Attributte: %s", PK_Attribute.c_str());

	if( PK_Attribute.length()==0 )
		return pDataGrid;

	if( PK_Attribute.substr( 0, 2 )=="!A" )
        PK_Attribute = PK_Attribute.substr( 2 );

    int PK_Picture;
    string Extension;

	if( PK_Attribute.substr( 0, 2 )=="!F" )
	{
        Extension = m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetPictureFromFileID(atoi(PK_Attribute.substr(2).c_str()),&PK_Picture);
        PK_Attribute = StringUtils::itos(m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetAttributeFromFileID( atoi( PK_Attribute.substr( 2 ).c_str( ) ) ) );
	}
	else
        Extension = m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetPictureFromAttributeID(atoi(PK_Attribute.c_str()),&PK_Picture);

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Transformed PK_Attributte: %s", PK_Attribute.c_str());

    string SQL="select DISTINCT Dest.FK_Attribute, Attribute.Name, AttributeType.Description "\
        "FROM File_Attribute As Source "\
        "JOIN File_Attribute As Dest "\
        "ON Source.FK_File=Dest.FK_File AND Source.FK_Attribute=" + PK_Attribute +
        " JOIN File ON Dest.FK_File=PK_File "\
        "JOIN Attribute ON Dest.FK_Attribute=PK_Attribute "\
        "JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "\
        "JOIN MediaType_AttributeType ON File.EK_MediaType=MediaType_AttributeType.EK_MediaType "\
        "AND MediaType_AttributeType.FK_AttributeType=Attribute.FK_AttributeType "\
        "WHERE Identifier>=2 ORDER BY AttributeType.Description limit 30;";

    PlutoSqlResult result;
    DB_ROW row;
    int RowCount=0;

    if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) )
    {
LoggerWrapper::GetInstance()->Write(LV_STATUS, "Transformed 2 PK_Attributte: %s", PK_Attribute.c_str());
        while( ( row=db_wrapper_fetch_row( result.r ) ) )
        {
LoggerWrapper::GetInstance()->Write(LV_STATUS, "Transformed 4 PK_Attributte: %s", PK_Attribute.c_str());
            string label = row[1];
            label += string( "\n" ) + row[2];
            pCell = new DataGridCell( label, row[0] );
            pDataGrid->SetData( 0, RowCount++, pCell );
        }
    }
LoggerWrapper::GetInstance()->Write(LV_STATUS, "Transformed 3 PK_Attributte: %s", PK_Attribute.c_str());

    size_t length=0;
    char *buffer=NULL;
    if( PK_Picture )
        buffer = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(PK_Picture) + "." + Extension,length);

	pos = 0;
    StringUtils::Tokenize(GridID, "_", pos);
    string controller = StringUtils::Tokenize(GridID, "_", pos);

    DCE::CMD_Update_Object_Image CMD_Update_Object_Image(m_dwPK_Device, atoi(controller.c_str()),
        StringUtils::itos(DESIGNOBJ_objCDCover_CONST),"1",buffer,(int) length,"");
    SendCommand(CMD_Update_Object_Image);

	return pDataGrid;
}

class DataGridTable *Media_Plugin::MediaItemAttr( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    DataGridTable *pDataGrid = new DataGridTable();
    DataGridCell *pCell;

    string SQL;

    int PK_File=0;
    string PK_Attribute = Parms;
    if( PK_Attribute.length()==0 )
        return NULL;
    if( PK_Attribute.substr(0,2)=="!F" )
        PK_File = atoi(PK_Attribute.substr(2).c_str());
    else
    {
        if( PK_Attribute.substr(0,2)=="!A" )
            PK_Attribute = PK_Attribute.substr(2);
//
        PK_File = m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetFileIDFromAttributeID(atoi(PK_Attribute.c_str()));

        if( !PK_File )
        {
            // They chose an attribute other than a file, like an album
            SQL = "select DISTINCT PK_Attribute,Description,Name FROM File_Attribute AS Source "\
                "JOIN File_Attribute As Dest ON Source.FK_File=Dest.FK_File "\
                "JOIN Attribute ON Dest.FK_Attribute=PK_Attribute  "\
                "JOIN C_AttributeType ON Attribute.FK_C_AttributeType=PK_C_AttributeType  "\
                "JOIN File ON Dest.FK_File=PK_File "\
                "JOIN C_Type_C_AttributeType ON C_Type_C_AttributeType.FK_C_Type=File.FK_C_Type "\
                "AND C_Type_C_AttributeType.FK_C_AttributeType=Attribute.FK_C_AttributeType "\
                "WHERE Identifier>2 AND Source.FK_Attribute=" + StringUtils::itos(atoi(PK_Attribute.c_str())) + " ORDER BY Description";
        }
    }

    string::size_type pos = 0;
    StringUtils::Tokenize(GridID, "_", pos);
    string controller = StringUtils::Tokenize(GridID, "_", pos);

    int PK_Picture;
    string Extension;
    if( PK_File )
        Extension = m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetPictureFromFileID(PK_File,&PK_Picture);
    else
        Extension = m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetPictureFromAttributeID(atoi(PK_Attribute.c_str()),&PK_Picture);

    size_t length=0;
    char *buffer=NULL;
    if( PK_Picture )
        buffer = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(PK_Picture) + "." + Extension,length);

    DCE::CMD_Update_Object_Image CMD_Update_Object_Image(m_dwPK_Device, atoi(controller.c_str()),
        StringUtils::itos(DESIGNOBJ_objCDCover_CONST),"1",buffer,(int) length,"");
    SendCommand(CMD_Update_Object_Image);

    // We may have already built the sql statement
    if( SQL.length()==0 )
    {
        SQL="SELECT PK_Attribute,Description,Name FROM File "\
        "JOIN File_Attribute ON FK_File=PK_File "\
        "JOIN Attribute ON FK_Attribute=PK_Attribute "\
        "JOIN C_AttributeType ON FK_C_AttributeType=PK_C_AttributeType "\
        "WHERE PK_File=" + StringUtils::itos(PK_File) + " ORDER By Description";
    }

    PlutoSqlResult result;
    DB_ROW row;
    int RowCount=0;

    if( (result.r=m_pDatabase_pluto_media->db_wrapper_query_result(SQL)) )
    {
        while( (row=db_wrapper_fetch_row(result.r)) )
        {
            string label = row[1];
            label += ": ";
            label += row[2];
            pCell = new DataGridCell(label,row[0]);
            pDataGrid->SetData(0,RowCount++,pCell);
        }
    }
    return pDataGrid;
}

class DataGridTable *Media_Plugin::DevicesPipes( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    DataGridTable *pDataGrid = new DataGridTable();
    //DataGridCell *pCell;

	EntertainArea *pEntertainArea = m_mapEntertainAreas_Find( atoi(Parms.c_str()) );
	if( !pEntertainArea || !pEntertainArea->m_pMediaDevice_ActiveDest )
		return pDataGrid;

	int iRow=0;
	DevicesPipes_Loop( pMessage->m_dwPK_Device_From, pEntertainArea->m_pMediaDevice_ActiveDest->m_pDeviceData_Router,
		pDataGrid,iRow);
	return pDataGrid;
}


void Media_Plugin::DevicesPipes_Loop(int PK_Orbiter,DeviceData_Router *pDevice,DataGridTable *&pDataGrid,int &iRow,Command *pCommand_Input,Command *pCommand_Output,vector<int> *p_vectDevice)
{
	if( !pDevice )
		return;

	bool bCreatedVect=false;
	if( !p_vectDevice )
	{
		p_vectDevice = new vector<int>;
		bCreatedVect=true;
	}

	// Be sure we skip over recursive entries
	for(size_t s=0;s<p_vectDevice->size();++s)
		if( (*p_vectDevice)[s]==pDevice->m_dwPK_Device )
			return;
	p_vectDevice->push_back(pDevice->m_dwPK_Device);

	for(map<int, class Pipe *>::iterator it=pDevice->m_mapPipe_Active.begin();
		it!=pDevice->m_mapPipe_Active.end();++it)
	{
		Pipe *pPipe = (*it).second;
		DeviceData_Router *pDevice_Pipe = m_pRouter->m_mapDeviceData_Router_Find(pPipe->m_pDevice_To->m_dwPK_Device);
		if( pDevice_Pipe && pDevice_Pipe!=pDevice )
			DevicesPipes_Loop(PK_Orbiter,(DeviceData_Router *)pDevice_Pipe,pDataGrid,iRow,
				pPipe->m_pCommand_Input,pPipe->m_pCommand_Output,p_vectDevice);
	}

	if( pDevice->m_pDevice_MD && pDevice!=pDevice->m_pDevice_MD )
		DevicesPipes_Loop(PK_Orbiter,(DeviceData_Router *) pDevice->m_pDevice_MD,pDataGrid,iRow,0,0,p_vectDevice);

	DataGridCell *pCell = new DataGridCell( pDevice->m_sDescription);
	pCell->m_Colspan = 4;
	pCell->m_pMessage = new Message(m_dwPK_Device,pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Generic_On_CONST,2,COMMANDPARAMETER_Retransmit_CONST,"1",COMMANDPARAMETER_PK_Pipe_CONST,"-1");
	pDataGrid->SetData(0,iRow,pCell);

	if( pCommand_Input )
	{
		pCell = new DataGridCell( pCommand_Input->m_sDescription );
		pCell->m_pMessage = new Message(m_dwPK_Device,pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Input_Select_CONST,2,COMMANDPARAMETER_Retransmit_CONST,"1",COMMANDPARAMETER_PK_Command_Input_CONST,StringUtils::itos(pCommand_Input->m_dwPK_Command).c_str());
		pDataGrid->SetData(4,iRow,pCell);
	}
	if( pCommand_Output )
	{
		pCell = new DataGridCell( pCommand_Output->m_sDescription );
		pCell->m_pMessage = new Message(m_dwPK_Device,pDevice->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Output_Select_CONST,2,COMMANDPARAMETER_Retransmit_CONST,"1",COMMANDPARAMETER_PK_Command_Output_CONST,StringUtils::itos(pCommand_Output->m_dwPK_Command).c_str());
		pDataGrid->SetData(5,iRow,pCell);
	}

	pCell = new DataGridCell( "Advanced" );
    pCell->m_pMessage = new Message(m_dwPK_Device,PK_Orbiter,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Goto_DesignObj_CONST,
		1,COMMANDPARAMETER_PK_DesignObj_CONST,StringUtils::itos(DESIGNOBJ_mnuDeviceControl_CONST).c_str());
	pCell->m_pMessage->m_vectExtraMessages.push_back(
		new Message(m_dwPK_Device,PK_Orbiter,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_Set_Variable_CONST,2,
		COMMANDPARAMETER_PK_Variable_CONST,StringUtils::itos(VARIABLE_Datagrid_Input_CONST).c_str(),
		COMMANDPARAMETER_Value_To_Assign_CONST,StringUtils::itos(pDevice->m_dwPK_Device).c_str())
	);

	pDataGrid->SetData(6,iRow++,pCell);

	if( bCreatedVect )
		delete p_vectDevice;
}

class DataGridTable *Media_Plugin::MediaAttrFile( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	// Given a file, return the media attributes for it
    DataGridTable *pDataGrid = new DataGridTable();

	if( Parms.size()<3 || Parms[0]!='!' || (Parms[1]!='F' && Parms[1]!='r') )
		return pDataGrid; // This shouldn't happen

	string sDiscOrFile = Parms[1]=='F' ? "File" : "Disc";
	string sSQL = "SELECT DISTINCT PK_Attribute,Name,Description FROM " + sDiscOrFile + " "
		"JOIN " + sDiscOrFile + "_Attribute ON FK_" + sDiscOrFile + "=PK_" + sDiscOrFile + " "
		"JOIN Attribute ON FK_Attribute=PK_Attribute "
		"JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "
		"LEFT JOIN MediaType_AttributeType ON MediaType_AttributeType.FK_AttributeType = PK_AttributeType AND MediaType_AttributeType.EK_MediaType=" + sDiscOrFile + ".EK_MediaType "
		"WHERE FK_" + sDiscOrFile + "=" + Parms.substr(2) +
		" AND Identifier>0 "
		" ORDER BY MediaSortOption IS NULL,MediaSortOption";

	int iRow=0;
	list<DataGridCell *> listCellsAtBottom; // All the ones with mediasort=NULL; mysql puts them at the top, not the bottom
	DataGridCell *pCell;
	PlutoSqlResult result;
    DB_ROW row;
	if( (result.r=m_pDatabase_pluto_media->db_wrapper_query_result( sSQL )) )
	{
        while( ( row=db_wrapper_fetch_row( result.r ) ) )
		{
			pCell = new DataGridCell("","!A" + string(row[0]));
			pCell->m_mapAttributes["Title"]=row[2];
			pCell->m_mapAttributes["Name"]=row[1];
			pDataGrid->SetData(0,iRow++,pCell);
		}
	}

	return pDataGrid;
}

class DataGridTable *Media_Plugin::MediaAttrCurStream( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    DataGridTable *pDataGrid = new DataGridTable();
    //DataGridCell *pCell;

	EntertainArea *pEntertainArea = m_mapEntertainAreas_Find( atoi(Parms.c_str()) );
	if( !pEntertainArea || !pEntertainArea->m_pMediaStream )
		return pDataGrid;

	int iRow=0;
	for(map<int,list_int>::iterator it=pEntertainArea->m_pMediaStream->m_mapPK_Attribute.begin();it!=pEntertainArea->m_pMediaStream->m_mapPK_Attribute.end();++it)
	{
		Row_AttributeType *pRow_AttributeType = m_pDatabase_pluto_media->AttributeType_get()->GetRow(it->first);
		for(list_int::iterator itli=it->second.begin();itli!=it->second.end();++itli)
		{
			Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(*itli);
			if( pRow_AttributeType && pRow_Attribute )
			{
				string sName = pRow_AttributeType->Description_get() + ": ";
				sName += pRow_Attribute->Name_get();
				DataGridCell *pCell = new DataGridCell( sName, StringUtils::itos(*itli) );
				pDataGrid->SetData(0,iRow++,pCell);
			}
		}
	}

	MediaFile *pMediaFile = pEntertainArea->m_pMediaStream->GetCurrentMediaFile();
	if( pMediaFile )
	{
		string sNamePrefix;
		/* Why would the user want the filename again??
		if( pMediaFile->m_dwPK_File )
			sNamePrefix = "(" + pMediaFile->m_sFilename + ") ";  // It's a real file
		else
			sNamePrefix = "(#" + StringUtils::itos(pEntertainArea->m_pMediaStream->m_iDequeMediaFile_Pos+1) + ") ";
		*/

		for( map<int,list_int>::iterator it=pMediaFile->m_mapPK_Attribute.begin();it!=pMediaFile->m_mapPK_Attribute.end();++it)
		{
			Row_AttributeType *pRow_AttributeType = m_pDatabase_pluto_media->AttributeType_get()->GetRow(it->first);
			for(list_int::iterator itli=it->second.begin();itli!=it->second.end();++itli)
			{
				Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(*itli);
				if( pRow_AttributeType && pRow_Attribute )
				{
					string sName = sNamePrefix + pRow_AttributeType->Description_get() + "\n" + pRow_Attribute->Name_get();
					DataGridCell *pCell = new DataGridCell( sName, StringUtils::itos(*itli) );
					pDataGrid->SetData(0,iRow++,pCell);
				}
			}
		}
		AddMediaSectionToDataGrid(pDataGrid,iRow,pMediaFile->m_dequeMediaSection,sNamePrefix);
		AddMediaTitlesToDataGrid(pDataGrid,iRow,pMediaFile->m_dequeMediaTitle,sNamePrefix);
	}

	AddMediaTitlesToDataGrid(pDataGrid,iRow,pEntertainArea->m_pMediaStream->m_dequeMediaTitle,"");

	return pDataGrid;
}

void Media_Plugin::AddMediaTitlesToDataGrid(DataGridTable *pDataGrid,int &iRow,deque<MediaTitle *> &dequeMediaTitle,string sPreface,bool bThumbnail)
{
	for(size_t s=0;s<dequeMediaTitle.size();++s)
	{
		MediaTitle *pMediaTitle = dequeMediaTitle[s];
		string sNamePrefix = "(Title " + StringUtils::itos(s+1) + ") ";

		for( map<int,list_int>::iterator it=pMediaTitle->m_mapPK_Attribute.begin();it!=pMediaTitle->m_mapPK_Attribute.end();++it)
		{
			Row_AttributeType *pRow_AttributeType = m_pDatabase_pluto_media->AttributeType_get()->GetRow(it->first);
			for(list_int::iterator itli=it->second.begin();itli!=it->second.end();++itli)
			{
				Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(*itli);
				if( pRow_AttributeType && pRow_Attribute && (bThumbnail==false || CanThumbnail(pRow_AttributeType->PK_AttributeType_get())) )
				{
					string sName = sNamePrefix + pRow_AttributeType->Description_get() + "\n" + pRow_Attribute->Name_get();
					DataGridCell *pCell = new DataGridCell( sName, "!A" + StringUtils::itos(*itli) );
					pDataGrid->SetData(0,iRow++,pCell);
				}
			}
		}
		AddMediaSectionToDataGrid(pDataGrid,iRow,pMediaTitle->m_dequeMediaSection,sNamePrefix,bThumbnail);
	}
}

void Media_Plugin::AddMediaSectionToDataGrid(DataGridTable *pDataGrid,int &iRow,deque<MediaSection *> &dequeMediaSection,string sPreface,bool bThumbnail)
{
	for(size_t s=0;s<dequeMediaSection.size();++s)
	{
		MediaSection *pMediaSection = dequeMediaSection[s];

		for( map<int,list_int>::iterator it=pMediaSection->m_mapPK_Attribute.begin();it!=pMediaSection->m_mapPK_Attribute.end();++it)
		{
			Row_AttributeType *pRow_AttributeType = m_pDatabase_pluto_media->AttributeType_get()->GetRow(it->first);
			for(list_int::iterator itli=it->second.begin();itli!=it->second.end();++itli)
			{
				Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(*itli);
				if( pRow_AttributeType && pRow_Attribute && (bThumbnail==false || CanThumbnail(pRow_AttributeType->PK_AttributeType_get())) )
				{
					string sName = sPreface + " #" + StringUtils::itos(s+1) + " ";
					sName += pRow_AttributeType->Description_get() + "\n" + pRow_Attribute->Name_get();
					DataGridCell *pCell = new DataGridCell( sName, "!A" + StringUtils::itos(*itli) );
					pDataGrid->SetData(0,iRow++,pCell);
				}
			}
		}
	}
}


class DataGridTable *Media_Plugin::FloorplanMediaChoices( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    DataGridTable *pDataGrid = new DataGridTable();

    EntertainArea *pEntertainArea = m_mapEntertainAreas_Find(atoi(Parms.c_str()));
    if( !pEntertainArea )
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find entertainment area for flooplan");
        return new DataGridTable();
    }

    string sWhereClause = string("JOIN CommandGroup_Command ON CommandGroup_Command.FK_CommandGroup=PK_CommandGroup ") +
        "JOIN CommandGroup_EntertainArea ON CommandGroup_EntertainArea.FK_CommandGroup=PK_CommandGroup " +
        "WHERE FK_EntertainArea=" + StringUtils::itos(pEntertainArea->m_iPK_EntertainArea) + " AND FK_Command=" + StringUtils::itos(COMMAND_MH_Play_Media_CONST);
    vector<Row_CommandGroup *> vectRow_CommandGroup;
    m_pDatabase_pluto_main->CommandGroup_get()->GetRows(sWhereClause,&vectRow_CommandGroup);

    for(size_t s=0;s<vectRow_CommandGroup.size();++s)
    {
        Row_CommandGroup *pRow_CommandGroup = vectRow_CommandGroup[s];
        DataGridCell *pDataGridCell = new DataGridCell(pRow_CommandGroup->Description_get(),StringUtils::itos(pRow_CommandGroup->PK_CommandGroup_get()));
        pDataGrid->SetData(0,s,pDataGridCell);
    }

    return pDataGrid;
}

class DataGridTable *Media_Plugin::ActiveMediaStreams( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ready to populate active media stream...");
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    DataGridTable *pDataGrid = new DataGridTable();

    int iRow=0;
    for(MapMediaStream::iterator it=m_mapMediaStream.begin();it!=m_mapMediaStream.end();++it)
    {
        MediaStream *pMediaStream = (*it).second;
		bool bPrivate=false;
		for( map<int,class EntertainArea *>::iterator itEntAreas=pMediaStream->m_mapEntertainArea.begin(); itEntAreas != pMediaStream->m_mapEntertainArea.end(); itEntAreas++)
		{
			if( itEntAreas->second->m_bMediaIsPrivate )
			{
				bPrivate=true;
				continue;
			}
		}
		if( bPrivate )
			continue;
        pMediaStream->m_iOrder = iRow;
        DataGridCell *pDataGridCell = new DataGridCell(pMediaStream->m_sMediaDescription,StringUtils::itos(pMediaStream->m_iStreamID_get()));
        pDataGridCell->m_AltColor = UniqueColors[iRow];
        pDataGrid->SetData(0,iRow++,pDataGridCell);
    }
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Done with active media streams...");
    return pDataGrid;
}

class DataGridTable *Media_Plugin::AvailablePlaylists( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    //int nWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
    int nHeight = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Height_CONST].c_str());
	
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::AvailablePlaylists Called to populate: %s", Parms.c_str());
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

    if( Parms.length( )==0 )
        return NULL; // Nothing passed in yet

    // select PK_Playlist, Name from Playlist where EK_USER IN ( 0, 33128 ) LIMIT 30;;

    int userID = atoi(Parms.c_str());

    string SQL = "SELECT PK_Playlist, Name from Playlist where EK_User IS NULL OR EK_User IN ( 0, " + StringUtils::itos(userID) + " ) ORDER BY NAME LIMIT 30";

    PlutoSqlResult result;
    DB_ROW row;
    int RowCount=0;

    DataGridTable *pDataGrid = new DataGridTable();
    if( (result.r=m_pDatabase_pluto_media->db_wrapper_query_result(SQL)) )
        while( (row=db_wrapper_fetch_row(result.r)) )
		{
            // 	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Adding this entry \"%s\" to the position %d", row[1], RowCount);
			if( nHeight==1 ) // It's a horizontal list
				pDataGrid->SetData(RowCount++,0,new DataGridCell(row[1], row[0]));
			else
				pDataGrid->SetData(0,RowCount++,new DataGridCell(row[1], row[0]));
		}

    return pDataGrid;
}

class DataGridTable *Media_Plugin::DVDSubtitles( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    DataGridTable *pDataGrid = new DataGridTable();

	vector<EntertainArea *> vectEntertainArea;
    DetermineEntArea( pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea );
	if( vectEntertainArea.size()==0 || vectEntertainArea[0]->m_pMediaStream==NULL )
		return pDataGrid;
	MediaStream *pMediaStream = vectEntertainArea[0]->m_pMediaStream;
	string Data = GetCurrentDeviceData( pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device, DEVICEDATA_Subtitles_CONST );
	string::size_type pos=0;
	int iCurrent = atoi(StringUtils::Tokenize(Data,"\n",pos).c_str());
	DataGridCell *pCell = new DataGridCell("Off", "-1");
	pDataGrid->SetData(0,0,pCell);
	for(int i=0;pos!=string::npos && pos<Data.size();++i)
	{
		DataGridCell *pCell = new DataGridCell( StringUtils::Tokenize(Data,"\n",pos), StringUtils::itos(i) );
		pDataGrid->SetData(0,i+1,pCell);
	}
	*iPK_Variable=VARIABLE_Misc_Data_1_CONST;
	*sValue_To_Assign=StringUtils::itos(iCurrent);
	return pDataGrid;
}

class DataGridTable *Media_Plugin::DVDAudioTracks( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    DataGridTable *pDataGrid = new DataGridTable();

	vector<EntertainArea *> vectEntertainArea;
    DetermineEntArea( pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea );
	if( vectEntertainArea.size()==0 || vectEntertainArea[0]->m_pMediaStream==NULL )
		return pDataGrid;
	MediaStream *pMediaStream = vectEntertainArea[0]->m_pMediaStream;
	string Data = GetCurrentDeviceData( pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device, DEVICEDATA_Audio_Tracks_CONST );
	string::size_type pos=0;
	int iCurrent = atoi(StringUtils::Tokenize(Data,"\n",pos).c_str());
	for(int i=0;pos!=string::npos && pos<Data.size();++i)
	{
		DataGridCell *pCell = new DataGridCell( StringUtils::Tokenize(Data,"\n",pos), StringUtils::itos(i) );
		pDataGrid->SetData(0,i,pCell);
	}
	*iPK_Variable=VARIABLE_Misc_Data_2_CONST;
	*sValue_To_Assign=StringUtils::itos(iCurrent);
	return pDataGrid;
}

class DataGridTable *Media_Plugin::DVDAngles( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    DataGridTable *pDataGrid = new DataGridTable();

	vector<EntertainArea *> vectEntertainArea;
    DetermineEntArea( pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea );
	if( vectEntertainArea.size()==0 || vectEntertainArea[0]->m_pMediaStream==NULL )
		return pDataGrid;
	MediaStream *pMediaStream = vectEntertainArea[0]->m_pMediaStream;
	string Data = GetCurrentDeviceData( pMediaStream->m_pMediaDevice_Source->m_pDeviceData_Router->m_dwPK_Device, DEVICEDATA_Angles_CONST );
	string::size_type pos=0;
	int iCurrent = atoi(StringUtils::Tokenize(Data,"\n",pos).c_str());
	for(int i=0;pos!=string::npos && pos<Data.size();++i)
	{
		DataGridCell *pCell = new DataGridCell( StringUtils::Tokenize(Data,"\n",pos), StringUtils::itos(i) );
		pDataGrid->SetData(0,i,pCell);
	}
	*iPK_Variable=VARIABLE_Misc_Data_3_CONST;
	*sValue_To_Assign=StringUtils::itos(iCurrent);
	return pDataGrid;
}

class DataGridTable *Media_Plugin::Bookmarks( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    int nWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());

    DataGridTable *pDataGrid = new DataGridTable();
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::Bookmarks Called to populate: %s", Parms.c_str());
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	// If this is called to browse the bookmarks within the currently playing file, this will be called with
	// MediaType=0 and we will assign the EntArea.  Otherwise the MediaType and current user will be passed in
	EntertainArea *pEntertainArea=NULL;
	string::size_type pos=0;
	int PK_MediaType = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
	int PK_Users = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());

	string sWhere;
	if( PK_MediaType )
	{
		// We're browsing all files of a type, not bookmars for a particular file or disc
		string sTypes;
		if( PK_MediaType==MEDIATYPE_pluto_StoredVideo_CONST )
			sTypes = TOSTRING(MEDIATYPE_pluto_StoredVideo_CONST) "," TOSTRING(MEDIATYPE_pluto_DVD_CONST);
		else if( PK_MediaType==MEDIATYPE_pluto_StoredAudio_CONST )
			sTypes = TOSTRING(MEDIATYPE_pluto_StoredAudio_CONST) "," TOSTRING(MEDIATYPE_pluto_CD_CONST);
		else
			sTypes = StringUtils::itos(PK_MediaType);

		sWhere = "EK_MediaType IN (" + sTypes + ") AND FK_File IS NOT NULL AND (EK_Users IS NULL OR EK_Users="+StringUtils::itos(PK_Users)+")";
	}
	else
	{
		vector<EntertainArea *> vectEntertainArea;
		// Only an Orbiter will tell us to play media
		DetermineEntArea( pMessage->m_dwPK_Device_From, 0, "", vectEntertainArea );
		if( vectEntertainArea.size() )
		{
			pEntertainArea = vectEntertainArea[0];
			if( pEntertainArea->m_pMediaStream )
			{
				if( pEntertainArea->m_pMediaStream->m_dwPK_Disc )
					sWhere = "FK_Disc=" + StringUtils::itos(pEntertainArea->m_pMediaStream->m_dwPK_Disc) + " AND (EK_Users IS NULL OR EK_Users="+StringUtils::itos(PK_Users)+")";
				else if( pEntertainArea->m_pMediaStream->m_dequeMediaFile.size() &&
					pEntertainArea->m_pMediaStream->m_iDequeMediaFile_Pos>=0 &&
					pEntertainArea->m_pMediaStream->m_iDequeMediaFile_Pos<pEntertainArea->m_pMediaStream->m_dequeMediaFile.size() )
				{
					MediaFile *pMediaFile = pEntertainArea->m_pMediaStream->GetCurrentMediaFile();
					sWhere = "FK_File=" + StringUtils::itos(pMediaFile->m_dwPK_File) + " AND (EK_Users IS NULL OR EK_Users="+StringUtils::itos(PK_Users)+")";
				}
			}
		}
	}

	if( sWhere.size()==0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Media_Plugin::Bookmarks no where clause %s",Parms.c_str());
		return pDataGrid;
	}

	vector<Row_Bookmark *> vectRow_Bookmark;
	m_pDatabase_pluto_media->Bookmark_get()->GetRows(sWhere,&vectRow_Bookmark);

	for(size_t s=0;s<vectRow_Bookmark.size();++s)
	{
		Row_Bookmark *pRow_Bookmark = vectRow_Bookmark[s];

        if(nWidth > 1)
        {
            DataGridCell *pDataGridCell,*pDataGridCell_Cover,*pDataGridCell_Preview;
            pDataGridCell = new DataGridCell(pRow_Bookmark->Description_get(), StringUtils::itos(pRow_Bookmark->PK_Bookmark_get()));
            pDataGridCell_Cover = new DataGridCell("", StringUtils::itos(pRow_Bookmark->PK_Bookmark_get()));
            pDataGridCell_Preview = new DataGridCell("", StringUtils::itos(pRow_Bookmark->PK_Bookmark_get()));

            pDataGrid->SetData(0,s,pDataGridCell_Cover);
            pDataGrid->SetData(1,s,pDataGridCell_Preview);
            pDataGrid->SetData(2,s,pDataGridCell);

            Row_Picture *pRow_Picture=NULL;
            if( (pRow_Picture=pRow_Bookmark->FK_Picture_getrow())!=NULL )
            {
                size_t iSize;
                char *pBuffer = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(pRow_Picture->PK_Picture_get()) + "_tn." + pRow_Picture->Extension_get(),iSize);
                if( !pBuffer )
	                pBuffer = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(pRow_Picture->PK_Picture_get()) + "." + pRow_Picture->Extension_get(),iSize);
                if( pBuffer )
                {
                    pDataGridCell_Preview->m_pGraphicData = pBuffer;
                    pDataGridCell_Preview->m_GraphicLength = iSize;
                }
            }

			int PK_Picture = 0;
            Row_File *pRow_File = pRow_Bookmark->FK_File_getrow();

			if(pRow_File)
			{
				string sFilePath = pRow_File->Path_get();
				string sFileName = pRow_File->Filename_get();
				m_pMediaAttributes->m_pMediaAttributes_LowLevel->GetPictureFromFileID(pRow_File->PK_File_get(),&PK_Picture);

				if(PK_Picture > 0)
				{
					size_t iSize;
					char *pBuffer = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(PK_Picture) + "_tn.jpg",iSize);
					if( !pBuffer )
						pBuffer = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(PK_Picture) + ".jpg",iSize);
					if( pBuffer )
					{
						pDataGridCell_Cover->m_pGraphicData = pBuffer;
						pDataGridCell_Cover->m_GraphicLength = iSize;
					}
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"pic file 2 %p",pBuffer);
				}
				else
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"File %s pic %d",(pRow_File->Path_get() + "/" + pRow_File->Filename_get()).c_str( ),PK_Picture);
			}
        }
        else
        {
            Row_File *pRow_File = pRow_Bookmark->FK_File_getrow();
            string sItem;
			if( pRow_File )
				sItem = pRow_File->Filename_get() +  
	                "\n" + pRow_Bookmark->Description_get();
			else
				sItem = pRow_Bookmark->Description_get();

			DataGridCell *pDataGridCell = new DataGridCell(sItem, 
                StringUtils::itos(pRow_Bookmark->PK_Bookmark_get()));
            pDataGrid->SetData(0,s,pDataGridCell);
        }
	}

    return pDataGrid;
}

class DataGridTable *Media_Plugin::BookmarksByMediaType( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	DataGridTable *pDataGrid = new DataGridTable();

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::BookmarksByMediaType Called to populate: %s", Parms.c_str());
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
	string::size_type pos=0;
	int PK_MediaType = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
	int PK_Users = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());
	int iMaxColumns = atoi(StringUtils::Tokenize(Parms,",",pos).c_str());

	string sWhere = "EK_MediaType=" + StringUtils::itos(PK_MediaType) + " AND (EK_Users IS NULL OR EK_Users="+StringUtils::itos(PK_Users)+")";
	vector<Row_Bookmark *> vectRow_Bookmark;
	m_pDatabase_pluto_media->Bookmark_get()->GetRows(sWhere,&vectRow_Bookmark);
	int iRow=0,iColumn=0;

	for(size_t s=0;s<vectRow_Bookmark.size();++s)
	{
		Row_Bookmark *pRow_Bookmark = vectRow_Bookmark[s];
		DataGridCell *pDataGridCell = new DataGridCell("", StringUtils::itos(pRow_Bookmark->PK_Bookmark_get()));
		Row_Picture *pRow_Picture=NULL;
		if( (pRow_Picture=pRow_Bookmark->FK_Picture_getrow())!=NULL )
		{
			size_t iSize;
			char *pBuffer = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(pRow_Picture->PK_Picture_get()) + "_tn." + pRow_Picture->Extension_get(),iSize);
			if( pBuffer )
			{
				pDataGridCell->m_pGraphicData = pBuffer;
				pDataGridCell->m_GraphicLength = iSize;
			}
		}
		pDataGridCell->m_Rowspan=2;
		pDataGrid->SetData(iColumn,iRow,pDataGridCell);
		pDataGridCell = new DataGridCell(pRow_Bookmark->Description_get(), StringUtils::itos(pRow_Bookmark->PK_Bookmark_get()));
		pDataGrid->SetData(iColumn,iRow+2,pDataGridCell);

		if( iColumn++ >= iMaxColumns-1 )
		{
			iColumn=0;
			iRow+=3;
		}
	}

	return pDataGrid;
}

class DataGridTable *Media_Plugin::CaptureCardAudioPorts( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::CaptureCardAudioPorts");

	PLUTO_SAFETY_LOCK(mm, m_MediaMutex);	// Just to be safe.
	
	// See what computer we're scanning for ports on.
	int PK_Device_Topmost = DatabaseUtils::GetTopMostDevice(m_pDatabase_pluto_main, pMessage->m_dwPK_Device_From);
	if ( PK_Device_Topmost == 0)
		return pDataGrid;	// Empty table, because we couldn't figure out which computer.

	vector <Row_Device *> vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows("JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory = " TOSTRING(DEVICECATEGORY_Capture_Card_Audio_Ports_CONST),&vectRow_Device);
	int iRow=0;
	for (vector<Row_Device *>::iterator it=vectRow_Device.begin();it!=vectRow_Device.end();++it)
	{
		Row_Device *pRow_Device = *it;
		int PK_Device_Topmost_Comp = DatabaseUtils::GetTopMostDevice(m_pDatabase_pluto_main,pRow_Device->PK_Device_get());
		if ( PK_Device_Topmost_Comp!=PK_Device_Topmost )
			continue;	// This device is on another computer.
		
		vector<Row_Device_DeviceData *> vectRow_Device_DeviceData;
		m_pDatabase_pluto_main->Device_DeviceData_get()->GetRows("IK_DeviceData=" + StringUtils::itos(pRow_Device->PK_Device_get()) + " AND FK_DeviceData=" TOSTRING(DEVICEDATA_FK_Device_Capture_Card_Audio_P_CONST),&vectRow_Device_DeviceData);
		
		string sDescription = pRow_Device->Description_get();
		Row_Device *pRow_Device_Parent = pRow_Device->FK_Device_ControlledVia_getrow();
		while( pRow_Device_Parent )
		  {
		    sDescription += " / " + pRow_Device_Parent->Description_get();
		    pRow_Device_Parent = pRow_Device_Parent->FK_Device_ControlledVia_getrow();
		  }

		pCell = new DataGridCell(sDescription,StringUtils::itos(pRow_Device->PK_Device_get()));
		if ( vectRow_Device_DeviceData.size() )
		  {
		    Row_Device_DeviceData *pRow_Device_DeviceData = (*vectRow_Device_DeviceData.begin());
		    Row_Device *pRow_Device = pRow_Device_DeviceData->FK_Device_getrow();
		    if ( pRow_Device )
		      {
			pCell->m_mapAttributes["InUse_PK_Device"] = StringUtils::itos(pRow_Device->PK_Device_get());
			pCell->m_mapAttributes["InUse_Device_Description"] = pRow_Device->Description_get();
		      }
		  }
		pDataGrid->SetData(0,iRow++,pCell);
	}

	return pDataGrid;

}

class DataGridTable *Media_Plugin::CaptureCardPorts( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::CaptureCardPorts");
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	// See what computer we're scanning for ports on
	int PK_Device_Topmost = DatabaseUtils::GetTopMostDevice(m_pDatabase_pluto_main,pMessage->m_dwPK_Device_From);
	if( PK_Device_Topmost==0 )
		return pDataGrid; // Shouldn't happen

	vector<Row_Device *> vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows("JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" TOSTRING(DEVICECATEGORY_Capture_Card_Ports_CONST),&vectRow_Device);
	int iRow=0;
	for(vector<Row_Device *>::iterator it=vectRow_Device.begin();it!=vectRow_Device.end();++it)
	{
		Row_Device *pRow_Device = *it;
		int PK_Device_Topmost_Comp = DatabaseUtils::GetTopMostDevice(m_pDatabase_pluto_main,pRow_Device->PK_Device_get());
		if( PK_Device_Topmost_Comp!=PK_Device_Topmost )
			continue;  // It's not a port on this computer

		vector<Row_Device_DeviceData *> vectRow_Device_DeviceData;
		m_pDatabase_pluto_main->Device_DeviceData_get()->GetRows( "IK_DeviceData=" + StringUtils::itos(pRow_Device->PK_Device_get()) + " AND FK_DeviceData=" TOSTRING(DEVICEDATA_FK_Device_Capture_Card_Port_CONST), &vectRow_Device_DeviceData );

		string sDescription = pRow_Device->Description_get();
		Row_Device *pRow_Device_Parent = pRow_Device->FK_Device_ControlledVia_getrow();
		while( pRow_Device_Parent )
		{
			sDescription += " / " + pRow_Device_Parent->Description_get();
			pRow_Device_Parent = pRow_Device_Parent->FK_Device_ControlledVia_getrow();
		}

		pCell = new DataGridCell(sDescription,StringUtils::itos(pRow_Device->PK_Device_get()));
		if( vectRow_Device_DeviceData.size() )
		{
			Row_Device_DeviceData *pRow_Device_DeviceData = (*vectRow_Device_DeviceData.begin());
			Row_Device *pRow_Device = pRow_Device_DeviceData->FK_Device_getrow();
			if( pRow_Device )
			{
				pCell->m_mapAttributes["InUse_PK_Device"] = StringUtils::itos(pRow_Device->PK_Device_get());
				pCell->m_mapAttributes["InUse_Device_Description"] = pRow_Device->Description_get();
			}
		}
		pDataGrid->SetData(0,iRow++,pCell);
	}

	return pDataGrid;
}
class DataGridTable *Media_Plugin::DevicesForCaptureCardPort( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::DevicesForCaptureCardPort");
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	Row_Device *pRow_Device_Requestor = m_pDatabase_pluto_main->Device_get()->GetRow(pMessage->m_dwPK_Device_From);
	if( !pRow_Device_Requestor )
		return NULL; // Shouldn't happen

	// a where clause to select all devices that are a/v or cameras in the current room first
	string sSQL = 
		"JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate "
		"JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory "
		"WHERE FK_DeviceCategory in ("
		TOSTRING(DEVICECATEGORY_AV_CONST) "," TOSTRING(DEVICECATEGORY_Surveillance_Cameras_CONST) 
		") OR FK_DeviceCategory_Parent IN ("
		TOSTRING(DEVICECATEGORY_AV_CONST) "," TOSTRING(DEVICECATEGORY_Surveillance_Cameras_CONST) 
		") "
		"ORDER BY FK_Room=" + StringUtils::itos(pRow_Device_Requestor->FK_Room_get()) + " DESC";

	vector<Row_Device *> vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows(sSQL,&vectRow_Device);
	int iRow=0;
	for(vector<Row_Device *>::iterator it=vectRow_Device.begin();it!=vectRow_Device.end();++it)
	{
		Row_Device *pRow_Device = *it;

		string sDescription = pRow_Device->Description_get();
		Row_Device *pRow_Device_Parent = pRow_Device->FK_Device_ControlledVia_getrow();
		while( pRow_Device_Parent )
		{
			sDescription += " / " + pRow_Device_Parent->Description_get();
			pRow_Device_Parent = pRow_Device_Parent->FK_Device_ControlledVia_getrow();
		}


		pCell = new DataGridCell(sDescription,StringUtils::itos(pRow_Device->PK_Device_get()));
		pDataGrid->SetData(0,iRow++,pCell);
	}

	return pDataGrid;
}

class DataGridTable *Media_Plugin::DevicesNeedingProviders( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	m_pRouter->m_pRow_Installation_get()->Reload();  // Be sure we have the country in case the user just specified it
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::DevicesNeedingProviders");
	Row_Device *pRow_Device_From = m_pDatabase_pluto_main->Device_get()->GetRow(pMessage->m_dwPK_Device_From);
	if( !pRow_Device_From )
		return NULL; // Shouldn't happen
	int PK_Device_Topmost = DatabaseUtils::GetTopMostDevice(m_pDatabase_pluto_main,pMessage->m_dwPK_Device_From);
	if( PK_Device_Topmost==0 )
		return pDataGrid; // Shouldn't happen

	// Get a list of all devices which have media of the type live tv
	int iRow=0;
	string sSQL = "SELECT PK_Device,FK_MediaType,PK_DeviceTemplate_MediaType FROM Device JOIN DeviceTemplate_MediaType ON DeviceTemplate_MediaType.FK_DeviceTemplate = Device.FK_DeviceTemplate and FK_MediaType IN (" TOSTRING(MEDIATYPE_np_LiveTV_CONST) "," TOSTRING(MEDIATYPE_pluto_LiveTV_CONST) ") ";
    PlutoSqlResult result;
    DB_ROW row;
	if( ( result.r=m_pDatabase_pluto_main->db_wrapper_query_result(sSQL)) )
	{
        while( ( row=db_wrapper_fetch_row( result.r ) ) )
		{
			Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow( atoi(row[0]) );
			if( !pRow_Device )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Media_Plugin::DevicesNeedingProviders can't find row_device for %s", row[0]);
				continue; // Shouldn't happen
			}

			if( pRow_Device->Disabled_get()==1 || pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Media_Players_CONST 
				|| DatabaseUtils::DeviceIsWithinCategory(m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICECATEGORY_Orbiter_CONST) )
			{
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::DevicesNeedingProviders skipping embedded %s/%d/%d", 
						row[0], (int) pRow_Device->Disabled_get(), pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get());
					continue; // Skip the internal sources, and orbiters which use this table for another purpose
			}

			if( pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_PVR_CONST || 
				pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Cable_Boxes_CONST || 
				pRow_Device->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_Satellite_Boxes_CONST )
			{
				Row_Device *pRow_Device_CaptureCard=NULL;
				// If this is a cable/sat box, then we only need a provider if it's connected to a capture card.  otherwise there's no way to get the video into the system anyway
				Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_FK_Device_Capture_Card_Port_CONST);
				if( pRow_Device_DeviceData )
					pRow_Device_CaptureCard	= m_pDatabase_pluto_main->Device_get()->GetRow( atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str()) );
				if( pRow_Device_CaptureCard == NULL )
				{
					LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::DevicesNeedingProviders skipping box %d because no capt card",pRow_Device->PK_Device_get());
					continue;
				}
			}

			string sDescription = pRow_Device->Description_get();
			Row_Device *pRow_Device_Parent = pRow_Device->FK_Device_ControlledVia_getrow();
			if( pRow_Device_Parent && pRow_Device_Parent->Disabled_get()==1 )
			{
				Row_DeviceTemplate *pRow_DeviceTemplate = pRow_Device_Parent->FK_DeviceTemplate_getrow();
				if( pRow_DeviceTemplate && pRow_DeviceTemplate->FK_DeviceCategory_get()!=DEVICECATEGORY_Infrared_Interface_CONST && pRow_DeviceTemplate->FK_DeviceCategory_get()!=DEVICECATEGORY_Infrared_Receivers_CONST )
					continue;
			}
			while( pRow_Device_Parent )
			{
				sDescription += " / " + pRow_Device_Parent->Description_get();
				pRow_Device_Parent = pRow_Device_Parent->FK_Device_ControlledVia_getrow();
			}

			int PK_Device_Topmost_Comp = DatabaseUtils::GetTopMostDevice(m_pDatabase_pluto_main,pRow_Device->PK_Device_get());
			if( PK_Device_Topmost_Comp!=PK_Device_Topmost && pRow_Device->FK_Room_get()!=pRow_Device_From->FK_Room_get() )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Media_Plugin::DevicesNeedingProviders skipping diff room %d %d/%d, %d/%d",
					pRow_Device->PK_Device_get(), PK_Device_Topmost_Comp, PK_Device_Topmost, pRow_Device->FK_Room_get(), pRow_Device_From->FK_Room_get());
				continue;  // It's not a port on this computer, and it's not in the same room
			}


			pCell = new DataGridCell(sDescription,StringUtils::itos(pRow_Device->PK_Device_get()));
			pDataGrid->SetData(0,iRow++,pCell);

			Row_Device_DeviceData *pRow_Device_DeviceData_VideoStandard = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_Type_CONST);
			pCell->m_mapAttributes["PK_DeviceTemplate_MediaType"] = row[2];

			Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device->PK_Device_get(),DEVICEDATA_EK_MediaProvider_CONST);
			Row_MediaProvider *pRow_MediaProvider = NULL;
			if( pRow_Device_DeviceData )
			{
				pRow_Device_DeviceData->Reload();
				pRow_MediaProvider = m_pDatabase_pluto_media->MediaProvider_get()->GetRow(atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str()));
			}
			if( pRow_MediaProvider )
			{
				pCell->m_mapAttributes["PK_MediaProvider"] = StringUtils::itos(pRow_MediaProvider->PK_MediaProvider_get());
				Row_ProviderSource *pRow_ProviderSource = pRow_MediaProvider->FK_ProviderSource_getrow();
				if( pRow_ProviderSource )
					pCell->m_mapAttributes["MediaProvider_Selected"] = pRow_ProviderSource->Description_get() + " " + pRow_MediaProvider->Description_get();
				else if( pRow_Device_DeviceData_VideoStandard )
				{
					pRow_Device_DeviceData_VideoStandard->Reload();
					pCell->m_mapAttributes["MediaProvider_Selected"] = pRow_Device_DeviceData_VideoStandard->IK_DeviceData_get();
				}
			}
			else if( pRow_Device_DeviceData_VideoStandard )
			{
				pRow_Device_DeviceData_VideoStandard->Reload();
				pCell->m_mapAttributes["MediaProvider_Selected"] = pRow_Device_DeviceData_VideoStandard->IK_DeviceData_get();
			}

			// See what we have as the media providers for this type
			vector<Row_ProviderSource *> vectRow_ProviderSource;
			m_pDatabase_pluto_media->ProviderSource_get()->GetRows(
				"EK_MediaType=" + string(row[1]) + " AND EK_Country=" + StringUtils::itos( m_pRouter->m_pRow_Installation_get()->FK_Country_get() ),
				&vectRow_ProviderSource);
		
			if( vectRow_ProviderSource.size()==0 )
				pCell->m_mapAttributes["Description"]="None";
			else if( vectRow_ProviderSource.size()==1 )  // There's only 1 to choose from.  Pass the info
			{
				Row_ProviderSource *pRow_ProviderSource = vectRow_ProviderSource[0];
				pCell->m_mapAttributes["PK_ProviderSource"] = StringUtils::itos(pRow_ProviderSource->PK_ProviderSource_get());
				pCell->m_mapAttributes["Description"]=pRow_ProviderSource->Description_get();
				pCell->m_mapAttributes["Comments"]=pRow_ProviderSource->Comments_get();
				pCell->m_mapAttributes["UserNamePassword"]=(pRow_ProviderSource->UserNamePassword_get() ? "1" : "0");
				pCell->m_mapAttributes["ProviderCommandLine"]=pRow_ProviderSource->ProviderCommandLine_get();
				pCell->m_mapAttributes["DeviceCommandLine"]=pRow_ProviderSource->DeviceCommandLine_get();
				pCell->m_mapAttributes["PackageCommandLine"]=pRow_ProviderSource->PackageCommandLine_get();
				pCell->m_mapAttributes["LineupCommandLine"]=pRow_ProviderSource->LineupCommandLine_get();
			}			
		}
	}
	return pDataGrid;
}

class DataGridTable *Media_Plugin::ProvidersForDevice( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	return NULL;
}

class DataGridTable *Media_Plugin::ThumbnailableAttributes( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    PLUTO_SAFETY_LOCK( mm, m_MediaMutex );
    DataGridTable *pDataGrid = new DataGridTable();

	EntertainArea *pEntertainArea = m_mapEntertainAreas_Find( atoi(Parms.c_str()) );
	if( !pEntertainArea || !pEntertainArea->m_pMediaStream )
		return pDataGrid;

	int iRow=0;

	list<DataGridCell *> listDataGridCell; // So we can force some attributes to the top

	for(map<int,list_int>::iterator it=pEntertainArea->m_pMediaStream->m_mapPK_Attribute.begin();it!=pEntertainArea->m_pMediaStream->m_mapPK_Attribute.end();++it)
	{
		Row_AttributeType *pRow_AttributeType = m_pDatabase_pluto_media->AttributeType_get()->GetRow(it->first);
		for(list_int::iterator itli=it->second.begin();itli!=it->second.end();++itli)
		{
			Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(*itli);
			if( pRow_AttributeType && pRow_Attribute && CanThumbnail(pRow_AttributeType->PK_AttributeType_get()) )
			{
				string sName = pRow_AttributeType->Description_get() + ": ";
				sName += pRow_Attribute->Name_get();
				DataGridCell *pCell = new DataGridCell( sName, "!A" + StringUtils::itos(*itli) );
				if( pRow_AttributeType->PK_AttributeType_get()==ATTRIBUTETYPE_Title_CONST )
					pDataGrid->SetData(0,iRow++,pCell);
				else if( pRow_AttributeType->PK_AttributeType_get()==ATTRIBUTETYPE_Performer_CONST || pRow_AttributeType->PK_AttributeType_get()==ATTRIBUTETYPE_Album_CONST )
					listDataGridCell.push_front(pCell);
				else
					listDataGridCell.push_back(pCell);
			}
		}
	}

	MediaFile *pMediaFile = pEntertainArea->m_pMediaStream->GetCurrentMediaFile();
	if( pMediaFile )
	{
		string sNamePrefix;

		if( pMediaFile->m_dwPK_File )
		{
			DataGridCell *pCell = new DataGridCell( "File: " + pMediaFile->m_sFilename, "!F" + StringUtils::itos(pMediaFile->m_dwPK_File) );
			pDataGrid->SetData(0,iRow++,pCell);
		}
		
		for( map<int,list_int>::iterator it=pMediaFile->m_mapPK_Attribute.begin();it!=pMediaFile->m_mapPK_Attribute.end();++it)
		{
			Row_AttributeType *pRow_AttributeType = m_pDatabase_pluto_media->AttributeType_get()->GetRow(it->first);
			for(list_int::iterator itli=it->second.begin();itli!=it->second.end();++itli)
			{
				Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(*itli);
				if( pRow_AttributeType && pRow_Attribute && CanThumbnail(pRow_AttributeType->PK_AttributeType_get()))
				{
					string sName = sNamePrefix + pRow_AttributeType->Description_get() + "\n" + pRow_Attribute->Name_get();
					DataGridCell *pCell = new DataGridCell( sName, "!A" + StringUtils::itos(*itli) );
					if( pRow_AttributeType->PK_AttributeType_get()==ATTRIBUTETYPE_Title_CONST )
						pDataGrid->SetData(0,iRow++,pCell);
					else if( pRow_AttributeType->PK_AttributeType_get()==ATTRIBUTETYPE_Performer_CONST || pRow_AttributeType->PK_AttributeType_get()==ATTRIBUTETYPE_Album_CONST )
						listDataGridCell.push_front(pCell);
					else
						listDataGridCell.push_back(pCell);
				}
			}
		}
		AddMediaSectionToDataGrid(pDataGrid,iRow,pMediaFile->m_dequeMediaSection,sNamePrefix,true);
		AddMediaTitlesToDataGrid(pDataGrid,iRow,pMediaFile->m_dequeMediaTitle,sNamePrefix,true);
	}

	for(list<DataGridCell *>::iterator it=listDataGridCell.begin();it!=listDataGridCell.end();++it)
		pDataGrid->SetData(0,iRow++,*it);

	AddMediaTitlesToDataGrid(pDataGrid,iRow,pEntertainArea->m_pMediaStream->m_dequeMediaTitle,"",true);

	return pDataGrid;
}

bool Media_Plugin::CanThumbnail(int PK_AttributeType)
{
	switch(PK_AttributeType)
	{
	case ATTRIBUTETYPE_Director_CONST:
	case ATTRIBUTETYPE_Performer_CONST:
	case ATTRIBUTETYPE_Album_CONST:
	case ATTRIBUTETYPE_Track_CONST:
	case ATTRIBUTETYPE_Genre_CONST:
	case ATTRIBUTETYPE_Channel_CONST:
	case ATTRIBUTETYPE_Episode_CONST:
	case ATTRIBUTETYPE_Program_CONST:
	case ATTRIBUTETYPE_Title_CONST:
	case ATTRIBUTETYPE_Conductor_CONST:
	case ATTRIBUTETYPE_ComposerWriter_CONST:
	case ATTRIBUTETYPE_Studio_CONST:
	case ATTRIBUTETYPE_Chapter_CONST:
	case ATTRIBUTETYPE_Producer_CONST:
	case ATTRIBUTETYPE_Executive_Producer_CONST:
		return true;
	default:
		return false;
	}
}

