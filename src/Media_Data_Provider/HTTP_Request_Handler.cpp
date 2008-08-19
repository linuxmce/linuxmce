/*
Copyright (C) 2004 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

*/

#include "HTTP_Request_Handler.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCE/Command_Impl.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "PlutoUtils/DBHelper.h"
#include "Generic_Mysql_Provider.h"

#include "pluto_main/Define_MediaType.h"
#include "pluto_media/Define_AttributeType.h"

#include <vector>
#include <iostream>
using namespace std;
using namespace DCE;

#include "../Data_Provider_Catalog_Plugin/Data_Provider_Factory.h"
#include "Media_Data_Provider.h"

string HTTP_Request_Socket::ProcessParameters(string sRequest, string *sParameter)
{
	string::size_type pos = sRequest.find('?');
	if( pos==string::npos )
		return sRequest;

	*sParameter = sRequest.substr(pos+1);
	return sRequest.substr(0,pos);
}

bool HTTP_Request_Socket::PreprocessRequest(string sID,string sParameters,string *sData_String)
{
	if( sID=="/UE/event" )
	{
		*sData_String="{}";
		return true;
	}
	else if( sID=="/UE/databaseStatus" && sParameters.find("isDatabaseBusy")!=string::npos )
	{
		*sData_String="false";
		return true;
	}
	else if( sID=="willThereBeCake" )
	{
		*sData_String="[[\"n33l5m0m\"]]";
		return true;
	}
	else if( sID=="getContainerInfoByPrefixCount" )
	{
		*sData_String="[[\"1\"]]";
		return true;
	}
	else if( sID=="getContainerInfoByPrefixChunk" )
	{
		*sData_String="[[\"12\"],[\"China\"],[\"3\"],[\"30\"]]";
		return true;
	}
	else if( sID=="getImageForAlbum" )
	{
		*sData_String="[[\"12\"],[\"http://192.168.0.101:9876/FS/C%3a/Program%20Files/Hillcrest%20Labs/AV%20Server/htdocs/media/thumbs/1/P1000207.hillcrest_thumb.JPG.jpg\"],[null],[\"China\"]]";
		return true;
	}
	else if( sID=="getHostForObject" )
	{
		*sData_String="[[\"localhost\"]]";
		return true;
	}
	else if( sID=="getNumberOfPhotosInAlbum" )
	{
		*sData_String="[[\"30\"]]";
		return true;
	}

	else if( sID=="getSingleField" )
	{
		*sData_String="[[\"3\"]]";
		return true;
	}

	else if( sID=="getItemsOfAlbumSortedByDateChu" )
	{
		*sData_String="[[\"13\",\"15\",\"16\",\"17\",\"18\",\"19\",\"20\",\"21\",\"22\",\"23\",\"24\",\"25\",\"26\",\"27\",\"28\",\"29\",\"30\",\"31\",\"32\",\"33\",\"34\",\"35\",\"36\",\"37\",\"38\",\"39\",\"40\",\"41\",\"42\",\"43\"],[\"http://192.168.0.101:9876/FS/C%3a/Program%20Files/Hillcrest%20Labs/AV%20Server/htdocs/media/thumbs/1/P1000207.hillcrest_thumb.JPG.jpg\",\"http://192.168.0.101:9876/FS/C%3a/Program%20Files/Hillcrest%20Labs/AV%20Server/htdocs/media/thumbs/1/P1000208.hillcrest_thumb.JPG.jpg\",\"http://192.168.0.101:9876/FS/C%3a/Program%20Files/Hillcrest%20Labs/AV%20Server/htdocs/media/thumbs/1/P1000209.hillcrest_thumb.JPG.jpg\",\"http://192.168.0.101:9876/FS/C%3a/Program%20Files/Hillcrest%20Labs/AV%20Server/htdocs/media/thumbs/1/P1000210.hillcrest_thumb.JPG.jpg\",\"http://192.168.0.101:9876/FS/C%3a/Program%20Files/Hillcrest%20Labs/AV%20Server/htdocs/media/thumbs/1/P1000211.hillcrest_thumb.JPG.jpg\",\"http://192.168.0.101:9876/FS/C%3a/Program%20Files/Hillcrest%20Labs/AV%20Server/htdocs/media/thumbs/1/P1000212.hillcrest_thumb.JPG.jpg\",\"http://192.168.0.101:9876/FS/C%3a/Program%20Files/Hillcrest%20Labs/AV%20Server/htdocs/media/thumbs/1/P1000213.hillcrest_thumb.JPG.jpg\",\"http://192.168.0.101:9876/FS/C%3a/Program%20Files/Hillcrest%20Labs/AV%20Server/htdocs/media/thumbs/1/P1000215.hillcrest_thumb.JPG.jpg\",\"http://192.168.0.101:9876/FS/C%3a/Program%20Files/Hillcrest%20Labs/AV%2";
		return true;
	}

	else if( sID=="canDelete" )
	{
		*sData_String="false";
		return true;
	}

	return false;
}

