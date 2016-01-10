/*
 R_GetAll_psc_id

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */


/**
 *
 * @file R_GetAll_psc_id.cpp
 * @brief source file for the R_GetAll_psc_id class
 *
 */

#include "R_GetAll_psc_id.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "sqlCVSprocessor.h"
#include "Database.h"
#include "Repository.h"
#include "CommonFunctions.h"

#include <iostream>
#include <sstream>

using namespace sqlCVS;

R_GetAll_psc_id::R_GetAll_psc_id( string sTable, vector<int> *p_vectRestrictions)
{
	m_sTable=sTable;
	m_vectRestrictions=*p_vectRestrictions;
}

bool R_GetAll_psc_id::ProcessRequest( class RA_Processor *pRA_Processor )
{
	(( sqlCVSprocessor * ) pRA_Processor)->LogActivityTime();

	cout << "R_GetAll_psc_id" << endl;
	std::ostringstream sSQL;
	// See notes in Table::DetermineDeletions
	sSQL << "SELECT psc_id,psc_batch FROM `" << m_sTable << "` WHERE " <<  g_GlobalConfig.GetRestrictionClause(m_sTable,&m_vectRestrictions) << " ORDER BY psc_id";

	PlutoSqlResult res;
	MYSQL_ROW row=NULL;
	res.r = g_GlobalConfig.m_pDatabase->mysql_query_result( sSQL.str( ) );
	if( !res.r )
	{
		cerr << "Problem retrieving rows with query1: " << sSQL.str() << endl;
		m_cProcessOutcome=INTERNAL_ERROR;
	}
	else
	{
		while( ( row = mysql_fetch_row( res.r ) ) )
		{
			if( !row[0] )
			{
				cerr << "Found NULL in query: " << sSQL.str() << endl;
				m_cProcessOutcome=INTERNAL_ERROR;
				return true; /**<< Request successfully processed */
			}
			m_vectAll_psc_id.push_back( pair<int,int>(atoi( row[0] ),(row[1] ? atoi(row[1]) : 0)) );
		}
		m_cProcessOutcome=SUCCESSFULLY_PROCESSED;
	}

	return true; /**<< Request successfully processed */
}


