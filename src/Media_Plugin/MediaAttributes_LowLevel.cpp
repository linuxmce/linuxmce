/*
 MediaAttributes

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "PlutoUtils/CommonIncludes.h"
#include "MediaAttributes.h"
#include "Logger.h"
#include "PlutoUtils/DatabaseUtils.h"

// pluto_media related database access
#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Define_DeviceData.h"

#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Table_File_Attribute.h"
#include "pluto_media/Table_MediaType_AttributeType.h"
#include "pluto_media/Table_File.h"
#include "pluto_media/Table_Bookmark.h"
#include "pluto_media/Table_Playlist.h"
#include "pluto_media/Table_PlaylistEntry.h"
#include "pluto_media/Table_Disc.h"
#include "pluto_media/Table_Picture_Disc.h"
#include "pluto_media/Table_Disc_Attribute.h"
#include "pluto_media/Table_DiscLocation.h"
#include "pluto_media/Table_Picture.h"
#include "pluto_media/Table_Picture_File.h"
#include "pluto_media/Table_Picture_Attribute.h"
#include "pluto_media/Define_AttributeType.h"

#ifndef WIN32
#include <dirent.h>
#endif

#include "PlutoUtils/FileUtils.h"

#include "MediaFile.h"

using namespace DCE;



/*
void MediaAttributes_LowLevel::ChangeAttribute( int OldPK_AttributeType, int NewPK_AttributeType, string OldName, string NewName )
{
}

void MediaAttributes_LowLevel::ChangeAttribute( int PK_Attribute, string NewName )
{
}
*/
listMediaAttribute *MediaAttributes_LowLevel::AttributesFromString( string Input )
{
    listMediaAttribute *plistMediaAttribute = new listMediaAttribute( );

    string::size_type pos=0;
    while( true )
    {
        MediaAttribute *mi = new MediaAttribute( );
        mi->m_PK_Attribute = atoi( StringUtils::Tokenize( Input, "|", pos ).c_str( ) );
        mi->m_PK_AttributeType = atoi( StringUtils::Tokenize( Input, "|", pos ).c_str( ) );
        mi->m_sName = StringUtils::Tokenize( Input, "|", pos );
        if( !mi->m_PK_Attribute && !mi->m_PK_AttributeType )
        {
            delete mi;
            break;
        }
        plistMediaAttribute->push_back( mi );
    }

    return plistMediaAttribute;
}

string MediaAttributes_LowLevel::AttributesToString( listMediaAttribute *plistMediaAttribute )
{
    string Result;
    for( listMediaAttribute::iterator it = plistMediaAttribute->begin( );it != plistMediaAttribute->end( );++it )
    {
        MediaAttribute *mi = ( *it );
        Result += StringUtils::itos( mi->m_PK_Attribute ) + "|";
        Result += StringUtils::itos( mi->m_PK_AttributeType ) + "|";
        Result += mi->m_sName + "|";
    }

    return Result;
}

listMediaPicture *MediaAttributes_LowLevel::PicturesFromString( string Input )
{
    listMediaPicture *plistMediaPicture = new listMediaPicture( );

    string::size_type pos=0;
    while( true )
    {
        MediaPicture *mp = new MediaPicture( );
        mp->Path = StringUtils::Tokenize( Input, "|", pos );
        if( mp->Path.length( )==0 )
        {
            delete mp;
            break;
        }
        mp->Extension = FileUtils::FindExtension( mp->Path );
        plistMediaPicture->push_back( mp );
    }

    return plistMediaPicture;
}

string MediaAttributes_LowLevel::PicturesToString( listMediaPicture *plistMediaPicture )
{
    string Result;
    for( listMediaPicture::iterator it = plistMediaPicture->begin( );it != plistMediaPicture->end( );++it )
    {
        MediaPicture *mp = ( *it );
        Result += mp->Path + "|";
    }

    return Result;
}

string MediaAttributes_LowLevel::GetFilePathFromFileID( int PK_File, int *PK_MediaType )
{
    string SQL = "SELECT Path,Filename,EK_MediaType FROM File WHERE PK_File=" + StringUtils::itos( PK_File );
    PlutoSqlResult result;
    DB_ROW row;
    if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) )
    {
        string strPath = row[0];
        string strFile = row[1];
		if( PK_MediaType && row[2] )
			*PK_MediaType = atoi(row[2]);
        return strPath + "/" + strFile;
    }

	if(NULL != PK_MediaType)
		*PK_MediaType = 0;

    return "";
}

int MediaAttributes_LowLevel::GetFileIDFromAttributeID( int PK_Attribute )
{
    string Result;
    string SQL = "SELECT PK_File FROM File "\
        "JOIN File_Attribute ON FK_File=PK_File "\
        "JOIN Attribute ON FK_Attribute=PK_Attribute "\
        "JOIN MediaType_AttributeType ON File.EK_MediaType=MediaType_AttributeType.EK_MediaType "\
        "AND Type_AttributeType.FK_AttributeType=Attribute.FK_AttributeType "\
        "WHERE Identifier=1 AND FK_Attribute=" + StringUtils::itos( PK_Attribute );

    PlutoSqlResult result;
    DB_ROW row;
    if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) )
        return atoi( row[0] );
    return 0;
}

string MediaAttributes_LowLevel::GetFilePathsFromAttributeID( int PK_Atribute )
{
    string Result;
    string SQL = "select DISTINCT Path "\
        "FROM File_Attribute As Source "\
        "JOIN File_Attribute As Dest "\
        "ON Source.FK_File=Dest.FK_File AND Source.FK_Attribute=" + StringUtils::itos( PK_Atribute ) + " "\
        "JOIN File ON Dest.FK_File=PK_File "\
        "ORDER BY Path";

    PlutoSqlResult result;
    DB_ROW row;
    if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) )
    {
        while( ( row=db_wrapper_fetch_row( result.r ) ) )
        {
            Result += row[0];
            Result += "|";
        }
    }
    return Result;
}

int MediaAttributes_LowLevel::GetFileIDFromFilePath( string File )
{
    // moved here since we want to fallback on the windows like functionality when we don't have an attributte for the file.
	PlutoSqlResult result;
    DB_ROW row;

	/*
#ifdef WIN32
    string::size_type s;
    while( ( s=File.find( "Z:/" ) )!=string::npos )
        File.replace( s, 3, "/home/public/data/" );
    while( ( s=File.find( "Z:\\" ) )!=string::npos )
        File.replace( s, 3, "/home/public/data/" );
    while( ( s=File.find( '\\' ) )!=string::npos )
        File.replace( s, 1, "/" );

	string Path = FileUtils::BasePath(File);
	if( Path.length() && Path[ Path.length()-1 ]=='/' )
		Path = Path.substr(0,Path.length()-1);
	string SQL = "SELECT PK_File FROM File WHERE Path='" + StringUtils::SQLEscape( Path ) +
		"' AND Filename='" + StringUtils::SQLEscape( FileUtils::FilenameWithoutPath(File) ) + "'";
    if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) )
        return atoi( row[0] );

    return 0; // Can't do this in Windows
#else
	*/

	string Path = FileUtils::BasePath(File);
	if( Path.length() && Path[ Path.length()-1 ]=='/' )
		Path = Path.substr(0,Path.length()-1);

#ifdef WIN32
	Path = StringUtils::Replace(Path, "\\", "/"); // replacing all the \ in a windows path with /
#endif

	string SQL = "SELECT PK_File FROM File WHERE Path='" + StringUtils::SQLEscape( Path ) +
		"' AND Filename='" + StringUtils::SQLEscape( FileUtils::FilenameWithoutPath(File) ) + "'";
	if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) )
		return atoi( row[0] );
	else
		return 0;

}

string MediaAttributes_LowLevel::GetAnyPictureUnderDirectory( string File, int *PK_Picture, int MaxDepthToSearch )
{
    int FilesToScan=5;
#ifdef WIN32
    intptr_t ptrFileList;
    _finddata_t finddata;
    string::size_type s;
    while( ( s=File.find( '/' ) )!=string::npos )
        File.replace( s, 1, "\\" );

    ptrFileList = _findfirst( ( File+"\\*.*" ).c_str( ), &finddata );
    while( ptrFileList!=-1 )
    {
        if( MaxDepthToSearch && ( finddata.attrib==_A_SUBDIR ) && finddata.name[0]!='.' )
        {
            string Extension = GetAnyPictureUnderDirectory( File + "\\" + finddata.name, PK_Picture, MaxDepthToSearch-1 );
            if( *PK_Picture )
                return Extension;
        }
        else if ( finddata.name[0] != '.' )
        {
            FilesToScan--;
            string Extension = GetPictureFromFilePath( File + "\\" + finddata.name, PK_Picture );
            if( *PK_Picture )
                return Extension;
        }
        if(     !FilesToScan )
        {
            *PK_Picture=0;
            return "";
        }
        if( _findnext( ptrFileList, &finddata )<0 )
            break;
    }

    _findclose( ptrFileList );

#else
    DIR *dirp = opendir( File.c_str( ) );
    struct dirent entry;
    struct dirent *direntp = &entry;
    if ( dirp == NULL )
    {
        LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "opendir2 %s failed: %s", File.c_str( ), strerror( errno ) );
        return "";
    }
    while ( dirp != NULL && ( readdir_r( dirp, direntp, &direntp ) == 0 ) && direntp )
    {
#ifdef DEBUG
LoggerWrapper::GetInstance()->Write( LV_STATUS, "GetPicture: Reading %s %s depth %d",File.c_str(),entry.d_name,MaxDepthToSearch);
#endif
        if( MaxDepthToSearch && ( entry.d_type==DT_DIR ) && entry.d_name[0]!='.' )
        {
            string Extension = GetAnyPictureUnderDirectory( File + "/" + entry.d_name, PK_Picture, MaxDepthToSearch-1 );
            if( *PK_Picture )
            {
                closedir( dirp );
                return Extension;
            }
        }
        else if ( entry.d_name[0] != '.' )
        {
            FilesToScan--;
            string Extension = GetPictureFromFilePath( File + "/" + entry.d_name, PK_Picture );

#ifdef DEBUG
LoggerWrapper::GetInstance()->Write( LV_STATUS, "GetPicture: Reading %s %s got pic %d %s",File.c_str(),entry.d_name,*PK_Picture,Extension.c_str());
#endif
            if( *PK_Picture )
            {
                closedir( dirp );
                return Extension;
			}
        }
        if ( ! FilesToScan )
        {
            closedir( dirp );
            *PK_Picture=0;
            return "";
        }
    }
    closedir( dirp );