void HTTP_Request_Socket::PreprocessMediaDbRequest(string &sDB,string &sRequest,string &sParms,string &sQuery)
{
	if( sRequest=="getNumberofItemsForPrefix" )
	{
		if( sParms=="1" )
			sParms=TOSTRING(ATTRIBUTETYPE_Performer_CONST);
		else if( sParms=="2" )
			sParms=TOSTRING(ATTRIBUTETYPE_Album_CONST);
		else if( sParms=="0" )
			sParms=TOSTRING(ATTRIBUTETYPE_Performer_CONST);
	}
	else if( sRequest=="getNumberOfPhotosInAlbum" || sRequest=="getLengthOfAlbum" )
	{
		string sSQL = "SELECT Path,Filename FROM File WHERE PK_File=" + sParms;
		PlutoSqlResult result;
		DB_ROW row;
		if( ( result.r=m_pGeneric_Mysql_Provider->m_pMedia_Provider_InstantiationInfo->m_pDBHelper->db_wrapper_query_result( sSQL ) )!=NULL && (row=db_wrapper_fetch_row( result.r ))!=NULL )
			sQuery = "SELECT count(PK_File) FROM File where Path='" + string(row[0]) + "/" + row[1] + "' AND Missing=0 AND IsDirectory=0";
	}
	/*
	else if( sRequest=="getItemsOfAlbumSortedByDateChunk" || sRequest=="getScreensaverUrisRotationsChunk" || sRequest=="getSongUrisOfAlbum" )
	{
		string::size_type pos = sParms.find(',');
		if( pos!=string::npos )
		{
			string sSQL = "SELECT Path,Filename,EK_MediaType FROM File WHERE PK_File=" + sParms.substr(0,pos);
			PlutoSqlResult result;
			DB_ROW row;
			if( ( result.r=m_pGeneric_Mysql_Provider->m_pMedia_Provider_InstantiationInfo->m_pDBHelper->db_wrapper_query_result( sSQL ) )!=NULL && (row=db_wrapper_fetch_row( result.r ))!=NULL )
			{
				if( sRequest=="getSongUrisOfAlbum" )
//					sQuery = "select PK_File,concat('http://192.168.0.101:9876/FS/z:/', mid(Path,7), '/', Filename) FROM File WHERE Path='" + string(row[0]) + "/" + row[1] + "' AND Missing=0 AND IsDirectory=0 ORDER BY Filename LIMIT %2 OFFSET %3";
					sQuery = "select PK_File,concat(Path, '/', Filename) FROM File WHERE Path='" + string(row[0]) + "/" + row[1] + "' AND Missing=0 AND IsDirectory=0 ORDER BY Filename LIMIT %2 OFFSET %3";
				else if( sRequest=="getItemsOfAlbumSortedByDateChunk" )
				{
					if( atoi(row[2])==MEDIATYPE_pluto_StoredVideo_CONST ) 
						sQuery = 
//							"select PK_File,concat('http://192.168.0.101:9876/FS/z:/mediapics/',FK_Picture,'_tn.jpg'),0,Attribute.Name FROM File "
							"select PK_File,concat('/home/mediapics/',FK_Picture,'_tn.jpg'),0,Attribute.Name FROM File "
							"JOIN Picture_File ON Picture_File.FK_File=PK_File "
							"JOIN File_Attribute ON File_Attribute.FK_File=PK_File "
							"JOIN Attribute ON FK_Attribute=PK_Attribute AND FK_AttributeType=13 "
							"WHERE Path='" + string(row[0]) + "/" + row[1] + "' AND Missing=0 AND IsDirectory=0 ORDER BY Filename LIMIT %2 OFFSET %3";
					else
//						sQuery = "select PK_File,concat('http://192.168.0.101:9876/FS/z:/', mid(Path,7), '/', Filename),0,Filename FROM File WHERE Path='" + string(row[0]) + "/" + row[1] + "' AND Missing=0 AND IsDirectory=0 ORDER BY Filename LIMIT %2 OFFSET %3";
						sQuery = "select PK_File,concat(Path, '/', Filename),0,Filename FROM File WHERE Path='" + string(row[0]) + "/" + row[1] + "' AND Missing=0 AND IsDirectory=0 ORDER BY Filename LIMIT %2 OFFSET %3";
				}
				else
//					sQuery = "select concat('http://192.168.0.101:9876/FS/z:/', mid(Path,7), '/', Filename),0,PK_File FROM File WHERE Path='" + string(row[0]) + "/" + row[1] + "' AND Missing=0 AND IsDirectory=0 ORDER BY Filename LIMIT %2 OFFSET %3";
					sQuery = "select concat(Path, '/', Filename),0,PK_File FROM File WHERE Path='" + string(row[0]) + "/" + row[1] + "' AND Missing=0 AND IsDirectory=0 ORDER BY Filename LIMIT %2 OFFSET %3";
			}
		}
	}
	*/
}


