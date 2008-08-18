/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "Media_Data_Provider.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "pluto_media/Database_pluto_media.h"
#include "../Data_Provider_Catalog_Plugin/Data_Provider_Factory.h"
#include "HTTP_Request_Handler.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Media_Data_Provider::Media_Data_Provider(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Media_Data_Provider_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pDBHelper=NULL;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Media_Data_Provider::~Media_Data_Provider()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool Media_Data_Provider::GetConfig()
{
	if( !Media_Data_Provider_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	m_pDBHelper = new DBHelper();
	if( !m_pDBHelper->DBConnect( "192.168.0.100", "root", "", "pluto_media" ) )
    {
        LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Media_Data_Provider::GetConfig Cannot connect to database!" );
        m_bQuit_set(true);
        return false;
    }

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Media_Data_Provider::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

void Media_Data_Provider::PostConnect()
{
	m_pData_Provider_Factory = new Data_Provider_Factory(this);
	m_pMedia_Provider_InstantiationInfo = new Media_Provider_InstantiationInfo(m_pDBHelper);
	m_pGeneric_Mysql_Provider = new Generic_Mysql_Provider(m_pMedia_Provider_InstantiationInfo);

	string sQueries=
"getNumberOfAlbumsByArtist\t"
	"select count(PK_Attribute) from Attribute where FK_AttributeType=%1\t"
	"itemId\n"
"getContainerInfoByPrefixCount\t"
	"select count(FK_Attribute) FROM Attribute_MediaType JOIN Attribute ON FK_Attribute=PK_Attribute WHERE FK_AttributeType=3 AND EK_MediaType=%mt\t"
	"HCPrefix\n"
"getContainerInfoByPrefixChunk\t"
	"select FK_Attribute,Name FROM Attribute_MediaType JOIN Attribute ON FK_Attribute=PK_Attribute WHERE FK_AttributeType=3 AND EK_MediaType=%mt ORDER BY Name LIMIT %2 OFFSET %3;\t"
	"HCPrefix,Limit,Offset\n"
"getImageForAlbum\t"
	"select FK_Attribute,concat('/home/mediapics/',FK_Picture,'_tn.jpg'),'NULL',FK_Attribute FROM Picture_Attribute WHERE FK_Attribute=%1 LIMIT 1\t"
	"PK_Attribute\n"
"getHostForObject\t"
	"select 'localhost'\t\n"
"getNumberOfPhotosInAlbum\t"
	"SELECT NumFiles FROM Attribute_MediaType WHERE FK_Attribute=%1\t"
	"FK_Attribute\n"
"getLengthOfAlbum\t"
	"SELECT NumFiles FROM Attribute_MediaType WHERE FK_Attribute=%1\t"
	"FK_Attribute\n"
"canDelete\t"
	"select 'false'\t\n"
"getSingleField\t"
	"select 3\t\n"
"getItemsOfAlbumSortedByDateChunk\t"
		"select PK_File,concat(Path, '/', Filename),0,Filename FROM File "
		"JOIN File_Attribute ON FK_File=PK_File WHERE FK_Attribute=%1 AND Missing=0 AND IsDirectory=0 "
		"ORDER BY Filename LIMIT %2 OFFSET %3\t"
	"PK_Attribute,Limit,Offset\n"
"getItemsOfAlbumSortedByTitleChunk\t"
		"SELECT PK_File,concat('file:///home/mediapics/', FK_Picture, '.jpg'),0,Filename FROM File "
		"JOIN File_Attribute ON File_Attribute.FK_File=PK_File LEFT JOIN Picture_File ON Picture_File.FK_File=PK_File "
		"WHERE FK_Attribute=%1 AND Missing=0 AND IsDirectory=0 ORDER BY Filename LIMIT %2 OFFSET %3\t"
	"PK_Attribute,Limit,Offset\n" 
	//SELECT object.objId, %resURI(res_t), object.rotation, object.title from aggr_object object INNER JOIN aggr_items ON aggr_items.ref = object.objId INNER JOIN aggr_res res_t ON res_t.objId=object.objId and res_t.usage='thumbnail' WHERE aggr_items.objId = '%1' order by object.sortTitle LIMIT %2 OFFSET %3;
"getScreensaverUrisRotationsChunk\t"
		"select PK_File,concat('file://', Path, '/', Filename),0,Filename FROM File "
		"JOIN File_Attribute ON FK_File=PK_File WHERE FK_Attribute=%1 AND Missing=0 AND IsDirectory=0 "
		"ORDER BY Filename LIMIT %2 OFFSET %3\t"
	"PK_Attribute,Limit,Offset\n"
"getPhotoUrisRotationsChunk\t"
		"SELECT concat('file://', Path, '/', Filename),0,FK_File FROM File "
		"JOIN File_Attribute ON FK_File=PK_File "
		"WHERE FK_Attribute=%1 AND Missing=0 AND IsDirectory=0 "
		"ORDER BY Filename LIMIT %2 OFFSET %3\t"
		"PK_Attribute,Limit,Offset\n" 
		// URL,rotation,objectID
"getVideoUrisOfAlbum\t"
		"SELECT PK_File,concat('file://', Path, '/', Filename),0,FK_File FROM File "
		"JOIN File_Attribute ON FK_File=PK_File WHERE FK_Attribute=%1 AND Missing=0 AND IsDirectory=0 "
		"ORDER BY Filename LIMIT %2 OFFSET %3\t"
	"PK_Attribute,Limit,Offset\n" 
	// SELECT object.objId, %resURI(res_t) from aggr_object object INNER JOIN aggr_items ON aggr_items.ref = object.objId INNER JOIN aggr_res res_t ON res_t.objId=object.objId WHERE aggr_items.objId = '%1' and res_t.type='content' order by object.sortTitle LIMIT %2 OFFSET %3;
"getSongUrisOfAlbum\t"
	"\t"
	"PK_File,Limit,Offset\n"
"batchGetStartLettersAndNumberofItemsForPrefix\t"
		"select distinct left(Name,1),10 AS N FROM Attribute_MediaType JOIN Attribute ON FK_Attribute=PK_Attribute "
		"WHERE FK_AttributeType=%at AND EK_MediaType=%mt ORDER BY N\t"
	"HCPrefix\n" 
	// select substr, count from aggr_substrs where prefix = '%1' order by substr;
"getUriForArtistsBySortTitleBetween\t"
		"select concat('/home/mediapics/', FK_Picture, '.jpg') FROM Attribute_MediaType "
		"JOIN Attribute ON Attribute_MediaType.FK_Attribute=PK_Attribute JOIN Picture_Attribute ON Picture_Attribute.FK_Attribute=PK_Attribute "
		"WHERE FK_AttributeType=2 AND EK_MediaType=4 AND Name >= %1 and Name <= %2 "
		"Order BY Name limit %3 offset %4\t"
	"From,To,Limit,Offset\n" 
	// select %resURI(res_t) from aggr_object object_t, aggr_res res_t WHERE object_t.prefix = 1 and res_t.objId=object_t.objId and res_t.usage='thumbnail' and object_t.sortTitle >= '%1' and object_t.sortTitle <= '%2' order by object_t.sortTitle limit %3 offset %4;
"batchGetIdTitleUriForArtistsBySortTitleBetween\t"
	"select PK_Attribute,Name,concat('/home/mediapics/', FK_Picture, '.jpg') FROM Attribute_MediaType "
	"JOIN Attribute ON Attribute_MediaType.FK_Attribute=PK_Attribute "
	"JOIN Picture_Attribute ON Picture_Attribute.FK_Attribute=PK_Attribute "
	"WHERE FK_AttributeType=2 AND EK_MediaType=4 AND Name >= %1 and Name <= %2 "
	"LIMIT %3 OFFSET %4\t"
	"From,To,Limit,Offset\n" 
	// select object_t.objId, object_t.title, %resURI(res_t) from aggr_object object_t, aggr_res res_t WHERE object_t.prefix = 1 and res_t.objId=object_t.objId and res_t.usage='albumCover' and object_t.sortTitle >= '%1' and object_t.sortTitle <= '%2' order by object_t.sortTitle limit %3 offset %4;

"getNumberOfArtistsBySortTitleBetween\t"
	"select count(PK_Attribute) FROM Attribute_MediaType "
	"JOIN Attribute ON Attribute_MediaType.FK_Attribute=PK_Attribute "
	"WHERE FK_AttributeType=2 AND EK_MediaType=4 AND Name >= %1 and Name <= %2\t"
	"From,To\n" 
	// select count(*) from aggr_object object_t WHERE object_t.prefix = 1 and object_t.sortTitle >= '%1' and object_t.sortTitle <= '%2';

	
//fix
"getNameSongCountForArtist\t"
	"select 'songone',3\t"
	"PK_Attribute\n"
	//SELECT o.title, count(*) from aggr_object o, aggr_items i WHERE o.objId='%1' AND o.objId=i.objId AND i.targetPrefix=2 GROUP BY o.objId;

//fix
"getIdTitleUriHostForAlbumsOfArtist\t"
	"select 123, 'foo', '/home/mediapics/611.jpg', '888'\t"
	"PK_Attribute,Limit,Offset\n"
	// select o.objId, o.title, %resURI(r), o.hostKey from aggr_object o, aggr_items i, aggr_items ia, aggr_res r WHERE i.objId='%1' and i.targetPrefix=0 and o.objId=i.ref AND o.objId=ia.ref AND r.objId=o.objId and r.usage = 'albumCover' GROUP BY o.objId ORDER BY o.sortTitle limit %2 offset %3


//fix
"getArtistCountForAlbumsOfArtist\t"
	"select 5\t"
	"PK_Attribute\n"
	// SELECT count(*) FROM (SELECT o.objId objId, o.sortTitle sortTitle FROM aggr_object o, aggr_items i WHERE i.objId='%1' and i.targetPrefix=0 and o.objId=i.ref) s, aggr_relations r WHERE r.objId in (s.objId) GROUP BY s.objId ORDER BY sortTitle;

//fix
"getNumberOfSongsInAlbumOrByArtist\t"
	"select 3\t"
	"\n"
	// select count(distinct(ref)) from aggr_items WHERE objId='%1' and targetPrefix=2;

//fix
"batchGetSongsArtistsOfAlbumBySortField\t"
	"select 8674, 'my title', 'my ref', 'oa title', 'oatrack' FROM File limit 3\t"
	"\n"
	// select o.objId, o.title, r.ref, o_a.title, o.originalTrackNumber from aggr_object o, aggr_items i INNER JOIN aggr_relations r ON r.objId=o.objId INNER JOIN aggr_object o_a ON r.ref=o_a.objId  AND r.targetPrefix=1 WHERE i.objId='%1' and i.ref=o.objId order by (case when o.originalTrackNumber=-1 then 9999 else o.originalTrackNumber end), o.sortTitle limit %2 offset %3;

"getNumberofItemsForPrefix\t"
	"select count(PK_Attribute) from Attribute where FK_AttributeType=%1\t"
	"Prefix\n";


	m_pGeneric_Mysql_Provider->RegisterQueries(sQueries,m_pData_Provider_Factory);
	m_pData_Provider_Factory->RegisterCatalog();

	m_pHTTP_Request_Handler = new HTTP_Request_Handler(m_pGeneric_Mysql_Provider);
	m_pHTTP_Request_Handler->StartListening(9876);
}

//<-dceag-createinst-b->!

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Media_Data_Provider::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Media_Data_Provider::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c971-b->

	/** @brief COMMAND: #971 - Request Data Provider */
	/** Request data from this provider */
		/** @param #10 ID */
			/** The ID of the data being requested */
		/** @param #48 Value */
			/** An number representing this particular request.  If Multiple is true, the command can be resent with the same Value to get the same output */
		/** @param #109 Data String */
			/** The resulting Data */
		/** @param #202 Parameters */
			/** The parameters for the query */
		/** @param #259 Multiple */
			/** if true, we may make multiple requests and pass back in the same output Value */
		/** @param #279 Format Input */
			/** The format for the parameters */
		/** @param #280 Format Output */
			/** The desired format to retrieve the data */
		/** @param #281 Cache */
			/** Empty = Use Default. Y=Keep a copy of the output in cache, N=Do not keep this result in cache */

void Media_Data_Provider::CMD_Request_Data_Provider(string sID,string sParameters,bool bMultiple,string sFormat_Input,string sFormat_Output,string sCache,int *iValue,string *sData_String,string &sCMD_Result,Message *pMessage)
//<-dceag-c971-e->
{
	m_pData_Provider_Factory->Populate(pMessage->m_dwPK_Device_From, sID, sParameters, bMultiple, sFormat_Input, sFormat_Output, sCache, iValue, sData_String);
}