#endif
return "";
}

string MediaAttributes_LowLevel::GetPictureFromFilePath( string File, int *PK_Picture )
{
    int PK_File = GetFileIDFromFilePath( File );

// LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got file id: %d", PK_File);

    if( PK_File )
        return GetPictureFromFileID( PK_File, PK_Picture );

	*PK_Picture=0;
    return "";
}

string MediaAttributes_LowLevel::GetPictureFromFileID( int PK_File, int *PK_Picture )
{
	if( !PK_File )
	{
		*PK_Picture=0;
		return "";
	}

	string SQL = "select PK_Picture, Extension FROM Picture_File JOIN Picture ON "\
        "FK_Picture=PK_Picture "\
        "WHERE FK_File=" + StringUtils::itos( PK_File );

//	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaAttributes_LowLevel::GetPictureFromFileID() Running query: %s", SQL.c_str());
    PlutoSqlResult result;
    DB_ROW row;
    if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) )
    {
        *PK_Picture = atoi( row[0] );
        return row[1];
    }

    // Let's to see if any of the attributes associated with this file have pictures
    SQL = "SELECT FK_Picture, Extension from File_Attribute "\
        "JOIN Picture_Attribute ON Picture_Attribute.FK_Attribute=File_Attribute.FK_Attribute "\
        "JOIN Picture ON FK_Picture=PK_Picture "\
        "WHERE FK_File=" + StringUtils::itos( PK_File );

//	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaAttributes_LowLevel::GetPictureFromFileID() Running another query: %s", SQL.c_str());
    if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) )
    {
        *PK_Picture = atoi( row[0] );
        return row[1];
    }

    *PK_Picture=0;
    return "";
}

string MediaAttributes_LowLevel::GetPictureFromDiscID( int PK_Disc, int *PK_Picture )
{
	if( !PK_Disc )
	{
		*PK_Picture=0;
		return "";
	}

	string SQL = "select PK_Picture, Extension FROM Picture_Disc JOIN Picture ON "\
        "FK_Picture=PK_Picture "\
        "WHERE FK_Disc=" + StringUtils::itos( PK_Disc );

//	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaAttributes_LowLevel::GetPictureFromDiscID() Running query: %s", SQL.c_str());
    PlutoSqlResult result;
    DB_ROW row;
    if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) )
    {
        *PK_Picture = atoi( row[0] );
        return row[1];
    }

    // Let's to see if any of the attributes associated with this Disc have pictures
    SQL = "SELECT FK_Picture, Extension from Disc_Attribute "\
        "JOIN Picture_Attribute ON Picture_Attribute.FK_Attribute=Disc_Attribute.FK_Attribute "\
        "JOIN Picture ON FK_Picture=PK_Picture "\
        "WHERE FK_Disc=" + StringUtils::itos( PK_Disc );

//	LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaAttributes_LowLevel::GetPictureFromDiscID() Running another query: %s", SQL.c_str());
    if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) )
    {
        *PK_Picture = atoi( row[0] );
        return row[1];
    }

    *PK_Picture=0;
    return "";
}

string MediaAttributes_LowLevel::GetPictureFromAttributeID( int PK_Attribute, int *PK_Picture )
{
    string SQL = "select PK_Picture, Extension FROM Picture_Attribute JOIN Picture ON "\
        "FK_Picture=PK_Picture "\
        "WHERE FK_Attribute=" + StringUtils::itos( PK_Attribute ) + " limit 1";
    PlutoSqlResult result,result2;
    DB_ROW row;
    if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) )
    {
        *PK_Picture = atoi( row[0] );
        return row[1];
    }

	// Let's to see if any of the attributes associated with this file have pictures
    SQL = "select DISTINCT FK_Picture, Extension "\
        "FROM File_Attribute AS Source "\
        "JOIN File_Attribute As Dest ON Source.FK_File=Dest.FK_File "\
        "JOIN Picture_Attribute ON Picture_Attribute.FK_Attribute=Dest.FK_Attribute "\
        "JOIN Picture ON FK_Picture=PK_Picture "\
        "WHERE Source.FK_Attribute=" + StringUtils::itos( PK_Attribute ) + " limit 1";

    if( ( result2.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result2.r ) ) )
    {
        *PK_Picture = atoi( row[0] );
        return row[1];
    }

    *PK_Picture=0;
    return "";
}
int MediaAttributes_LowLevel::GetAttributeFromFileID( int PK_File )
{
    string SQL = "select PK_Attribute FROM File_Attribute "\
        "JOIN Attribute ON FK_Attribute=PK_Attribute "\
        "JOIN File ON FK_File=PK_File "\
        "JOIN MediaType_AttributeType ON File.EK_MediaType=MediaType_AttributeType.EK_MediaType "\
        "AND MediaType_AttributeType.FK_AttributeType=Attribute.FK_AttributeType "\
        "WHERE Identifier=1 AND PK_File=" + StringUtils::itos( PK_File );

    PlutoSqlResult result;
    DB_ROW row;

    if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) )
        return atoi( row[0] );
    return 0;
}
int MediaAttributes_LowLevel::GetAttributeFromFilePath( string File )
{
    string SQL = "select PK_Attribute FROM File_Attribute "\
        "JOIN Attribute ON FK_Attribute=PK_Attribute "\
        "JOIN File ON FK_File=PK_File "\
        "JOIN MediaType_AttributeType ON File.EK_MediaType=MediaType_AttributeType.EK_MediaType "\
        "AND MediaType_AttributeType.FK_AttributeType=Attribute.FK_AttributeType "\
        "WHERE Identifier=1 AND Path='" + StringUtils::SQLEscape( File ) + "'";

    PlutoSqlResult result;
    DB_ROW row;

    if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) )
        return atoi( row[0] );
    return 0;
}


void MediaAttributes_LowLevel::PurgeDequeMediaFile(deque<MediaFile *> &dequeMediaFile)
{
	for(size_t s=0;s<dequeMediaFile.size();++s)
		delete dequeMediaFile[s];
	dequeMediaFile.clear();
}

void MediaAttributes_LowLevel::MarkAsMissing(int iKey, string fileName)
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Marking %s as missing", fileName.c_str());

    string SQL = "UPDATE File SET Missing=1 WHERE PK_File=" + StringUtils::itos( iKey );

    m_pDatabase_pluto_media->threaded_db_wrapper_query_withID( SQL );
}

void operator+= (deque<MediaFile *> &dTarget, deque<MediaFile *> &dAdditional)
{
    for(size_t s=0;s<dAdditional.size();++s)
        dTarget.push_back(dAdditional[s]);
}

Row_Attribute *MediaAttributes_LowLevel::GetAttributeFromDescription(int PK_MediaType,int PK_AttributeType,string sName,int PK_Attribute_Related)
{
	if( StringUtils::OnlyWhiteSpace(sName) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MediaAttributes_LowLevel::GetAttributeFromDescription got a blank attribute");
		return NULL;
	}
	vector<Row_Attribute *> vectRow_Attribute;
	Row_MediaType_AttributeType *pMediaType_AttributeType = NULL;
	if( PK_MediaType==MEDIATYPE_pluto_CD_CONST )
		pMediaType_AttributeType = m_pDatabase_pluto_media->MediaType_AttributeType_get()->GetRow(MEDIATYPE_pluto_StoredAudio_CONST,PK_AttributeType);
	else
		pMediaType_AttributeType = m_pDatabase_pluto_media->MediaType_AttributeType_get()->GetRow(PK_MediaType,PK_AttributeType);

	if( PK_AttributeType==ATTRIBUTETYPE_Album_CONST && PK_Attribute_Related )
	{
		// Find another file, disc, or download with the same name and the same performer
		string sWhere =
			"LEFT JOIN File_Attribute AS FA ON FA.FK_Attribute=PK_Attribute "
			"LEFT JOIN File_Attribute AS FA2 ON FA.FK_File=FA2.FK_File AND FA2.FK_Attribute = " + StringUtils::itos(PK_Attribute_Related) + " "
			"LEFT JOIN Disc_Attribute AS DA ON DA.FK_Attribute=PK_Attribute "
			"LEFT JOIN Disc_Attribute AS DA2 ON DA.FK_Disc=DA2.FK_Disc AND DA2.FK_Attribute = " + StringUtils::itos(PK_Attribute_Related) + " "
			"LEFT JOIN Download_Attribute AS LA ON LA.FK_Attribute=PK_Attribute "
			"LEFT JOIN Download_Attribute AS LA2 ON LA.FK_Download=LA2.FK_Download AND LA2.FK_Attribute = " + StringUtils::itos(PK_Attribute_Related) + " "
			"WHERE FK_AttributeType=" TOSTRING(ATTRIBUTETYPE_Album_CONST) " AND Name='" + StringUtils::SQLEscape(sName) + "' "
			"AND (FA2.FK_Attribute IS NOT NULL OR DA2.FK_Attribute IS NOT NULL OR LA2.FK_Attribute IS NOT NULL)";
		m_pDatabase_pluto_media->Attribute_get()->GetRows(sWhere,&vectRow_Attribute);

string sSQL = "select * FROM Attribute " + sWhere;
PlutoSqlResult result;
DB_ROW row;
    result.r=m_pDatabase_pluto_media->db_wrapper_query_result( sSQL );

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MediaAttributes_LowLevel::GetAttributeFromDescription album rows %d PK_MediaType %d PK_AttributeType %d string sName %s PK_Attribute_Related %d size %d",
result.r ? (int) result.r->row_count : -1,
PK_MediaType, PK_AttributeType, sName.c_str(), PK_Attribute_Related, (int) vectRow_Attribute.size());

	if( result.r )
{
	while( ( row=db_wrapper_fetch_row( result.r ) ) )
    {
		string s;
		for(unsigned int i=0;i<result.r->field_count;++i)
		{
			s += row[i] ? row[i] : "*NULL";
			s+= "\t";
		}
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"%s", s.c_str());
	}
}


	}
	else if( !pMediaType_AttributeType || pMediaType_AttributeType->CombineAsOne_get()==1 )
	{
		string sWhere = "FK_AttributeType=" + StringUtils::itos(PK_AttributeType) + " AND Name='" + StringUtils::SQLEscape(sName) + "'";
		m_pDatabase_pluto_media->Attribute_get()->GetRows(sWhere,&vectRow_Attribute);
	}