/** @test
VR_IdentifyPhone::VR_IdentifyPhone( unsigned long EstablishmentID,
		string sBluetooth, u_int64_t MacAddress, unsigned long CachedRecordVersion, unsigned long CachedPlutoId,
		unsigned long IdentifiedPlutoId, string IdentiedPlutoIdPin, class Customer *pCustomer )
	: RA_Request( )
{
}

VR_IdentifyPhone::VR_IdentifyPhone( unsigned long size, const char *data )
	: RA_Request( size, data )
{
	m_iUseCache=0;
	m_iRecordVersion=0;
	m_iPlutoId=0;
	m_pImage=NULL;
	m_iImageSize=0;
	m_pCustomer=NULL;

	m_iEstablishmentID = Read_unsigned_long( );
	Read_string( m_sBluetoothID );
	m_iMacAddress = Read_int64( );
	m_iCachedRecordVersion = Read_unsigned_long( );
	m_iCachedPlutoId = Read_unsigned_long( );
	m_iIdentifiedPlutoId = Read_unsigned_long( );
	Read_string( m_sIdentifiedPlutoIdPin );

	m_dwRequestSize = ( unsigned long ) ( m_pcCurrentPosition-m_pcDataBlock );

//	cout << "Received request to ID Mac " << m_iMacAddress << " ID " << m_iIdentifiedPlutoId << "\n";
}

void VR_IdentifyPhone::ConvertRequestToBinary( )
{
	RA_Request::ConvertRequestToBinary( );

	Write_unsigned_long( m_iEstablishmentID );
	Write_string( m_sBluetoothID );
	Write_int64( m_iMacAddress );
	Write_unsigned_long( m_iCachedRecordVersion );
	Write_unsigned_long( m_iCachedPlutoId );
	Write_unsigned_long( m_iIdentifiedPlutoId );
	Write_string( m_sIdentifiedPlutoIdPin );

	m_dwRequestSize = ( unsigned long ) ( m_pcCurrentPosition-m_pcDataBlock );
	m_pcRequest = m_pcDataBlock;
}

void VR_IdentifyPhone::ConvertResponseToBinary( )
{
	RA_Request::ConvertResponseToBinary( );

	Write_unsigned_char( m_iUseCache );
	Write_unsigned_long( m_iRecordVersion );
	Write_unsigned_long( m_iPlutoId );
	Write_unsigned_char( m_iImageType );
	Write_unsigned_long( m_iImageSize );
	Write_block( m_pImage, m_iImageSize );
	Write_string( m_sEmail );
	Write_string( m_sNickname );
	Write_string( m_sFirstName );
	Write_string( m_sLastName );
	Write_string( m_sAddress );
	Write_string( m_sCity );
	Write_string( m_sState );
	Write_string( m_sZip );
	Write_string( m_sCountry );
	Write_string( m_sGender );
	Write_string( m_sBirthdate );
	Write_string( m_sComments );
	Write_string( m_sEstablishmentCustomerId );
	Write_unsigned_short( ( unsigned short ) MYSTL_SIZEOF_LIST( m_listPhoneNumbers ) );
	MYSTL_ITERATE_LIST( m_listPhoneNumbers, VIPPhoneNumber, pPhone, iPN )
	{
		Write_unsigned_char( pPhone->m_iPhoneNumberType );
		Write_string( pPhone->m_sNumber );
	}

	Write_unsigned_short( ( unsigned short ) MYSTL_SIZEOF_LIST( m_listAttributes ) );
	MYSTL_ITERATE_LIST( m_listAttributes, MiscVIPAttribute, pAttr, iVA )
	{
		Write_string( pAttr->m_sType );
		Write_string( pAttr->m_sValue );
	}

	m_dwResponseSize = ( unsigned long ) ( m_pcCurrentPosition-m_pcDataBlock );
	m_pcResponse = m_pcDataBlock;
}


bool VR_IdentifyPhone::ParseResponse( unsigned long size, const char *data )
{
	RA_Request::ParseResponse( size, data );

	m_iUseCache=Read_unsigned_char( );
	m_iRecordVersion=Read_unsigned_long( );
	m_iPlutoId=Read_unsigned_long( );
	m_iImageType=Read_unsigned_char( );
	m_iImageSize=Read_unsigned_long( );
	m_pImage=Read_block( m_iImageSize );
	Read_string( m_sEmail );
	Read_string( m_sNickname );
	Read_string( m_sFirstName );
	Read_string( m_sLastName );
	Read_string( m_sAddress );
	Read_string( m_sCity );
	Read_string( m_sState );
	Read_string( m_sZip );
	Read_string( m_sCountry );
	Read_string( m_sGender );
	Read_string( m_sBirthdate );
	Read_string( m_sComments );
	Read_string( m_sEstablishmentCustomerId );

	unsigned short NumPhoneNumbers = Read_unsigned_short( );

	for( unsigned short iNP=0;iNP<NumPhoneNumbers;++iNP )
	{
		VIPPhoneNumber *pPhone = new VIPPhoneNumber( );
		pPhone->m_iPhoneNumberType=Read_unsigned_char( );
		Read_string( pPhone->m_sNumber );
		MYSTL_ADDTO_LIST( m_listPhoneNumbers, pPhone );
	}

	unsigned short NumAttributes=Read_unsigned_short( );
	for( unsigned short iNA=0;iNA<NumAttributes;++iNA )
	{
		string Type, Value;
		Read_string( Type );
		Read_string( Value );

		MiscVIPAttribute *pAttr = new MiscVIPAttribute( Type, Value );
		MYSTL_ADDTO_LIST( m_listAttributes, pAttr );
	}

	m_dwResponseSize = ( unsigned long ) ( m_pcCurrentPosition-m_pcDataBlock );

	return true;
}

bool VR_IdentifyPhone::ProcessRequest( )
{
#ifdef VIPSERVER
	m_iImageSize=0;
	m_pImage=NULL;

	cout << "Processing request to ID Mac " << m_iMacAddress << " ID " << m_iIdentifiedPlutoId << "\n";

	if( !m_iMacAddress )
	{
		UsageLog << "Request from " << m_iEstablishmentID << " had empty MAC"; }
		m_cProcessOutcome=INVALID_REQUEST_DATA;
		return true;
	}

	m_cProcessOutcome=SUCCESSFULLY_PROCESSED;

	unsigned long PKID_C_PhoneStatus=0, RecordVersion=0;
	string FileName; // File to send to the phone

	MySqlSafetyRes rsMacAddress, rsPlutoId, rsUsers, rsAddress, rsExtra, rsShare, rsPhone;
	MYSQL_ROW MacAddressRow=NULL;
	MYSQL_ROW PlutoIdRow=NULL;
	MYSQL_ROW UsersRow=NULL, AddressRow=NULL, ExtraDataRow=NULL, ShareRow=NULL, PhoneRow=NULL;

	std::ostringstream s;
	s << "SELECT FKID_PlutoID, FKID_C_PhoneStatus, FKID_BinaryVersion, BinaryRevision, NoBinary, Revision, BinaryFilename FROM MacAddress LEFT JOIN PhoneModel ON FKID_PhoneModel=PKID_PhoneModel LEFT JOIN BinaryVersion ON FKID_BinaryVersion=PKID_BinaryVersion WHERE PKID_MacAddress=" << m_iMacAddress;

	cout << "Executing: " << s.str( ) << "\n";
	if( ( rsMacAddress.r = g_pPlutoConfig->mysql_query_result( s.str( ) ) ) && ( MacAddressRow=mysql_fetch_row( rsMacAddress.r ) ) )
	{
		// See if: This phone does take a binary, and NoBinary is not set, and there is revision
		if( MacAddressRow[2] && ( !MacAddressRow[4] || MacAddressRow[4][0]=='0' ) && MacAddressRow[5] )
		{
			if( !MacAddressRow[3] || atoi( MacAddressRow[3] )<atoi( MacAddressRow[5] ) )
				FileName = MacAddressRow[6]; // This is going to the phone
		}
		cout << "Found Mac " << m_iMacAddress << " associated with user: " << MacAddressRow[0] << "\n";
		m_iPlutoId = atoi( MacAddressRow[0] );
		PKID_C_PhoneStatus = atoi( MacAddressRow[1] );

		UsageLog << "Request from " << m_iEstablishmentID << " mac: " << m_iMacAddress <<
			" PlutoID: " << m_iPlutoId << " Status: " << PKID_C_PhoneStatus; }

		s.str( "" );
		s << "SELECT RecordVersion, Email FROM PlutoId WHERE PKID_PlutoId=" << m_iPlutoId;
		if( ( rsPlutoId.r = g_pPlutoConfig->mysql_query_result( s.str( ) ) ) && ( PlutoIdRow=mysql_fetch_row( rsPlutoId.r ) ) )
		{
			m_iRecordVersion = atoi( PlutoIdRow[0] );
			UsageLog << "Request from " << m_iEstablishmentID <<
				" PlutoID: " << m_iPlutoId << " has version " << m_iCachedRecordVersion << "/" << m_iRecordVersion; }
			if( m_iCachedRecordVersion == m_iRecordVersion )
			{
				cout << "using cached version\n";
				UsageLog << "Request from " << m_iEstablishmentID <<
					" PlutoID: " << m_iPlutoId << " using cache"; }
			  m_iUseCache=1;
				goto CheckForVMC;
			}
		}
	}

	// The Establishment doensn't have a current cached version. First let's see if this is the first time
	// the phone has registered. This would mean we didn't find the MacAddress in the database, but the
	// Establishment did pass in a PlutoId that it derived from the Bluetooth ID.
	if( !m_iPlutoId && m_iIdentifiedPlutoId )
	{
		UsageLog << "Request from " << m_iEstablishmentID <<
			" IdentiedPlutoID: " << m_iIdentifiedPlutoId << " " << m_sIdentifiedPlutoIdPin; }
		s.str( "" );
		s << "SELECT RecordVersion, Email, FKID_PhoneModel, NoBinary, BinaryFilename FROM PlutoId "
			<< " LEFT JOIN Users on FKID_PlutoId=PKID_PlutoId "
			<< " LEFT JOIN PhoneModel ON FKID_PhoneModel=PKID_PhoneModel "
			<< " LEFT JOIN BinaryVersion ON FKID_BinaryVersion=PKID_BinaryVersion "
			<< " WHERE PKID_PlutoId=" << m_iIdentifiedPlutoId << " AND PIN='"
			<< m_sIdentifiedPlutoIdPin << "'";

		if( ( rsPlutoId.r = g_pPlutoConfig->mysql_query_result( s.str( ) ) ) && ( PlutoIdRow=mysql_fetch_row( rsPlutoId.r ) ) )
		{
			cout << "Adding to database Mac " << m_iMacAddress << " ID " << m_iIdentifiedPlutoId << "\n";
			UsageLog << "Request from " << m_iEstablishmentID <<
				" IdentiedPlutoID: " << m_iIdentifiedPlutoId << " " << m_sIdentifiedPlutoIdPin << " adding to database"; }
			// We found the record
			m_iPlutoId = m_iIdentifiedPlutoId;
			// Add it to the MacAddress table
//			sprintf( sql, "INSERT INTO MacAddress ( PKID_MacAddress, FKID_PlutoId, FKID_C_PhoneStatus ) "
//				"VALUES ( %I64d, %d, %d )", m_iMacAddress, ( int ) m_iPlutoId, ( int ) C_PHONESTATUS_NEW_PHONE_CONST );


			// See if there is a model specified, and the the nobinary flag is not set, and there is a binary version
			if( ( !PlutoIdRow[3] || PlutoIdRow[3][0]=='0' ) && PlutoIdRow[2] && PlutoIdRow[4] )
				FileName = PlutoIdRow[4];

			s.str( "" );
			s << "INSERT INTO MacAddress ( PKID_MacAddress, FKID_PlutoId, FKID_C_PhoneStatus, NoBinary, FKID_PhoneModel ) " <<
				"VALUES ( " << m_iMacAddress << ", " << m_iPlutoId << ", " << C_PHONESTATUS_NEW_PHONE_CONST <<
				", " << ( PlutoIdRow[3] ? PlutoIdRow[3] : "0" ) << // No Binary
				", " << ( PlutoIdRow[2] ? PlutoIdRow[2] : "NULL" ) << // Phone Model
				" )";

			cout << "Executing: " << s << "\n";
			g_pPlutoConfig->threaded_mysql_query( s.str( ) );
		}
	}

	if( !m_iPlutoId )
	{
		cout << "No records found\n";
		UsageLog << "Request from " << m_iEstablishmentID <<
			" IdentiedPlutoID: " << m_iIdentifiedPlutoId << " " << m_sIdentifiedPlutoIdPin << " not one of ours"; }
		m_cProcessOutcome=RECORD_NOT_FOUND;
		return true;
	}

	s.str( "" );
	s << "SELECT Nickname, Gender, BirthDate, FirstName, LastName FROM Users where FKID_PlutoID=" << m_iPlutoId;
	if( ( rsUsers.r = g_pPlutoConfig->mysql_query_result( s.str( ) ) ) )
		UsersRow=mysql_fetch_row( rsUsers.r );

	s.str( "" );
	s << "SELECT Address, ZipCode, City.Name as City, C_State.Name As State FROM Address LEFT JOIN City on FKID_City=PKID_City LEFT JOIN C_State ON FKID_C_State=PKID_C_State where FKID_PlutoID=" << m_iPlutoId;
	if( ( rsAddress.r = g_pPlutoConfig->mysql_query_result( s.str( ) ) ) )
		AddressRow=mysql_fetch_row( rsAddress.r );

	s.str( "" );
	s << "SELECT AreaCode, Number, Extension, FKID_PhoneNumberCategory FROM PhoneNumber where FKID_PlutoID=" << m_iPlutoId;
	rsPhone.r = g_pPlutoConfig->mysql_query_result( s.str( ) );

	s.str( "" );
	s << "SELECT ShareText FROM Users_EstablishmentCategory INNER JOIN Establishment ON Establishment.FKID_EstablishmentCategory=Users_EstablishmentCategory.FKID_EstablishmentCategory WHERE FKID_PlutoId_User="
		<< m_iEstablishmentID << " AND FKID_PlutoID=" << m_iPlutoId;

	if( ( rsExtra.r = g_pPlutoConfig->mysql_query_result( s.str( ) ) ) )
	{
		while( ExtraDataRow=mysql_fetch_row( rsExtra.r ) )
		{
			MYSTL_ADDTO_LIST( m_listAttributes, new MiscVIPAttribute( "extra", ExtraDataRow[0] ) );
		}
	}

	s.str( "" );
	s << "SELECT FKID_C_Field FROM ShareWithEstablishment WHERE FKID_PlutoId=" << m_iPlutoId;

	if( ( rsShare.r = g_pPlutoConfig->mysql_query_result( s.str( ) ) ) )
	{
		while( ShareRow=mysql_fetch_row( rsShare.r ) )
		{
			cout << "Sharing field " << ShareRow[0] << "\n";
			switch( atoi( ShareRow[0] ) )
			{
			case	USERS_NICKNAME:
				if( UsersRow && UsersRow[0] )
					m_sNickname=UsersRow[0];
				break;
			case	PLUTOID_EMAIL:
				if( PlutoIdRow && PlutoIdRow[1] )
				m_sEmail = PlutoIdRow[1];
				break;
			case	USERS_GENDER:
				if( UsersRow && UsersRow[1] )
				m_sGender = UsersRow[1];
				break;
			case	USERS_BIRTHDATE:
				if( UsersRow && UsersRow[2] )
				m_sBirthdate = UsersRow[2];
				break;
			case	USERS_FIRSTNAME:
				if( UsersRow && UsersRow[3] )
				m_sFirstName = UsersRow[3];
				break;
			case	USERS_LASTNAME:
				if( UsersRow && UsersRow[4] )
				m_sLastName = UsersRow[4];
				break;
			case	PKID_ADDRESS:
				if( AddressRow && AddressRow[0] )
				m_sAddress = AddressRow[0];
				if( AddressRow && AddressRow[2] )
				m_sCity = AddressRow[2];
				if( AddressRow && AddressRow[3] )
				m_sState = AddressRow[3];
				if( AddressRow && AddressRow[1] )
				m_sZip = AddressRow[1];
				break;
			case	PKID_PHONENUMBER:
				if( rsPhone.r )
				{
					while( PhoneRow=mysql_fetch_row( rsPhone.r ) )
					{
						VIPPhoneNumber *pn = new VIPPhoneNumber( );
						pn->m_iPhoneNumberType=( PhoneRow[3] ? atoi( PhoneRow[3] ) : 0 );
						pn->m_sNumber=string( "( " ) + ( PhoneRow[0] ? PhoneRow[0] : "" ) +
							" ) " + ( PhoneRow[1] ? PhoneRow[1] : "" ) +
							( PhoneRow[2]!=NULL ? string( " ext: " ) + PhoneRow[2] : "" );
						MYSTL_ADDTO_LIST( m_listPhoneNumbers, pn );
					}
				}
				break;
			case	_FP_PICTURE:
				FILE *file = fopen( ( g_pPlutoConfig->m_sPicturePath + StringUtils::itos( m_iPlutoId ) + ".jpg" ).c_str( ), "rb" );
				cout << "Tried to open " << g_pPlutoConfig->m_sPicturePath << m_iPlutoId << ".jpg result: " << file << "\n";
				m_iImageType=GRAPHICTYPE_JPG;
				if( file )
				{
					fseek( file, 0L, SEEK_END );
					m_iImageSize=ftell( file );
					if( m_iImageSize==0 )
						fclose( file );
					else
					{
						fseek( file, 0L, SEEK_SET );
						m_pImage=( char * )malloc( m_iImageSize+1 );
						size_t bytesread=fread( m_pImage, 1, m_iImageSize, file );
						fclose( file );
					}
				}
			};
		}
	}

  m_iUseCache=0;

CheckForVMC:
	// Let's see if we're storing the Establishment's customer ID, or if there's a VMC
	// that we should automatically send to the phone
	MYSQL_ROW CustomerDataRow=NULL;
	s.str( "" );
	s << "SELECT Users_Establishment.EstablishmentCustomerID, Users_Establishment.AutoSendVMC, Establishment.AutoSendVMC "
		"FROM Establishment LEFT JOIN Users_Establishment ON FKID_PlutoId_Establishment=Establishment.FKID_PlutoId WHERE "
		"FKID_PlutoId_User=" << m_iPlutoId << " AND FKID_PlutoId_Establishment=" << m_iEstablishmentID;
	cout << "Looking up user: " << m_iPlutoId << " establishment: " << m_iEstablishmentID << "\n";
	MySqlSafetyRes rsCustomerDataRow;
	if( ( rsCustomerDataRow.r = g_pPlutoConfig->mysql_query_result( s.str( ) ) ) && ( CustomerDataRow=mysql_fetch_row( rsCustomerDataRow.r ) ) )
	{
		cout << "Found a record\n";
		const char *AutoSendVMC=NULL;
		if( CustomerDataRow[0] )
			m_sEstablishmentCustomerId = CustomerDataRow[0];
		if( CustomerDataRow[1] )
			AutoSendVMC = CustomerDataRow[1];
		else if( CustomerDataRow[2] )
			AutoSendVMC = CustomerDataRow[2];

		if( AutoSendVMC )
		{
			cout << "Sending menu " << AutoSendVMC;
			UsageLog << "Sending menu " << g_pPlutoConfig->m_sMenuPath << AutoSendVMC; }

			VR_ShowMenu *pMenu = new VR_ShowMenu( ( g_pPlutoConfig->m_sMenuPath + AutoSendVMC ).c_str( ) );
			if( !pMenu->m_pMenuCollection )
			{
				ErrorLog << "Couldn't send menu " << g_pPlutoConfig->m_sMenuPath << AutoSendVMC; }
				delete pMenu;  // Menu wasn't found
			}
			else
			{
				VA_ForwardRequestToPhone *pbefore = new VA_ForwardRequestToPhone( 0, pMenu, m_iMacAddress );
				MYSTL_ADDTO_LIST( m_listActions, pbefore );
			}
		}
	}

	LogVisit( );

#endif
	return true;
}


void VR_IdentifyPhone::LogVisit( )
{
#ifdef VIPSERVER
	char sql[100];
	cout << "Visit Logged\n";
	sprintf( sql, "INSERT INTO Visit ( FKID_PlutoId_User, FKID_PlutoId_Establishment ) VALUES ( %d, %d )", m_iPlutoId, m_iEstablishmentID );
	g_pPlutoConfig->threaded_mysql_query( sql );
#endif
}
*/
