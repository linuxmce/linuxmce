#ifdef WIN32
	#include <winsock.h>
#endif
#include <iostream>
#include <stdio.h>
#include <string>

#include <map>
#include <vector>

using namespace std;

#include "Database_pluto_media.h"


#include "DCEConfig.h"


using namespace DCE;
Database_pluto_media::Database_pluto_media(Logger *pLogger)
{
	LoggerWrapper::SetInstance(pLogger);
tblAttribute=NULL;
tblAttributeType=NULL;
tblAttribute_Settings=NULL;
tblBookmark=NULL;
tblCoverArtScan=NULL;
tblCoverArtScanEntry=NULL;
tblDisc=NULL;
tblDiscLocation=NULL;
tblDisc_Attribute=NULL;
tblDisc_Users=NULL;
tblDownload=NULL;
tblDownloadSource=NULL;
tblDownload_Attribute=NULL;
tblFile=NULL;
tblFileFormat=NULL;
tblFileGroup=NULL;
tblFile_Attribute=NULL;
tblFile_Users=NULL;
tblLongAttribute=NULL;
tblMediaProvider=NULL;
tblMediaSource=NULL;
tblMediaSubType=NULL;
tblMediaType_AttributeType=NULL;
tblMediaType_FileFormat=NULL;
tblMediaType_MediaSource=NULL;
tblMediaType_MediaSubType=NULL;
tblPicture=NULL;
tblPicture_Attribute=NULL;
tblPicture_Disc=NULL;
tblPicture_Download=NULL;
tblPicture_File=NULL;
tblPlaylist=NULL;
tblPlaylistEntry=NULL;
tblProviderSource=NULL;
tblRipStatus=NULL;
tblSearchToken=NULL;
tblSearchToken_Attribute=NULL;
tblpsc_media_batdet=NULL;
tblpsc_media_bathdr=NULL;
tblpsc_media_batuser=NULL;
tblpsc_media_repset=NULL;
tblpsc_media_schema=NULL;
tblpsc_media_tables=NULL;
}

Database_pluto_media::~Database_pluto_media()
{
	DeleteAllTables();
}

bool Database_pluto_media::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
bool bResult=true;
if( tblAttribute!=NULL )
	if( !Commit_Attribute(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblAttributeType!=NULL )
	if( !Commit_AttributeType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblAttribute_Settings!=NULL )
	if( !Commit_Attribute_Settings(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblBookmark!=NULL )
	if( !Commit_Bookmark(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCoverArtScan!=NULL )
	if( !Commit_CoverArtScan(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblCoverArtScanEntry!=NULL )
	if( !Commit_CoverArtScanEntry(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDisc!=NULL )
	if( !Commit_Disc(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDiscLocation!=NULL )
	if( !Commit_DiscLocation(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDisc_Attribute!=NULL )
	if( !Commit_Disc_Attribute(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDisc_Users!=NULL )
	if( !Commit_Disc_Users(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDownload!=NULL )
	if( !Commit_Download(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDownloadSource!=NULL )
	if( !Commit_DownloadSource(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDownload_Attribute!=NULL )
	if( !Commit_Download_Attribute(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFile!=NULL )
	if( !Commit_File(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFileFormat!=NULL )
	if( !Commit_FileFormat(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFileGroup!=NULL )
	if( !Commit_FileGroup(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFile_Attribute!=NULL )
	if( !Commit_File_Attribute(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFile_Users!=NULL )
	if( !Commit_File_Users(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblLongAttribute!=NULL )
	if( !Commit_LongAttribute(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaProvider!=NULL )
	if( !Commit_MediaProvider(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaSource!=NULL )
	if( !Commit_MediaSource(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaSubType!=NULL )
	if( !Commit_MediaSubType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaType_AttributeType!=NULL )
	if( !Commit_MediaType_AttributeType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaType_FileFormat!=NULL )
	if( !Commit_MediaType_FileFormat(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaType_MediaSource!=NULL )
	if( !Commit_MediaType_MediaSource(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaType_MediaSubType!=NULL )
	if( !Commit_MediaType_MediaSubType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPicture!=NULL )
	if( !Commit_Picture(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPicture_Attribute!=NULL )
	if( !Commit_Picture_Attribute(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPicture_Disc!=NULL )
	if( !Commit_Picture_Disc(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPicture_Download!=NULL )
	if( !Commit_Picture_Download(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPicture_File!=NULL )
	if( !Commit_Picture_File(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPlaylist!=NULL )
	if( !Commit_Playlist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPlaylistEntry!=NULL )
	if( !Commit_PlaylistEntry(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblProviderSource!=NULL )
	if( !Commit_ProviderSource(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblRipStatus!=NULL )
	if( !Commit_RipStatus(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSearchToken!=NULL )
	if( !Commit_SearchToken(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSearchToken_Attribute!=NULL )
	if( !Commit_SearchToken_Attribute(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_media_batdet!=NULL )
	if( !Commit_psc_media_batdet(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_media_bathdr!=NULL )
	if( !Commit_psc_media_bathdr(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_media_batuser!=NULL )
	if( !Commit_psc_media_batuser(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_media_repset!=NULL )
	if( !Commit_psc_media_repset(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_media_schema!=NULL )
	if( !Commit_psc_media_schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_media_tables!=NULL )
	if( !Commit_psc_media_tables(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
return bResult;

}

void Database_pluto_media::DeleteAllTables()
{
DeleteTable_Attribute();
DeleteTable_AttributeType();
DeleteTable_Attribute_Settings();
DeleteTable_Bookmark();
DeleteTable_CoverArtScan();
DeleteTable_CoverArtScanEntry();
DeleteTable_Disc();
DeleteTable_DiscLocation();
DeleteTable_Disc_Attribute();
DeleteTable_Disc_Users();
DeleteTable_Download();
DeleteTable_DownloadSource();
DeleteTable_Download_Attribute();
DeleteTable_File();
DeleteTable_FileFormat();
DeleteTable_FileGroup();
DeleteTable_File_Attribute();
DeleteTable_File_Users();
DeleteTable_LongAttribute();
DeleteTable_MediaProvider();
DeleteTable_MediaSource();
DeleteTable_MediaSubType();
DeleteTable_MediaType_AttributeType();
DeleteTable_MediaType_FileFormat();
DeleteTable_MediaType_MediaSource();
DeleteTable_MediaType_MediaSubType();
DeleteTable_Picture();
DeleteTable_Picture_Attribute();
DeleteTable_Picture_Disc();
DeleteTable_Picture_Download();
DeleteTable_Picture_File();
DeleteTable_Playlist();
DeleteTable_PlaylistEntry();
DeleteTable_ProviderSource();
DeleteTable_RipStatus();
DeleteTable_SearchToken();
DeleteTable_SearchToken_Attribute();
DeleteTable_psc_media_batdet();
DeleteTable_psc_media_bathdr();
DeleteTable_psc_media_batuser();
DeleteTable_psc_media_repset();
DeleteTable_psc_media_schema();
DeleteTable_psc_media_tables();
}

bool Database_pluto_media::Connect(string host, string user, string pass, string sDBName, int port)
{
return DBConnect(host, user, pass, sDBName, port);
}

bool Database_pluto_media::Connect(class DCEConfig *pDCEConfig)
{
	return Connect(pDCEConfig->m_sDBHost,pDCEConfig->m_sDBUser,pDCEConfig->m_sDBPassword,pDCEConfig->m_sDBName,pDCEConfig->m_iDBPort);
}