LoggerWrapper::GetInstance()->Write(LV_CRITICAL,
"MediaAttributes_LowLevel::GetAttributeFromDescription PK_MediaType %d PK_AttributeType %d string sName %s PK_Attribute_Related %d size %d",
PK_MediaType, PK_AttributeType, sName.c_str(), PK_Attribute_Related, (int) vectRow_Attribute.size());


	if( vectRow_Attribute.size() )
		return vectRow_Attribute[0];
	else
	{
		Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->AddRow();
		pRow_Attribute->FK_AttributeType_set(PK_AttributeType);
		pRow_Attribute->Name_set(sName);
		pRow_Attribute->Table_Attribute_get()->Commit();
		return pRow_Attribute;
	}
}

Row_Picture * MediaAttributes_LowLevel::AddPicture(char *pData,int iData_Size,string sFormat,string sURL)
{
	sFormat = StringUtils::ToLower(sFormat);
	string sMediaPicsFolder = "/home/mediapics/";

	if(NULL == pData) //picture not downloaded yet 
	{
#ifdef WIN32
		string sDownloadedFile = "C:\\Temp\\picture.tmp";
#else
		string sDownloadedFile = "/tmp/picture.tmp";
#endif

		string sCommand = "wget " + sURL + " -O " + sDownloadedFile + " -T 10 -t 1";
		system(sCommand.c_str());

		size_t nSize = 0;
		pData = FileUtils::ReadFileIntoBuffer(sDownloadedFile, nSize);
		FileUtils::DelFile(sDownloadedFile);
		iData_Size = int(nSize);

#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Wget command line: '%s' ; file saved size: %d", sCommand.c_str(), nSize);
#endif			
	}

	if(iData_Size != 0 && NULL != pData )
	{
		Row_Picture *pRow_Picture = m_pDatabase_pluto_media->Picture_get()->AddRow();
		pRow_Picture->Extension_set(sFormat);
		pRow_Picture->URL_set(sURL);
		m_pDatabase_pluto_media->Picture_get()->Commit();

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes_LowLevel::AddPicture %d %s",
			pRow_Picture->PK_Picture_get(),sURL.c_str());

		string sPictureFileName = sMediaPicsFolder + StringUtils::itos(pRow_Picture->PK_Picture_get()) + "." + sFormat;
		FILE *file = fopen(sPictureFileName.c_str(), "wb");
		if( !file )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot create bookmark pic file");
			pRow_Picture->Delete();
			m_pDatabase_pluto_media->Picture_get()->Commit();
			return NULL;
		}

		fwrite((void *) pData,iData_Size,1,file);
		fclose(file);
		string Cmd = "convert -scale 256x256 -antialias /home/mediapics/" + StringUtils::itos( pRow_Picture->PK_Picture_get() ) + "." + sFormat +
			" /home/mediapics/" + StringUtils::itos( pRow_Picture->PK_Picture_get() ) + "_tn." + sFormat;
		int result;
		if( ( result=system( Cmd.c_str( ) ) )!=0 )
			LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Thumbnail picture %s returned %d", Cmd.c_str( ), result );

		return pRow_Picture;
	}

	return NULL;
}

Row_Picture * MediaAttributes_LowLevel::AddPicture(string sFile)
{
	if( !FileUtils::FileExists(sFile) )
		return NULL;

	string sFormat = StringUtils::ToLower(FileUtils::FindExtension(sFile));
	string sMediaPicsFolder = "/home/mediapics/";

	Row_Picture *pRow_Picture = m_pDatabase_pluto_media->Picture_get()->AddRow();
	pRow_Picture->Extension_set("jpg");
	pRow_Picture->URL_set(sFile);
	m_pDatabase_pluto_media->Picture_get()->Commit();

	string sPictureFileName = sMediaPicsFolder + StringUtils::itos(pRow_Picture->PK_Picture_get()) + ".jpg";

	if( sFormat!="jpg" )
	{
		// All our pictures can only be jpg
		string Cmd = "convert \"" + sFile + "\" \"" + sPictureFileName + "\"";
		int result;
		if( ( result=system( Cmd.c_str( ) ) )!=0 )
			LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Thumbnail picture %s returned %d", Cmd.c_str( ), result );
		else
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes_LowLevel::AddPicture %d %s result: %s",
				pRow_Picture->PK_Picture_get(),sFile.c_str(),Cmd.c_str());
	}
	else
	{
		bool bResult = FileUtils::PUCopyFile(sFile,sPictureFileName); /** < Copies a file from sSource to sDestination.  returns false if it fails, true otherwise */
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes_LowLevel::AddPicture %d %s result: %d",
			pRow_Picture->PK_Picture_get(),sFile.c_str(),(int) bResult);
	}

	string Cmd = "convert -scale 256x256 -antialias /home/mediapics/" + StringUtils::itos( pRow_Picture->PK_Picture_get() ) + "." + sFormat +
		" /home/mediapics/" + StringUtils::itos( pRow_Picture->PK_Picture_get() ) + "_tn." + sFormat;
	int result;
	if( ( result=system( Cmd.c_str( ) ) )!=0 )
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Thumbnail picture %s returned %d", Cmd.c_str( ), result );

	return pRow_Picture;
}

void MediaAttributes_LowLevel::UpdateSearchTokens(Row_Attribute *pRow_Attribute)
{
	int PK_Attribute = pRow_Attribute->PK_Attribute_get();
    PlutoSqlResult result;
    DB_ROW row, row2;
    string SQL = "SELECT Name FROM Attribute WHERE PK_Attribute=" + StringUtils::itos( PK_Attribute );
    if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) && ( row=db_wrapper_fetch_row( result.r ) ) )
    {
        SQL = "DELETE FROM SearchToken_Attribute WHERE FK_Attribute=" + StringUtils::itos( PK_Attribute );
        m_pDatabase_pluto_media->threaded_db_wrapper_query( SQL );
        char *ptr = row[0];
        string::size_type pos=0;
        while( ptr && *ptr )
        {
            string s( ptr );
            string Token=StringUtils::UpperAZ09Only( StringUtils::Tokenize( s, " ", pos ) );
            if( Token.length( )==0 )
                break;
            SQL = "SELECT PK_SearchToken FROM SearchToken WHERE Token='" +
                StringUtils::SQLEscape( Token ) + "'";
            int PK_SearchToken=0;
			PlutoSqlResult result2;
            if( ( result2.r=m_pDatabase_pluto_media->db_wrapper_query_result( SQL ) ) && ( row2=db_wrapper_fetch_row( result2.r ) ) )
            {
                PK_SearchToken = atoi( row2[0] );
            }
            else
            {
                SQL = "INSERT INTO SearchToken( Token ) VALUES( '" +
                    StringUtils::SQLEscape( Token ) + "' )";
                PK_SearchToken = m_pDatabase_pluto_media->threaded_db_wrapper_query_withID( SQL );
            }
            if( PK_SearchToken )
            {
                SQL = "INSERT INTO SearchToken_Attribute(FK_SearchToken,FK_Attribute) VALUES( " +
                    StringUtils::itos( PK_SearchToken ) + ", " +
                    StringUtils::itos( PK_Attribute ) + " )";
                m_pDatabase_pluto_media->threaded_db_wrapper_query( SQL, true );
            }
        }
    }
    else
        LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot update tokens for %d", PK_Attribute );
}

int MediaAttributes_LowLevel::Parse_Misc_Media_ID(int PK_MediaType,listMediaAttribute &listMediaAttribute_,string sValue,int PK_File)
{
	// The format is as follows.  Each line (\n terminated) contains the following:
	// The first line only is the disc id
	// Track \t PK_AttributeType \t Section \t Name/Description
	// Track refers to separate tracks on a cd, which, if ripped, would be separate files.
	// Section, however, refers to chapters within a DVD--there is only file, but it has sections

	vector<string> vectAttributes;
	StringUtils::Tokenize(sValue,"\n",vectAttributes);
	if( vectAttributes.size()<1 )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MediaAttributes_LowLevel::Parse_Misc_Media_ID -- empty");
		return 0;
	}

	// Because we need to get the performers before the albums so we can consolidate the albums
	// store pending albums in mapAlbum, and the corresponding performers in mapPerformer,
	// and process all at once at the end of the loop
	map< pair<int,int>, string > mapAlbum;
	map< pair<int,int>, int > mapPerformer;
	
	int PK_Disc=0;
	if( PK_File || (PK_Disc=IsDiscAlreadyIdentified(vectAttributes[0],listMediaAttribute_))==0 )
	{
		Row_Attribute *pRow_Attribute;
	    pRow_Attribute = GetAttributeFromDescription(PK_MediaType,ATTRIBUTETYPE_Disc_ID_CONST,vectAttributes[0]); 
		if( pRow_Attribute==NULL )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"MediaAttributes_LowLevel::Parse_Misc_Media_ID attribute disc is empty");
			return PK_Disc;
		}
		
		listMediaAttribute_.push_back( new MediaAttribute(0,0,ATTRIBUTETYPE_Disc_ID_CONST,pRow_Attribute->PK_Attribute_get(),pRow_Attribute->Name_get()) );
		for(size_t s=1;s<vectAttributes.size();++s)
		{
			string sLine = vectAttributes[s];
			string::size_type pos=0;

			int Track = atoi(StringUtils::Tokenize(sLine,"\t",pos).c_str());
			int PK_AttributeType = atoi(StringUtils::Tokenize(sLine,"\t",pos).c_str());
			int Section = atoi(StringUtils::Tokenize(sLine,"\t",pos).c_str());
			string sWholeName = StringUtils::Tokenize(sLine,"\t",pos);
			if( !PK_AttributeType || sWholeName.size()==0 )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Empty/invalid media attribute %d/%s",PK_AttributeType,sWholeName.c_str());
				continue;
			}

			// Some attributes may be ; delimited lists
			pos=0;
			string sName;
			while(pos<sWholeName.size()-1)
			{
				if( PK_AttributeType==ATTRIBUTETYPE_Director_CONST || 
					PK_AttributeType==ATTRIBUTETYPE_Performer_CONST ||
					PK_AttributeType==ATTRIBUTETYPE_Conductor_CONST ||
					PK_AttributeType==ATTRIBUTETYPE_ComposerWriter_CONST )
						sName=StringUtils::Tokenize(sWholeName,";",pos);
				else
				{
					sName=sWholeName;
					pos=string::npos;
				}

				StringUtils::TrimSpaces(sName);
				if( sName.size()==0 )
					continue;

				if( PK_AttributeType==ATTRIBUTETYPE_Album_CONST )
				{
					mapAlbum[ make_pair<int,int> (Track,Section) ] = sName;
					continue;
				}

				pRow_Attribute = GetAttributeFromDescription(PK_MediaType,PK_AttributeType,sName);
				if( pRow_Attribute==NULL )
				{
					LoggerWrapper::GetInstance()->Write(LV_WARNING,"MediaAttributes_LowLevel::Parse_Misc_Media_ID attribute %d is empty",PK_AttributeType);
					return PK_Disc;
				}

				if( PK_AttributeType==ATTRIBUTETYPE_Performer_CONST )
					mapPerformer[ make_pair<int,int> (Track,Section) ] = pRow_Attribute->PK_Attribute_get();

				LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes_LowLevel::Parse_Misc_Media_ID added attribute %p %d %s",
					pRow_Attribute, (pRow_Attribute ? pRow_Attribute->PK_Attribute_get() : 0), sName.c_str());

				listMediaAttribute_.push_back( new MediaAttribute(
					Track,Section,pRow_Attribute->FK_AttributeType_get(),pRow_Attribute->PK_Attribute_get(),pRow_Attribute->Name_get()) );
			}
		}

		for(map< pair<int,int>, string >::iterator it=mapAlbum.begin();it!=mapAlbum.end();++it)
		{
			int PK_Attribute_Performer = mapPerformer[it->first];

			pRow_Attribute = GetAttributeFromDescription(PK_MediaType,ATTRIBUTETYPE_Album_CONST,it->second,PK_Attribute_Performer);
			if( pRow_Attribute==NULL )
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING,"MediaAttributes_LowLevel::Parse_Misc_Media_ID attribute ATTRIBUTETYPE_Album_CONST is empty");
				return PK_Disc;
			}
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes_LowLevel::Parse_Misc_Media_ID added attribute %p %d %s",
				pRow_Attribute, (pRow_Attribute ? pRow_Attribute->PK_Attribute_get() : 0), it->second.c_str());

			listMediaAttribute_.push_back( new MediaAttribute(
				it->first.first,it->first.second,pRow_Attribute->FK_AttributeType_get(),pRow_Attribute->PK_Attribute_get(),pRow_Attribute->Name_get()) );
		}
		if( PK_MediaType==MEDIATYPE_pluto_CD_CONST )
		{
			MediaAttribute *pMediaAttribute_Album = GetMediaAttribute(0,0,ATTRIBUTETYPE_Album_CONST,listMediaAttribute_);
			// For CD's be sure the album is stored with each track too
			if( pMediaAttribute_Album )
			{
				int MaxTracks = GetMaxTracks(listMediaAttribute_);
				for(int i=0;i<MaxTracks;++i)
				{
					MediaAttribute *p = GetMediaAttribute(i,0,ATTRIBUTETYPE_Album_CONST,listMediaAttribute_);
					if( !p )
						listMediaAttribute_.push_back( new MediaAttribute(
							i,0,pMediaAttribute_Album->m_PK_AttributeType,pMediaAttribute_Album->m_PK_Attribute,
							pMediaAttribute_Album->m_sName) );
				}
			}
		}
		if( PK_File )
			AddIdentifiedFileToDB(PK_MediaType,PK_File,listMediaAttribute_);
		else
			PK_Disc = AddIdentifiedDiscToDB(PK_MediaType,vectAttributes[0],listMediaAttribute_);
	}
	
	int Tracks;
	FixMediaAttributes(listMediaAttribute_,Tracks);
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Parse_misc_Media_ID done");
	return PK_Disc;
}