void HTTP_Request_Socket::HandleRequest(string sRequest)
{
	int iValue;
	string sData_String;
	string sParameters;
	string sID = ProcessParameters( StringUtils::URLDecode(sRequest), &sParameters );

	if( sID.size()>4 && sID.substr(0,4)=="/FS/" )
	{
		string sFile = sID.substr(4);
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "HTTP_Request_Socket::HandleRequest serving file: %s Parms: %s", sID.c_str(), sParameters.c_str() );

		size_t size=0;
		char *pBuf = FileUtils::ReadFileIntoBuffer(sFile,size);

		string sData = "HTTP/1.1 200 OK\r\n"
			"Date: Mon, 04 Aug 2008 01:31:40 GMT\r\n"
			"Server: Apache/2.2.8 (Win32)\r\n"
			"Content-Length: " + StringUtils::itos((int) size) + "\r\n"
			"Content-Type: image/jpeg"
			"\r\n\r\n";

		SendData((int) sData.size(), sData.c_str());

		if( size )
			SendData((int) size,pBuf);
		else
			LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "HTTP_Request_Socket::HandleRequestfile not found %s", sFile.c_str());

		return;
	}

	bool bResponse = PreprocessRequest(sID,sParameters,&sData_String);
	if( bResponse )
	{
		if( sID.find("/UE/event")==string::npos )  // Don't log this
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "HTTP_Request_Socket::HandleRequest handled internally  ID: %s Parms: %s", sID.c_str(), sParameters.c_str() );
	}
	else
	{
		if( sID=="/UE/media" && sParameters.size()>9 && sParameters.substr(0,8)=="Request=" )
		{
			string::size_type pos=9;
			string sDB = StringUtils::Tokenize( sParameters, ",", pos );
			string sRequest = StringUtils::Tokenize( sParameters, ",", pos );
			string sParms = pos < sParameters.size() ? sParameters.substr(pos) : "";
			StringUtils::Replace(&sDB,"\"","");
			StringUtils::Replace(&sDB," ","");
			StringUtils::Replace(&sRequest,"\"","");
			StringUtils::Replace(&sRequest," ","");
			if( sParms.size()>=1 && sParms[sParms.size()-1]==']' )
				sParms = sParms.substr( 0, sParms.size()-1 );

			string sQuery;
			PreprocessMediaDbRequest(sDB,sRequest,sParms,sQuery);

			bResponse = m_pGeneric_Mysql_Provider->Populate(0,sRequest,sParms,false,"text","json","",&iValue,&sData_String,sQuery);
			if( bResponse==false )
			{
				LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "HTTP_Request_Socket::HandleRequest bad DB %s Request %s Parms: %s", sDB.c_str(), sRequest.c_str(), sParms.c_str() );
			}
		}
		if( bResponse==false )
		{
			LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "HTTP_Request_Socket::HandleRequest %s Parms: %s", sID.c_str(), sParameters.c_str() );
		}
		else
		{
			static int iRequestID=0;
			string sFile = "/temp/req" + StringUtils::itos(iRequestID++);
			FILE *file = fopen(sFile.c_str(),"wb");
			fprintf(file,"ID: %s\nParms: %s\n", sID.c_str(), sParameters.c_str());
			fputs(sData_String.c_str(),file);
			fclose(file);
			LoggerWrapper::GetInstance()->Write( LV_STATUS, "HTTP_Request_Socket::HandleRequest handled with db ID: %s Parms: %s File: %s Response: %s", sID.c_str(), sParameters.c_str(), sFile.c_str(), sData_String.c_str() );
		}
	}

	string sResponse = "HTTP/1.1 200 OK\r\n"
		"Date: Mon, 04 Aug 2008 00:54:28 GMT\r\n"
		"Server: Apache/2.2.8 (Win32)\r\n"
		"Content-Length: " + StringUtils::itos((int) sData_String.size()) + "\r\n"
		"Content-Type: text/plain\r\n"
		"\r\n"
		+ sData_String;

	SendData((int) sResponse.size(),sResponse.c_str());
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "HTTP_Request_Socket::HandleRequest sending %s", sResponse.c_str());
}

bool HTTP_Request_Socket::ServeClient()
{
	string sMessage;
	while( !m_pListener->m_bTerminate )
	{
		if ( !ReceiveString( sMessage ) || m_pListener->m_bTerminate )
		{
			break;
		}
		string::size_type space=0;
		if( sMessage.size()>6 && (space=sMessage.find(' ',4))!=string::npos && sMessage.substr(0,4)=="GET " )
			m_sGet = sMessage.substr(4, space-4);
		else if( sMessage.empty() && m_sGet.empty()==false )
		{
			HandleRequest(m_sGet);
			m_sGet="";
		}
	}
	return false;
}