int MediaAttributes_LowLevel::Parse_CDDB_Media_ID(int PK_MediaType,listMediaAttribute &listMediaAttribute_,string sValue)
{
	string::size_type pos=0,pos2=0;

	string sCDDBID = StringUtils::Tokenize(sValue,"\t",pos);
	Row_Attribute *pRow_Attribute;
	int PK_Attribute_Performer=0;

	// Before proceeding, see if this disc is already in the database
	int PK_Disc=0;
	if( (PK_Disc=IsDiscAlreadyIdentified(sCDDBID,listMediaAttribute_))==0 )
	{
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Parse_CDDB_Media_ID not already id'd");

		// The cddb info is space delimited
	    pRow_Attribute = GetAttributeFromDescription(PK_MediaType,ATTRIBUTETYPE_CDDB_CONST,StringUtils::Tokenize(sCDDBID," ",pos2)); 
		if( pRow_Attribute==NULL )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"ediaAttributes_LowLevel::Parse_CDDB_Media_ID attribute id is empty");
			return PK_Disc;
		}

		listMediaAttribute_.push_back( new MediaAttribute(
			0,0,pRow_Attribute->FK_AttributeType_get(),pRow_Attribute->PK_Attribute_get(),pRow_Attribute->Name_get()) );

		int NumTracks=atoi(StringUtils::Tokenize(sCDDBID," ",pos2).c_str()); // cddb id

		// The actual data is now tab delimited.
		string s = StringUtils::Tokenize(sValue,"\t",pos);
		if( s.size() )
		{
			pRow_Attribute = GetAttributeFromDescription(PK_MediaType,ATTRIBUTETYPE_Performer_CONST,s); 
			if( pRow_Attribute )
			{
				listMediaAttribute_.push_back( new MediaAttribute(
					0,0,pRow_Attribute->FK_AttributeType_get(),pRow_Attribute->PK_Attribute_get(),pRow_Attribute->Name_get()) );
				PK_Attribute_Performer = pRow_Attribute->PK_Attribute_get();
			}
		}

		s = StringUtils::Tokenize(sValue,"\t",pos);
		if( s.size() )
		{
			pRow_Attribute = GetAttributeFromDescription(PK_MediaType,ATTRIBUTETYPE_Album_CONST,s,PK_Attribute_Performer); 
			if( pRow_Attribute )
				listMediaAttribute_.push_back( new MediaAttribute(
					0,0,pRow_Attribute->FK_AttributeType_get(),pRow_Attribute->PK_Attribute_get(),pRow_Attribute->Name_get()) );
		}

		s = StringUtils::Tokenize(sValue,"\t",pos);
		if( s.size() )
		{
			pRow_Attribute = GetAttributeFromDescription(PK_MediaType,ATTRIBUTETYPE_Genre_CONST,s); 
			if( pRow_Attribute )
				listMediaAttribute_.push_back( new MediaAttribute(
					0,0,pRow_Attribute->FK_AttributeType_get(),pRow_Attribute->PK_Attribute_get(),pRow_Attribute->Name_get()) );
		}

		// Read the info for the tracks.  NumTracks normally should = pMediaStream->m_dequeMediaFile.size()
		for(int s = 0; s < NumTracks; ++s)
		{
			string str = StringUtils::Tokenize(sValue,"\t\n",pos);
			if( str.size() )
			{
				pRow_Attribute = GetAttributeFromDescription(PK_MediaType,ATTRIBUTETYPE_Title_CONST,str); 
				if( pRow_Attribute )
					listMediaAttribute_.push_back( new MediaAttribute(
						s,0,pRow_Attribute->FK_AttributeType_get(),pRow_Attribute->PK_Attribute_get(),pRow_Attribute->Name_get()) );
			}
		}
		PK_Disc = AddIdentifiedDiscToDB(PK_MediaType,sCDDBID,listMediaAttribute_);
	}

	int Tracks;
	FixMediaAttributes(listMediaAttribute_,Tracks);
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Parse_CDDB_Media_ID done");
	return PK_Disc;
}

bool MediaAttributes_LowLevel::FixMediaAttributes(listMediaAttribute &listMediaAttribute_,int &Tracks)
{
	bool bOK=true;
	Tracks=0;
	map<string,int> mapSongs;
	for(listMediaAttribute::iterator it=listMediaAttribute_.begin();it!=listMediaAttribute_.end();++it)
	{
		MediaAttribute *pMediaAttribute = *it;
		pMediaAttribute->m_sName = FileUtils::ValidFileName(pMediaAttribute->m_sName);
		if( pMediaAttribute->m_Title_Track>Tracks )
			Tracks=pMediaAttribute->m_Title_Track;

		if( pMediaAttribute->m_PK_AttributeType==ATTRIBUTETYPE_Title_CONST && pMediaAttribute->m_Title_Track )
		{
			// Be sure the same song isn't on the same disc twice with the same name, since it will cause
			// a problem when we want to rip
			int Track = mapSongs[pMediaAttribute->m_sName];
			if( Track && Track!=pMediaAttribute->m_Title_Track )  // It's here twice
				pMediaAttribute->m_sName = pMediaAttribute->m_sName + " (" + StringUtils::itos(pMediaAttribute->m_Title_Track) + ")";
			mapSongs[pMediaAttribute->m_sName]=pMediaAttribute->m_Title_Track;
		}
	}

	// This has tracks and songs, be sure we have the required information
	if( Tracks && mapSongs.size() )
	{
		map<int,bool> mapTracks; // confirm which ones are here
		for(map<string,int>::iterator it=mapSongs.begin();it!=mapSongs.end();++it)
			mapTracks[it->second]=true;

		if( Tracks != int(mapTracks.size()) )
		{
			string sMissing;
			for(int i=1;i<=Tracks;++i)
				if( !mapTracks[i] )
				{
					listMediaAttribute_.push_back( new MediaAttribute(i,0,ATTRIBUTETYPE_Title_CONST,0,"Track " + StringUtils::itos(i)) );
					sMissing += StringUtils::itos(i) + ",";
				}
			
			bOK=false;
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"MediaAttributes_LowLevel::FixMediaAttributes missing song info for %s",sMissing.c_str());
		}
	}
	return bOK;
}

int MediaAttributes_LowLevel::IsDiscAlreadyIdentified(string sIdentifiedDisc,listMediaAttribute &listMediaAttribute_)
{
	vector<Row_Disc *> vectRow_Disc;
	m_pDatabase_pluto_media->Disc_get()->GetRows("ID='" + StringUtils::SQLEscape(sIdentifiedDisc) + "'",&vectRow_Disc);
	if( vectRow_Disc.size()==0 )
		return 0;

	Row_Disc *pRow_Disc = vectRow_Disc[0];
	return IsDiscAlreadyIdentified(pRow_Disc,listMediaAttribute_);
}

int MediaAttributes_LowLevel::IsDiscAlreadyIdentified(int PK_Device,int Slot,listMediaAttribute &listMediaAttribute_)
{
	vector<Row_DiscLocation *> vectRow_DiscLocation;
	m_pDatabase_pluto_media->DiscLocation_get()->GetRows("EK_Device=" + StringUtils::itos(PK_Device) + " AND Slot=" + StringUtils::itos(Slot),&vectRow_DiscLocation);
	if( vectRow_DiscLocation.size()==0 )
		return 0;

	Row_DiscLocation *pRow_DiscLocation = vectRow_DiscLocation[0];
	return IsDiscAlreadyIdentified(pRow_DiscLocation->FK_Disc_getrow(),listMediaAttribute_);
}

int MediaAttributes_LowLevel::IsDiscAlreadyIdentified(Row_Disc *pRow_Disc,listMediaAttribute &listMediaAttribute_)
{
	vector<Row_Disc_Attribute *> vectRow_Disc_Attribute;
	pRow_Disc->Disc_Attribute_FK_Disc_getrows(&vectRow_Disc_Attribute);

	if( vectRow_Disc_Attribute.size()<2 )
		return 0; // If it was identified successfully, we'd have at least 2 entries.  It could have been a failed identify before, so we'll retry
	for(size_t s=0;s<vectRow_Disc_Attribute.size();++s)
	{
		Row_Disc_Attribute *pRow_Disc_Attribute = vectRow_Disc_Attribute[s];
		Row_Attribute *pRow_Attribute = pRow_Disc_Attribute->FK_Attribute_getrow();
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Already in DB %p %p %d",pRow_Disc_Attribute,pRow_Attribute,(pRow_Attribute ? pRow_Attribute->PK_Attribute_get() : 0));
		if( !pRow_Attribute )
			continue; // Should never happen
		listMediaAttribute_.push_back( new MediaAttribute(
			pRow_Disc_Attribute->Track_get(),pRow_Disc_Attribute->Section_get(),
			pRow_Attribute->FK_AttributeType_get(),pRow_Attribute->PK_Attribute_get(),pRow_Attribute->Name_get()) );
	}
	return pRow_Disc->PK_Disc_get();
}

MediaAttribute *MediaAttributes_LowLevel::GetMediaAttribute(int Title_Track,int Section,int PK_AttributeType,listMediaAttribute &listMediaAttribute_)
{
	for(listMediaAttribute::iterator it=listMediaAttribute_.begin();it!=listMediaAttribute_.end();++it)
	{
		MediaAttribute *pMediaAttribute = *it;
		if( pMediaAttribute->m_Title_Track==Title_Track &&
				pMediaAttribute->m_Section==Section &&
				pMediaAttribute->m_PK_AttributeType==PK_AttributeType )
			return pMediaAttribute;
	}
	return NULL;
}

int MediaAttributes_LowLevel::GetMaxTracks(listMediaAttribute &listMediaAttribute_)
{
	int Tracks=0;
	for(listMediaAttribute::iterator it=listMediaAttribute_.begin();it!=listMediaAttribute_.end();++it)
	{
		MediaAttribute *pMediaAttribute = *it;
		if( pMediaAttribute->m_Title_Track>Tracks )
			Tracks = pMediaAttribute->m_Title_Track;
	}
	return Tracks;
}

int MediaAttributes_LowLevel::AddIdentifiedDiscToDB(int PK_MediaType,string sIdentifiedDisc,listMediaAttribute &listMediaAttribute_)
{
	Row_Disc *pRow_Disc;
	
	vector<Row_Disc *> vectRow_Disc;
	m_pDatabase_pluto_media->Disc_get()->GetRows("ID='" + StringUtils::SQLEscape(sIdentifiedDisc) + "'",&vectRow_Disc);

	size_t Existing = vectRow_Disc.size();

	if( Existing )
		pRow_Disc = vectRow_Disc[0];
	else
	{
		pRow_Disc = m_pDatabase_pluto_media->Disc_get()->AddRow();
		pRow_Disc->ID_set(sIdentifiedDisc);
		pRow_Disc->EK_MediaType_set(PK_MediaType);
		pRow_Disc->Table_Disc_get()->Commit();
	}

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "MediaAttributes_LowLevel::AddIdentifiedDiscToDB ID %s/%d existing %d num attr %d",
		sIdentifiedDisc.c_str(),pRow_Disc->PK_Disc_get(),Existing,(int) listMediaAttribute_.size());

	for(listMediaAttribute::iterator it=listMediaAttribute_.begin();it!=listMediaAttribute_.end();++it)
	{
		MediaAttribute *pMediaAttribute = *it;
		Row_Attribute *pRow_Attribute ;
		if( !pMediaAttribute->m_PK_Attribute )
		{
			int PK_Attribute_Performer=0;
			if( pMediaAttribute->m_PK_AttributeType==ATTRIBUTETYPE_Album_CONST )
			{
				for(listMediaAttribute::iterator it=listMediaAttribute_.begin();it!=listMediaAttribute_.end();++it)
				{
					MediaAttribute *pMediaAttributePerformer = *it;
					if( pMediaAttributePerformer->m_PK_AttributeType==ATTRIBUTETYPE_Album_CONST )
					{
						PK_Attribute_Performer = pMediaAttributePerformer->m_PK_Attribute;
						break;
					}
				}
			}
			pRow_Attribute = GetAttributeFromDescription(PK_MediaType,pMediaAttribute->m_PK_AttributeType,pMediaAttribute->m_sName,PK_Attribute_Performer);
			if( pRow_Attribute )
				pMediaAttribute->m_PK_Attribute=pRow_Attribute->PK_Attribute_get();
		}
		else
			pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(pMediaAttribute->m_PK_Attribute);
		if( !pRow_Attribute )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MediaAttributes_LowLevel::AddIdentifiedDiscToDB now can't find attribute %d",pMediaAttribute->m_PK_Attribute);
			continue;
		}
		Row_Disc_Attribute *pRow_Disc_Attribute = m_pDatabase_pluto_media->Disc_Attribute_get()->GetRow(pRow_Disc->PK_Disc_get(),pRow_Attribute->PK_Attribute_get(),pMediaAttribute->m_Title_Track,pMediaAttribute->m_Section);

		LoggerWrapper::GetInstance()->Write( LV_STATUS, "MediaAttributes_LowLevel::AddIdentifiedDiscToDB PK_Disc %d PK_Attribute %d Existing %p",
			pRow_Disc->PK_Disc_get(),pRow_Attribute->PK_Attribute_get(),pRow_Disc_Attribute);
	
		if( !pRow_Disc_Attribute )
		{
			pRow_Disc_Attribute = m_pDatabase_pluto_media->Disc_Attribute_get()->AddRow();
			pRow_Disc_Attribute->FK_Disc_set(pRow_Disc->PK_Disc_get());
			pRow_Disc_Attribute->FK_Attribute_set(pRow_Attribute->PK_Attribute_get()); 
			pRow_Disc_Attribute->Track_set(pMediaAttribute->m_Title_Track);
			pRow_Disc_Attribute->Section_set(pMediaAttribute->m_Section);
			pRow_Disc_Attribute->Table_Disc_Attribute_get()->Commit();
		}
	}

	return pRow_Disc->PK_Disc_get();
}

int MediaAttributes_LowLevel::AddIdentifiedFileToDB(int PK_MediaType,int PK_File,listMediaAttribute &listMediaAttribute_)
{
	Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(PK_File);
	if( !pRow_File )
		return 0; // Shouldnt happen

	for(listMediaAttribute::iterator it=listMediaAttribute_.begin();it!=listMediaAttribute_.end();++it)
	{
		MediaAttribute *pMediaAttribute = *it;
		Row_Attribute *pRow_Attribute ;
		if( !pMediaAttribute->m_PK_Attribute )
		{
			int PK_Attribute_Performer=0;
			if( pMediaAttribute->m_PK_AttributeType==ATTRIBUTETYPE_Album_CONST )
			{
				for(listMediaAttribute::iterator it=listMediaAttribute_.begin();it!=listMediaAttribute_.end();++it)
				{
					MediaAttribute *pMediaAttributePerformer = *it;
					if( pMediaAttributePerformer->m_PK_AttributeType==ATTRIBUTETYPE_Album_CONST )
					{
						PK_Attribute_Performer = pMediaAttributePerformer->m_PK_Attribute;
						break;
					}
				}
			}
			pRow_Attribute = GetAttributeFromDescription(PK_MediaType,pMediaAttribute->m_PK_AttributeType,pMediaAttribute->m_sName,PK_Attribute_Performer);
			if( pRow_Attribute )
				pMediaAttribute->m_PK_Attribute=pRow_Attribute->PK_Attribute_get();
		}
		else
			pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(pMediaAttribute->m_PK_Attribute);
		if( !pRow_Attribute )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MediaAttributes_LowLevel::AddIdentifiedFileToDB now can't find attribute %d",pMediaAttribute->m_PK_Attribute);
			continue;
		}
		Row_File_Attribute *pRow_File_Attribute = m_pDatabase_pluto_media->File_Attribute_get()->GetRow(pRow_File->PK_File_get(),pRow_Attribute->PK_Attribute_get(),pMediaAttribute->m_Title_Track,pMediaAttribute->m_Section);
		if( !pRow_File_Attribute )
		{
			pRow_File_Attribute = m_pDatabase_pluto_media->File_Attribute_get()->AddRow();
			pRow_File_Attribute->FK_File_set(pRow_File->PK_File_get());
			pRow_File_Attribute->FK_Attribute_set(pRow_Attribute->PK_Attribute_get()); 
			pRow_File_Attribute->Track_set(pMediaAttribute->m_Title_Track);
			pRow_File_Attribute->Section_set(pMediaAttribute->m_Section);
			pRow_File_Attribute->Table_File_Attribute_get()->Commit();
		}
	}

	return pRow_File->PK_File_get();
}

int MediaAttributes_LowLevel::AddPictureToDisc(int PK_Disc,char *pPictureData,size_t sizePicture,string sURL)
{
	Row_Disc *pRow_Disc = m_pDatabase_pluto_media->Disc_get()->GetRow(PK_Disc);
	if( pPictureData && sizePicture && pRow_Disc )
	{
		Row_Picture *pRow_Picture = AddPicture(pPictureData, int(sizePicture), "jpg",sURL);
		if( !pRow_Picture )
			return 0;

		AddPictureToDisc(pRow_Disc->PK_Disc_get(),pRow_Picture->PK_Picture_get());

		// Find all attributes that we added that identify this disc and need the same picture
		string sWhere="SELECT Attribute.* FROM Attribute"
			" JOIN Disc_Attribute ON FK_Attribute=PK_Attribute"
			" JOIN Disc ON FK_Disc=PK_Disc"
			" JOIN MediaType_AttributeType ON Attribute.FK_AttributeType=MediaType_AttributeType.FK_AttributeType"
			" AND MediaType_AttributeType.EK_MediaType=" + StringUtils::itos(pRow_Disc->EK_MediaType_get()==MEDIATYPE_pluto_CD_CONST ? MEDIATYPE_pluto_StoredAudio_CONST : pRow_Disc->EK_MediaType_get()) +
			" WHERE PK_Disc=" + StringUtils::itos(pRow_Disc->PK_Disc_get()) + " AND Identifier=1";
		vector<Row_Attribute *> vectRow_Attribute;
		m_pDatabase_pluto_media->Attribute_get()->GetRows(sWhere,&vectRow_Attribute);
		for(size_t s=0;s<vectRow_Attribute.size();++s)
		{
			Row_Attribute *pRow_Attribute = vectRow_Attribute[s];
			AddPictureToAttribute(pRow_Picture->PK_Picture_get(),pRow_Attribute->PK_Attribute_get());
		}
		return pRow_Picture->PK_Picture_get();
	}
	return 0;
}

int MediaAttributes_LowLevel::AddPictureToFile(int PK_File,char *pPictureData,size_t sizePicture,string sURL)
{
	Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(PK_File);
	if( pPictureData && sizePicture && pRow_File )
	{
		Row_Picture *pRow_Picture = AddPicture(pPictureData, int(sizePicture), "jpg",sURL);
		if( !pRow_Picture )
			return 0;

		AddPictureToFile(pRow_File->PK_File_get(),pRow_Picture->PK_Picture_get());

		// Find all attributes that we added that identify this File and need the same picture
		string sWhere="SELECT Attribute.* FROM Attribute"
			" JOIN File_Attribute ON FK_Attribute=PK_Attribute"
			" JOIN File ON FK_File=PK_File"
			" JOIN MediaType_AttributeType ON Attribute.FK_AttributeType=MediaType_AttributeType.FK_AttributeType"
			" AND MediaType_AttributeType.EK_MediaType=" + StringUtils::itos(pRow_File->EK_MediaType_get()==MEDIATYPE_pluto_CD_CONST ? MEDIATYPE_pluto_StoredAudio_CONST : pRow_File->EK_MediaType_get()) +
			" WHERE PK_File=" + StringUtils::itos(pRow_File->PK_File_get()) + " AND Identifier=1";
		vector<Row_Attribute *> vectRow_Attribute;
		m_pDatabase_pluto_media->Attribute_get()->GetRows(sWhere,&vectRow_Attribute);
		for(size_t s=0;s<vectRow_Attribute.size();++s)
		{
			Row_Attribute *pRow_Attribute = vectRow_Attribute[s];
			AddPictureToAttribute(pRow_Picture->PK_Picture_get(),pRow_Attribute->PK_Attribute_get());
		}
		return pRow_Picture->PK_Picture_get();
	}
	return 0;
}

Row_File *MediaAttributes_LowLevel::AddDirectoryToDatabase(int PK_MediaType,string sDirectory)
{
	vector<Row_File *> vectRow_File;
	m_pDatabase_pluto_media->File_get()->GetRows("Path='" + StringUtils::SQLEscape(FileUtils::ExcludeTrailingSlash(FileUtils::BasePath(sDirectory))) +
		"' AND Filename='" + StringUtils::SQLEscape(FileUtils::FilenameWithoutPath(sDirectory)) + "' AND IsDirectory=1",
		&vectRow_File);

	if( vectRow_File.size()>0 )
		return vectRow_File[0];

	int nEK_Users_Private = GetOwnerForPath(FileUtils::ExcludeTrailingSlash(FileUtils::BasePath(sDirectory)));

	Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->AddRow();
	pRow_File->DateAdded_set(StringUtils::SQLDateTime(time(NULL)));
	pRow_File->EK_MediaType_set(PK_MediaType);
	pRow_File->Path_set(FileUtils::ExcludeTrailingSlash(FileUtils::BasePath(sDirectory)));
	pRow_File->Filename_set( FileUtils::FilenameWithoutPath(sDirectory) );
	pRow_File->IsDirectory_set(1);
	pRow_File->Source_set("F");
	pRow_File->INode_set( FileUtils::GetInode( pRow_File->Path_get() + "/" + pRow_File->Filename_get() ) );

	if(nEK_Users_Private != 0)
		pRow_File->EK_Users_Private_set(nEK_Users_Private);
	else
		pRow_File->EK_Users_Private_setNull(true);

	m_pDatabase_pluto_media->File_get()->Commit();

	LoggerWrapper::GetInstance()->Write( LV_STATUS, "MediaAttributes_LowLevel::AddDirectoryToDatabase %s PK_File %d Inode %d",
		(pRow_File->Path_get() + "/" + pRow_File->Filename_get()).c_str(), pRow_File->PK_File_get(), pRow_File->INode_get() );

	// See what is 1 level up, and be sure it's in there too, as long as we're not at the top of the 'data' tree
	string sParent = FileUtils::BasePath(sDirectory);
	string sParentDirectory = FileUtils::BasePath(sParent);
	if( sParentDirectory.length()>5 && sParentDirectory.substr( sParentDirectory.length()-4 )!="data" )
		AddDirectoryToDatabase(PK_MediaType,sParent);

	return pRow_File;
}

int MediaAttributes_LowLevel::AddRippedDiscToDatabase(int PK_Disc,int PK_MediaType,string sDestination,string sTracks)
{
	int PK_File=0;
	if(FileUtils::DirExists(sDestination))
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes_LowLevel::AddRippedDiscToDatabase %s is a dir: %s",sDestination.c_str(),sTracks.c_str());
#endif
		Row_File *pRow_File = AddDirectoryToDatabase(PK_MediaType==MEDIATYPE_pluto_CD_CONST ? MEDIATYPE_pluto_StoredAudio_CONST : PK_MediaType,sDestination);
		PK_File = pRow_File->PK_File_get();
		AddDiscAttributesToFile(pRow_File->PK_File_get(),PK_Disc,-1);  // Track ==0
		vector<Row_Disc_Attribute *> vectRow_Disc_Attribute;
		m_pDatabase_pluto_media->Disc_Attribute_get()->GetRows(
			"JOIN Attribute ON FK_Attribute=PK_Attribute WHERE FK_Disc=" + StringUtils::itos(PK_Disc) + 
			" AND FK_AttributeType=" + StringUtils::itos(ATTRIBUTETYPE_Disc_ID_CONST),&vectRow_Disc_Attribute);
		Row_Disc_Attribute *pRow_Disc_Attribute_DiscID=NULL;
		if( vectRow_Disc_Attribute.size()>0 )
			pRow_Disc_Attribute_DiscID = vectRow_Disc_Attribute[0];

		string::size_type pos=0;
		while( pos<sTracks.size() )
		{
			// Now get all the tracks
			string s = StringUtils::Tokenize(sTracks,"|",pos);
			string::size_type p2=0;
			int iTrack = atoi(StringUtils::Tokenize(s,",",p2).c_str());
			string sTrackName;
			if( p2<s.size() && p2!=string::npos )
				sTrackName = s.substr(p2);
			
			// There should be a '.lock' file
			list<string> listFiles;
			FileUtils::FindFiles(listFiles,sDestination,sTrackName + ".*.lock");
			if( listFiles.size()!=1 )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find ripped track lock file: %s/%s",sDestination.c_str(),sTrackName.c_str());
				continue;
			}
			string sLockFile = listFiles.front();
			listFiles.clear();
			FileUtils::FindFiles(listFiles,sDestination,FileUtils::FileWithoutExtension(sLockFile)); // Now find the file without the lock
			if( listFiles.size()!=1 )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find ripped track for lock file: %s",sLockFile.c_str());
				continue;
			}
			string sRippedFile = listFiles.front();

			vector<Row_File *> vectRow_File;
			m_pDatabase_pluto_media->File_get()->GetRows("Path='" + StringUtils::SQLEscape(FileUtils::ExcludeTrailingSlash(sDestination)) +
				"' AND Filename='" + StringUtils::SQLEscape(FileUtils::FilenameWithoutPath(sRippedFile)) + "'",
				&vectRow_File);

			int nEK_Users_Private = GetOwnerForPath(FileUtils::ExcludeTrailingSlash(sDestination));

			Row_File *pRow_File = NULL;
			if(vectRow_File.size() > 0)
			{
				pRow_File = vectRow_File[0];
				pRow_File->Reload();
			}
			else
				pRow_File = m_pDatabase_pluto_media->File_get()->AddRow();

			if( PK_MediaType==MEDIATYPE_pluto_CD_CONST )
				pRow_File->EK_MediaType_set(MEDIATYPE_pluto_StoredAudio_CONST);
			else
				pRow_File->EK_MediaType_set(PK_MediaType);
			pRow_File->DateAdded_set(StringUtils::SQLDateTime(time(NULL)));
			pRow_File->Path_set(FileUtils::ExcludeTrailingSlash(sDestination));
			pRow_File->Filename_set( sRippedFile );
			pRow_File->Source_set("F");
			pRow_File->INode_set( FileUtils::GetInode( pRow_File->Path_get() + "/" + pRow_File->Filename_get() ) );

			if(nEK_Users_Private != 0)
				pRow_File->EK_Users_Private_set(nEK_Users_Private);
			else
				pRow_File->EK_Users_Private_setNull(true);

			m_pDatabase_pluto_media->File_get()->Commit();

			LoggerWrapper::GetInstance()->Write( LV_STATUS, "MediaAttributes_LowLevel::AddRippedDiscToDatabase %s PK_File %d Inode %d",
				(pRow_File->Path_get() + "/" + pRow_File->Filename_get()).c_str(), pRow_File->PK_File_get(), pRow_File->INode_get() );

#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes_LowLevel::AddRippedDiscToDatabase %s calling AddDiscAttributesToFile for %s",sDestination.c_str(),s.c_str());
#endif
			AddDiscAttributesToFile(pRow_File->PK_File_get(),PK_Disc,iTrack);

			// Be sure the disc id is associated with each track
			if( pRow_Disc_Attribute_DiscID )
			{
				Row_File_Attribute *pRow_File_Attribute = m_pDatabase_pluto_media->File_Attribute_get()->GetRow(
					pRow_File->PK_File_get(),pRow_Disc_Attribute_DiscID->FK_Attribute_get(),
					iTrack,0);
				if( !pRow_File_Attribute )
				{
					pRow_File_Attribute = m_pDatabase_pluto_media->File_Attribute_get()->AddRow();
					pRow_File_Attribute->FK_File_set(pRow_File->PK_File_get());
					pRow_File_Attribute->FK_Attribute_set(pRow_Disc_Attribute_DiscID->FK_Attribute_get());
					pRow_File_Attribute->Section_set(0);
					pRow_File_Attribute->Track_set(iTrack);
					m_pDatabase_pluto_media->File_Attribute_get()->Commit();
				}
			}

                        // Ensure that File track number is set
                        if (iTrack>0)
                        {
#ifdef DEBUG
                          LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes_LowLevel::AddRippedDiscToDatabase setting track number for %s to %i",s.c_str(), iTrack);
#endif
                          vector<Row_File_Attribute *> vFileAttrs;
                          m_pDatabase_pluto_media->File_Attribute_get()->GetRows("JOIN Attribute ON FK_Attribute=PK_Attribute WHERE FK_AttributeType=" + StringUtils::itos(ATTRIBUTETYPE_Track_CONST) + " AND FK_File=" + StringUtils::itos(pRow_File->PK_File_get()),&vFileAttrs);
                          if (vFileAttrs.size()==0)
                          {
#ifdef DEBUG
                            LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes_LowLevel::AddRippedDiscToDatabase - no previous attributes found, adding new one");
#endif
                            Row_Attribute *pAttr = m_pDatabase_pluto_media->Attribute_get()->AddRow();
                            pAttr->Name_set(StringUtils::itos(iTrack));
                            pAttr->FK_AttributeType_set(ATTRIBUTETYPE_Track_CONST);
                            m_pDatabase_pluto_media->Attribute_get()->Commit();

                            Row_File_Attribute *pRow_File_Attribute = m_pDatabase_pluto_media->File_Attribute_get()->AddRow();
                            pRow_File_Attribute->FK_File_set(pRow_File->PK_File_get());
                            pRow_File_Attribute->FK_Attribute_set(pAttr->PK_Attribute_get());
                            m_pDatabase_pluto_media->File_Attribute_get()->Commit();
                          }
                          else
                          {
#ifdef DEBUG
                            LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes_LowLevel::AddRippedDiscToDatabase - not modifying existing attribute");
#endif
                          }
                        }

			FileUtils::DelFile(sDestination + "/" + sLockFile);
		}
	}
	else
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes_LowLevel::AddRippedDiscToDatabase %s is not a dir",sDestination.c_str());
#endif
		string sFileNameBase = FileUtils::FilenameWithoutPath(sDestination);
		sDestination = FileUtils::BasePath(sDestination);
		// It's not a directory, so it should be a file
		// There should be a '.lock' file
		list<string> listFiles;
		FileUtils::FindFiles(listFiles,sDestination,sFileNameBase + ".*.lock");
#ifndef WIN32  // For testing under windows
		if( listFiles.size()!=1 )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find ripped disc lock file: %s / %s %d",sDestination.c_str(),sFileNameBase.c_str(),(int) listFiles.size());
			return 0;
		}
		string sLockFile = listFiles.front();
#else
		string sLockFile = sDestination + "/" + sFileNameBase + ".dvd.lock";
#endif

		listFiles.clear();
		FileUtils::FindFiles(listFiles,sDestination,FileUtils::FileWithoutExtension(sLockFile)); // Now find the file without the lock
#ifndef WIN32 // For testing
		if( listFiles.size()!=1 )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find ripped disc for lock file: %s",sLockFile.c_str());
			return 0;
		}
		string sRippedFile = listFiles.front();
#else
		StringUtils::Replace(&sDestination, "\\", "/"); // replacing all the \ in a windows path with /
		StringUtils::Replace(&sFileNameBase, "\\", "/"); // replacing all the \ in a windows path with /
		string sRippedFile = sDestination + "/" + sFileNameBase + ".dvd";
#endif

#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes_LowLevel::AddRippedDiscToDatabase %s is not a dir, calling AddDirectoryToDatabase",sDestination.c_str());
#endif
		AddDirectoryToDatabase(PK_MediaType,FileUtils::BasePath(sDestination));

		vector<Row_File *> vectRow_File;
		m_pDatabase_pluto_media->File_get()->GetRows("Path='" + StringUtils::SQLEscape(FileUtils::ExcludeTrailingSlash(sDestination)) +
			"' AND Filename='" + StringUtils::SQLEscape(FileUtils::FilenameWithoutPath(sRippedFile)) + "'",
			&vectRow_File);

		Row_File *pRow_File = NULL;
		if(vectRow_File.size() > 0)
		{
 			pRow_File = vectRow_File[0];
			pRow_File->Reload();
		}
		else
			pRow_File = m_pDatabase_pluto_media->File_get()->AddRow();

		int nEK_Users_Private = GetOwnerForPath(FileUtils::ExcludeTrailingSlash(sDestination));

		pRow_File->DateAdded_set(StringUtils::SQLDateTime(time(NULL)));
		pRow_File->EK_MediaType_set(PK_MediaType);
		pRow_File->Path_set(FileUtils::ExcludeTrailingSlash(sDestination));
		pRow_File->Filename_set( FileUtils::FilenameWithoutPath(sRippedFile) );
		pRow_File->Source_set("F");
		pRow_File->INode_set( FileUtils::GetInode( pRow_File->Path_get() + "/" + pRow_File->Filename_get() ) );

		if(nEK_Users_Private != 0)
			pRow_File->EK_Users_Private_set(nEK_Users_Private);
		else
			pRow_File->EK_Users_Private_setNull(true);

		m_pDatabase_pluto_media->File_get()->Commit();

		PK_File = pRow_File->PK_File_get();

		LoggerWrapper::GetInstance()->Write( LV_STATUS, "MediaAttributes_LowLevel::AddRippedDiscToDatabase %s PK_File %d Inode %d",
			(pRow_File->Path_get() + "/" + pRow_File->Filename_get()).c_str(), pRow_File->PK_File_get(), pRow_File->INode_get() );

#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes_LowLevel::AddRippedDiscToDatabase %s is not a dir, calling AddDiscAttributesToFile",sDestination.c_str());
#endif
		AddDiscAttributesToFile(pRow_File->PK_File_get(),PK_Disc,-1);  // We won't have tracks then we ripped.  -1=ripped whole thing
		FileUtils::DelFile(sDestination + "/" + sLockFile);
	}
	return PK_File;
}

void MediaAttributes_LowLevel::AddDiscAttributesToFile(int PK_File,int PK_Disc,int Track)
{
	if( !PK_Disc )
		return; // It's not a known disc
	Row_Disc *pRow_Disc = m_pDatabase_pluto_media->Disc_get()->GetRow( PK_Disc );
	Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow( PK_File );
	if( !pRow_Disc || !pRow_File )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MediaAttributes_LowLevel::AddDiscAttributesToFile called with missing file %d disc %d",PK_File,PK_Disc);
		return;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes_LowLevel::AddDiscAttributesToFile called with file %d disc %d",PK_File,PK_Disc);

	vector<Row_Picture_Disc *> vectRow_Picture_Disc;
	pRow_Disc->Picture_Disc_FK_Disc_getrows(&vectRow_Picture_Disc);
	for(size_t s=0;s<vectRow_Picture_Disc.size();++s)
	{
		// Check if it's already there in case this disc was already added
		Row_Picture_File *pRow_Picture_File = m_pDatabase_pluto_media->Picture_File_get()->GetRow(
			vectRow_Picture_Disc[s]->FK_Picture_get(),PK_File);
		if( !pRow_Picture_File )
		{
			pRow_Picture_File=m_pDatabase_pluto_media->Picture_File_get()->AddRow();
			pRow_Picture_File->FK_File_set(PK_File);
			pRow_Picture_File->FK_Picture_set( vectRow_Picture_Disc[s]->FK_Picture_get() );
			m_pDatabase_pluto_media->Picture_File_get()->Commit();
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes_LowLevel::AddDiscAttributesToFile called with file %d disc %d PIC %d",
			PK_File,PK_Disc,pRow_Picture_File->FK_Picture_get());
	}

	vector<Row_Disc_Attribute *> vectRow_Disc_Attribute;
	string sWhere = "FK_Disc=" + StringUtils::itos(PK_Disc);
	if( Track!=-1 )
	{
		sWhere += " AND Track=" + StringUtils::itos(Track);
		pRow_Disc->FK_File_set(pRow_File->PK_File_get());  // The discs has been ripped to this file
		m_pDatabase_pluto_media->Disc_get()->Commit();
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes_LowLevel::AddDiscAttributesToFile where condition %s", sWhere.c_str());

	m_pDatabase_pluto_media->Disc_Attribute_get()->GetRows(sWhere,&vectRow_Disc_Attribute);

	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes_LowLevel::AddDiscAttributesToFile Disc_Attribute_get num of rows %d", vectRow_Disc_Attribute.size());

	for(size_t s=0;s<vectRow_Disc_Attribute.size();++s)
	{
		// Check if it's already there in case this disc was already added
		Row_File_Attribute *pRow_File_Attribute = m_pDatabase_pluto_media->File_Attribute_get()->GetRow(
			PK_File,vectRow_Disc_Attribute[s]->FK_Attribute_get(),
			vectRow_Disc_Attribute[s]->Track_get(),vectRow_Disc_Attribute[s]->Section_get() );

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes_LowLevel::AddDiscAttributesToFile Searching for following record in File_Attribute: "
			"attribute %d, file %d, track %d, section %d -- found %p", 
			vectRow_Disc_Attribute[s]->FK_Attribute_get(), PK_File, 
			vectRow_Disc_Attribute[s]->Track_get(),vectRow_Disc_Attribute[s]->Section_get(), 
			pRow_File_Attribute);

		if( !pRow_File_Attribute )
		{
			pRow_File_Attribute = m_pDatabase_pluto_media->File_Attribute_get()->AddRow();
			pRow_File_Attribute->FK_File_set(PK_File);
			pRow_File_Attribute->FK_Attribute_set( vectRow_Disc_Attribute[s]->FK_Attribute_get() );
			pRow_File_Attribute->Track_set( vectRow_Disc_Attribute[s]->Track_get() );
			pRow_File_Attribute->Section_set( vectRow_Disc_Attribute[s]->Section_get() );
			m_pDatabase_pluto_media->File_Attribute_get()->Commit();

			LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes_LowLevel::AddDiscAttributesToFile adding the following record in File_Attribute: "
				"attribute %d, file %d, track %d, section %d", 
				vectRow_Disc_Attribute[s]->FK_Attribute_get(), PK_File, 
				vectRow_Disc_Attribute[s]->Track_get(),vectRow_Disc_Attribute[s]->Section_get());
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"MediaAttributes_LowLevel::AddDiscAttributesToFile NOT copying attribute %d from disc to file, already exists", vectRow_Disc_Attribute[s]->FK_Attribute_get());
		}
	}
}

string MediaAttributes_LowLevel::GetDefaultDescriptionForMediaFile(MediaFile *pMediaFile)
{
	string sSQL;
	
	if( pMediaFile->m_dwPK_Disk )
		sSQL = "SELECT FK_AttributeType,Attribute.Name FROM Disc_Attribute JOIN Attribute ON FK_Attribute=PK_Attribute WHERE FK_Disc = "
			+ StringUtils::itos(pMediaFile->m_dwPK_Disk) + " AND Track=0 AND Section=0 AND FK_AttributeType IN ("
			TOSTRING(ATTRIBUTETYPE_Title_CONST) "," TOSTRING(ATTRIBUTETYPE_Performer_CONST) "," TOSTRING(ATTRIBUTETYPE_Album_CONST) ")";
	else
		sSQL = "SELECT FK_AttributeType,Attribute.Name FROM File_Attribute JOIN Attribute ON FK_Attribute=PK_Attribute WHERE FK_File = "
			+ StringUtils::itos(pMediaFile->m_dwPK_File) + " AND Track=0 AND Section=0 AND FK_AttributeType IN ("
			TOSTRING(ATTRIBUTETYPE_Title_CONST) "," TOSTRING(ATTRIBUTETYPE_Performer_CONST) "," TOSTRING(ATTRIBUTETYPE_Album_CONST) ")";
		
    PlutoSqlResult result;
    DB_ROW row;
	string sAlbum,sPerformer;
    if( ( result.r=m_pDatabase_pluto_media->db_wrapper_query_result( sSQL ) ) )
	{
		while ( row=db_wrapper_fetch_row( result.r ) )
		{
			switch( atoi(row[0]) )
			{
			case ATTRIBUTETYPE_Title_CONST:
				return row[1]; // This has highest priority
			case ATTRIBUTETYPE_Performer_CONST:
				sPerformer = row[1];
			case ATTRIBUTETYPE_Album_CONST:
				sAlbum = row[1];
			}
		}
	}
	if( sPerformer.size() )  // Performer has higher priority than album
		return sPerformer;
	else if( sAlbum.size() )
		return sAlbum;
	else
		return pMediaFile->m_sFilename;
}


int MediaAttributes_LowLevel::GetAttributeFromFile(int PK_File,int PK_AttributeType,string &sAttribute)
{
	string sSQL = "JOIN File_Attribute ON FK_Attribute=PK_Attribute WHERE FK_AttributeType=" + StringUtils::itos(PK_AttributeType) + " and FK_File=" + StringUtils::itos(PK_File) + " limit 1";
	vector<Row_Attribute *> vectRow_Attribute;
	m_pDatabase_pluto_media->Attribute_get()->GetRows(sSQL,&vectRow_Attribute);
	if( vectRow_Attribute.empty() )
		return 0;

	sAttribute = vectRow_Attribute[0]->Name_get();
	return vectRow_Attribute[0]->PK_Attribute_get();
}

int MediaAttributes_LowLevel::GetAttributeFromDisc(int PK_Disc,int PK_AttributeType,string &sAttribute)
{
	string sSQL = "JOIN Disc_Attribute ON FK_Attribute=PK_Attribute WHERE FK_AttributeType=" + StringUtils::itos(PK_AttributeType) + " and FK_Disc=" + StringUtils::itos(PK_Disc) + " limit 1";
	vector<Row_Attribute *> vectRow_Attribute;
	m_pDatabase_pluto_media->Attribute_get()->GetRows(sSQL,&vectRow_Attribute);
	if( vectRow_Attribute.empty() )
		return 0;

	sAttribute = vectRow_Attribute[0]->Name_get();
	return vectRow_Attribute[0]->PK_Attribute_get();
}

void MediaAttributes_LowLevel::AddAttributeToFile(Row_File *pRow_File,Row_Attribute *pRow_Attribute,int Track,int Section)
{
	Row_File_Attribute *pRow_File_Attribute = m_pDatabase_pluto_media->File_Attribute_get()->GetRow(pRow_File->PK_File_get(),pRow_Attribute->PK_Attribute_get(),Track,Section);
	if( !pRow_File_Attribute )
	{
		pRow_File_Attribute = m_pDatabase_pluto_media->File_Attribute_get()->AddRow();
		pRow_File_Attribute->FK_File_set(pRow_File->PK_File_get());
		pRow_File_Attribute->FK_Attribute_set(pRow_Attribute->PK_Attribute_get());
		pRow_File_Attribute->Track_set(Track);
		pRow_File_Attribute->Section_set(Section);
		m_pDatabase_pluto_media->File_Attribute_get()->Commit();
	}
}

void MediaAttributes_LowLevel::AddAttributeToDisc(Row_Disc *pRow_Disc,Row_Attribute *pRow_Attribute,int Track,int Section)
{
	Row_Disc_Attribute *pRow_Disc_Attribute = m_pDatabase_pluto_media->Disc_Attribute_get()->GetRow(pRow_Disc->PK_Disc_get(),pRow_Attribute->PK_Attribute_get(),Track,Section);
	if( !pRow_Disc_Attribute )
	{
		pRow_Disc_Attribute = m_pDatabase_pluto_media->Disc_Attribute_get()->AddRow();
		pRow_Disc_Attribute->FK_Disc_set(pRow_Disc->PK_Disc_get());
		pRow_Disc_Attribute->FK_Attribute_set(pRow_Attribute->PK_Attribute_get());
		pRow_Disc_Attribute->Track_set(Track);
		pRow_Disc_Attribute->Section_set(Section);
		m_pDatabase_pluto_media->Disc_Attribute_get()->Commit();
	}
}

int MediaAttributes_LowLevel::AddPictureToAttribute(int PK_Attribute,char *pPictureData,size_t sizePicture,string sURL)
{
	Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(PK_Attribute);
	if( pPictureData && sizePicture && pRow_Attribute )
	{
		Row_Picture *pRow_Picture = AddPicture(pPictureData, int(sizePicture), "jpg",sURL);
		if( !pRow_Picture )
			return 0;

		AddPictureToAttribute(pRow_Attribute->PK_Attribute_get(),pRow_Picture->PK_Picture_get());
		return pRow_Picture->PK_Picture_get();
	}
	return 0;
}

void MediaAttributes_LowLevel::AddPictureToDisc(int PK_Disc,int PK_Picture)
{
	Row_Picture_Disc *pRow_Picture_Disc = m_pDatabase_pluto_media->Picture_Disc_get()->GetRow(PK_Picture,PK_Disc);
	if( !pRow_Picture_Disc )
	{
		pRow_Picture_Disc = m_pDatabase_pluto_media->Picture_Disc_get()->AddRow();
		pRow_Picture_Disc->FK_Disc_set( PK_Disc );
		pRow_Picture_Disc->FK_Picture_set( PK_Picture );
		m_pDatabase_pluto_media->Picture_Disc_get()->Commit();
	}
}

void MediaAttributes_LowLevel::AddPictureToFile(int PK_File,int PK_Picture)
{
	Row_Picture_File *pRow_Picture_File = m_pDatabase_pluto_media->Picture_File_get()->GetRow(PK_Picture,PK_File);
	if( !pRow_Picture_File )
	{
		pRow_Picture_File = m_pDatabase_pluto_media->Picture_File_get()->AddRow();
		pRow_Picture_File->FK_File_set( PK_File );
		pRow_Picture_File->FK_Picture_set( PK_Picture );
		m_pDatabase_pluto_media->Picture_File_get()->Commit();
	}
}

void MediaAttributes_LowLevel::AddPictureToAttribute(int PK_Attribute,int PK_Picture)
{
	Row_Picture_Attribute *pRow_Picture_Attribute = m_pDatabase_pluto_media->Picture_Attribute_get()->GetRow(PK_Picture,PK_Attribute);
	if( !pRow_Picture_Attribute )
	{
		pRow_Picture_Attribute = m_pDatabase_pluto_media->Picture_Attribute_get()->AddRow();
		pRow_Picture_Attribute->FK_Attribute_set( PK_Attribute );
		pRow_Picture_Attribute->FK_Picture_set( PK_Picture );
		m_pDatabase_pluto_media->Picture_Attribute_get()->Commit();
	}
}

int MediaAttributes_LowLevel::GetMediaType(Row_DiscLocation *pRow_DiscLocation)
{
	string s = pRow_DiscLocation->Type_get();
	if( s.size()<1 )
		return 0;

	switch( s[0] )
	{
	case 'c':
		return MEDIATYPE_pluto_CD_CONST;
	case 'd':
		return MEDIATYPE_pluto_DVD_CONST;
	case 'M':
		return MEDIATYPE_pluto_StoredVideo_CONST;
	}
	return 0;
}

void MediaAttributes_LowLevel::GetDefaultRippingName(Row_Disc *pRow_Disc,string *sFilename,string *sDirectory)
{
	if( pRow_Disc->EK_MediaType_get()==MEDIATYPE_pluto_CD_CONST )
	{
		string sPerformer,sAlbum;
		GetAttributeFromDisc(pRow_Disc->PK_Disc_get(),ATTRIBUTETYPE_Performer_CONST,sPerformer);
		GetAttributeFromDisc(pRow_Disc->PK_Disc_get(),ATTRIBUTETYPE_Album_CONST,sAlbum);
		*sFilename = FileUtils::ValidFileName(sPerformer);
		if(sFilename->size())
			*sFilename += "/"; // We got a performer

		*sFilename += FileUtils::ValidFileName(sAlbum);
		*sDirectory = "audio";
	}
	else
		*sDirectory = "videos";

	if( sFilename->empty() )
	{
		string sTitle;
		GetAttributeFromDisc(pRow_Disc->PK_Disc_get(),ATTRIBUTETYPE_Title_CONST,sTitle);
		*sFilename = FileUtils::ValidFileName(sTitle);
	}
}

int MediaAttributes_LowLevel::GetOwnerForPath(string sPath)
{
	// like this: /home/user_xxx/data

	int nEK_Users_Private = 0;
	const string csHomeUserPath = "/home/user_";

	if(sPath.find(csHomeUserPath) == 0)
	{
		string sRemainingPath = sPath.substr(csHomeUserPath.length());
		string sUserId = sRemainingPath.substr(0, sRemainingPath.find("/"));
		nEK_Users_Private = atoi(sUserId.c_str());
	}

	return nEK_Users_Private;
}
